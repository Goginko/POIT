// *********************************************************************
// *                         E04 - EEPROM                              *
// *                            EEPROM                                 *
// *********************************************************************
// Example of simple using the EEPROM and IQRF debugging.
//
// Intended for:
//   HW: - (DC)TR-72D + CK-USB-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   - This example demonstrates working with EEPROM (including the
//     Application Info array) using related OS functions.
//   - There is also an illustrative explanation of debugging with the
//     breakpoint(x) macro and with the IQRF IDE Debug here.
//   - The breakpoint(x) macro stops program executing and helpful
//     information is read from the module and displayed. For
//     orientation in debug points the parameter of breakpoint macro is used.
//     See IQRF IDE Help for details.
//   - EEPROM area 0xC0 – 0xFF is dedicated to OS. It is not accessible
//     by the user.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E04-EEPROM.c
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
#pragma packedCdataStrings 0    // Store the string unpacked (one byte in one location)

#pragma cdata[__EESTART + 20] = "abcdefghij"
                                // 10 B to be stored to the internal EEPROM
                                
// Lowest 2 kB of external EEPROM can be accessed also by the pragma directive.
//   See IQRF OS User's Guide, Data memory (serial EEPROM) chapter for details.
#pragma cdata[__EEESTART] = 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,\
                            16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
                                // 32 B to be stored to the external EEPROM
                                
// *********************************************************************
void APPLICATION()
{
uns8 i;

	if(!eeeReadData(0x000))     // External EEPROM test
        breakpoint(255);        // Read unsuccessful  (e.g. due to damaged or not populated memory device).
                                //   Additionally, the _eeeError flag is set.
    if(_eeeError)
    {
        pulsingLEDR();          // External EEPROM error
        while(1);
    }
    
    clearBufferINFO();
    
    for (i=0; i<20; i++)
        eeWriteByte(i, i); 		// Write 20 B to EEPROM,
                                //   address starting from 0,
                                //   incremental values 0x00 to 0x13.

    i = eeReadByte(0x05);    	// Read one byte from address 0x05

    breakpoint(0);              // breakpoint 0 (see IQRF-macros.h) - it should be:
                                //   EEPROM == 0x00, 0x01,..., 0x13
                                //   i == 5 (RAM[0x190])

    eeReadData(0, 10);          // Read 10 B from EEPROM (from 0)
                                //   and store to bufferINFO
    breakpoint(1);              // breakpoint 1 - it should be:
                                //   bufferINFO == 0x00, 0x01, ...0x09

    clearBufferINFO();          // Fill bufferINFO with zeros
    breakpoint(2);              // breakpoint 2 - it should be:
                                //   bufferINFO == zeros

    eeWriteData(3, 5);          // Save the first 5 B from bufferINFO
                                //   to EEPROM, starting at address 3
    breakpoint(3);              // breakpoint 3 - it should be:
                                //   EEPROM[3-7] == 0

    eeReadByte(3);              // Result saved in W register as well
    i = W;                   	//   i = W = EEPROM[3]
                                //   the same as i = eeReadByte(3);
    breakpoint(4);              // breakpoint 4 - it should be:
                                //   i == 0

    appInfo();                  // Application Info (EEPROM[160-191])
                                //   copied to bufferINFO
    breakpoint(5);              // breakpoint 5 - it should be:
                                // bufferINFO[0-31] == EEPROM[160-191]
//----------------------------------------------------------------------
							// External EEPROM
	clearBufferINFO();          // Fill bufferINFO with zeros
    memoryLimit = 16;
	eeeWriteData(0x400);		// Save the 16 B from bufferINFO
                                //   to ext. EEPROM, starting at address 0x400
    memoryLimit = 16;
	eeeReadData(0x400);			// Read 16 B from ext. EEPROM (from 0x400)
                                //   and store to bufferINFO
    breakpoint(6);              // breakpoint 6 - it should be:
								//   bufferINFO[0-15] == 0x00

	bufferINFO[0] = 'a';		// change bufferINFO
	bufferINFO[1] = 'b';
	bufferINFO[2] = 'c';
    memoryLimit = 16;
	eeeWriteData(0x400);		// Save the 16 B from bufferINFO
                                //   to ext. EEPROM, starting at address 0x400

	clearBufferINFO();          // Fill bufferINFO with zeros
    memoryLimit = 16;
	eeeReadData(0x400);			// Read 16 B from ext. EEPROM (from 0x400)
                                //   and store to bufferINFO
    breakpoint(7);              // breakpoint 7 - it should be:
								//   bufferINFO == 'a', 'b', 'c', 0x00 ...

    clearBufferINFO();          // Fill bufferINFO with zeros
    memoryLimit = 32;
	eeeReadData(0x000);			// Read 32 B from ext. EEPROM (from 0x000)
                                //   and store to bufferINFO
    breakpoint(8);              // breakpoint 8 - it should be:
								//   bufferINFO == 0, 1, 2, ...
	while(1);
}
// *********************************************************************

