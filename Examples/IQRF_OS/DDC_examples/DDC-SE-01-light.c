// *********************************************************************
// *                           DDC-SE-01-light			               *
// *                            Demo example						   *
// *********************************************************************
// Using of Analog-to-Digital Converter with light sensor connected.
//
// Intended for:
//   HW: - (DC)TR-72D or DDC-76D-01 + DK-EVAL-04x development kit with DDC-SE-01 connected
//
//   OS: - v4.03D
//
// Description:
//   AD conversion is called in 2s period. Three light statuses are
//   recognized. RF packet is sent on every status change. It can be
//   received by example E03-TR.c and displayed in terminal of IQRF IDE.
//   For more info about ADC setting see PIC datasheet
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    DDC-SE-01-light.c
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
#define DAY   		1
#define TWILIGHT  	2
#define NIGHT 		3

#define limit1 		0x0295
#define limit2 		0x00F0

// *********************************************************************
void InitADC(void);
void ReadAnalogInput(void);

// *********************************************************************
uns16 ADC_result;
uns8 state, prew_state;

// *********************************************************************

void APPLICATION()
{
	TRISA.0 = 1;							// Light senzor pin C1 (AN0) as input
	InitADC();                               
	pulseLEDG();							// reset indication
	waitDelay(10);
	
	while(1)
	{	
		waitDelay(200);						// reading of AN input is every 2s
        ReadAnalogInput();

		if (ADC_result > limit1)
			state = NIGHT;
		else if (ADC_result < limit2)
			state = DAY;
		else
			state = TWILIGHT;
			
		if (state != prew_state)
		{
			prew_state = state;
		
			switch(state)
			{
				case DAY:
					bufferRF[0] = 'D';
					bufferRF[1] = 'A';
					bufferRF[2] = 'Y';
					DLEN = 3;
					break;
					
				case TWILIGHT:
					bufferRF[0] = 'T';
					bufferRF[1] = 'W';
					bufferRF[2] = 'I';
					bufferRF[3] = 'L';
					bufferRF[4] = 'I';
					bufferRF[5] = 'G';
					bufferRF[6] = 'H';
					bufferRF[7] = 'T';
					DLEN = 8;
					break;
					
				case NIGHT:
					bufferRF[0] = 'N';
					bufferRF[1] = 'I';
					bufferRF[2] = 'G';
					bufferRF[3] = 'H';
					bufferRF[4] = 'T';
					DLEN = 5;
					break;
			}
	
			pulseLEDR();			
			PIN  = 0;
			RFTXpacket();
		}
	}	
}
// *********************************************************************
// Note:
// If a packet received the A/D converter control registers are changed so InitADC
// must be called again (see RFRXpacket function side effects in IQRF Reference Guide).
// This is not case of this example because it does not call RFRXpacket function.

void InitADC(void)			// ADC init (for more info about registers see PIC datasheet)
{
	ANSELA.0 = 1;							// pin C1 (AN0) as analog input
	ADCON0  = 0b00000001;        			// ADC setting (AN0 channel)
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
