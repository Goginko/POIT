// *********************************************************************
//                             DEMO EXAMPLE
//                    RF receiver with authentication
// *********************************************************************
//
// Intended for:
//   HW: - (DC)TR-72D + CK-USB-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   This example shows how to make a receiver with simple authentication.
//   - The transceiver permanently attempts to receive RF packets.
//   - If any packet is received:
//     - LED flashes once
//     - Received data is stored in buffer INFO.
//     - The acknowledge is sent and the receiver is waiting for confirmation.
//     - If confirmed the received data from buffer INFO is sent via SPI.
//       This can be displayed e.g. with the IQRF IDE Terminal.
//   Simple random generator can be replaced by a sophisticated
//   one by the user.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    RX-AUTH.c
// Version: v2.05                                   Revision: 24/09/2017
//
// Revision history:
//   v2.05: 24/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//   v2.01: 23/02/2016  Tested for OS 3.08D.
//   v2.00: 02/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
uns8 random_counter, dlen_backup;

// *********************************************************************
void APPLICATION()
{
    enableSPI();
    moduleInfo();                       // Random_counter based on module ID
    random_counter = bufferINFO[0];

    while (1)
    {
        toutRF = 50;

        if (RFRXpacket())
        {
            pulseLEDR();                // LED indication - message received
            dlen_backup = DLEN;
            copyBufferRF2INFO();        // Backup of received messsage
                                        // The message must be confirmed before
                                        //   sending via SPI

                                        // Send acknowledge:
                                        //  1B = 'A', 2B = random value x
            bufferRF[0] = 'A';
            bufferRF[1] = random_counter;
            PIN = 0;
            DLEN = 2;
            waitMS(30);
            RFTXpacket();
            toutRF = 10;

            if (RFRXpacket())           // Wait for 2B of confirmation:
                                        //  1B = 'C', 2B = x ^ 0x5F
            {
                if ((DLEN == 2) && (bufferRF[0] == 'C'))
                {
                    if (bufferRF[1] == (random_counter ^ 0x5F))
                    {                   // The message was confirmed
                        pulseLEDG();
                        copyBufferINFO2COM();
                        startSPI(dlen_backup);
                    }
                }
            }
        }

        ++random_counter;
    }
}
// *********************************************************************
