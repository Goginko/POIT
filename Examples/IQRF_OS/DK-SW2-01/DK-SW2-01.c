// *********************************************************************
// *                           DK-SW2-01                               *
// *                          Demo example                             *
// *********************************************************************
// Example for development kit DK-SW2-01.
//
// Intended for:
//   HW: - (DC)TR-72D + DK-SW2-01 development kit
//
//   OS: - v4.03D
//		
// Description:
//	 This example receives RF packets and controls relays and LEDs. 
//	 Packets can be sent from IQRF IDE Terminal using e.g. E03-TR.c
//   example. This example can also cooperate with DDC-IO-01.c example.
//
//   RF packet	Function
//	 "S1" 		switch RE1 and LED1 on
//	 "S2" 		switch RE1 and LED1 off
//	 "S3" 		switch RE2 and LED2 on
//	 "S4" 		switch RE2 and LED2 off
//
//	 The button SW1 controls RE1 and LED1.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    DK-SW2-01.c
// Version: v1.01                                   Revision: 24/09/2017
//
// Revision history:
//   v1.01: 24/09/2018  Tested for OS 4.03D.
//   v1.00: 26/06/2018  First release.
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// LED pins
// RC4 = C7 = LED1 (Green)
#define	LED1_LAT	LATC.4
#define	LED1_TRIS	TRISC.4
// RC5 = C8 = LED2 (Red)
#define	LED2_LAT	LATC.5 
#define	LED2_TRIS	TRISC.5
// Relay pins
// RC2 = C2 = RE1
#define	RE1_LAT		LATC.2
#define	RE1_TRIS	TRISC.2
// RA0 = C1 = RE2
#define	RE2_LAT		LATA.0 
#define	RE2_TRIS	TRISA.0
// Switch inputs
// RB4 = C5 = SW1
#define	SW1_PORT	PORTB.4
#define	SW1_TRIS	TRISB.4
// RC3 = C6 = SW2
#define	SW2_PORT	PORTC.3 
#define	SW2_TRIS	TRISC.3

#define	RX_FILTER		0

// *********************************************************************

void APPLICATION()
{
	// Set LED1, LED2, RE1 and RE2 as outputs with low level
	RE1_LAT = 0;
	RE1_TRIS = 0;
	RE2_LAT = 0;
	RE2_TRIS = 0;
	LED1_LAT = 0;
	LED1_TRIS = 0;
	LED2_LAT = 0;
	LED2_TRIS = 0;

	pulseLEDG();						// Reset indication
	waitDelay(10);
	_checkRFcfg_PQT = 1;				// Enable preamble quality test

	while (1)
	{
		if (checkRF(RX_FILTER))
		{
			if (RFRXpacket())
			{
				pulseLEDR();
				
				if (bufferRF[0] == 'S')		// Test on valid packet
				{
					switch (bufferRF[1])
					{
						case '1':			// Packet: S1
							RE1_LAT = 1;	// Switch RE1 and LED1 on
							LED1_LAT = 1;
							break;

						case '2':			// Packet: S2
							RE1_LAT = 0; // Switch RE1 and LED1 off
							LED1_LAT = 0;
							break;

						case '3':			// Packet: S3
							RE2_LAT = 1; // Switch RE2 and LED2 on
							LED2_LAT = 1;
							break;

						case '4':			// Packet: S4
							RE2_LAT = 0; // Switch RE2 and LED2 off
							LED2_LAT = 0;
							break;
					}
				}
			}
		}
		// SW1 controls RE1 and LED1
		if (!SW1_PORT)
		{
			RE1_LAT = 1;	// Switch RE1 and LED1 on
			LED1_LAT = 1;
		}
		
		if (!SW2_PORT)
		{
			RE1_LAT = 0;	// Switch RE1 and LED1 off
			LED1_LAT = 0;
		}
	}
}
