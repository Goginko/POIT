// *********************************************************************
// *                       E10 - RFMODE-RX                             *
// *                         RF receiver                               *
// *********************************************************************
// RF modes using.
// For more information about RF modes see IQRF OS Ref. Guide and User´s Guide.
//
// Example E10 consists of 2 programs:
//   - RFMODE-TX
//   - RFMODE-RX
//
// Intended for:
//   HW: - (DC)TR-72D + DK-EVAL-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   This example shows how to use RF modes. It consists of 3 parts with
//   the same function but with different RX mode. Appropriate part can be
//   choosed using #define RX_MODE below. When the correct RF packet is
//   received the answer is sent back.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E10-RFMODE-RX.c
// Version: v2.05                                   Revision: 21/09/2018
//
// Revision history:
//   v2.05: 21/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//						Preamble quality test added.
//                      Improved communication reliability by putting a delay before RFTXpacket.
//   v2.01: 22/02/2016  Tested for OS 3.08D.
//   v2.00: 03/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
#define STD_MODE			           	// set STD_MODE or LP_MODE or XLP_MODE
#define	RX_FILTER				0
// *********************************************************************

#ifdef STD_MODE
void APPLICATION()
{
	pulseLEDG();
    setRFmode(_WPE | _RX_STD | _TX_STD);
	_checkRFcfg_PQT = 1;				// Enable preamble quality test
    toutRF = 1;                         // Wait Packed End active so the toutRF can be set to minimum

    while (1)
    {
		if (checkRF(RX_FILTER))
        {
            if (RFRXpacket())
            {
                if (bufferRF[0] != 'S')
                    continue;
                
                pulseLEDR();                
                bufferRF[1] = '_';
                bufferRF[2] = 'o';
                bufferRF[3] = 'k';
                PIN = 0;
                DLEN = 4;
                waitMS(5);              // Required min. delay before RFTXpacket.
                RFTXpacket();			// Send answer
            }
        }
	}
}

#elif defined LP_MODE
void APPLICATION()
{
	pulseLEDG();
	waitDelay(10);						// Wait for end of LED flash
	setRFmode(_WPE | _RX_LP | _TX_STD);	// LP RX mode - RFIC is periodicaly switched ON/OFF
	toutRF = 10;						// [cycle], 1 cycle is cca 47ms
    checkRF(RX_FILTER);                 // To set internal filtering for LP receiving

    while (1)
    {
		if (RFRXpacket())
		{
			if (bufferRF[0] != 'L')
				continue;
			
            pulseLEDR();
			waitDelay(5);				// Wait for end of LED flash
			bufferRF[1] = '_';
			bufferRF[2] = 'o';
			bufferRF[3] = 'k';
			PIN = 0;
			DLEN = 4;
			RFTXpacket();				// Send answer
		}
	}
}

#elif defined XLP_MODE
void APPLICATION()
{
	pulseLEDG();
	waitDelay(10);						// Wait for end of LED flash
	setRFmode(_WPE | _RX_XLP | _TX_STD);// XLP RX mode - RFIC is periodicaly switched ON/OFF
	toutRF = 10;						// [cycle], 1 cycle is cca 790ms
    checkRF(RX_FILTER);                 // To set internal filtering for XLP receiving

    while (1)
    {
		if (RFRXpacket())
		{
			if (bufferRF[0] != 'X')
				continue;
			
            pulseLEDR();
			waitDelay(5);				// Wait for end of LED flash
			bufferRF[1] = '_';
			bufferRF[2] = 'o';
			bufferRF[3] = 'k';
			PIN = 0;
			DLEN = 4;
			RFTXpacket();				// Send answer
		}
	}
}

#else
	#error RF mode is not selected.
#endif
// *********************************************************************
