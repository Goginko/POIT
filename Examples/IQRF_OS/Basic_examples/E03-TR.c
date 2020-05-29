// *********************************************************************
// *                           E03 - TR                                *
// *                        RF transceiver                             *
// *********************************************************************
// Example of RF transmitter/receiver. Suitable for a PC link.
//
// Intended for:
//   HW: - (DC)TR-72D + CK-USB-04x development kit
//
//   OS: - v4.03D
//	
// Description:
//   - This program applied in two kits is a bidirectional RF-SPI interface.
//   - Received RF data is sent via SPI and vice versa. For sending and
//     receiving SPI data e.g. the IQRF IDE Terminal can be used
//     (See IQRF IDE Help).
//   - Received SPI data is sent via RF.
//   - PIN should be updated in case of bidirectional RF communication
//     before every RF transmission.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E03-TR.c
// Version: v2.05                                   Revision: 21/09/2018
//
// Revision history:
//   v2.05: 21/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//						Preamble quality test added.
//   v2.01: 22/02/2016  Tested for OS 3.08D.
//   v2.00: 02/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
#define	RX_FILTER				0

// *********************************************************************
void APPLICATION()
{
	enableSPI();                     
	setRFmode(_WPE | _RX_STD | _TX_STD);
	_checkRFcfg_PQT = 1;			// Enable preamble quality test
    toutRF = 1;                     // Wait Packed End active so the toutRF can be set to minimum
   
    while (1)                       // Main cycle (perpetually repeated)
    {
		if (checkRF(RX_FILTER))		// RF signal detection (takes cca 1ms)
		{
			if (RFRXpacket())       // If anything was received
			{
				pulseLEDR();        // LED indication
				copyBufferRF2COM(); // Copy received RF data from bufferRF to bufferCOM
				startSPI(DLEN);     //  and send it via SPI
			}
		}
		
		if (getStatusSPI())         // Update SPIstatus, check SPI busy
                                    // SPIpacketLength: data length
             continue;              // Wait until message is picked up
			
		if (_SPIRX)                 // Anything received?
        {                           // Yes:
            if (_SPICRCok)          // CRCM matched?
            {                       // Yes:
 				DLEN = SPIpacketLength;
				copyBufferCOM2RF();	// Copy received SPI data from bufferCOM to bufferRF
				PIN = 0;
				RFTXpacket();
				pulseLEDG();		// LED indication
			}

			startSPI(0);            // Restart SPI communication
		}
    }
}

// *********************************************************************
