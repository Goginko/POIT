// *********************************************************************
// *                       E10 - RFMODE-TX                             *
// *                        RF transmiter                              *
// *********************************************************************
// RF modes using.
// For more information about RF modes see IQRF OS Ref. Guide and User´s Guide.
//
// Example E10 consists of 2 programs:
// - RFMODE-TX
// - RFMODE-RX
//
// Intended for:
//   HW: - (DC)TR-72D + CK-USB-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   This example shows how to use RF modes. It should be
//   used with IQRF IDE Terminal to send RF packet in different RF modes
//   using SPI command.
//   Commands:
//   - S: Send request in STD mode
//   - L: Send request in LP mode
//   - X: Send request in XLP mode
//	 An answer from RFMODE-RX is expected after the request is sent.
//   The RFMODE-RX must use appropriate RF mode.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E10-RFMODE-TX.c
// Version: v2.05                                   Revision: 21/09/2018
//
// Revision history:
//   v2.05: 21/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//						Preamble quality test added.
//   v2.01: 22/02/2016  Tested for OS 3.08D.
//   v2.00: 03/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
#define	RX_FILTER				0

// *********************************************************************
void APPLICATION()
{
	enableSPI();
	appInfo();                      		// Copy menu from EEPROM
    copyBufferINFO2COM();           		//   (from Appl. info) to bufferCOM
    startSPI(32);                   		// And then try to send it via SPI

    while (1)
    {
        setRFmode(_WPE | _RX_STD | _TX_STD);
		_checkRFcfg_PQT = 1;				// Enable preamble quality test
        toutRF = 1;                         // Wait Packed End active so the toutRF can be set to minimum
    
        if (checkRF(RX_FILTER))
        {
            if (RFRXpacket())
            {
                if (bufferRF[0] == bufferCOM[0])
				{
					copyBufferRF2COM();
					startSPI(DLEN);			// If received send it via SPI
					pulseLEDG();
					continue;
				}
            }
        }
   
		if (getStatusSPI())
			continue;
		
		if (_SPIRX) 						// Is anything received via SPI?
		{
			if (!_SPICRCok)
				goto restart_SPI;
				
			stopSPI();
													
			switch (bufferCOM[0]) 			// bufferCOM[0] - RF Mode
			{										
				case 'S':
					setRFmode(_TX_STD);		// Standard TX preamble
					break;
					
				case 'L':
					setRFmode(_TX_LP);		// Prolonged TX preamble according to LP mode
					break;
					
				case 'X':
					setRFmode(_TX_XLP);		// Prolonged TX preamble according to XLP mode
					break;
					
				default:
					goto restart_SPI;
			}	
			
			bufferRF[0] = bufferCOM[0];		// RF mode
			PIN = 0;
			DLEN = 1;
			pulseLEDR();
			RFTXpacket();					// Send request
			
restart_SPI:
			startSPI(0);
		}
	}
}

// *********************************************************************
#pragma packedCdataStrings 0
//                             00000000001111111111222222222233
#pragma cdata[__EEAPPINFO] =  "Commands: S L X                 "

// *********************************************************************

