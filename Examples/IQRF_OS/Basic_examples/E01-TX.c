// *********************************************************************
// *                           E01 - TX                                *
// *                        RF transmitter                             *
// *********************************************************************
// Example of simple RF transmission.
//
// Intended for:
//   HW: - (DC)TR-72D + DK-EVAL-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   - This example shows how to make a simple transmitter. It can be
//     tested with the E02-RX example.
//   - RF message is sent (and red LED flashes once) whenever the
//     pushbutton on the kit is pressed.
//   - Content of the messages = Application data.
//     (It is the 32 B array in EEPROM dedicated to user information
//     about the application - see the IQRF OS manual.
//     The "01234567890123456789012345678901" data array is used here.
//   - To lower power consumption the module utilizes the Sleep mode
//     and Wake-up on pin change for response to the button.
//     Wake-up on pin change is under user's control
//     and has to be enabled by the user when needed.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E01-TX.c
// Version: v2.05                                   Revision: 21/09/2018
//
// Revision history:
//   v2.05: 21/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//   v2.01: 22/02/2016  Tested for OS 3.08D.
//   v2.00: 03/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
void APPLICATION()             	// Obligatory assigning - see E00-START
{
	appInfo();                 	// Copy Application data
								//   from EEPROM to bufferINFO
    copyBufferINFO2RF();       	//   and then copy it to bufferRF

    while (1)                  	// Main cycle (perpetually repeated)
    {
 		sleepWOC();				// Sleep with wake-up on pin change (see IQRF-macros.h)
	
        if (buttonPressed)     	//   If button is pressed
        {
			pulseLEDR();       	//      LED indication
            PIN = 0;
            DLEN = 10;         	//      Setup RF packet length
								//        (only 10 B of Application
								//        info to be sent)
            RFTXpacket();      	//      Transmit the message
            waitDelay(25);     	//      and wait 250ms (25*10ms)
        }                      	//   End of "button pressed" cycle
    }                          	// End of main cycle
}

// *********************************************************************

#pragma packedCdataStrings 0    // Store the string unpacked
                                //   (one byte in one location)

//                             00000000001111111111222222222233
#pragma cdata[__EEAPPINFO] =  "01234567890123456789012345678901"
                                // 32 B to be stored to the
                                // Application Info array in EEPROM
// *********************************************************************

