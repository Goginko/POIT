// *********************************************************************
// *                           DDC-SE-01-ptm			               *
// *                           Demo example	     					   *
// *********************************************************************
// Using of Analog-to-Digital Converter with potentiometer connected.
//
// Intended for:
//   HW: - (DC)TR-72D or DDC-76D-01 + DK-EVAL-04x development kit with DDC-SE-01 connected
//
//   OS: - v4.03D
//
// Description:
//   AD conversion is called in 1s period. RF packet is sent on every
//   change of result (out of tolerance). It can be received by example 
//   E03-TR.c and displayed in terminal of IQRF IDE.
//   For more info about ADC setting see PIC datasheet
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    DDC-SE-01-ptm.c
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
#define TOLERANCE 		2

// *********************************************************************
void InitADC(void);
void ReadAnalogInput(void);

// *********************************************************************
uns16 ADC_result, H_limit, L_limit, voltage, tmp;

// *********************************************************************

void APPLICATION()                  
{
	TRISA.5 = 1;							// potentiometer pins C5 (AN4) as input 
	TRISC.6 = 1;
	TRISB.4 = 1;
	InitADC();
	pulseLEDG();							// reset indication
	waitDelay(10);
	H_limit = 0x0FFF;
	L_limit = 0x0FFF;
	
	while(1)
	{	
		waitDelay(100);						// reading of AN input is every 1s
		ReadAnalogInput();
											
		if ((ADC_result > H_limit) || (ADC_result < L_limit))
		{									// AD result has changed more then +- tolerance
											//   from last measurement
											
			if (ADC_result < TOLERANCE)		// new limits setting
			{
				H_limit = TOLERANCE;
				L_limit = 0;	
			}
			else
			{								
				H_limit = ADC_result + TOLERANCE;
				L_limit = ADC_result - TOLERANCE;
			}
											// resolution is tenth volts because
			voltage = ADC_result * 30;		//   FREE complirer supports max. 16bit variables
			voltage /= 1024;
			tmp = voltage / 10;
			
			bufferRF[0] = tmp;
			bufferRF[0] += '0'; 
			bufferRF[1]  = '.'; 			
			bufferRF[2] = voltage % 10;
			bufferRF[2] += '0';
			bufferRF[3] = 'V';	

			pulseLEDR();
			PIN  = 0;
			DLEN = 4;
			RFTXpacket(); 
		}			
	}	
}
// *********************************************************************
// Note:
// If a packet received the A/D converter control registers are changed so InitADC
// must be called again (see RFRXpacket function side effects in IQRF Reference Guide).
// This is not case of this example because it does not call RFRXpacket function.

void InitADC(void)			// ADC init (for more info see PIC datasheet)
{
	ANSELA.5 = 1;							// pin C5 (AN4) as analog input 
	ADCON0  = 0b00010001;        			// ADC setting (AN4 channel)
	ADCON1  = 0b11010000;					// ADC result - right justified, Fosc/16
}
//----------------------------------------------------------------------

void ReadAnalogInput(void)
{
	GO = 1;                         		// start ADC
	while(GO);                      		// wait for ADC finish
	
	ADC_result.high8 = ADRESH & 0x03;		// 10b result is stored
	ADC_result.low8  = ADRESL;				//   in ADRESH and ADRESL
}
//----------------------------------------------------------------------
