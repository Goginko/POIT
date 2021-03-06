// ***********************************************************************************
//   Custom DPA Handler code example - User peripheral implementation - Dallas 18B20 *
// ***********************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-UserPeripheral-18B20.c,v $
// Version: $Revision: 1.35 $
// Date:    $Date: 2020/02/20 17:18:58 $
//
// Revision history:
//   2017/03/13  Release for DPA 3.00
//   2016/09/12  Release for DPA 2.28
//   2015/08/05  Release for DPA 2.20
//   2014/05/26  Release for DPA 2.11
//   2014/10/31  Release for DPA 2.10
//   2014/05/26  Release for DPA 2.01
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// This example implements the user peripheral reading from Dallas 18B20
// PNUM = 0x20 and PCMD = 0 returns 2 bytes with result read from Dallas 18B20
// Dallas 18B20 is connected to MCU pin (see below) with 10k pull-up resistor and not using parasite power

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

//############################################################################################

// If defined then CRC is checked
#define	USE_ONEWIRE_CRC

// Special temperature value to indicate a sensor error 
#define	DS18B20_ERROR_TEMPERATURE 0xF800

// Sensor connected to PORT C.3 (compatible with DDC-SE-01)
#define OneWire_TRIS         TRISC.3
#define OneWire_IO_IN        PORTC.3
#define OneWire_IO_OUT       LATC.3

// Reads temperature from sensor
uns16 Ds18B20GetTemp();
// Writes sensor configuration (resolution)
bit Ds18B20WriteConfig( uns8 value );

// DS18B20 commands
#define CMD_READROM		  0x33
#define CMD_CONVERTTEMP	  0x44
#define CMD_CPYSCRATCHPAD 0x48
#define CMD_WSCRATCHPAD	  0x4e
#define CMD_MATCHROM	  0x55
#define CMD_RPWRSUPPLY	  0xb4
#define CMD_RECEEPROM	  0xb8
#define CMD_RSCRATCHPAD	  0xbe
#define CMD_SKIPROM		  0xcc
#define CMD_ALARMSEARCH	  0xec
#define CMD_SEARCHROM	  0xf0

// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // Handler presence mark
  clrwdt();

  // Detect DPA event to handle
  switch ( GetDpaEvent() )
  {
    // -------------------------------------------------
    case DpaEvent_Interrupt:
      // Do an extra quick background interrupt work
      // ! The time spent handling this event is critical.If there is no interrupt to handle return immediately otherwise keep the code as fast as possible.
      // ! Make sure the event is the 1st case in the main switch statement at the handler routine.This ensures that the event is handled as the 1st one.
      // ! It is desirable that this event is handled with immediate return even if it is not used by the custom handler because the Interrupt event is raised on every MCU interrupt and the �empty� return handler ensures the shortest possible interrupt routine response time.
      // ! Only global variables or local ones marked by static keyword can be used to allow reentrancy.
      // ! Make sure race condition does not occur when accessing those variables at other places.
      // ! Make sure( inspect.lst file generated by C compiler ) compiler does not create any hidden temporary local variable( occurs when using division, multiplication or bit shifts ) at the event handler code.The name of such variable is usually Cnumbercnt.
      // ! Do not call any OS functions except setINDFx().
      // ! Do not use any OS variables especially for writing access.
      // ! All above rules apply also to any other function being called from the event handler code, although calling any function from Interrupt event is not recommended because of additional MCU stack usage.

DpaHandleReturnTRUE:
      return TRUE;

      // -------------------------------------------------
    case DpaEvent_Init:
      // Do a one time initialization before main loop starts

      // Setup DS18B20 for 9bit precision, conversion takes 94ms (see datasheet)
      //Ds18B20WriteConfig( 0b0.00.00000 );

      //// Setup DS18B20 for 12bit precision, conversion takes 750ms (see datasheet)
      Ds18B20WriteConfig( 0b0.11.00000 );

      break;

      // -------------------------------------------------
    case DpaEvent_DpaRequest:
      // Called to interpret DPA request for peripherals
      // -------------------------------------------------
      // Peripheral enumeration
      if ( IsDpaEnumPeripheralsRequest() )
      {
        // We implement 1 user peripheral
        _DpaMessage.EnumPeripheralsAnswer.UserPerNr = 1;
        FlagUserPer( _DpaMessage.EnumPeripheralsAnswer.UserPer, PNUM_USER + 0 );
        _DpaMessage.EnumPeripheralsAnswer.HWPID = 0x000F;
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver = 0x9876;

        goto DpaHandleReturnTRUE;
      }
      // -------------------------------------------------
      // Get information about peripheral
      else if ( IsDpaPeripheralInfoRequest() )
      {
        if ( _PNUM == PNUM_USER + 0 )
        {
          _DpaMessage.PeripheralInfoAnswer.PerT = PERIPHERAL_TYPE_USER_AREA;
          _DpaMessage.PeripheralInfoAnswer.PerTE = PERIPHERAL_TYPE_EXTENDED_READ;
          goto DpaHandleReturnTRUE;
        }

        break;
      }
      // -------------------------------------------------
      else
      {
        // Handle peripheral command
        if ( _PNUM == PNUM_USER + 0 )
        {
          // Check command
          switch ( _PCMD )
          {
            case 0:
              // -------------------------------------------------
              // Read temperature
              if ( _DpaDataLength != 0 )
                DpaApiReturnPeripheralError( ERROR_DATA_LEN );

              uns16 temperature  @ _DpaMessage.Response.PData;
              temperature = Ds18B20GetTemp();
              if ( temperature == DS18B20_ERROR_TEMPERATURE )
                DpaApiReturnPeripheralError( ERROR_FAIL );

              _DpaDataLength = sizeof( temperature );
              goto DpaHandleReturnTRUE;

            default:
              // -------------------------------------------------
              // Invalid command
              DpaApiReturnPeripheralError( ERROR_PCMD );
          }
        }
      }
  }

  return FALSE;
}

//############################################################################################
// OneWire and Dallas 18B20 routines
// If there is an interrupt running that consumes a lot of MCU time (>450us once per 1ms) then use GIE=1/GIE=0 at whole OneWireResetAndCmd() for sure
//############################################################################################

//############################################################################################
void Delay5us( uns8 val @ W ) // Absolutely precise timing but val != 0
//############################################################################################
{
#if F_OSC == 16000000
  // 16 MHz
  // + 0.75us ( W=val, Call )
  for ( ;; )
  {         // loop time
    nop2();	// 0.50us
    nop2();	// 1.00us
    nop2();	// 1.50us
    nop2();	// 2.00us
    nop2();	// 2.50us
    nop2();	// 3.00us
    nop();	// 3.25us
    if ( --val == 0 ) // + 0.75us (W--, BTFS ) 
      return;         // + 0.25us 
    nop2();	// 4.50us
  }         // 5.00us (Goto)
#else
#error Unsupported oscillator frequency
#endif
}
//############################################################################################

#define	OneWireData0()	{ OneWire_TRIS = 0; }	  // 0.5us @ 16MHz
#define OneWireData1()  { OneWire_TRIS = 1; }	  // 0.5us @ 16MHz

//############################################################################################
void OneWireWriteByte( uns8 byte )
//############################################################################################
{
  uns8 bitLoop = 8;
  do
  {
    // Next sequence is time precision critical
    GIE = FALSE;

    OneWireData0();
    nop2();	        // 1 us [0.5 us]
#if F_OSC == 16000000
    nop2();	        // [1.0 us]
#endif
    if ( byte & 1 )	// 2.5 us [1.75us]
      OneWireData1();

    // End of time precision critical sequence
    GIE = TRUE;

    // 60us minimum in total, does not have to be precise
    Delay5us( ( 60 - 3 ) / 5 + 1 );
    OneWireData1();

    byte >>= 1;
  } while ( --bitLoop != 0 );
}

//############################################################################################
uns8 OneWireReadByte()
//############################################################################################
{
  uns8 result;
  uns8 bitLoop = 8;
  do
  {
    // Next sequence is time precision critical
    GIE = FALSE;

    OneWireData0();
    nop2();	        // 1 us [0.5 us]
#if F_OSC == 16000000
    nop2();	        // [1.0 us]
#endif
    OneWireData1();			// 2 us [1.5 us]
    Delay5us( 15 / 5 );		// 17 us [16.5 us]

    Carry = 0;				// 17.5 us [16.75 us]
    if ( OneWire_IO_IN )	// 18.5 us [ 17.25 us] (condition must not modify Carry)
      Carry = 1;

    // End of time precision critical sequence
    GIE = TRUE;				// must not modify Carry
    result = rr( result );

    // 60us minimum in total, does not have to be precise
    Delay5us( ( 60 - 20 ) / 5 + 1 );
  } while ( --bitLoop != 0 );

  return result;
}

//############################################################################################
bit OneWireResetAndCmd( uns8 cmd )
//############################################################################################
{
  // Setting the pin once to low is enough
  OneWire_IO_OUT = 0;
  // Reset pulse
  OneWireData0();
  Delay5us( 500 / 5 );
  // Reset pulse end
  OneWireData1();
  // Next sequence is time precision critical
  GIE = FALSE;
  // Wait for presence pulse
  Delay5us( 70 / 5 );
  // End of time precision critical sequence
  GIE = TRUE;
  // Presence pulse?
  if ( OneWire_IO_IN )
  {
    // No presence, finish initialization sequence
    Delay5us( ( 500 - 70 ) / 5 );
    return FALSE;
  }
  else
  {
    // Presence OK, finish initialization sequence
    Delay5us( ( 500 - 70 ) / 5 );
    // OneWire: Skip ROM
    OneWireWriteByte( CMD_SKIPROM );
    // OneWire: Send command
    OneWireWriteByte( cmd );
    return TRUE;
  }
}

//############################################################################################

#ifdef USE_ONEWIRE_CRC
// One Wire CRC accumulator
static uns8 OneWireCrc;

//############################################################################################
void UpdateOneWireCrc( uns8 value @ W )
//############################################################################################
{
  // Based on http://www.dattalo.com/technical/software/pic/crc_8bit.c
  OneWireCrc ^= W;
#pragma updateBank 0 /* OFF */	
  value = 0;
  if ( OneWireCrc.0 ) // 1 instruction
    value ^= 0x5e;	  // 1 instruction
  if ( OneWireCrc.1 ) // 1 instruction
    value ^= 0xbc;	  // ...
  if ( OneWireCrc.2 )
    value ^= 0x61;	// ...
  if ( OneWireCrc.3 )
    value ^= 0xc2;	// ...
  if ( OneWireCrc.4 )
    value ^= 0x9d;	// ...
  if ( OneWireCrc.5 )
    value ^= 0x23;	// ...
  if ( OneWireCrc.6 )
    value ^= 0x46;	// ...
  if ( OneWireCrc.7 )
    value ^= 0x8c;	// 0x8C is reverse polynomial representation (normal is 0x31)
  OneWireCrc = value;
#pragma updateBank 1 /* ON */
}

#endif

//############################################################################################
uns16 Ds18B20GetTemp()
//############################################################################################
{
  // OneWire: Convert T
  if ( OneWireResetAndCmd( CMD_CONVERTTEMP ) )
  {
    // Wait for conversion to finish
    startCapture();
    for ( ;; )
    {
      // 1s timeout
      captureTicks();
      if ( param3.low8 > ( 1000 / 10 ) )
      {
        // Timeout
        Carry = FALSE;
        break;
      }

      if ( OneWireReadByte() == 0xff )
      {
        // Temperature ready to be read
        Carry = TRUE;
        break;
      }
    }

    // OneWire: Read Scratchpad
    if ( Carry && OneWireResetAndCmd( CMD_RSCRATCHPAD ) )
    {
      // Read Scratchpad bytes
      uns16 temperature;

#ifdef USE_ONEWIRE_CRC
      // Initialize crc
      OneWireCrc = 0;
      // Will read later rest of scratchpad bytes and update CRC (initialization is here to avoid MOVLB later)
      uns8 byteLoop = 9 - 2;

      // Temperature LSB into result & CRC
      UpdateOneWireCrc( temperature.low8 = OneWireReadByte() );
      // Temperature MSB into result & CRC
      UpdateOneWireCrc( temperature.high8 = OneWireReadByte() );

      // Read rest of scratchpad 
      do
      {
        UpdateOneWireCrc( OneWireReadByte() );
      } while ( --byteLoop != 0 );

      // Check correct CRC
      if ( OneWireCrc == 0 )
        return temperature;
#else
      temperature.low8 = OneWireReadByte();
      temperature.high8 = OneWireReadByte();
      return temperature;
#endif
    }
  }
  // Some error occurred
  return DS18B20_ERROR_TEMPERATURE;
}

//############################################################################################
bit Ds18B20WriteConfig( uns8 value )
//############################################################################################
{
  // OneWire: Write Scratchpad
  if ( OneWireResetAndCmd( CMD_WSCRATCHPAD ) )
  {
    // Write TL = ? (we dot not care the value)
    OneWireWriteByte( W );
    // Write TH = ? (we dot not care the value)
    OneWireWriteByte( W );
    // Write Config byte
    OneWireWriteByte( value );

    // OneWire: Copy Scratchpad
    if ( OneWireResetAndCmd( CMD_CPYSCRATCHPAD ) )
      return TRUE;
  }
  return FALSE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
