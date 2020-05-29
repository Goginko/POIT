/*********************************************************************
 *
 *         SPI Routines for communication with IQRF TR module
 *
 *********************************************************************
 * FileName:        IQRF_SPI.c
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
 * 22/07/2010  IQRF_SPI_Init function modified (Rev. 1.2)
 * 12/07/2012  TR-5xD supported (Rev. 1.3)
 * 14/05/2014  Recommended delay between SPI bytes modified (Rev. 1.4)
 * 04/11/2014  Comments slightly changed (Rev. 1.5)
 * 06/08/2015  Modified for TR-7xD modules with faster SPI (Rev. 1.6)
 * 02/10/2015  Timing modified (Rev. 1.7)
********************************************************************/
#define __IQRF_SPI_C

#include "IQRF_SPI.h"

unsigned char IQRF_SPI_TxBuf[IQRF_SPI_BUFFER_SIZE];
unsigned char IQRF_SPI_RxBuf[IQRF_SPI_BUFFER_SIZE];
unsigned char IQRF_SPI_Busy, iq_DLEN, iq_PTYPE, iq_spistat, IQRF_SPI_Task_Message;
unsigned char tmp_cnt, iq_pac_len;
unsigned int Check_Status_Timer;


/*********************************************************************
 * Function:        void IQRF_SPI_Init(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initializes the SPI module to communicate to
 *                  IQRF TR module.
 *
 * Note:            This function is called only once during lifetime
 *                  of the application.  
 ********************************************************************/
void IQRF_SPI_Init(void)
{
	TR_SS_TRIS = 0;			            // output
	TR_SDO_TRIS = 0;                    // output					
 	TR_SCK_TRIS = 0;			        // output	
 	TR_SDI_TRIS = 1;			        // input
   	
	TR_SS_IO = 1;					    // idle state
	
	TR_SPISTATbits.SPIEN = 0;
	TR_SPICON1 = 0x013C;				// 250kHz, CKP=0, CKE=1, MASTER mode
    TR_SPICON2 = 0;
    TR_SPISTAT = 0;    					
    TR_SPISTATbits.SPIEN = 1;
    
    Check_Status_Timer = 0;
    IQRF_SPI_Busy = 0;
    IQRF_SPI_Task_Message = 0;
}

/*********************************************************************
 * Function:        unsigned char IQRF_SPI_Byte(unsigned char Tx_Byte)
 *
 * PreCondition:    IQRF_SPI_Init() is already called.
 *
 * Input:           Tx_Byte - byte to be sent
 *
 * Output:          Received byte
 *
 * Side Effects:    None
 *
 * Overview:        Sends/receives 1 byte via SPI
 *
 * Note:            It uses Delay5us() which must be defined 
 *                  according used MCU clock.  
 ********************************************************************/

unsigned char IQRF_SPI_Byte(unsigned char Tx_Byte)
{
	TR_SPI_IF = 0;
	Delay5us();
	
	TR_SSPBUF = Tx_Byte;
	while (!TR_SPI_IF);     	 		// wait until data is transmitted/received
		
	Delay5us();
	TR_SPI_IF = 0;						// must be if sharing with other SPI device
	return TR_SSPBUF;	
}

/*********************************************************************
 * Function:        unsigned char IQRF_SPI_GetCRCM(void)
 *
 * PreCondition:    Data must be prepared in IQRF_SPI_TxBuf and 
 *                  iq_DLEN must be valid.
 *
 * Input:           None
 *
 * Output:          CRCM result
 *
 * Side Effects:    None
 *
 * Overview:        Returns CRCM according IQRF SPI specification.
 *
 * Note:            None
 ********************************************************************/
unsigned char IQRF_SPI_GetCRCM(void)
{
 unsigned char i, crc_val;

	crc_val = 0x5F;

	for(i = 0; i < (iq_DLEN + 2); i++) 	
		crc_val ^= IQRF_SPI_TxBuf[i];
		
	return crc_val;
}

/*********************************************************************
 * Function:        unsigned char IQRF_SPI_CheckCRCS(void)
 *
 * PreCondition:    Data must be prepared in IQRF_SPI_RxBuf. 
 *                  iq_DLEN and iq_PTYPE must be valid.
 *
 * Input:           None
 *
 * Output:          CRCS result (0 - error, 1 - OK)
 *
 * Side Effects:    None
 *
 * Overview:        Returns CRCS result according IQRF SPI specification.
 *
 * Note:            None
 ********************************************************************/
unsigned char IQRF_SPI_CheckCRCS(void)
{
 unsigned char i, crc_val;

	crc_val = 0x5F ^ iq_PTYPE;
	
	for (i = 2; i < (iq_DLEN + 2); i++)
		crc_val ^= IQRF_SPI_RxBuf[i];
		
	if (IQRF_SPI_RxBuf[iq_DLEN + 2] == crc_val)
	    return 1;	                            // CRCS ok
	else                                     
	    return 0;                               // CRCS error
}

/*********************************************************************
 * Function:        void Start_IQRF_SPI(void)
 *
 * PreCondition:    Data must be prepared in IQRF_SPI_TxBuf. 
 *                  iq_DLEN and iq_PTYPE must be valid.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Prepares SPI packet to be sent and activates
 *                  IQRF_SPI_Task.
 *
 * Note:            None
 ********************************************************************/
void Start_IQRF_SPI(void)
{
    IQRF_SPI_TxBuf[iq_DLEN + 2] = IQRF_SPI_GetCRCM();
    
    iq_pac_len = iq_DLEN + 4;			        // length of whole packet (user data + CMD + PTYPE + CRCM + SPI_CHECK)     
	tmp_cnt = 0;					            // counter of sent SPI bytes
	
	if (iq_PTYPE & 0x80)		                // PTYPE bit7 test
    	IQRF_SPI_Busy = IQRF_BUSY_WRITING;		  
    else
        IQRF_SPI_Busy = IQRF_BUSY_READING;      
}

/*********************************************************************
 * Function:        void IQRF_SPI_Task(void)
 *
 * PreCondition:    IQRF_SPI_Init(), Start_IQRF_SPI() is already called. 
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        If IQRF_SPI_Busy is > 0 one byte from IQRF_SPI_TxBuf 
 *                  is sent/received. After whole packet is sent the 
 *                  IQRF_SPI_Task_Message is set.
 *                  
 *                  If IQRF_SPI_Busy is 0 the SPI_CHECK is sent and 
 *                  SPISTAT is received. If there is data in bufferCOM 
 *                  of TR module it is automatically read. Received data
 *                  is in IQRF_SPI_RxBuf.
 *
 * Note:            This function should be called from main loop every
 *                  150us. Calling of this function makes delay
 *                  between SPI bytes. 
 ********************************************************************/

void IQRF_SPI_Task(void)
{
unsigned char last_spistat;

    if (TR_SPISTATbits.SPIEN == 0)			    // SPI peripheral of MCU must be enabled
        return;                                 

	if (IQRF_SPI_Busy)						    // any SPI packet to read/write?
	{                                           // yes - read/write 1 SPI byte
		TR_SS_IO = 0;							// SS - active
		IQRF_SPI_RxBuf[tmp_cnt] = IQRF_SPI_Byte(IQRF_SPI_TxBuf[tmp_cnt]);
		
		tmp_cnt++;								// tmp_cnt cleared in Start_IQRF_SPI()
												
		if (tmp_cnt == iq_pac_len || tmp_cnt == IQRF_SPI_BUFFER_SIZE)					
		{										// packet sent or buffer overflow
    		TR_SS_IO = 1;						// SS - idle
			
		    if ((IQRF_SPI_RxBuf[iq_DLEN + 3] == TR_STAT_CRCM_OK) && IQRF_SPI_CheckCRCS())
			{									// CRC ok
				switch (IQRF_SPI_Busy)
				{
					case IQRF_BUSY_WRITING:		
						IQRF_SPI_Task_Message = IQRF_SPI_WRITE_OK;	
						break;
					
					case IQRF_BUSY_READING:		
						IQRF_SPI_Task_Message = IQRF_SPI_DATA_READY;	
						break;
				}
			}
			else
			{									// CRC error
				switch (IQRF_SPI_Busy)
				{
					case IQRF_BUSY_WRITING:		
						IQRF_SPI_Task_Message = IQRF_SPI_WRITE_ERR;	
						break;
					
					case IQRF_BUSY_READING:		
						IQRF_SPI_Task_Message = IQRF_SPI_READ_ERR;	
						break;
				}
			}
			
			IQRF_SPI_Busy = IQRF_NOT_BUSY;
			iq_spistat = 0;						// to recovery SPI status after packet communication
		}
	}
	else										// no SPI packet to read/write
	{											
		if (++Check_Status_Timer >= 66)         // 66 * every 150us called from main loop = cca 10ms
  		{									    // every cca 10ms - sends SPI_CHECK to get TR module status					
  	     	Check_Status_Timer = 0;
  	     	
			TR_SS_IO = 0;						// SS - active
  	     	last_spistat = IQRF_SPI_Byte(TR_CMD_SPI_CHECK);
			TR_SS_IO = 1;						// SS - idle
   	     	
  	     	if (iq_spistat != last_spistat)		// the TR module status must be 2x the same
  	     	{
		  	    iq_spistat = last_spistat;
	  	    	return;
	  	    }
  	     	
  	     	if ((iq_spistat & 0xC0) == 0x40)   	
           	{                                   // data ready in bufferCOM of TR module
	           	Clear_IQRF_SPI_TxBuf();
	           	
	           	if (iq_spistat == 0x40)
                    iq_DLEN = 64;               // SPI state 0x40 means 64B
                else    
	           	    iq_DLEN = iq_spistat & 0x3F; // clear bit 7,6 - rest is length (1 - 63B)
	           	
	           	iq_PTYPE = iq_DLEN;
	           	IQRF_SPI_TxBuf[0] = TR_CMD_WR_RD;
	           	IQRF_SPI_TxBuf[1] = iq_PTYPE;
	           	IQRF_SPI_TxBuf[iq_DLEN + 2] = IQRF_SPI_GetCRCM();
	           	Start_IQRF_SPI();               // prepare IQRF_SPI_Task to send packet
			}
  	  	}	
    }
}
//---------------------------------------------------------------------------
