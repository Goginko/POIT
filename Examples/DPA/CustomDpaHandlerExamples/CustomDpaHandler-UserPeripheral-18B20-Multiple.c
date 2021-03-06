// ********************************************************************************************
//   Custom DPA Handler code example - User peripheral implementation - Multiple Dallas 18B20 *
// ********************************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-UserPeripheral-18B20-Multiple.c,v $
// Version: $Revision: 1.21 $
// Date:    $Date: 2020/02/20 17:18:58 $
//
// Revision history:
//   2017/03/13  Release for DPA 3.00
//   2016/09/12  Release for DPA 2.28
//   2015/12/16  Release for DPA 2.24
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// This example implements the user peripheral reading temperature from one Dallas 18B20 temperature sensor 
// There are more such sensors connected to the 1-Wire bus
// * PNUM = 0x20, PCMD = 0 and PDATA="8 byte ROM code" of the sensor, returns 2 bytes with temperature value read from the selected Dallas 18B20
// * PNUM = 0x20, PCMD = 1 returns list of up to seven "8 byte ROM codes" of connected 1-Wire devices
// Dallas 18B20 sensors are connected to MCU pin (see below) with 10k pull-up resistor and not using parasite power

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

//############################################################################################

// Special temperature value to indicate a sensor error 
#define	DS18B20_ERROR_TEMPERATURE 0xF800

// Sensor connected to PORT C.3 (compatible with multiple DDC-SE-01)
#define OneWire_TRIS         TRISC.3
#define OneWire_IO_IN        PORTC.3
#define OneWire_IO_OUT       LATC.3

// Reads temperature from sensor
uns16 Ds18B20GetTemp();
// Writes sensor configuration (resolution)
bit Ds18B20WriteConfig( uns8 value );
// Searches 1-Wire bus device IDs
uns8 OneWireSearch();

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

// What to send to 1-Wire bus after Reset condition and before sending command
#define	ROM_SKIP  0
#define	ROM_MATCH 1
#define	ROM_none  2

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
      Ds18B20WriteConfig( 0b0.00.00000 );

      //// Setup DS18B20 for 12bit precision, conversion takes 750ms (see datasheet)
      //Ds18B20WriteConfig( 0b0.11.00000 );

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
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver = 0xaabb;

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
              if ( _DpaDataLength != 8 )
                DpaApiReturnPeripheralError( ERROR_DATA_LEN );

              uns16 temperature	@ _DpaMessage.Response.PData;
              temperature = Ds18B20GetTemp();
              if ( temperature == DS18B20_ERROR_TEMPERATURE )
                DpaApiReturnPeripheralError( ERROR_FAIL );

              _DpaDataLength = sizeof( temperature );
              goto DpaHandleReturnTRUE;

            case 1:
              // -------------------------------------------------
              // Read 1-Wire ROM codes
              if ( _DpaDataLength != 0 )
                DpaApiReturnPeripheralError( ERROR_DATA_LEN );

              _DpaDataLength = OneWireSearch();
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
void OneWireWriteBit( bit value )
//############################################################################################
{
  // Next sequence is time precision critical
  GIE = FALSE;

  OneWireData0();
  nop2();	        // 1 us [0.5 us]
#if F_OSC == 16000000
  nop2();	        // [1.0 us]
#endif
  if ( value )	// 2.5 us [1.75us]
    OneWireData1();

  // End of time precision critical sequence
  GIE = TRUE;

  // 60us minimum in total, does not have to be precise
  Delay5us( ( 60 - 3 ) / 5 + 1 );
  OneWireData1();
}

//############################################################################################
void OneWireWriteByte( uns8 byte )
//############################################################################################
{
  uns8 bitLoop = 8;
  do
  {
    byte = rr( byte );
    OneWireWriteBit( Carry );
  } while ( --bitLoop != 0 );
}

//############################################################################################
bit OneWireReadBit()
//############################################################################################
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

  nop();				// 17.5 us [16.75 us]
  if ( OneWire_IO_IN )	// 18.5 us [ 17.25 us]
  {
    GIE = TRUE;
    // 60us minimum in total, does not have to be precise
    Delay5us( ( 60 - 20 ) / 5 + 1 );
    return TRUE;
  }
  else
  {
    GIE = TRUE;
    // 60us minimum in total, does not have to be precise
    Delay5us( ( 60 - 20 ) / 5 + 1 );
    return FALSE;
  }
}

//############################################################################################
uns8 OneWireReadByte()
//############################################################################################
{
  uns8 result;
  uns8 bitLoop = 8;
  do
  {
    // Read one bit
    Carry = OneWireReadBit();
    result = rr( result );
  } while ( --bitLoop != 0 );

  return result;
}

//############################################################################################
bit OneWireResetAndCmd( uns8 ROM, uns8 cmd )
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
    switch ( ROM )
    {
      case ROM_SKIP:
        // OneWire: Skip ROM
        OneWireWriteByte( CMD_SKIPROM );
        break;

      case ROM_MATCH:
        // OneWire: Match ROM (must not be used during search because usage o FSR0)
        OneWireWriteByte( CMD_MATCHROM );
        FSR0 = _DpaMessage.Request.PData;
        do
        {
          OneWireWriteByte( *FSR0++ );
        } while ( FSR0L != ( &_DpaMessage.Request.PData[8] & 0xFF ) );
        break;

      case ROM_none:
        break;
    }

    OneWireWriteByte( cmd );
    return TRUE;
  }
}

//############################################################################################

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


//############################################################################################
uns16 Ds18B20GetTemp()
//############################################################################################
{
  // OneWire: Convert T
  if ( OneWireResetAndCmd( ROM_MATCH, CMD_CONVERTTEMP ) )
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
    if ( Carry && OneWireResetAndCmd( ROM_MATCH, CMD_RSCRATCHPAD ) )
    {
      // Read Scratchpad bytes
      uns16 temperature;

      // Initialize CRC
      OneWireCrc = 0;
      // Will read later rest of scratchpad bytes and update CRC (initialization is here to avoid MOVLB later)
      uns8 byteLoop = 9 - 2;

      // Temperature LSB into result & CRC
      UpdateOneWireCrc( temperature.low8 = OneWireReadByte() );
      // Temperature MSB into result & CRC
      UpdateOneWireCrc( temperature.high8 = OneWireReadByte() );

      // Read rest of Scratchpad 
      do
      {
        UpdateOneWireCrc( OneWireReadByte() );
      } while ( --byteLoop != 0 );

      // Check correct CRC
      if ( OneWireCrc == 0 )
        return temperature;
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
  if ( OneWireResetAndCmd( ROM_SKIP, CMD_WSCRATCHPAD ) )
  {
    // Write TL = ? (we dot not care the value)
    OneWireWriteByte( W );
    // Write TH = ? (we dot not care the value)
    OneWireWriteByte( W );
    // Write configuration byte
    OneWireWriteByte( value );

    // OneWire: Copy Scratchpad
    if ( OneWireResetAndCmd( ROM_SKIP, CMD_CPYSCRATCHPAD ) )
      return TRUE;
  }
  return FALSE;
}

//############################################################################################
uns8 OneWireSearch()
//############################################################################################
{
  // Based on Maxim Integrated APPLICATION NOTE 187 

  // reset the search state
  uns8 LastDiscrepancy = 0;
  // FSR0 points to the current ROM id
  FSR0 = _DpaMessage.Response.PData;
  // FSR1 points to the last read ROM id
  // Start loop only if Search command is confirmed
  for ( FSR1 = FSR0; OneWireResetAndCmd( ROM_none, CMD_SEARCHROM ); )
  {
    // initialize for search
    uns8 rom_byte_number = 0;
    uns8 last_zero = 0;
    OneWireCrc = 0;
    uns8 id_bit_number = 1;
    uns8 rom_byte_mask = 0x01;

    // loop to do the search
    do
    {
      // read a bit and its complement
      bit id_bit = OneWireReadBit();
      bit cmp_id_bit = OneWireReadBit();

      // check for no devices on 1-wire
      if ( id_bit && cmp_id_bit )
        break;
      else
      {
        bit search_direction;

        // all devices coupled have 0 or 1
        if ( id_bit != cmp_id_bit )
          search_direction = id_bit;  // bit write value for search
        else
        {
          // if this discrepancy is before the Last Discrepancy on a previous next then pick the same as last time
          if ( id_bit_number < LastDiscrepancy )
            search_direction = ( *FSR1 & rom_byte_mask ) != 0;
          else
            // if equal last pick 1, if not then pick 0
            search_direction = id_bit_number == LastDiscrepancy;

          // if 0 was picked then record its position in LastZero
          if ( !search_direction )
            last_zero = id_bit_number;
        }

        // set or clear the bit in the ROM byte rom_byte_number with mask rom_byte_mask
        if ( search_direction )
          setINDF0( *FSR0 | rom_byte_mask );
        else
          setINDF0( *FSR0 & ~rom_byte_mask );

        // serial number search direction write bit
        OneWireWriteBit( search_direction );

        // increment the byte counter id_bit_number
        id_bit_number++;
        // and shift the mask rom_byte_mask
        rom_byte_mask <<= 1;

        // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
        if ( rom_byte_mask == 0 )
        {
          // Check for result overflow
          if ( FSR0L == ( &_DpaMessage.Response.PData[sizeof( _DpaMessage.Response.PData )] & 0xff ) )
            break;

          // compute CRC, move pointers, increment index and reset byte mask
          UpdateOneWireCrc( *FSR0++ );
          FSR1++;
          rom_byte_number++;
          rom_byte_mask.0 = 1;
        }
      }
    } while ( !rom_byte_number.3 /* !=8 */ );  // loop until through all ROM bytes 0-7 

    // if the search was successful then
    if ( rom_byte_number.3 /* ==8 */ && OneWireCrc == 0 )
    {
      // search successful so set LastDiscrepancy
      LastDiscrepancy = last_zero;

      // check for last device
      if ( LastDiscrepancy == 0 )
        break;
    }
    else
      break;

    // FSR1 points to the last ROM ID read (optimized against FSR1 = FSR0 - 8)
    FSR1 = FSR0;
    FSR1 -= 8;
  }

  // return length of read bytes
  return FSR0L - ( &_DpaMessage.Response.PData[0] & 0xff );
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
