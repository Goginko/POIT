// *********************************************************************
// *                           DDC-RE-01                               *
// *                          Demo example                             *
// *********************************************************************
// Example for development kit DDC-RE-01.
//
// Intended for:
//   HW: - (DC)TR-72D or DDC-76D-01 + DK-EVAL-04x development kit with DDC-RE-01 connected
//
//   OS: - v4.03D
//		
// Description:
//	 This example receives RF packets and controls relays. 
//	 Packets can be sent from IQRF IDE Terminal using e.g. E03-TR.c
//   example. This example can also cooperate with DDC-IO-01.c example.
//
//   RF packet	Function
//	 "S1" 		switch RE1 on
//	 "S2" 		switch RE1 off
//	 "S3" 		switch RE2 on
//	 "S4" 		switch RE2 off
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    DDC-RE-01.c
// Version: v2.06                                   Revision: 21/09/2018
//
// Revision history:
//   v2.06: 21/09/2018  Tested for OS 4.03D.
//   v2.05: 22/08/2017  Tested for OS 4.02D.
//   v2.03: 03/03/2017  Tested for OS 4.00D.
//						Preamble quality test added.
//   v2.02: 23/02/2016  Tested for OS 3.08D.
//   v2.01: 17/09/2015  Tested with DDC-76D-01.
//   v2.00: 01/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
#define RE1_TRIS 		TRISC.5			// C8/SDO pin connected to the RE1
#define RE1_IO 			LATC.5

#define RE2_TRIS 		TRISC.2			// C2/IO2 pin connected to the RE2
#define RE2_IO 			LATC.2

#define	RX_FILTER		0

// *********************************************************************

void APPLICATION()
{
	RE1_IO = 0;
	RE1_TRIS = 0;						// pin as output
	RE2_IO = 0;
	RE2_TRIS = 0;						// pin as output
	
	pulseLEDG();						// reset indication
	waitDelay(10);
	_checkRFcfg_PQT = 1;				// Enable preamble quality test

	while (1)
	{
		if (checkRF(RX_FILTER))
        {
            if (RFRXpacket())
            {
                pulseLEDR();
                
                if (bufferRF[0] == 'S')		// test on valid packet
                {
                    switch (bufferRF[1])
                    {
                        case '1':			// packet: S1
                            RE1_IO = 1;		// switch RE1 on
                            break;
                            
                        case '2':			// packet: S2
                            RE1_IO = 0;		// switch RE1 off
                            break;	
                            
                        case '3':			// packet: S3
                            RE2_IO = 1;		// switch RE2 on
                            break;
                            
                        case '4':			// packet: S4
                            RE2_IO = 0;		// switch RE2 off
                            break;	
                    }
                }
            }
        }
	}
}
//----------------------------------------------------------------------
