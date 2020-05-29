// *********************************************************************
// *                         E05 - DELAYS                              *
// *                   IQRF timing and LED control                     *
// *********************************************************************
// Example of simple using time and LED related OS functions.
//
// Intended for:
//   HW: - (DC)TR-72D + CK-USB-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   - Timing parameters and default values setup during boot:
//     - IQRF "tick"       10 ms   Fixed, can not be changed.
//     - toutRF timeout:  500 ms   If nothing is received during this
//                                   time, RFRXpacket() is terminated.
//                                   Not used in this example.
//     - OnPulsingLED:     50 ms   Time for LEDs ON after calling
//                                   pulsingLEDx or pulseLEDx functions.
//     - OffPulsingLED:   200 ms   Time for LED OFF after calling
//                                   pulsingLEDx functions.
//
//   - Timing is primarily based on IQRF ticks
//     (10 ms intervals generated on OS background).
//   - The waitDelay() function waits specified number of ticks
//     (on OS foreground).
//   - The startDelay() function counts specified number of ticks
//     (on OS background) and gives the result (in progress/finished)
//     in the isDelay() bit value.
//   - Ticks are counted on OS background even by the Capture
//     functions. startCapture() resets the counter, captureTicks()
//     stores number of ticks from the last startCapture() to param3
//     and number of ticks from the last captureTicks() to param4.
//   - Specified delay functions should not be used when other specified
//     OS functions. See IQRF OS Reference guide, side effects.
//   - If combined with waitMS(), actual delays in ticks can be slightly
//     different from exact values due to latency and rounding.
//
//   - LED can be controlled directly - LEDR = 1/0 or by OS functions
//     pulseLEDR()/stopLEDR(). Take care of pin direction (TRIS) in
//     case of direct control.
//   - pulsingLEDR() can be used for blinking on OS background
//     until this is stopped e.g. with the stopLEDR() function.
//   - Similar functions are available also for green LED
//
//   - The program operates as follows:
//     - reset the Capture counter
//     - Delay 1.5s
//     - 5 x LED flash (50ms on, 950ms off)
//     - Delay 1.5s
//     - Blinking on background for 0.9s - 50ms on, 200ms off
//       (4 flashes)
//     - Delay 1s
//     - Captured ticks copied to param3
//     - 3 x LED flash (0.5s on, 0.5s off)
//     - Delay 1s
//     - Delay 10ms
//     - Captured ticks copied to param3 and param4 and then to x and y.
//     - Captured ticks can be watched using the IQRF IDE Debug
//       (see IQRF IDE Help).
//   - Expected captured times stated in comments (the "should be"
//     values) are for guidance only (not respecting MCU overheads).
//
//   - The watchdog is repeatedly cleared. If omitted, the module is
//     regularly reset from the watchdog overflow and the program can be
//     untimely aborted and re-initialized. You can try to comment
//     the clrwdt() commands or shorten the WDT period and see results.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E05-DELAYS.c
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
void APPLICATION()
{
uns16 m, n;                  	// Variables to store time information.
                                // They are 2B each, from the first user location,
								// low significant byte first.
                                // (see the RAM map in the OS manual)

uns8 i;                  		// Work variable i. It is stored
                                // just next to y.

                                // You can watch the values in IQRF IDE Debug.
 
	setWDTon_4s();				// WDT on, timeout 4 s (see IQRF-macros.h)
	
START:
    startCapture();             // Resets counter of ticks
    waitDelay(150);             // Delay 1.5s separates possible
                                // programming from set of flashes
    clrwdt();

    for (i=0; i<5; i++)  		// 5x flash LED:
    {
        pulseLEDR();            //   One LED flash on OS background
                                //   for defined time
                                //   (setOnPulsingLED, 50 ms default).
        waitDelay(100);         //   Wait 1s (100 ticks = 100 x 10ms).
                                //   Waiting on OS foregroung starts
                                //   immediately when LED is on.
        clrwdt();
    }

    waitDelay(150);             // Delay 1.5s separates 1st and 2nd set
                                // of flashes.
    clrwdt();

    pulsingLEDR();              // Start LED flashing based on
                                // pulsingOnLED/pulsingOffLED times.
    startDelay(90);             // Start delay counting 0.9s
                                // on OS background.
                                // (3x whole cycle + part of 4th cycle)
    while (isDelay())           // Wait until started delay is over.
        clrwdt();               //   Clears watchdog while waiting
		
    stopLEDR();                 // Stops LED activity (4 flashes).

    waitDelay(100);             // Delay 1s
    clrwdt();

    captureTicks();             // Captured ticks will be available
                                // in param3.

//................................ param3 should be 0x3DE  ~9.9s
//                              1.5 + 5x(0.05+0.95) + 1.5 + 0.9 + 1 = 9.9

                                // Other functions:
    for (i=0; i<3; i++)   		// Repeat 3x:
    {
        _LEDR = 1;              //   Switch LEDR on
        waitDelay(50);          //   Delay 0.5s
        _LEDR = 0;              //   Switch LEDR off
        waitDelay(50);          //   Delay 0.5s
        clrwdt();               //
    }

    waitDelay(100);             // Delay 1s (100 x 10ms)
    waitMS(12);                 // Delay 12ms
    clrwdt();

    captureTicks();             // Captured ticks from the last
                                // startCapture() will be available
                                // in param3.
                                // Captured ticks from the last
                                // captureTicks() will be available
                                // in param4.

//................................ param4 should be 0x191  ~4.01 s
//................................ param3 should be 0x56F ~13.91 s
//                                 3 + 1 + 0.01 = 4.01
//                                 9.9 + 4.01 = 13.91

    i = 0;                     	// Not necessary (keeping tidy only)
    m = param3;                 // Store captured ticks for future use
    n = param4;                 // Store captured ticks for future use
                                // param3/4 are now available
                                // for free usage again.
    breakpoint(0);              // To check capture result in
                                // variables m and n.
    goto START;                 // Repeat the whole program again
}

// *********************************************************************

