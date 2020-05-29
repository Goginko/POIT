// *********************************************************************
//                       PWM output controled via RF
//                             Demo example
// *********************************************************************
//
// Intended for:
//   HW: - (DC)TR-72D + DK-EVAL-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   - this example receives RF commands and controls PWM output on C5 pin.
//   - use example E03-TR as a transmiter 
//	 - use IQRF IDE 4.47 or higher - Terminal (Use Numbers must be checked, see Help)
//   - import prepared macros file PWM_macros.iqrfmcr to the IQRF IDE
//
//   Commands to be sent from the transmiter: 'P'<PWM duty cycle>
//		<PWM duty cycle>: 25 ~ 0.1ms, 50 ~0.2ms, ..., 225 ~ 0.9ms
//		(e.g. "P#050" -> PWM duty cycle = 0.2ms)
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    TR_PWM.c
// Version: v2.05                                   Revision: 24/09/2017
//
// Revision history:
//   v2.05: 24/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//						Preamble quality test added.
//   v2.01: 23/02/2016  Tested for OS 3.08D.
//   v2.00: 01/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
#define	RX_FILTER				0

// *********************************************************************
void initPWM();

// *********************************************************************

void APPLICATION()
{
	setWDTon_4s();						// WDT on, timeout 4 s (see IQRF-macros.h)
    setLEDR();
    waitDelay(50);                          // Startup indication
    stopLEDR();
 	initPWM();
    setRFmode(_WPE | _RX_STD | _TX_STD);
	_checkRFcfg_PQT = 1;				// Enable preamble quality test
    toutRF = 1;                         // Wait Packed End active so the toutRF can be set to minimum
	
    while (1)
    {	
        clrwdt();
        
		if (checkRF(RX_FILTER))
        {
            if (RFRXpacket())               
            {
                if (bufferRF[0] == 'P')
                {
                    pulseLEDR();
                    CCPR3L = bufferRF[1];
                }
            }
        }
    }
}
// *********************************************************************

void initPWM()							// CCP3 is used with C5 pin as PWM output
{
	TRISC.6 = 1;						// C5 as input 
	TRISA.5 = 1;
	TRISB.4 = 1;

	CCPTMRS0 = 0b00100000;   			// Set timer6 as CCP3 timer		
	CCP3CON = 0b00111100;				// CCP3 single mode
	CCPR3L = 25;						// PWM duty cycle: 0.1ms
	CCP3SEL = 0;						// Select RC6 as P3A function output
	
	T6CON = 0b00000010;					// Prescaler: 1:16
	PR6 = 250;							// PWM period: 1ms @ 16 MHz CPU clock

	TMR6IF = 0;
	TMR6ON = 1;
	while (!TMR6IF);					// To send a complete duty cycle and period on the first PWM
	TRISC.6 = 0;						// C5 as output
}
// *********************************************************************
