// *********************************************************************
// *                           E02 - RX                                *
// *                          RF receiver                              *
// *********************************************************************
// Example of simple RF receiving.
//
// Intended for:
//   HW: - (DC)TR-72D + CK-USB-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   - This example shows how to make a receiver.
//   - The transceiver permanently attempts to receive RF packets.
//     If any packet is received:
//     - LED flashes once
//     - Received data is sent via SPI. This can be
//       displayed e.g. with the IQRF IDE Terminal (See IQRF IDE Help).
//   - The length (DLEN) of the packet actually received is
//     automatically set within the RFRXpacket() function.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E02-RX.c
// Version: v2.05                                   Revision: 21/09/2018
//
// Revision history:
//   v2.05: 21/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//   v2.01: 22/02/2016  Tested for OS 3.08D.
//   v2.00: 02/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
void APPLICATION()                  // Obligatory assigning
{
	enableSPI();                    // Enable SPI

    while (1)                       // Main cycle (perpetually repeated)
    {
        if (RFRXpacket())           // If anything was received
        {
			pulseLEDR();            //   LED indication
            copyBufferRF2COM();     //   Copy received RF data
                                    //     from bufferRF to bufferCOM
            startSPI(DLEN);         //     and send it via SPI
        }
    }
}

// *********************************************************************

