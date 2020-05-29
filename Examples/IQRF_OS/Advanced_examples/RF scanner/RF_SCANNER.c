// *********************************************************************
// *                          RF scanner                               *
// *                         Demo example                              *
// *********************************************************************
//
// Intended for:
//   HW: - (DC)TR-72D + CK-USB-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   - This example scanes RF channels in range from MIN_CHANNEL to MAX_CHANNEL
//	   (see Channel map in IQRF OS User's Guide - Appendix 2)
//	 - RSSI values are stored in bufferCOM
//	 - Use IQRF IDE Terminal to watch RSSI values
//	   (Enable decimal data displaying in Terminal Log window to watch direct RSSI values)
//   - Minimum recommended distance between the scanner and tested TR module is few meters.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    RF_SCANNER.c
// Version: v2.06                                   Revision: 24/09/2017
//
// Revision history:
//   v2.06: 24/09/2018  Tested for OS 4.03D.
//   v2.05: 23/08/2017  Tested for OS 4.02D.
//   v2.03: 13/03/2017  Tested for OS 4.00D.
//						checkRF(90) used as optimized value for all RF bands (868, 916, 433 MHz)
//   v2.02: 27/09/2016  checkRF(100) used instead of fast checkRF.
//   v2.01: 23/02/2016  Fast checkRF used.
//                      Tested for OS 3.08D.
//   v2.00: 02/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
#define	MIN_CHANNEL				0
#define	MAX_CHANNEL				61

// *********************************************************************
void APPLICATION()
{
uns8 i, val;

	enableSPI();
    setLEDR();
    waitDelay(50);                          // Startup indication
    stopLEDR();
	i = MIN_CHANNEL;
    
	while (1)
	{
		setRFchannel(i);
        checkRF(90);               	// Parameter 90 is mandatory to be RSSI detection more accurate
		val = getRSSI();            // See getRSSI description in IQRF OS Reference Guide how to calculate value in dBm
		writeToRAM(bufferCOM + i, val);

		if (++i > MAX_CHANNEL)
		{
            startSPI(i);
			i = MIN_CHANNEL;
            while(getStatusSPI());  // Until the packet is picked-up
		}
	}
}

// *********************************************************************
