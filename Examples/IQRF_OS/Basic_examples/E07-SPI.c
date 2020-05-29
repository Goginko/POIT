// *********************************************************************
// *                           E07 - SPI                               *
// *                              SPI                                  *
// *********************************************************************
// SPI communication.
//
// Intended for:
//   HW: - (DC)TR-72D + CK-USB-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   This example shows how to use SPI related functions. It should be
//   used with IQRF IDE Terminal to test how SPI communication works.
//   SPI commands from Terminal are interpreted in TR module.
//   Commands:
//   - 1 to 3: Number of LED flashes
//   - i:      Send the "0123456789" string via SPI
//   - d:      Disable SPI peripheral
//   - s:      Stop SPI communication
//   SPI status of the module is displayed in the lower left corner
//   of the IQRF IDE main window.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E07-SPI.c
// Version: v2.05                                   Revision: 21/09/2018
//
// Revision history:
//   v2.05: 21/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//   v2.01: 22/02/2016  Tested for OS 3.08D.
//   v2.00: 02/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
void APPLICATION()
{
uns8 i;

    enableSPI();                    // Enable SPI Rx/Tx on background

    appInfo();                      // Copy menu from EEPROM
    copyBufferINFO2COM();           // (from Appl. info) to bufferCOM
    startSPI(32);                   // and then try to send it via SPI

    while (1)
    {
        if (getStatusSPI())         // Update SPIstatus, check SPI busy
                                    //   SPIpacketLength: data length
                                    //   param2: SPI status
            continue;               // Wait until message is picked up

        if (_SPIRX)                 // Anything received?
        {                           // Yes:
            if (!_SPICRCok)         //   CRCM matched?
            {                       //     No:
                startSPI(0);        //       Restart SPI communication
                continue;           //       and continue
            }

            waitDelay(35);          // Wait 350 ms before indication
            clrwdt();

            switch (bufferCOM[0])   // Check what is in bufferCOM
            {								
                case '3':                     	// 3 LED flashes
                            pulseLEDR();
                            waitDelay(35);
                case '2':                     	// 2 LED flashes
                            pulseLEDR();
                            waitDelay(35);
                case '1':                     	// 1 LED flash
                            pulseLEDR();
                            waitDelay(35);
                            startSPI(0);      	// Restart SPI
                            break;
							
                case 'i':
                            for (i=0; i<10; i++)
                                writeToRAM(bufferCOM+i, i+'0');
								
                            startSPI(10);
                            break;
							
                case 'd':
                case 's':
                            if (bufferCOM[0] == 'd')
                                disableSPI();   // Disables SPI

                            if (bufferCOM[0] == 's')
                                stopSPI();      // After stopSPI() SPI
                                                // returns "STOP" status

                            for (i=0; i<20; i++)
                            {
                                pulseLEDR();  	// 20 x LED flash
                                waitDelay(50);
                                clrwdt();
                            }
							
                            enableSPI();    	// Both stopped or disabled
												//   communication restarted
                            break;          	//   after 20 cycles.
							
                default:                    	// Unknown command:
                            stopLEDR();       	// Long flash LED (1 s)
                            _LEDR = 1;
                            waitDelay(100);
                            _LEDR = 0;
                            startSPI(0);      	// and restart SPI
                            break;
            }
        }
    }
}

// *********************************************************************
#pragma packedCdataStrings 0
//                             00000000001111111111222222222233
#pragma cdata[__EEAPPINFO] =  "Commands: 3 2 1 i d s           "

// *********************************************************************

