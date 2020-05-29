// *********************************************************************
//                    D/A converter controled via RF
//                            Demo example 
// *********************************************************************
//
// Intended for:
//   HW: - (DC)TR-76D and appropriate user HW
//
//   OS: - v4.03D
//
// Description:
//   - this example controls DAC output via received RF commands
//	 - pin Q10 (RA2) is used as DAC output. 
//     OS uses this pin for red LED control, so do not use OS LEDR functions.
//   - use example E03-TR as a transmiter 
//	 - use IQRF IDE 4.47 or higher - Terminal (Use Numbers must be checked, see Help)
//   - import prepared macros file DAC_macros.iqrfmcr to the IQRF IDE
//
//   Commands to be sent from the transmiter: 'D'<DAC output>
//		<DAC output>: 0 ~ 0 V, ..., 16 ~ 1.5 V, ..., 31 ~ 2.9 V
//		For more exact calculation see MCU datasheet.
//		(e.g. "D#016" -> DAC output = 1.5 V)
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    TR_DAC.c
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
void initDAC();

// *********************************************************************

void APPLICATION()
{
	setLEDR();
    waitDelay(50);                          // Startup indication
    stopLEDR();
    initDAC();
    setRFmode(_WPE | _RX_STD | _TX_STD);
	_checkRFcfg_PQT = 1;					// Enable preamble quality test
    toutRF = 1;                             // Wait Packed End active so the toutRF can be set to minimum
	
    while (1)
    {	
        if (checkRF(RX_FILTER))
        {
            if (RFRXpacket())               
            {
                if (bufferRF[0] == 'D')
                {
                    pulseLEDG();
                    DACCON1 = bufferRF[1];	// DAC output control (pin RA2)
                }
            }
        }
    }
}
// *********************************************************************

void initDAC()				
{
	DACCON0 = 0b10100000;			// DAC on, output enabled, from VDD to VSS
	DACCON1 = 16;					// Default voltage output. 16 means 1.5 V.
}
// *********************************************************************
