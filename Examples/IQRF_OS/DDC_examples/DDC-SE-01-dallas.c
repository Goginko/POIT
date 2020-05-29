// *********************************************************************
// *                          DDC-SE-01-dallas			       		   *
// *                            Demo example                           *
// *********************************************************************
// Temperature measurement with Dallas DS18B20 sensor via 1-Wire bus
//
// Intended for:
//   HW: - (DC)TR-72D or DDC-76D-01 + DK-EVAL-04x development kit with DDC-SE-01 connected
//
//   OS: - v4.03D
//
// Description:
//   Temperature is measured and sent via RF in 5 s period. It can be
//   received by example E03-TR.c and displayed in terminal of IQRF IDE.
//   Jumper DS18B20 must be connected (DDC-SE-01).
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    DDC-SE-01-dallas.c
// Version: v2.06                                   Revision: 21/09/2018
//
// Revision history:
//   v2.06: 21/09/2018  Tested for OS 4.03D.
//   v2.05: 22/08/2017  Tested for OS 4.02D.
//   v2.03: 03/03/2017  Tested for OS 4.00D.
//   v2.02: 23/02/2016  Tested for OS 3.08D.
//   v2.01: 17/09/2015  Tested with DDC-76D-01.
//   v2.00: 01/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
// If defined then CRC is checked
#define	USE_ONEWIRE_CRC

// Special temperature value to indicate a sensor error 
#define	DS18B20_ERROR_TEMPERATURE   0x8000

// Sensor connected to PORT C.3 (compatible with DDC-SE-01)
#define OneWire_TRIS    TRISC.3
#define OneWire_IO_IN   PORTC.3			// As input
#define OneWire_IO_OUT  LATC.3			// As output

// *********************************************************************
// Reads temperature from sensor
uns16 Ds18B20GetTemp();

// *********************************************************************
uns16 temperature, tmp;
uns8 i, tenths;
// *********************************************************************

void APPLICATION()
{
	setWDTon_4s();
    pulseLEDG();						// Reset indication
	waitDelay(10);

	while (1)
    {
        clrwdt();
        temperature = Ds18B20GetTemp();
        
		if (temperature != DS18B20_ERROR_TEMPERATURE)
                                        // This function takes about 630 ms
                                        //   and GIE is disabled during this time
        {
            bufferRF[0] = 'T';          // Prepare data for RF
            bufferRF[1] = '=';
            bufferRF[5] = '.';
            bufferRF[7] = 0xB0;			// '°'
            bufferRF[8] = 'C';

            if (temperature.high8 & 0x80)   // Is it negative?
            {                               // Yes
                temperature  = ~temperature;
                temperature++;
                bufferRF[2] = '-';
            }
            else
                bufferRF[2] = ' ';          // No

            tenths = temperature.low8 & 0x0F;   // Save bits 0 - 3 (tenths)
            temperature >>= 4;

            i = temperature.low8 / 10;
            bufferRF[3] = i + '0';          // Tens

            i = temperature.low8 % 10;
            bufferRF[4] = i + '0';          // Ones

            tmp = tenths * 625;
            i = tmp / 1000;
            tmp %= 1000;

            if ((tmp >= 500) && (i < 9))    // Rounding
                i++;

            bufferRF[6] = i +'0';           // Tenths
            DLEN = 9;
        }
        else
		{
            bufferRF[0] = 's';
            bufferRF[1] = 'e';
            bufferRF[2] = 'n';
            bufferRF[3] = 's';
            bufferRF[4] = 'o';
            bufferRF[5] = 'r';
            bufferRF[6] = '_';
            bufferRF[7] = 'e';
            bufferRF[8] = 'r';
            bufferRF[9] = 'r';
            DLEN = 10;
		}

        PIN = 0;
        RFTXpacket();                   // Send data via RF
        pulseLEDR();

        waitDelay(200);
        clrwdt();
        waitDelay(250);
    }
}

// *********************************************************************
// OneWire and Dallas 18B20 routines
// If there is an interrupt running that consumes a lot of MCU time (>450us once per 1ms) then use GIE=1/GIE=0 at whole OneWireResetAndCmd() for sure
// *********************************************************************

// *********************************************************************
void Delay5us( uns8 val @ W ) // Absolutely precise timing but val != 0
// *********************************************************************
{ 
  // MCU clock: 16 MHz
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
}
// *********************************************************************

#define	OneWireData0()	{ OneWire_TRIS = 0; }	  // 0.5us
#define OneWireData1()  { OneWire_TRIS = 1; }	  // 0.5us

// *********************************************************************
void OneWireWriteByte( uns8 byte )
// *********************************************************************
{
  uns8 bitLoop = 8;
  do
  {
	// Next sequence is time precision critical
	GIE = 0;

	OneWireData0();
	nop2();	        // 0.5 us
    nop2();	        // 1.0 us
	if ( byte & 1 )	// 1.75 us
	  OneWireData1();

	// End of time precision critical sequence
	GIE = 1;

	// 60us minimum in total, does not have to be precise
	Delay5us( ( 60 - 3 ) / 5 + 1 );
	OneWireData1();

	byte >>= 1;
  } while ( --bitLoop != 0 );
}

// *********************************************************************
uns8 OneWireReadByte()
// *********************************************************************
{
  uns8 result;
  uns8 bitLoop = 8;
  do
  {
	// Next sequence is time precision critical
	GIE = 0;

	OneWireData0();
	nop2();					// 0.5 us
    nop2();					// 1.0 us
	OneWireData1();			// 1.5 us
	Delay5us( 15 / 5 );		// 16.5 us

	Carry = 0;				// 16.75 us
	if ( OneWire_IO_IN )	// 17.25 us (condition must not modify Carry)
	  Carry = 1;

	// End of time precision critical sequence
	GIE = 1;				// must not modify Carry
	result = rr( result );

	// 60us minimum in total, does not have to be precise
	Delay5us( ( 60 - 20 ) / 5 + 1 );
  } while ( --bitLoop != 0 );

  return result;
}

// *********************************************************************
bit OneWireResetAndCmd( uns8 cmd )
// *********************************************************************
{
  // Setting the pin once to low is enough
  OneWire_IO_OUT = 0;
  // Reset pulse
  OneWireData0();
  Delay5us( 500 / 5 );
  // Reset pulse end
  OneWireData1();
  // Next sequence is time precision critical
  GIE = 0;
  // Wait for presence pulse
  Delay5us( 70 / 5 );
  // End of time precision critical sequence
  GIE = 1;
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
	OneWireWriteByte( 0xCC );
	// OneWire: Send command
	OneWireWriteByte( cmd );
	return TRUE;
  }
}

// *********************************************************************

#ifdef USE_ONEWIRE_CRC
// One Wire CRC accumulator
static uns8 OneWireCrc;

// *********************************************************************
void UpdateOneWireCrc( uns8 value @ W )
// *********************************************************************
{
  // Based on http://www.dattalo.com/technical/software/pic/crc_8bit.c
  OneWireCrc ^= W;
#pragma update_RP 0 /* OFF */	
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
	value ^= 0x23;	// 0x23 = 0x46 rotate right
  if ( OneWireCrc.6 )
	value ^= 0x46;	// 0x46 = 0x8C rotate right
  if ( OneWireCrc.7 )
	value ^= 0x8c;	// 0x8C is reverse polynomial representation (normal is 0x31)
  OneWireCrc = value;
#pragma update_RP 1 /* ON */
}

#endif

// *********************************************************************
uns16 Ds18B20GetTemp()
// *********************************************************************
{
  // OneWire: Convert T
  if ( OneWireResetAndCmd( 0x44 ) )
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
	if ( Carry && OneWireResetAndCmd( 0xbe ) )
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
// *********************************************************************

