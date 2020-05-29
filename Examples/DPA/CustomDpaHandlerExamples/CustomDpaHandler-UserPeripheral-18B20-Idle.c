// ***********************************************************************************
//   Custom DPA Handler code example - User peripheral implementation - Dallas 18B20 *
// ***********************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-UserPeripheral-18B20-Idle.c,v $
// Version: $Revision: 1.26 $
// Date:    $Date: 2020/02/20 17:18:58 $
//
// Revision history:
//   2017/03/13  Release for DPA 3.00
//   2015/08/05  Release for DPA 2.20
//   2014/05/26  Release for DPA 2.11
//   2014/10/31  Release for DPA 2.10
//   2014/05/26  Release for DPA 2.01
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// This example implements the user peripheral reading from Dallas 18B20
// The sensor is continuously read on the background at Idle event
// PNUM = 0x20 and PCMD = 0 returns 2 bytes with result read from Dallas 18B20
// Dallas 18B20 is connected to MCU pin (see below) with 10k pull-up resistor and not using parasite power
// Also user FRC command is implemented

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

//############################################################################################

// Special temperature value to indicate a sensor error 
#define	DS18B20_ERROR_TEMPERATURE 0xF800

// Sensor connected to PORT C.3 (compatible with DDC-SE-01)
#define OneWire_TRIS         TRISC.3
#define OneWire_IO_IN        PORTC.3
#define OneWire_IO_OUT       LATC.3

// Writes sensor configuration (resolution)
bit Ds18B20WriteConfig( uns8 value );

// Resets OneWire
bit OneWireReset();
// Reads OneWire byte
uns8 OneWireReadByte();
// Writes OneWire byte
void OneWireWriteByte( uns8 byte );

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

#define	TICKS_LEN  10

// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // Finite machine states
  typedef enum
  {
    S_ResetConvertT = 0,
    S_SkipRomConvertT,
    S_CmdConvertT,

    S_WaitConvertT,

    S_ResetReadTemp,
    S_SkipRomReadTemp,
    S_CmdReadTemp,
    S_Byte1ReadTemp,
    S_Byte2ReadTemp
  } TState;

  // Handler presence mark
  clrwdt();

  // Finite machine state
  static uns8 state; // = S_ResetConvertT = 0

  // Pre-read lower temperature byte
  static uns8 temperatureByteLow;
  // Final temperature value
  static uns16 temperature;
  // Conversion timeout counter
  static uns8 timeout;

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

      //  If TMR6 interrupt occurred
      if ( TMR6IF )
      {
        // Unmask interrupt
        TMR6IF = 0;
        // Decrement count
        if ( timeout != 0 )
          timeout--;
      }

DpaHandleReturnTRUE:
      return TRUE;

      // -------------------------------------------------
    case DpaEvent_Idle:
      // Do a quick background work when RF packet is not received

      // Make sure 1Wire data pin at LATX.y is low as it might be set by another PORTX.? pin manipulation
      OneWire_IO_OUT = 0;

      // Run finite state machine
      skip( state );
#pragma computedGoto 1
      goto _S_ResetConvertT;
      goto _S_SkipRomConvertT;
      goto _S_CmdConvertT;
      goto _S_WaitConvertT;
      goto _S_ResetReadTemp;
      goto _S_SkipRomReadTemp;
      goto _S_CmdReadTemp;
      goto _S_Byte1ReadTemp;
      goto _S_Byte2ReadTemp;
#pragma computedGoto 0
      ;
      // --------------
_S_Byte2ReadTemp:
      temperature.high8 = OneWireReadByte();
      temperature.low8 = temperatureByteLow;

ResetMachine:
      state = S_ResetConvertT;
      goto ExitMachine;

      // --------------
_S_ResetConvertT:
_S_ResetReadTemp:
      if ( !OneWireReset() )
      {
_S_Error_Reset:
        temperature = DS18B20_ERROR_TEMPERATURE;
        goto ResetMachine;
      }
      goto NextState;

      // --------------
_S_SkipRomConvertT:
_S_SkipRomReadTemp:
      // OneWire: Skip ROM
      OneWireWriteByte( CMD_SKIPROM );
      goto NextState;

      // --------------
_S_CmdConvertT:
      // OneWire: Convert temperature
      OneWireWriteByte( CMD_CONVERTTEMP );
      // Setup timeout for approx 750 ms (the longest conversion time)
      timeout = 2 + 750 / TICKS_LEN;
      goto NextState;

      // --------------
_S_WaitConvertT:
      if ( OneWireReadByte() == 0xff )
        goto NextState;

      // Timeout?
      if ( timeout == 0 )
        goto _S_Error_Reset;

      goto ExitMachine;

      // --------------
_S_CmdReadTemp:
      // OneWire: Read scratchpad
      OneWireWriteByte( CMD_RSCRATCHPAD );
      goto NextState;

      // --------------
_S_Byte1ReadTemp:
      temperatureByteLow = OneWireReadByte();
      goto NextState;

      // --------------
NextState:
      ++state;

ExitMachine:
      break;

      // -------------------------------------------------
    case DpaEvent_Init:
      // Setup TMR6 to generate ticks on the background (ticks every 10ms)
#if F_OSC == 16000000
      PR6 = 250 - 1;
      T6CON = 0b0.1001.1.10;	// Prescaler 16, Postscaler 10, 16 * 10 * 250 = 40000 = 4MHz * 10ms
#else
#error Unsupported oscillator frequency
#endif

      // Set error temperature
      temperature = DS18B20_ERROR_TEMPERATURE;
      // Do a one time initialization before main loop starts
      Ds18B20WriteConfig( 0b0.11.00000 ); // Setup DS18B20 for 12bit precision, conversion takes 750ms (see datasheet)
      //Ds18B20WriteConfig( 0b0.00.00000 ); // Setup DS18B20 for 9bit precision, conversion takes 94ms (see datasheet)
      break;

      // -------------------------------------------------
    case DpaEvent_AfterSleep:
      // Called after woken up after sleep

      goto ResetMachine;

    case DpaEvent_FrcValue:
      // Called to get FRC value

      if ( _PCMD == ( FRC_USER_BYTE_FROM + 0 ) )
      {
        // Return temperature
        // Dallas returns 12 bits, so we have to do >> 4 to return fixed part

        W = swap( temperature.low8 );	//                W = L3 L2 L1 L0 L7 L6 L5 L4 
        responseFRCvalue = W & 0x0F;	// responseFRCvalue = 0  0  0  0  L7 L6 L5 L4
        W = swap( temperature.high8 );	//                W = H3 H2 H1 H0 H7 H6 H5 H4
        W &= 0xF0;						//                W = H3 H2 H1 H0 0  0  0  0 
        responseFRCvalue |= W;			// responseFRCvalue = H3 H2 H1 H0 L7 L6 L5 L4
        // Avoid returning 0
        if ( responseFRCvalue == 0 )
          responseFRCvalue = 127;
      }
      break;

      // -------------------------------------------------
    case DpaEvent_FrcResponseTime:
      // Called to get FRC response time

      if ( DataOutBeforeResponseFRC[0] == ( FRC_USER_BYTE_FROM + 0 ) )
        responseFRCvalue = _FRC_RESPONSE_TIME_40_MS;
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
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver = 0x2468;

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

              uns16 temperatureStore  @ _DpaMessage.Response.PData;
              temperatureStore = temperature;
              if ( temperatureStore == DS18B20_ERROR_TEMPERATURE )
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
    if ( byte.0 )	// 2.5 us [1.75us]
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
bit OneWireReset()
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
    return TRUE;
  }
}

//############################################################################################
void OneWireCmd( uns8 cmd )
//############################################################################################
{
  // OneWire: Skip ROM
  OneWireWriteByte( CMD_SKIPROM );
  // OneWire: Send command
  OneWireWriteByte( cmd );
}

//############################################################################################
bit Ds18B20WriteConfig( uns8 value )
//############################################################################################
{
  if ( OneWireReset() )
  {
    // Write Scratchpad
    OneWireCmd( CMD_WSCRATCHPAD );

    // Write TL = ? (we dot not care the value)
    OneWireWriteByte( W );
    // Write TH = ? (we dot not care the value)
    OneWireWriteByte( W );
    // Write Config byte
    OneWireWriteByte( value );

    if ( OneWireReset() )
    {
      //  Copy Scratchpad
      OneWireCmd( CMD_CPYSCRATCHPAD );
      return TRUE;
    }
  }
  return FALSE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
