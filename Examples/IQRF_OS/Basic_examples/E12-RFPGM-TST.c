// *********************************************************************
// *                           RFPGM-TST                               *
// *                  Demo program to test RF upload                   *
// *********************************************************************
// An example for RF wireless upload.
// See IQRF OS User's guide, IQRF OS Reference guide and www.iqrf.org/rfpgm.
//
// Intended for:
//   HW: - (DC)TR-72D + DK-EVAL-04x development kit
//
//   OS: - v4.03D
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E12-RFPGM-TST.c
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
// First pressing the pushbutton for 0.7 s terminates RFPGM mode

void APPLICATION()
{
//	enableRFPGM();            			// Enable RFPGM during development
    disableRFPGM();         			// Disable RFPGM for final application
	
    waitDelay(100);

    while (1)
    {
		pulseLEDR();          			// Change LED color to demonstrate
//		pulseLEDG();        			//   successful RF upload.
        waitDelay(10);

        if (buttonPressed)    	  		// Initiate RFPGM mode by the pushbutton
        {                      
			// All user peripheral interrupts must be disabled here (if used)
			
            runRFPGM();
        }
    }
}

// *********************************************************************

