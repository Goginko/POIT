// *********************************************************************
// *                          SPI master                               *
// *                         Demo example                              *
// *********************************************************************
//
// Intended for:
//   HW: - (DC)TR-72D + DK-EVAL-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   - example shows how to use SPI peripheral of MCU as an SPI master
//	 - connect logic analyzer to the DK-EVAL-04x to check the SPI communication:
//		TR module pin	DK-EVAL-04x pin 	SPI
//      --------------------------------------- 
//		C8				1					SDO
//		C7				2					SDI
//		C6				3					SCK
//		C5				4					SS
//		C4				7					GND
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    SPI_MASTER.c
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
#define SS_PIN		PORTA.5

// *********************************************************************
void masterSPIinit(void);
uns8 sendSPIbyte(uns8 tx_byte);
void sendSPIdata(uns8 data_len);

// *********************************************************************
void APPLICATION()
{
	setWDTon_4s();
    pulseLEDG();
	masterSPIinit();
	
	while (1)
	{
		clrwdt();
        appInfo();				// Copy Application data from EEPROM to bufferINFO
		copyBufferINFO2COM();	//   and then copy it to bufferCOM
		sendSPIdata(15);		// Send/receive 15 B via SPI from/to bufferCOM	
		waitDelay(10);
	}
}
// *********************************************************************

void masterSPIinit(void)        // Initialize PINs to inputs and outputs
{
	TRISC.5 = 0;				// RC5 as output SDO (C8)
	TRISC.4 = 1;				// RC4 as input SDI (C7)
	TRISC.3 = 0;				// RC3 as output SCK (C6)
	TRISA.5 = 0;				// RA5 as output SS (C5)
    
    
    moduleInfo();

    if (bufferINFO[5].7 == 0)   // TR module with connected pins?
    {                           // Yes
        TRISC.6 = 1;			// RC6 as input (connected to RA5 in parallel)
        TRISB.4 = 1;			// RB4 as input (connected to RA5 in parallel)
        TRISC.7 = 1;			// RC7 as input (connected to RC5 in parallel)
    }
                                  
	SS_PIN = 1;					// Set idle level of SS
    
    // Setup SPI
    // SSPSTAT
    //  Transmit occurs on SCK rising edge
    CKE = 1;
    // SSPCON1: (SSPCON1 cannot be accessed directly due to OS restriction)
    //  SPI enabled
    //  Idle state for clock is a low level
    //  CLK = 250kHz @ 16 MHz CPU clock
    writeToRAM( &SSPCON1, 0b0.0.1.0.0010 );
}
// *********************************************************************

// Send/receive data_len bytes via SPI from/to bufferCOM
void sendSPIdata(uns8 data_len)	
{
uns8 i, val;

    SS_PIN = 0;					// SS - active
	
    for (i = 0; i < data_len; i++)
	{
		val = sendSPIbyte(readFromRAM(bufferCOM + i));
		writeToRAM(bufferCOM + i, val);
	}
    
    SS_PIN = 1;					// SS - deactive
}
// *********************************************************************

// Send/receive 1 B via SPI
uns8 sendSPIbyte(uns8 tx_byte)
{
	SSPIF = 0;
	SSPBUF = tx_byte;
	while (!SSPIF);				// Wait until the byte is transmited/received
	return SSPBUF;				
}
// *********************************************************************

#pragma packedCdataStrings 0    // Store the string unpacked
                                //   (one byte in one location)
#pragma cdata[__EEAPPINFO] =  "SPI master demo"
                                // Stored to the Application Info array in EEPROM
// *********************************************************************

