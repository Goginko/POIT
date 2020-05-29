// *********************************************************************
//                      UART Link - interrupt driven
//                             Demo example
// *********************************************************************
//
// Intended for:
//   HW: - (DC)TR-72D + DK-EVAL-04x
//       - (DC)TR-72D + DS-UART (DK-PGM-01 with CAB-SER-02)
//
//   OS: - v4.03D
//
// Description:
//   This example works as UART (RS232) link. Received RF packet
//   is sent via UART and vice versa. UART receiving and transmiting use
//   MCU interrupt.
//   The bufferCOM is divided into two 32 B parts. First one works as a TX buffer
//   and second one as a circular RX buffer for UART data.
//   If the delay between RX UART data bytes is greater than 20 ms,
//   it it recognized as the end of the packet and then the packet is forwarded via RF.
//   This delay is measured by the Timer6. 
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    UART_LINK_INTR.c
// Version: v2.05                                   Revision: 24/09/2017
//
// Revision history:
//   v2.05: 24/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//						Preamble quality test added.
//   v2.01: 23/02/2016  Order of _enableUserInterrupt = x and RCIE = x swapped.
//                      Tested for OS 3.08D.
//   v2.00: 02/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
											// Set UART baudrate
#define BD_9600                        		// BD_19200, BD_9600, BD_4800
#define BUFFER_SIZE 	32					// DO NOT CHANGE! .. works only with 32
#define	RX_FILTER		0

// *********************************************************************

// UART function prototypes
void openUART(void);
void closeUART(void);
void restartUART(void);
uns8 getTxStatusUART(void);
void sendDataUART(uns8 length);
uns8 getRxStatusUART(void);
uns8 getRxByteUART(void);
void copyRxDataUART(uns16 to, uns8 length);
void errorsRecoveryUART(void);
void userIntRoutine(void); 
// *********************************************************************

// UART variables
uns8 TxData;  				// Number of bytes to transmit
uns8 TxBytePtr;				// Pointer to the byte to be send
uns8 RxData;  				// Number of received bytes
uns8 RxBufStartPtr;			// Circular buffer pointers
uns8 RxBufEndPtr; 		
bit RxBufOverflow;			// RX buffer overflow flag
// *********************************************************************

void APPLICATION()
{	
	openUART();
	setRFmode(_WPE | _RX_STD | _TX_STD);
	_checkRFcfg_PQT = 1;						// Enable preamble quality test
    toutRF = 1;                                 // Wait Packed End active so the toutRF can be set to minimum
   	
	while(1)
	{		
		if (TMR6IF)								
        {										// Gap between UART bytes was recognized
			TMR6ON = 0;							// Timer off
			TMR6IF = 0;
			
			if (RxBufOverflow)
			{
				restartUART();
				continue;
			}
			
			DLEN = getRxStatusUART();
            copyRxDataUART(bufferRF, DLEN);
			pulseLEDR();
            PIN = 0;
            RFTXpacket();
        }
		
        if (checkRF(RX_FILTER))
		{
			if (RFRXpacket())
			{
				if (DLEN > BUFFER_SIZE)			// TX UART buffer has 32 B
					DLEN = BUFFER_SIZE;
					
				while (getTxStatusUART());		// Wait until previous packet is sent
				pulseLEDG();
				copyMemoryBlock(bufferRF, bufferCOM, DLEN);
				sendDataUART(DLEN);     		// Send DLEN bytes from UART TX buffer
			}	
		}
	}    
}
// *************************************************************************

void openUART()							// Baudrate, 8N1
{   
	TRISC.5 = 1;                        // Pin connected with RX
    TRISC.7 = 1;                        // RX input
    TRISC.6 = 0;                        // TX output
    BAUDCON = 0;                        // Baudrate control setup
										// Baudrate @ 16MHz, high speed
#ifdef BD_19200
    SPBRGL = 51;
#elif defined BD_9600
    SPBRGL = 103;
#elif defined BD_4800
    SPBRGL = 207;
#else
	SPBRGL = 103;						// Default baudrate 9600 bd
#endif
	
	TXSTA = 0b00100100;                 // Async UART, high speed, 8 bit, TX enabled
    RCSTA = 0b10010000;                 // Continuous receiving, enable port, 8 bits
		
	RxData = 0;
	TxData = 0;
	TxBytePtr = 0;
	RxBufEndPtr = 0;
	RxBufStartPtr = 0;
	RxBufOverflow = 0;
										// Timer6 setting
	T6CON = 0x7B;						// Prescaler: x64, Postscaler: x16, Timer: off
	PR6 = 78;							// Period: cca 20ms for 16 MHz MCU clock
	TMR6IF = 0;
	
	_enableUserInterrupt = 1;
    RCIE = 1;							// UART RX interrupt enable
}
//-------------------------------------------------------------------------

void closeUART(void)
{					
	TXEN = 0;
	CREN = 0;
	TXIE = 0;
	RCIE = 0;
    _enableUserInterrupt = 0;
}
//-------------------------------------------------------------------------

void restartUART(void)
{
	CREN = 0;
	TXEN = 0;
	TXIE = 0;
	RCIE = 0;
    _enableUserInterrupt = 0;
	TxData = 0;
	RxData = 0;
	TxBytePtr = 0;
	RxBufEndPtr = 0;
	RxBufStartPtr = 0;
	RxBufOverflow = 0;
	CREN = 1;
	TXEN = 1;
    _enableUserInterrupt = 1;
	RCIE = 1;
}
//-----------------------------------------------------------------------------

void sendDataUART(uns8 length)			// Starts sending data in background
{ 		
	if (length > BUFFER_SIZE)
		length = BUFFER_SIZE;			// UART TX buffer has 32 B
	
	TxData = length;
	TxBytePtr = 0;
	TXIE = 1;							// UART TX interrupt enable
}
//---------------------------------------------------------------------------

uns8 getTxStatusUART(void)				// Returns number of bytes waiting to be send
{    			
	return TxData; 
}
//-------------------------------------------------------------------------

uns8 getRxStatusUART(void)				// Returns number of received bytes
{    			
	return RxData; 
}
//-------------------------------------------------------------------------

uns8 getRxByteUART()					// Get first unread byte from circular RX buffer
{					
uns8 tmp;

	if (RxData)
	{
		GIE=0;
		tmp = readFromRAM(bufferCOM + RxBufStartPtr + BUFFER_SIZE);
		RxBufStartPtr = (RxBufStartPtr + 1) % BUFFER_SIZE;
		--RxData;
		RxBufOverflow = 0;
		GIE=1;
		return tmp;
	}
	else
		return 0;
}
//-------------------------------------------------------------------------

void copyRxDataUART(uns16 to, uns8 length)
{
uns8 i, tmp;

	if (length > BUFFER_SIZE)
		length = BUFFER_SIZE;

	for (i = 0; i < length; i++)
	{
		tmp = getRxByteUART();
		writeToRAM(to + i, tmp);
	}
}
//-------------------------------------------------------------------------

void errorsRecoveryUART()				// OERR and FERR bits recovery - see PIC datasheet
{
    if (OERR)                           // Overrun recovery
    {	
        CREN = 0;
        CREN = 1;
    }

    if (FERR)                           // Framing error recovery
        FERR = 0;
}
// ***********************************************************************

// This function is invoked whenever any interrupt occurs (even OS)
// and the _enableUserInterrupt flag is set to 1.
// OS generates an interrupt in the period of system ticks.
// The user interrupt routine must start from address 0x3F00 (__USER_INTERRUPT macro). 
// The user interrupt routine must take less than 50 µs. 
// User interrupts must not be called in period shorter then 800 µs.
// IQRF OS functions except of setINDFx and getINDFx are not allowed.
// IQRF OS variables must not be used.
// Only global or local static user variables are allowed.
// For more information see IQRF OS User's Guide.

#pragma origin __USER_INTERRUPT
#pragma library 0
void userIntRoutine(void)			    // User interrupt service routine
{	
	if (RCIF)
	{									// Something was received via UART						
		errorsRecoveryUART();
		FSR0 = bufferCOM;
		FSR0 = FSR0 + (RxBufEndPtr + BUFFER_SIZE);
		setINDF0(RCREG);
		RxBufEndPtr = (RxBufEndPtr + 1) % BUFFER_SIZE;
		
		if (RxData < BUFFER_SIZE)
			++RxData;
		else							// Buffer overflow
		{		
			RxBufStartPtr = (RxBufStartPtr + 1) % BUFFER_SIZE;
			RxBufOverflow = 1;
		}
		
		TMR6 = 0;						// Timer reset
		TMR6ON = 1;						// Timer on
	}	
	
	if (TXIF && TxData)
	{									// There is still data to send and UART peripheral is free
		FSR0 = bufferCOM + TxBytePtr;
		TXREG = INDF0;					// Copies data from bufferCOM to TXREG
		++TxBytePtr;
		
		if (--TxData == 0)
			TXIE = 0;					// No more data to send, clear interrupt flag
	}
}
// *********************************************************************
