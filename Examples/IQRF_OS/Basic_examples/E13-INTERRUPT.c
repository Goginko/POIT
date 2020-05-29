// *********************************************************************
// *                        E13 - INTERRUPT                            *
// *                        Interrupt usage                            *
// *********************************************************************
// Simple example of interrupt using in application.
//
// Intended for:
//   HW: - (DC)TR-72D + DK-EVAL-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   - This example illustrates how to use the interrupt in user application.
//   - Interrupt on change (RB4 pin -> SW1 on DK-EVAL-04x) controls the red LED.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E13-INTERRUPT.c
// Version: v2.05                                   Revision: 21/09/2018
//
// Revision history:
//   v2.05: 21/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//   v2.01: 22/02/2016  IOCBF.4 = 0 used instead of writeToRAM(&IOCBF, IOCBF & 0xEF) for interrupt on pin change flag clearing.
//                      Order of _enableUserInterrupt = 1 and IOCIE = 1 swapped.
//                      Tested for OS 3.08D.
//   v2.00: 02/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
void userIntRoutine(void); 

// *********************************************************************
void APPLICATION()
{
    IOCBP.4 = 1;                		// Positive edge active (comment if not required)
    writeToRAM(&IOCBN, IOCBN | 0x10);   // Negative edge active (comment if not required)
                                        //   This bit (IOCBN.4) can not be accessed directly due to OS restriction
                                        
    IOCBF.4 = 0;                        // Clear interrupt on pin change flag

    _enableUserInterrupt = 1;           // User interrupt should be enabled sooner than any individual interrupt sources (e.g. TMR6IE = 1).
                                        //   Possible disabling should be done in reverse order.
    IOCIE = 1;                          // Interrupt on change enabled

    while (1);
}
// ---------------------------------------------------------------------

// This function is invoked whenever any interrupt occurs (even OS)
// and the _enableUserInterrupt flag is set to 1.
// OS generates an interrupt in the period of system ticks.
// The user interrupt routine must start from address 0x3F00 (__USER_INTERRUPT macro). 
// The user interrupt routine must take less than 50 µs. 
// User interrupts must not be called in period shorter then 800 µs.
// IQRF OS functions except of setINDFx and getINDFx are not allowed.
// IQRF OS variables must not be used.
// Only global or local static user variables are allowed.
// For more information see IQRF OS User's Guide.

#pragma origin __USER_INTERRUPT
#pragma library 0
void userIntRoutine(void)
{
    if (IOCBF.4)
    {
        IOCBF.4 = 0;                    // Clear interrupt on pin change flag
        toggleLEDR();
    }    
} 
// *********************************************************************
