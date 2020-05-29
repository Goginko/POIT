// *********************************************************************
// *                           DDC-IO-01                               *
// *                          Demo example                             *
// *********************************************************************
// Example for development kit DDC-IO-01.
//
// Intended for:
//   HW: - (DC)TR-72D or DDC-76D-01 + DK-EVAL-04x development kit with DDC-IO-01 connected
//
//   OS: - v4.03D
//		
// Description:
//   This example sends RF packet on every button press. Sent packets 
//	 can be shown in IQRF IDE Terminal using e.g. E03-TR.c example. 
//   This example can cooperate with DDC-RE-01 to control relays.
//   See example DDC-RE-01.c.
//
//   Button	RF packet	Function (DDC-RE-01)
//	 1		"S1x" 		switch RE1 on
//	 2		"S2x" 		switch RE1 off
//	 3		"S3x" 		switch RE2 on
//	 4		"S4x" 		switch RE2 off
//	 5		"S5x" 		---
//	 6		"S6x" 		---
//   (x: 0 - battery ok, 1 - battery error)
//
//   All DIP switches (DDC-IO-01) must be set as PULL DOWN.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    DDC-IO-01.c
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
#define SW1_TRIS 		    TRISC.5		// C8/SDO pin connected to the SW1
#define SW1 			    PORTC.5

#define SW2_TRIS 		    TRISC.4		// C7/SDI pin connected to the SW1
#define SW2 			    PORTC.4

#define SW3_TRIS 		    TRISC.3		// C6/SCK pin connected to the SW3
#define SW3 			    PORTC.3

#define SW4_TRIS 		    TRISA.5		// C5/SS pin connected to the SW4
#define SW4 			    PORTA.5

#define SW5_TRIS 		    TRISA.0		// C1/IO1 pin connected to the SW5
#define SW5 			    PORTA.0

#define SW6_TRIS 		    TRISC.2		// C2/IO2 pin connected to the SW6
#define SW6 			    PORTC.2

#define VOLTAGE_TRESHOLD	38          // cca 2.9V
					
// *********************************************************************

void APPLICATION()
{
	SW1_TRIS = 1;						// pins as inputs
	SW2_TRIS = 1;
	SW3_TRIS = 1;
	SW4_TRIS = 1;
	SW5_TRIS = 1;
	SW6_TRIS = 1;

	pulseLEDG();						// reset indication
	waitDelay(10);

	while (1)
	{
		if      (SW1) bufferRF[1] = '1';
		else if (SW2) bufferRF[1] = '2';	
		else if (SW3) bufferRF[1] = '3';
		else if (SW4) bufferRF[1] = '4';
		else if (SW5) bufferRF[1] = '5';
		else if (SW6) bufferRF[1] = '6';
		else 
			goto loop_end;				// no button was pressed

		if (getSupplyVoltage() < VOLTAGE_TRESHOLD)		
		{
			pulseLEDR();				// battery error
			bufferRF[2] = '1';
		}
		else
		{
			pulseLEDG();				// battery ok
			bufferRF[2] = '0';
		}
		
		bufferRF[0] = 'S';
		DLEN = 3;         				// setup RF packet length
		PIN = 0;
		RFTXpacket();      				// transmit the message
		
loop_end:		
		while (SW1 || SW2 || SW3 || SW4 || SW5 || SW6);	// all buttons must be released
		
		waitDelay(10);					// debounce time
	}
}
//----------------------------------------------------------------------

	