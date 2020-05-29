/*********************************************************************
 *
 *         SPI Routines for communication with IQRF TR module
 *
 *********************************************************************
 * FileName:        IQRF_SPI.h
 * Company:         IQRF Tech s.r.o.
 * Web:             www.iqrf.org
 * 
 * Intended for:
 *      Microchip PIC microcontrollers
 *
 * Tested:
 *      Processor:  PIC24F
 *      Compiler:   Microchip C30 v3.12
 *
 * Software License Agreement
 *
 * Copyright (C) 2010 MICRORISC s.r.o.  All rights reserved.
 *
 * You should refer to the license agreement accompanying this 
 * Software for additional information regarding your rights and 
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT 
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT 
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL 
 * MICRORISC BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF 
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS 
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE 
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER 
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT 
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 09/03/2010  Original (Rev. 1.0)
 * 10/03/2010  First release (Rev. 1.1)
 * 22/07/2010  IQRF_SPI.c modified (Rev. 1.2)
 * 12/07/2012  TR-5xD supported (Rev. 1.3)
 * 14/05/2014  Recommended delay between SPI bytes modified (Rev. 1.4)
 * 04/11/2014  Comments slightly changed (Rev. 1.5)
 * 06/08/2015  Modified for TR-7xD modules with faster SPI (Rev. 1.6)
 * 02/10/2015  Timing modified (Rev. 1.7)
 ********************************************************************/
#ifndef __IQRF_SPI_H
#define __IQRF_SPI_H

#include <string.h>

// For more information about SPI implementation in the IQRF TR modules
// see the IQRF SPI User Guide (www.iqrf.org)

// Hardware setup - depends on your HW - this setting is for PIC24FJ256GA106
#include <p24Fxxxx.h>

#define TR_SS_TRIS			    (TRISDbits.TRISD5)	
#define TR_SS_IO			    (LATDbits.LATD5)

#define TR_SCK_TRIS		        (TRISDbits.TRISD4)
#define TR_SCK_IO		        (LATDbits.LATD4)

#define TR_SDO_TRIS		        (TRISDbits.TRISD3)
#define TR_SDO_IO		        (LATDbits.LATD3)

#define TR_SDI_TRIS		        (TRISDbits.TRISD2)
#define TR_SDI_IO		        (PORTDbits.RD2)

#define TR_SPI_IF			    (IFS2bits.SPI2IF)
#define TR_SSPBUF			    (SPI2BUF)
#define TR_SPICON1			    (SPI2CON1)
#define TR_SPICON1bits		    (SPI2CON1bits)
#define TR_SPICON2			    (SPI2CON2)
#define TR_SPISTAT			    (SPI2STAT)
#define TR_SPISTATbits		    (SPI2STATbits)


// Size of IQRF_SPI buffers
#define IQRF_SPI_BUFFER_SIZE    68      

// IQRF SPI task message
#define IQRF_SPI_NO_MESSAGE	    0	    
#define IQRF_SPI_WRITE_OK		1	    
#define IQRF_SPI_WRITE_ERR	    2		
#define IQRF_SPI_DATA_READY	    3
#define IQRF_SPI_READ_ERR	    4

// IQRF SPI task status
#define IQRF_NOT_BUSY   	    0	    
#define IQRF_BUSY_WRITING		1	    
#define IQRF_BUSY_READING	    2
#define IQRF_BUSY_NO_MESSAGE    3		

//******************************************************************************
//		 	TR module SPI status
//******************************************************************************
#define TR_STAT_HW_ERR      	0xFF	// SPI not active
#define TR_STAT_SPI_DISABLED	0x00	// SPI not active
#define TR_STAT_CRCM_OK			0x3F	// SPI not ready (full buffer, last CRCM ok)
#define TR_STAT_CRCM_ERR		0x3E	// SPI not ready (full buffer, last CRCM error)
#define TR_STAT_COM_MODE	    0x80	// SPI ready (communication mode)
#define TR_STAT_PROG_MODE	    0x81	// SPI ready (programming mode)
#define TR_STAT_DEBUG_MODE		0x82	// SPI ready (debugging mode)
#define TR_STAT_SLOW_MODE    	0x83  	// SPI not working on background - Slow Mode
#define TR_STAT_USER_STOP	    0x07    // User Stop - after stopSPI();

//******************************************************************************
//		 	TR module SPI commands
//******************************************************************************
#define TR_CMD_SPI_CHECK  		0x00    // to get status of TR module
#define TR_CMD_WR_RD 	 		0xF0	// write/read bufferCOM in TR module

//******************************************************************************
//		 	IQRF SPI functions
//******************************************************************************
void 			IQRF_SPI_Init(void);
unsigned char 	IQRF_SPI_Byte(unsigned char Tx_Byte);
void 			IQRF_SPI_Task(void);
unsigned char 	IQRF_SPI_GetCRCM(void);
unsigned char 	IQRF_SPI_CheckCRCS(void);
void            Start_IQRF_SPI(void);

//******************************************************************************
//		 	IQRF SPI macros
//******************************************************************************
#define Clear_IQRF_SPI_TxBuf()		memset(IQRF_SPI_TxBuf, 0, IQRF_SPI_BUFFER_SIZE)
#define Clear_IQRF_SPI_RxBuf()		memset(IQRF_SPI_RxBuf, 0, IQRF_SPI_BUFFER_SIZE)

#define Delay5us()                 nops // use your delay according used MCU clock

#endif
