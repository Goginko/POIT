// *********************************************************************
// *                           E00-START                               *
// *                       Training program                            *
// *********************************************************************
// The first training program (without RF communication).
//
// Intended for:
//   HW: - (DC)TR-72D + CK-USB-04x, DK-EVAL-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   - Demonstration how to use the IQRF platform, how to create
//     and modify a simple application program, compile and upload
//     it into the module and observe results.
//   - LED flashes 3x (that is all, the program only waits in
//     an infinite loop then)
//
//   - Compile and upload the program without any changes first.
//   - LED should flash 3x, for 50 ms with ~1 s period.
//   - You can restart running with the Reset Module button in the
//     IQRF IDE window.
//   - Then change values as you wish.
//     - Number of flashes is specified in the for cycle
//     - Change the blink parameters (in 10 ms ticks)
//       via the setOnPulsingLED and setOffPulsingLED functions.
//     - Do you know what will happen if you remove (or comment //)
//       the clrwdt() command? Try and watch!
//
// Default values (specified in OS, can be changed at any time):
//   pulsingOnLED:       50 ms      Time for LED ON after calling
//                                    pulsingLEDR() or pulseLEDR().
//   pulsingOffLED:     200 ms      Time for LED OFF after
//                                    calling pulsingLEDR().
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E00-START.c
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
void APPLICATION()
{
uns8  i;                    		// Work variable definition

	setWDTon_4s();					// WDT on, timeout 4 s (see IQRF-macros.h)

	for (i = 0; i < 3; i++)     	// 3x flash red LED
    {
        pulseLEDR();               	// One flash for defined time
									//   (setOnPulsingLED)
									//   on OS background
        waitDelay(100);            	// Wait 1s (100 ticks x 10 ms)
    }

    while (1)                      	// Infinite loop
    {
		clrwdt();                 	// Clear watchdog to avoid WDT reset
    }
}

// *********************************************************************
