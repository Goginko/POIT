//*********************************************************************
//           SPI master communicating with IQRF TR module
//                         Demo example
//   Just for guidance - it should be adapted for given application
//*********************************************************************
#include "IQRF_SPI.h"

extern unsigned char IQRF_SPI_TxBuf[IQRF_SPI_BUFFER_SIZE], IQRF_SPI_RxBuf[IQRF_SPI_BUFFER_SIZE];
extern unsigned char IQRF_SPI_Busy, iq_DLEN, iq_PTYPE, iq_spistat, IQRF_SPI_Task_Message, iq_pac_len;

//*********************************************************************
int main(void)
{   
    other_Init();
    IQRF_SPI_Init();
    
    while(1)
    {
        other_Tasks();
    
        IQRF_SPI_Task();                            // call this task every 150us
    
        if (Button == pressed)
        {                                           // try to send packet
            
            if ((iq_spistat == TR_STAT_COM_MODE || iq_spistat == TR_STAT_SLOW_MODE) && IQRF_SPI_Busy == IQRF_NOT_BUSY)
    	    {                                       // TR module is in communication mode and the SPI is not busy
        	    Clear_IQRF_SPI_TxBuf();
     		                 
    		    iq_DLEN = 4; 		                // number of user bytes                       				
    	        iq_PTYPE = (iq_DLEN | 0x80); 		// PTYPE set bit7 - write to bufferCOM of TR module
    	        IQRF_SPI_TxBuf[0] = TR_CMD_WR_RD;
    	        IQRF_SPI_TxBuf[1] = iq_PTYPE;
    	        IQRF_SPI_TxBuf[2] = 'i';            // user data "iqrf"
    	        IQRF_SPI_TxBuf[3] = 'q'; 
    	        IQRF_SPI_TxBuf[4] = 'r'; 
    	        IQRF_SPI_TxBuf[5] = 'f'; 
     	        Start_IQRF_SPI();                   // activates IQRF_SPI_Task to send packet
            }
        }   
         
        if (IQRF_SPI_Task_Message)                  // any message from IQRF_SPI_Task?
		{	                                                            
			switch (IQRF_SPI_Task_Message)
			{
				case IQRF_SPI_WRITE_OK:             // last sending was successful
                                                    // insert your code
					break;
					
				case IQRF_SPI_WRITE_ERR:            // last sending was not successful
                                                    // insert your code
					break;
				
				case IQRF_SPI_DATA_READY:           // received packet is in IQRF_SPI_RxBuf
                                                    // insert your code
					break;
			}
			
			IQRF_SPI_Task_Message = 0;
		}
    }
}
//*********************************************************************
