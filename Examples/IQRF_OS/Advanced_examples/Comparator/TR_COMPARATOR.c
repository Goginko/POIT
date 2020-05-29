// *********************************************************************
//                    Analog comparator controled via RF
//                             Demo example 
// *********************************************************************
//
// Intended for:
//   HW: - (DC)TR-72D + DK-EVAL-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   - this example receives RF commands and controls DAC output (pos. input of Comparator)
//   - use example E03-TR as a transmiter 
//	 - use IQRF IDE 4.47 or higher - Terminal (Use Numbers must be checked, see Help)
//   - import prepared macros file comparator_macros.iqrfmcr to the IQRF IDE
//
//   Commands to be sent from the transmiter: 'C'<DAC output>
//		<DAC output>: 0 ~ 0 V, ..., 16 ~ 1.5 V, ..., 31 ~ 2.9 V
//		For more exact calculation see MCU datasheet.
//		(e.g. "C#016" -> DAC output = 1.5 V)
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    TR_COMPARATOR.c
// Version: v2.05                                   Revision: 24/09/2017
//
// Revision history:
//   v2.05: 24/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//						Preamble quality test added.
//   v2.01: 23/02/2016  Order of _enableUserInterrupt = 1 and C2IE = 1 swapped.
//                      Tested for OS 3.08D.
//   v2.00: 01/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
#define	RX_FILTER				0

// *********************************************************************
void initComparator();
void userIntRoutine(void);

// *********************************************************************

void APPLICATION()
{
	setLEDR();
    waitDelay(50);                          // Startup indication
    stopLEDR();
    initComparator();
	setRFmode(_WPE | _RX_STD | _TX_STD);
	_checkRFcfg_PQT = 1;					// Enable preamble quality test
    toutRF = 1;                             // Wait Packed End active so the toutRF can be set to minimum
    _enableUserInterrupt = 1;
    C2IE = 1;						        // Interrupt from Comparator 2 enable
    
    while (1)
    {	
        if (checkRF(RX_FILTER))
        {
            if (RFRXpacket())               
            {
                if (bufferRF[0] == 'C')
                {
                    pulseLEDR();
                    DACCON1 = bufferRF[1];	// DAC output control
                }
            }
        }		
    }
}
// *********************************************************************

void initComparator(void)			// Comparator 2 is used with C1 pin as negative input (C12IN0),
{									//   and DAC output as positive input.
									//   Result is in the C2OUT bit.

	DACCON0 = 0b10000000;			// DAC on, Vdd, Vss
	DACCON1 = 16;					// Voltage output (0 - 31), 16 ~ 1,5 V
	
	TRISA.0 = 1;					// Pin C1 (RA0) as input
	ANSELA.0 = 1;					// Pin C1 (AN0) as analog input

	CM2CON0 = 0b10010100;			// Comparator on, output internal only, output inverted
	CM2CON1 = 0b11010000;			// Interrupt on pos. and neg. edge,
									// +input from DAC voltage reference
									// -input from RA0 pin
	C2IF = 0;						// Clear interrupt flag
}
// ---------------------------------------------------------------------

// This function is invoked whenever any interrupt occurs (even OS)
// and the _enableUserInterrupt flag is set to 1.
// OS generates an interrupt in the period of system ticks.
// The user interrupt routine must start from address 0x3F00 (__USER_INTERRUPT macro). 
// The user interrupt routine must take less than 50 µs. 
// User interrupts must not be called in period shorter then 800 µs.
// IQRF OS functions except of setINDFx and getINDFx are not allowed.
// IQRF OS variables must not be used.
// Only global or local static user variables are allowed.
// For more information see IQRF OS User's Guide.

#pragma origin __USER_INTERRUPT
#pragma library 0
void userIntRoutine(void)
{					
    if (C2IF) 
	{
		C2IF = 0;					// Clear interrupt flag
									
		if (C2OUT) _LEDG = 1;		// LEDG is on if neg. input > pos. input
		else	   _LEDG = 0;
	}
} 
// *********************************************************************
