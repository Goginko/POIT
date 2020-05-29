// *********************************************************************
// *                        E15-EXTENDED_FLASH                         *
// *                          Demo example                             *
// *********************************************************************
// Demonstration how to use an extended Flash memory for the user application.
//
// Intended for:
//   HW: - (DC)TR-72D + DK-EVAL-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   - Extended memory contains two memory codepages.
//     The CC5X compiler can not switch between these codepages automatically.
//     Therefore the code must be devided by user according to the example below.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E15-EXTENDED_FLASH.c
// Version: v2.05                                   Revision: 21/09/2018
//
// Revision history:
//   v2.05: 21/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//   v2.01: 22/02/2016  Tested for OS 3.08D.
//   v1.00: 07/09/2015  First release
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
void testLEDR(void);
void testLEDG(void);

// *********************************************************************
void APPLICATION()              // Code in standard Flash
{
    while(1)
    {
        testLEDR();
        waitMS(150);
        testLEDG();
        waitMS(150);
    }
}

#pragma origin __EXTENDED_FLASH
void testLEDR(void)             // Code in first page of extended Flash
{
    pulseLEDR();
}

#pragma origin __EXTENDED_FLASH_NEXT_PAGE
void testLEDG(void)             // Code in second page of extended Flash
{
    pulseLEDG();
}
// *********************************************************************
