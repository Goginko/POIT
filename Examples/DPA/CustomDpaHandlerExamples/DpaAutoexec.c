// *********************************************************************
//   DPA Autoexec example                                              *
// *********************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: DpaAutoexec.c,v $
// Version: $Revision: 1.22 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2019/12/11  Release for DPA 4.11
//   2017/03/13  Release for DPA 3.00
//   2015/08/05  Release for DPA 2.20
//   2014/04/30  Release for DPA 2.10
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Before uploading this example make sure:
// 1. Autoexec is enabled at HWP Configuration
// 2. Used peripherals are enabled at HWP Configuration
// 3. External EEPROM upload is selected at IQRF IDE. Please note external EEPROM cannot be uploaded using RFPGM.

// ! Important: 
// Updating Custom DPA Handler code using OTA LoadCode command does not allow writing external EEPROM content. 
// Therefore the update of the Autoexec is not possible. It is recommended to avoid Autoexec when OTA is used.

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Do not check for the Custom DPA Handler existence (we only wrote to the external EEPROM)
#define	NO_CUSTOM_DPA_HANDLER

// Uncomment in order to test Example #3 at [C] device (EEEPROM peripheral space starts at different address)
//#define COORDINATOR_CUSTOM_HANDLER

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

//############################################################################################

// Choose example number 1-4
#define	EXAMPLE	1

// -------------------------------------------------------------------------------------------
#if EXAMPLE	== 1
// Example #1: Switch on both LEDs
#pragma cdata[ __EEESTART + AUTOEXEC_EEEPROM_ADDR ] = \
/* 1. Green LED ON */ \
5, PNUM_LEDG, CMD_LED_SET_ON, 0xff, 0xff, \
/* 2. Red LED ON */ \
5, PNUM_LEDR, CMD_LED_SET_ON, 0xff, 0xff, \
/* 3. Delay 1s (LEDs will be visible at LP mode) */ \
8, PNUM_IO, CMD_IO_SET, 0xff, 0xff, PNUM_IO_DELAY, 0xe8, 0x03, \
/* 4. End of Autoexec  */ \
0
#endif

// -------------------------------------------------------------------------------------------
#if EXAMPLE	== 2
// Example #2: Write "Hello" to UART peripheral (previously automatically opened as it must be enabled in the configuration)
#pragma cdata[ __EEESTART + AUTOEXEC_EEEPROM_ADDR ] = \
/* 1. Write "Hello" to UART  */ \
11, PNUM_UART, CMD_UART_WRITE_READ, 0xff, 0xff, /* no read */ 0xff, 'H',  'e',  'l',  'l',  'o', \
/* 3. End of Autoexec  */ \
0
#endif

// -------------------------------------------------------------------------------------------
#if EXAMPLE	== 3
// Example #3: Self modifying Autoexec
#pragma cdata[ __EEESTART + AUTOEXEC_EEEPROM_ADDR ] = \
/* 1. Red LED ON, after the 1st boot it will be self-modified to the Green LED ON + end of Autoexec  */ \
5, PNUM_LEDR, CMD_LED_SET_ON, 0xff, 0xff, \
/* 2. Delay 1s (red LED will be visible at LP mode) */ \
8, PNUM_IO, CMD_IO_SET, 0xff, 0xff, PNUM_IO_DELAY, 0xe8, 0x03, \
/* 3. Write to the EEEPROM a modified Autoexec so next time a Green LED will be ON  */ \
7 + 16, PNUM_EEEPROM, CMD_EEEPROM_XWRITE, 0xff, 0xff, AUTOEXEC_EEEPROM_ADDR & 0xFF, AUTOEXEC_EEEPROM_ADDR >> 8, \
	/* new 1. Green LED ON   */ \
	5, PNUM_LEDG, CMD_LED_SET_ON, 0xff, 0xff, \
    /* new 2. Delay 1s (green LED will be visible at LP mode) */ \
    8, PNUM_IO, CMD_IO_SET, 0xff, 0xff, PNUM_IO_DELAY, 0xe8, 0x03, \
	/* new 3. End of autoexec  */ \
	0, \
/* 4. End of Autoexec  */ \
0
#endif

// -------------------------------------------------------------------------------------------
#if EXAMPLE	== 4
// Example #4: set green LED output pin and set LED on for 1s and then off for 1s, works also at LP mode
#pragma cdata[ __EEESTART + AUTOEXEC_EEEPROM_ADDR ] = \
/* 1. Green LED IO is output */ \
8,  PNUM_IO, CMD_IO_DIRECTION, 0xff, 0xff, /* PORTB.7 = LEDG = Output */ PNUM_IO_TRISB, 0x80, 0x00, \
/* 2. Green LED on for 1s, Green LED off for 1s */ \
17, PNUM_IO, CMD_IO_SET, 0xff, 0xff, \
  /* PORTB.7 = 1 => LEDG on */ PNUM_IO_PORTB, 0x80, 0x80, \
  /* delay 1s */ PNUM_IO_DELAY, 0xe8, 0x03, \
  /* PORTB.7 = 0 => LEDG off */ PNUM_IO_PORTB, 0x80, 0x00, \
  /* delay 1s */ PNUM_IO_DELAY, 0xe8, 0x03, \
/* 3. End of Autoexec  */ \
0
#endif

//############################################################################################
