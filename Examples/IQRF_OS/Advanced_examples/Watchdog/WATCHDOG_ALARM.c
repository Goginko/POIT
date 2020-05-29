// *********************************************************************
//                          Watchdog - part 1
//                             Demo example
// *********************************************************************
//
// Intended for:
//   HW: - (DC)TR-72D + DK-EVAL-04x development kit with piezo buzzer connected
//
//   OS: - v4.03D
//
// Description:
//   This example receives RF packets. If no correct packet is received
//   for setup time the piezo buzzer is activated. To deactivate it the
//   button must be pressed.
//   See the second part watchdog_TX.c.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    WATCHDOG_ALARM.c
// Version: v2.05                                   Revision: 24/09/2017
//
// Revision history:
//   v2.05: 24/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//   v2.01: 23/02/2016  Tested for OS 3.08D.
//   v2.00: 02/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
#define BEEP_TRIS       TRISA.0         // C1 - pin connected to the piezo buzzer
#define BEEP_IO         LATA.0

#define THRESHOLD       3           	// limit for number of lost packets

#define HIGH_TONE       25          	// [5us] * 2 ~ cca 4kHz
#define LOW_TONE        50          	// [5us] * 2 ~ cca 2kHz
#define TONE_LENGTH     20          	// [ticks]

// *********************************************************************
// EEPROM
#pragma packedCdataStrings 0 //12345678901234567890123456789012
#pragma cdata[__EEAPPINFO] =  "watchdog"

// *********************************************************************
void waitDelay_5us(uns8 val);

// *********************************************************************
uns8 lost_packets, tone;

// *********************************************************************

void APPLICATION()
{
    BEEP_IO = 0;
    BEEP_TRIS = 0;                      // As output
    toutRF = 200;                       // 2s
    lost_packets = 0;
    pulsingLEDR();                      // 4x LED flash (boot indication)
    waitDelay(95);
    stopLEDR();
    appInfo();                          // Copy application data to buffer INFO

    while (1)
    {
        if (RFRXpacket())
        {
            if (compareBufferINFO2RF(8))    // Check for correct data
            {
                pulseLEDR();
                lost_packets = 0;       // OK - reset counter
            }
        }
        else
            lost_packets++;             // Packet not received

        if (lost_packets > THRESHOLD)   // THRESHOLD achieved?
        {                               // Yes
            tone = HIGH_TONE;
            startDelay(TONE_LENGTH);

            while (!buttonPressed)
            {                           // Until the button is pressed
                BEEP_IO ^= 1;
                waitDelay_5us(tone);

                if (!isDelay())
                {                       // Change tone
                    if (tone == LOW_TONE) tone = HIGH_TONE;
                    else                  tone = LOW_TONE;

                    startDelay(TONE_LENGTH);
                }
            }

            BEEP_IO = 0;
            lost_packets = 0;
        }
    }
}

// *********************************************************************
void waitDelay_5us( uns8 val @ W ) // Absolutely precise timing but val != 0
// *********************************************************************
// 16 MHz MCU CLK
{ // + 0.75us ( W=val, Call )
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

