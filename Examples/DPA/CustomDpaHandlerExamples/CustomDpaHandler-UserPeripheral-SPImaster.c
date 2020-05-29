// *********************************************************************************
//   Custom DPA Handler code example - User peripheral implementation - SPI master *
// *********************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-UserPeripheral-SPImaster.c,v $
// Version: $Revision: 1.27 $
// Date:    $Date: 2020/02/20 17:18:58 $
//
// Revision history:
//   2017/03/13  Release for DPA 3.00
//   2015/08/05  Release for DPA 2.20
//   2015/01/16  Release for DPA 2.12
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// This example implements Master SPI as a user peripheral

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

// *********************************************************************

// SPI master is controlled by data sent to the user peripheral PNUM=0x20, PCMD=0x00
// Data consists of subcommands. Each four available subcommand consists of one SCMD byte and optional data.
// If the subcommand reads data from SPI slave, then the read data is appended to the DPA response data

// #  Subcommand:  SCMD      data                        Read data                         
// 1. WriteRead:   00LL.LLLL _byte_1_ ... _byte_n_       _byte_1_ ... _byte_n_
// 2. Write:       10LL.LLLL _byte_1_ ... _byte_n_
// 3. Read:        01LL.LLLL                             _byte_1_ ... _byte_n_
// 4. Delay:       11DD.DDDD dddddddd
//
// Notes:
//  1.-3.  LLLLLL specifies length of written and/or read bytes to/from SPI slave
//  1.-3.  When LLLLLL is zero (SCMD values 0x00, 0x40, 0x80), then no data is written and/or read but next command 1.-3. will not deactivate _SS_ signal
//  3.     SDO is 0 during reading from SPI
//  4.     DDDDDD.dddddddd specifies 14 bit delay in 1 ms units to be performed

// Example:
// The following example shows how to control serial 32 kB SRAM memory 23K256 (http://www.microchip.com/23K256) connected as SPI slave to the SPI master peripheral
// 23K256 uses "SCK low when inactive" and "SDO/SDI valid on SCK rising edge"
//
// The example executes 3 actions using 5 subcommands:
//  1. Sets serial RAM to "Sequential mode"
//   Write to serial RAM SPI:  0x01{WRSR}, 0x40{status value}
//   Subcommand #1:            0x82{#1:write 2 bytes to SPI}, 0x01{WRSR}, 0x40{status value}
//
//  2. Writes 2 bytes 0xAA and 0xBB starting from address 2 to the RAM
//   Write to serial RAM SPI:  0x02{WRITE}, 0x00{address high8}, 0x02{address low8}, 0xAA{1st byte@2}, 0xBB{2nd byte@3}
//   Subcommand #2:            0x85{#2:write 5 bytes}, 0x02{WRITE}, 0x00{address high8}, 0x02{address low8}, 0xAA{1st byte@2}, 0xBB{2nd byte@3}
//
//  3. Reads 4 bytes from address 1 from the RAM
//   Write to serial RAM SPI:  0x03{READ}, 0x00{address high8}, 0x01{address low8} (keep _SS_ active after this SPI write)
//   Read from serial RAM SPI: 0x??{unknown byte@1}, 0xAA{previously written byte@2}, 0xBB{previously written byte@3}, 0x??{unknown byte@4}
//   Subcommands #3-5:         0x00{#3:keep _SS_ after subcommand #4}, 0x83{#4:write 3 bytes}, 0x03{READ}, 0x00{address high8}, 0x01{address low8}, 0x44{#5:read 4 bytes}
//
// DPA request:  PNUM=0x20, PCMD=0x00, Data[0x0F]=0x82, 0x01, 0x40, 0x85, 0x02, 0x00, 0x02, 0xAA, 0xBB, 0x00, 0x83, 0x03, 0x00, 0x01, 0x44
// DPA response: PNUM=0x20, PCMD=0x80, Data[0x04]=0x??, 0xAA, 0xBB, 0x??

// *********************************************************************

// SPI signals to PIN assignment:
//		TR module pin	DK-EVAL-04x pin 	SPI
//      --------------------------------------- 
//		C8				1					SDO
//		C7				2					SDI
//		C6				3					SCK
//		C5				4					_SS_
//		C4				7					GND

// _SS_ pin assignment (C5 = PORTA.5)
#define SS_PIN						LATA.5

// Custom peripheral request internal subcommands
// Write and read from SPI slave
#define	CMD_WRITE_READ				0x00
// Write to SPI slave
#define	CMD_WRITE					((uns8)0x80)
// Read from SPI slave
#define	CMD_READ					0x40
// Execute delay
#define	CMD_DELAY					((uns8)0xC0)

// Mask to get the data from the subcommand
#define	CMD_MASK					((uns8)0xC0)

// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // Handler presence mark
  clrwdt();

  // Detect DPA event to handle
  switch ( GetDpaEvent() )
  {
    // -------------------------------------------------
    case DpaEvent_Interrupt:
      // Do an extra quick background interrupt work
      // ! The time spent handling this event is critical.If there is no interrupt to handle return immediately otherwise keep the code as fast as possible.
      // ! Make sure the event is the 1st case in the main switch statement at the handler routine.This ensures that the event is handled as the 1st one.
      // ! It is desirable that this event is handled with immediate return even if it is not used by the custom handler because the Interrupt event is raised on every MCU interrupt and the “empty” return handler ensures the shortest possible interrupt routine response time.
      // ! Only global variables or local ones marked by static keyword can be used to allow reentrancy.
      // ! Make sure race condition does not occur when accessing those variables at other places.
      // ! Make sure( inspect.lst file generated by C compiler ) compiler does not create any hidden temporary local variable( occurs when using division, multiplication or bit shifts ) at the event handler code.The name of such variable is usually Cnumbercnt.
      // ! Do not call any OS functions except setINDFx().
      // ! Do not use any OS variables especially for writing access.
      // ! All above rules apply also to any other function being called from the event handler code, although calling any function from Interrupt event is not recommended because of additional MCU stack usage.

DpaHandleReturnTRUE:
      return TRUE;

      // -------------------------------------------------
    case DpaEvent_Init:
      // Do a one time initialization before main loop starts

      // Initialize PINs to inputs and outputs
      TRISC.5 = 0;				// RC5 as output SDO (C8)
      TRISC.4 = 1;				// RC4 as input SDI (C7)
      TRISC.3 = 0;				// RC3 as output SCK (C6)
      TRISA.5 = 0;				// RA5 as output SS (C5)

      // TR module with connected pins?
      moduleInfo();
      if ( bufferINFO[5].7 == 0 )
      {
        // Yes
        TRISC.6 = 1;				// RC6 as input (connected to RA5 in parallel)
        TRISB.4 = 1;				// RB4 as input (connected to RA5 in parallel)
        TRISC.7 = 1;				// RC7 as input (connected to RC5 in parallel)
      }

      // Set idle level of SS
      SS_PIN = 1;

      // Setup SPI
      // SSPSTAT
      //  Transmit occurs on SCK rising edge
      CKE = 1;
      // SSPCON1: (SSPCON1 cannot be accessed directly due to OS restriction)
      //  SPI enabled 
      //  Idle state for clock is a low level 
      //  CLK = 250kHz @ 16 MHz
      writeToRAM( &SSPCON1, 0b0.0.1.0.0010 );

      break;

      // -------------------------------------------------
    case DpaEvent_DpaRequest:
      // Called to interpret DPA request for peripherals
      // -------------------------------------------------
      // Peripheral enumeration
      if ( IsDpaEnumPeripheralsRequest() )
      {
        // We implement 1 user peripheral
        _DpaMessage.EnumPeripheralsAnswer.UserPerNr = 1;
        FlagUserPer( _DpaMessage.EnumPeripheralsAnswer.UserPer, PNUM_USER + 0 );
        _DpaMessage.EnumPeripheralsAnswer.HWPID = 0x000F;
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver = 0x6789;

        goto DpaHandleReturnTRUE;
      }
      // -------------------------------------------------
      // Get information about peripheral
      else if ( IsDpaPeripheralInfoRequest() )
      {
        if ( _PNUM == PNUM_USER + 0 )
        {
          _DpaMessage.PeripheralInfoAnswer.PerT = PERIPHERAL_TYPE_USER_AREA;
          _DpaMessage.PeripheralInfoAnswer.PerTE = PERIPHERAL_TYPE_EXTENDED_READ_WRITE;
          goto DpaHandleReturnTRUE;
        }

        break;
      }
      // -------------------------------------------------
      else
      {
        // Handle peripheral command
        if ( _PNUM == PNUM_USER + 0 )
        {
          // Check DPA command value
          if ( _PCMD != 0 )
            DpaApiReturnPeripheralError( ERROR_PCMD );

          // Check total data length
          if ( _DpaDataLength == 0 )
            DpaApiReturnPeripheralError( ERROR_DATA_LEN );

          // FSR0 points to the next byte from DPA request
          FSR0 = _DpaMessage.Request.PData;
          // FSR1 points to the place for the next read byte
          FSR1 = bufferINFO;
          // Flag for keeping SS
          bit keepSS = FALSE;
          do
          {
            // Get subcommand type from the 1st byte
            uns8 cmd @ userReg0;
            cmd = *FSR0 & CMD_MASK;
            // Get data part from the 1st byte
            uns8 dataFromCmd @ userReg1;
            dataFromCmd = *FSR0++ & ~CMD_MASK;

            // Write and/or read subcommand?
            if ( cmd != CMD_DELAY )
            {
              if ( dataFromCmd == 0 )
                // Keep SS after next SPI command
                keepSS = TRUE;
              else
              {
                // Yes!
                // SS - active
                SS_PIN = 0;

                // Loop all the bytes
                do
                {
                  // Writing?
                  if ( cmd == CMD_WRITE || cmd == CMD_WRITE_READ )
                    // Yes, get the byte from request
                    W = *FSR0++;
                  else
                    // No data to write, write 0 instead
                    W = 0;

                  // Reset interrupt flag
                  SSPIF = 0;
                  // Write the byte
                  SSPBUF = W;
                  // Wait until the byte is transmitted/received
                  while ( !SSPIF );

                  // Reading?
                  if ( cmd == CMD_WRITE_READ || cmd == CMD_READ )
                  {
                    // Yes!
                    setINDF1( SSPBUF );
                    FSR1++;
                  }
                  // Next byte to write and/or read?
                } while ( --dataFromCmd != 0 );

                if ( !keepSS )
                  // SS - deactivate
                  SS_PIN = 1;
                keepSS = FALSE;
              }
            }
            else
            {
              // Delay subcommand
              // Wait low8
              // WaitMS must not destroy FSRx registers (works well with current OS version)!
              waitMS( *FSR0++ );
              // Wait high6 * 256 ms
              for ( ; dataFromCmd != 0; dataFromCmd-- )
              {
                waitMS( 256 / 2 );
                clrwdt();
                waitMS( 256 / 2 );
              }
            }
            // Another subcommand?
          } while ( FSR0.low8 - (uns8)_DpaMessage.Request.PData != _DpaDataLength );

          // Compute data length
          _DpaDataLength = FSR1.low8 - (uns8)bufferINFO;
          // Copy read bytes to the response
          copyMemoryBlock( bufferINFO, _DpaMessage.Response.PData, _DpaDataLength );
          // The previous statement can be replaced by shorter copyBufferINFO2RF(), because _DpaMessage.Response.PData == bufferRF
          goto DpaHandleReturnTRUE;
        }
      }
  }

  return FALSE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
