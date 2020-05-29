// *********************************************************************
//                             DEMO EXAMPLE
//                    RF transmitter with authentication
// *********************************************************************
//
// Intended for:
//   HW: - (DC)TR-72D + DK-EVAL-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   This example shows how to make an RF transfer with simple
//   authentication. It can be tested with the RX-AUTH example.
//   - RF message is sent (and red LED flashes once) whenever the
//     pushbutton on the kit is pressed.
//   - Then the transmitter waits for acknowledge (100 ms).
//   - Received acknowledge is verified, confirmed back
//     and indicated by green LED.
//   Simple authentication algorithm can be replaced by a sophisticated
//   one by the user.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    TX-AUTH.c
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

void APPLICATION()
{
    while (1)
    {
        sleepWOC();				// Sleep with wake-up on pin change (see IQRF-macros.h)

        if (buttonPressed)
        {
            appInfo();          // Copy Application data from EEPROM to bufferINFO
            copyBufferINFO2RF();//   and then copy it to bufferRF
            pulseLEDR();        // LED indication - message sent
            PIN = 0;
            DLEN = 32;
            RFTXpacket();       //Transmit the message
            toutRF = 10;

            if (RFRXpacket())   // Wait for 2B acknowledge:
                                // 1-st byte = 'A', 2-nd byte = random value x
            {
                if ((DLEN == 2) && (bufferRF[0] == 'A'))  // verification
                {               // Send confirmation:
                                // 1-st byte = 'C', 2-nd byte = x ^ 0x5F
                    bufferRF[0] = 'C';
                    bufferRF[1] ^= 0x5F;;
                    PIN = 0;
                    waitMS(20);
                    RFTXpacket();
                    setLEDG();  // LED indication - confirmation sent
                }
            }

            waitDelay(25);
            stopLEDG();
        }
    }
}
// *********************************************************************

#pragma packedCdataStrings 0     // Store the string unpacked
                                 //   (one byte in one location)

//                             00000000001111111111222222222233
#pragma cdata[__EEAPPINFO] =  "01234567890123456789012345678901"
                                 // 32 B to be stored to the
                                 // Application Info array in EEPROM
// *********************************************************************
