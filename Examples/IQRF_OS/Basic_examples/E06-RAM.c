// *********************************************************************
// *                           E06 - RAM                               *
// *                              RAM                                  *
// *********************************************************************
// Usage of RAM and buffers.
//
// Intended for:
//   HW: - (DC)TR-72D + CK-USB-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   This example shows how to use RAM and buffers related functions.
//   The breakpoint(x) macro stops program execution for checking.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E06-RAM.c
// Version: v2.05                                   Revision: 21/09/2018
//
// Revision history:
//   v2.05: 21/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//                      getINDFx function removed.
//   v2.01: 22/02/2016  Tested for OS 3.08D.
//   v2.00: 03/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
void APPLICATION()
{
uns8 i;									// Work variable i. It is stored
										// from the first user location
										// (see the RAM map in the OS manual
										// You can watch the value in IQRF
										// IDE Debug.

    for (i=0; i<40; i++)        		// Writes 40 bytes to bufferRF
//      bufferRF[i] = i;                // Not allowed
                                        // Compiler uses INDFx in such cases
    writeToRAM(bufferRF+i, i);  		// Correct
	
    breakpoint(0);

    copyBufferRF2INFO();                // Copies bufferRF to bufferINFO
    breakpoint(1);

    copyBufferRF2COM();                 // Copies bufferRF to bufferCOM
    breakpoint(2);

    clearBufferINFO();                  // Clears bufferINFO
                                        //   (filled by zeroes)
    breakpoint(3);

    copyBufferINFO2RF();                // Copies bufferINFO to bufferRF
    breakpoint(4);

    copyBufferINFO2COM();               // Copies bufferINFO to bufferCOM
    breakpoint(5);

    bufferCOM[10] = 'A';                // You can use buffers as needed
    bufferINFO[11] = 11;
    bufferRF[0] = 0x55;

    i = readFromRAM(bufferCOM+10); 		// Shows usage of readFromRAM()
    breakpoint(6);

    copyBufferCOM2INFO();               // Copies bufferCOM to bufferINFO
    breakpoint(7);

    copyBufferCOM2RF();                	// Copies bufferCOM to bufferRF
    breakpoint(8);
										// Block memory copying
    copyMemoryBlock(bufferCOM+10, bufferINFO+1, 2);	// (from, to, bytes)
    breakpoint(9);

    appInfo();                         	// 32 B Application info copied
										//   from EEPROM to bufferINFO
    copyBufferINFO2RF();               	// the same copied to bufferRF
    bufferRF[10] = 0x01;               	// Position 10 changed
	
    if (!compareBufferINFO2RF(11))     	// Comparison of 11 B
										//   returns 1 if equal
										//   and 0 if not equal

    {                                  	// Check via IQRF IDE Debug:
        if (compareBufferINFO2RF(5))   	// bufferINFO and RF comparison:
        {								
            breakpoint(10);					// 11 B mismatch, 5 B match
        }
        else
        {
            breakpoint(11);					// 5 B mismatch
        }
    }
    else
    {
		breakpoint(12);						// 11 B match
    }
	
	clearBufferRF();					// Clears bufferRF
                                        //   (filled by zeroes)
	bufferAUX[0]  = 'A';
	bufferAUX[31] = 'B';
	swapBufferINFO();					// Content of bufferINFO and bufferAUX is swapped
    breakpoint(13);
	
	swapBufferINFO();					// Content of bufferINFO and bufferAUX is swapped back
    breakpoint(14);

	for (i=0; i<40; i++)				// Writes 40 bytes to bufferRF
		writeToRAM(bufferRF+i, i);
										// Block memory copying from bufferRF to bufferINFO
	FSR1 = bufferINFO + 5;				//   To
	
uns8 x;	
	for(i = 0; i < 8; i++)
	{
        x = bufferRF[i];                //   From
        setINDF1(x);
        FSR1++;
	}									// Result is the same as copyMemoryBlock(bufferRF, bufferINFO + 5, 8)
	
	breakpoint(15);
    while(1);
}

// *********************************************************************

