// *********************************************************************
// *                          E09 - LINK                               *
// *                           RF link                                 *
// *********************************************************************
// Example of simple RF link checker.
//
// Intended for:
//   HW: - (DC)TR-7xD (types with LEDs on board) + CK-USB-04x, DK-EVAL-04x development kit (2 pcs.)
//
//   OS: - v4.03D
//
// Description:
//   - This program applied in two devices perpetually transmits
//     and attempts to receive RF packets. Green LED indicates transmitting
//     and red LED indicates every successful receipt.
//   - Thus, red LED flashing indicates RF connection.
//     This can be used for RF range test.
//   - For better synchronization RF timeouts are set to vary slightly
//     from part to part.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E09-LINK.c
// Version: v2.06                                   Revision: 21/09/2018
//
// Revision history:
//   v2.06: 21/09/2018  Tested for OS 4.03D.
//   v2.05: 07/02/2018  Tx power for TR-77Dx lowered to 6.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//   v2.01: 07/12/2015  Comments slightly modified.
//   v2.00: 02/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
#define	RX_FILTER				0
#define DELAY 			        40    		// Delay between pings

// *********************************************************************

void APPLICATION()
{
uns8 random, mid0, trType;

    T6CON = 0b0.0000.1.00;                  // Enable Timer 6
    moduleInfo();
    mid0 = bufferINFO[0];                   // Get lowest byte of Module ID
	trType = bufferINFO[5] & 0xF0;			// Get TR type (high nibble of bufferINFO[5])
	if (trType == 0xC0)						// The max. allowed TX power for TR-77Dx is level 6.
		setRFpower(6);
		
    setRFmode(_WPE | _RX_STD | _TX_STD);
    toutRF = 1;

    while (1)
    {
        PIN = 0;
        DLEN = 10;
        RFTXpacket();                       // Send RF packet
        pulseLEDG();

        random = (TMR6 ^ mid0) & 0x07;
        random += DELAY;                    // Variable timeout based on TMR6 reg and lowest MID byte.

        startDelay(random);
        
        while (isDelay())
        {
            if (checkRF(RX_FILTER))
            {
                if (RFRXpacket())           // RF packet received?
                {                           // Yes:
                    pulseLEDR();            //   Indicate by red LED
                    waitDelay(DELAY >> 2);
                    break;
                }
            }
        }
    } 
}

// *********************************************************************
