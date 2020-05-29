// *********************************************************************
//                             DEMO EXAMPLE
//                             UART - LINK
// *********************************************************************
//
// Intended for:
//   HW: - (DC)TR-72D + DK-EVAL-04x
//       - (DC)TR-72D + DS-UART (DK-PGM-01 with CAB-SER-02)
//
//   OS: - v4.03D
//
// Description:
//   This example works as UART link. The received RF packet
//   is sent via UART and vice versa.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    UART_LINK.c
// Version: v2.05                                   Revision: 24/09/2017
//
// Revision history:
//   v2.05: 24/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//						Preamble quality test added.
//   v2.01: 23/02/2016  Tested for OS 3.08D.
//   v2.00: 02/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
											// set baudrate
#define BD_9600                        		// BD_19200, BD_9600, BD_4800
#define END_PACKET_DETECT   2               // End of packet detection [tick]
#define	RX_FILTER		    0

// *********************************************************************

void configure_IO(void);
void openUART(void);
void SendByteUART(uns8 tx_data);
void SendDataUART(uns8 data_len);
uns8 RxDataUART(void);
void UART_ErrorsRecovery(void);
// *********************************************************************

uns8 UART_dlen;
// *********************************************************************

void APPLICATION(void)
{
	openUART();                         // UART initialization
	setRFmode(_WPE | _RX_STD | _TX_STD);
	_checkRFcfg_PQT = 1;				// Enable preamble quality test
    toutRF = 1;                         // Wait Packed End active so the toutRF can be set to minimum

    while (1)
    {
 		if (checkRF(RX_FILTER))
        {
            if (RFRXpacket())
            {
                pulseLEDG();
                copyMemoryBlock(bufferRF, bufferCOM, DLEN);
                SendDataUART(DLEN);     // Send DLEN bytes from bufferCOM via UART
            }
        }

        UART_dlen = RxDataUART();       // Returns number of received data stored in bufferCOM

        if (UART_dlen)                  // If anything received via UART
        {
            pulseLEDR();
            DLEN = UART_dlen;
            copyMemoryBlock(bufferCOM, bufferRF, DLEN);
            PIN = 0;
            RFTXpacket();
        }
    }
}
//**********************************************************************

void configure_IO(void)
{
    TRISC.5 = 1;                        // Pin connected with RX
    TRISC.7 = 1;                        // RX input
    TRISC.6 = 0;                        // TX output
    BAUDCON = 0;                        // Baudrate control setup
}
//----------------------------------------------------------------------

void openUART(void)                     // Baudrate, 8N1
{
    configure_IO();
                                        // Baudrate @ 16MHz, high speed
#ifdef BD_19200
    SPBRGL = 51;
#endif
										
#ifdef BD_9600
    SPBRGL = 103;
#endif

#ifdef BD_4800
    SPBRGL = 207;
#endif

    TXSTA = 0b00100100;                 // Async UART, high speed, 8 bit, TX enabled
    RCSTA = 0b10010000;                 // Continuous receive, enable port, 8 bit
}
//----------------------------------------------------------------------

void SendByteUART(uns8 tx_data)         // Send a byte via UART
{
 	while (TXIF == 0); 					// Wait until bit PIR1.TXIF is cleared
    TXREG = tx_data;
}
//----------------------------------------------------------------------

void SendDataUART(uns8 data_len)        // Send data_len bytes from bufferCOM via UART
{
uns8 i;

    if (data_len > 64)                  // Max. 64B from bufferCOM
        data_len = 64;

    for (i = 0; i < data_len; i++)
        SendByteUART(readFromRAM(bufferCOM + i));
}
//----------------------------------------------------------------------

uns8 RxDataUART(void)                   // Returns number of received data stored in bufferCOM
{
uns8 i = 0;

    UART_ErrorsRecovery();
 
    if (RCIF)                    		// If anything received via UART
    {
        writeToRAM(bufferCOM, RCREG);
        i++;
        startDelay(END_PACKET_DETECT);  // Wait for next byte

        while (isDelay())
        {
			if (RCIF)            		// If anything received via UART
            {
                writeToRAM(bufferCOM + i, RCREG);
                i++;
                startDelay(END_PACKET_DETECT);  // Wait for next byte

                if (i >= 64)            // Max. 64B to bufferCOM
                    break;
            }
        }
    }

    return i;
}
//----------------------------------------------------------------------

void UART_ErrorsRecovery(void)          // OERR and FERR bits recovery - see PIC datasheet
{
    if (OERR)                           // UART overrun recovery
    {
        CREN = 0;
        CREN = 1;
    }

    if (FERR)                           // UART framing error recovery
        FERR = 0;
}
// *********************************************************************

