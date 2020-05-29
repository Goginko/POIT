// *********************************************************************
//                           Watchdog - part 2
//                             Demo example
// *********************************************************************
//
// Intended for:
//   HW: - (DC)TR-72D + DK-EVAL-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   This example periodically sends an RF message and then goes to the
//   Sleep mode. The period equals the WDT timeout.
//   It cooperates with the first part of the Watchdog example - watchdog_alarm.c.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    WATCHDOG_TX.c
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
// EEPROM
#pragma packedCdataStrings 0 //12345678901234567890123456789012
#pragma cdata[__EEAPPINFO] =  "watchdog"

// *********************************************************************

void APPLICATION()
{
    setWDTon_4s();						// WDT on, timeout 4 s (see IQRF-macros.h)
	pulsingLEDR();                      // 4x LED flash (boot indication)
    waitDelay(95);
    stopLEDR();
	
    appInfo();                          // Copy Application data
    copyBufferINFO2RF();                //  to buffer RF

    while (1)
    {
        clrwdt();
        iqrfSleep();                    // Wakeup on WDT every cca 4s

        pulseLEDR();                    // LED indication
        DLEN = 8;                       // Setup RF packet length
                                        //  (only 8 B of Application
                                        //  info to be sent)
        RFTXpacket();                   // Transmit the message
        waitDelay(5);                   // To finish pulseLED
    }
}
// *********************************************************************

