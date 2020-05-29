// *********************************************************************
//   DPA IO Setup example                                              *
// *********************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: DpaIoSetup.c,v $
// Version: $Revision: 1.12 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2017/03/13  Release for DPA 3.00
//   2015/08/05  Release for DPA 2.20
//   2014/04/30  Release for DPA 2.10
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Before uploading this example make sure:
// 1. IO Setup is enabled at HWP Configuration
// 2. Embedded IO peripheral is enabled at HWP Configuration
// 3. External EEPROM upload is selected at IQRF IDE. Please note external EEPROM cannot be uploaded using RFPGM.

// ! Important: 
// Updating Custom DPA Handler code using OTA LoadCode command does not allow writing external EEPROM content. 
// Therefore the update of the IO Setup is not possible. It is recommended to avoid IO Setup when OTA is used.

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Do not check for the Custom DPA Handler existence (we only wrote to the external EEPROM)
#define	NO_CUSTOM_DPA_HANDLER

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

//############################################################################################

// Example: set green LED output pin and set LED on for 1s and then off for 1s
#pragma cdata[ __EEESTART + IOSETUP_EEEPROM_ADDR ] = \
/* 1. Green LED IO is output */ \
8,  PNUM_IO, CMD_IO_DIRECTION, 0xff, 0xff, /* PORTB.7 = LEDG = Output */ PNUM_IO_TRISB, 0x80, 0x00, \
/* 2. Green LED on for 1s, Green LED off for 1s */ \
17, PNUM_IO, CMD_IO_SET, 0xff, 0xff, \
  /* PORTB.7 = 1 => LEDG on */ PNUM_IO_PORTB, 0x80, 0x80, \
  /* delay 1s */ PNUM_IO_DELAY, 0xe8, 0x03, \
  /* PORTB.7 = 0 => LEDG off */ PNUM_IO_PORTB, 0x80, 0x00, \
  /* delay 1s */ PNUM_IO_DELAY, 0xe8, 0x03, \
/* 3. End IO Setup  */ \
0

//############################################################################################
