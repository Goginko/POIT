// **************************************************************************
//   Custom DPA Handler code example - User peripheral supporting UART HW   *
// **************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-UserPeripheral-HW-UART.c,v $
// Version: $Revision: 1.12 $
// Date:    $Date: 2020/02/20 17:18:58 $
//
// Revision history:
//   2017/08/14  Release for DPA 3.01
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// This example implements the user peripheral supporting HW UART of the PIC
// This example works only at STD mode, not at LP mode

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

//############################################################################################

// TX byte to UART
void TxUART( uns8 data );
// RX byte from UART; W = read byte, Carry = byte was read
bit RxUART();

//############################################################################################

// UART baud rate
#define UART_BAUD 19200

// Number of UART received bytes
uns8 RxDataLengthUART;

// Length of RX and TX buffers, must be power of 2
#define UART_BUFFER_LENGTH     16

#if 0 != ( UART_BUFFER_LENGTH & ( UART_BUFFER_LENGTH - 1 ) )
#error UART_BUFFER_LENGTH is not power of 2
#endif

// Rx
// Circular RX UART buffer pointers
uns8 RxBufferPointerStartUART;
uns8 RxBufferPointerEndUART;
uns8 RxBufferUART[UART_BUFFER_LENGTH] /*@ PeripheralRam[0]*/;

// TX
// Circular TX UART buffer pointers
uns8 TxBufferPointerStartUART;
uns8 TxBufferPointerEndUART;
uns8 TxBufferUART[UART_BUFFER_LENGTH] /*@ PeripheralRam[UART_BUFFER_LENGTH]*/;

// Division macro with rounding
#define DIV(Dividend,Divisor) (((Dividend+((Divisor)>>1))/(Divisor)))
// PIC baud register computation
#define UART_SPBRG_VALUE( Baud )  ( DIV( F_OSC, ( ( ( uns24 )4  ) * ( Baud ) ) ) - 1 )


// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // Handler presence mark
  clrwdt();

  // TXIE state before sleep
  static bit wasTXIE;

  // Detect DPA event to handle
  switch ( GetDpaEvent() )
  {
    // -------------------------------------------------
    case DpaEvent_Interrupt:
      // Do an extra quick background interrupt work
      // ! The time spent handling this event is critical.If there is no interrupt to handle return immediately otherwise keep the code as fast as possible.
      // ! Make sure the event is the 1st case in the main switch statement at the handler routine.This ensures that the event is handled as the 1st one.
      // ! It is desirable that this event is handled with immediate return even if it is not used by the custom handler because the Interrupt event is raised on every MCU interrupt and the �empty� return handler ensures the shortest possible interrupt routine response time.
      // ! Only global variables or local ones marked by static keyword can be used to allow reentrancy.
      // ! Make sure race condition does not occur when accessing those variables at other places.
      // ! Make sure( inspect.lst file generated by C compiler ) compiler does not create any hidden temporary local variable( occurs when using division, multiplication or bit shifts ) at the event handler code.The name of such variable is usually Cnumbercnt.
      // ! Do not call any OS functions except setINDFx().
      // ! Do not use any OS variables especially for writing access.
      // ! All above rules apply also to any other function being called from the event handler code, although calling any function from Interrupt event is not recommended because of additional MCU stack usage.

      // -----------------------------------------------------------------------------------------
      // UART Receive
      //
      if ( RCIF )
      {
        // We ignore FERR
        FSR1L = RCREG;
        // Put the received byte in circular buffer
        if ( RxDataLengthUART < UART_BUFFER_LENGTH )
        {
          // One more byte received
          RxDataLengthUART++;
          // Prepare pointer
          FSR0 = RxBufferUART + RxBufferPointerEndUART;
          // Recalculate tail pointer
          // Optimization: Same as (UART_BUFFER_LENGTH is power of 2) : RxBufferPointerEndUART = ( RxBufferPointerEndUART + 1 ) % UART_BUFFER_LENGTH;
          RxBufferPointerEndUART++;
          RxBufferPointerEndUART &= ~UART_BUFFER_LENGTH;
          // Store byte
          setINDF0( FSR1L );
        }
      }

      // Overrun recovery (we do it after receiving UART byte in order to receive it as soon as possible)
      if ( OERR )
        CREN = 0;

      // Seems excess, but at the end it is shorted and faster than having this statement at else branch
      CREN = 1;

      // -----------------------------------------------------------------------------------------
      // UART Transmit
      //
      if ( TXIF && TXIE )
      {
        // Send byte from circular buffer to the UART
        FSR0 = TxBufferUART + TxBufferPointerStartUART;
        TxBufferPointerStartUART = ( TxBufferPointerStartUART + 1 ) % UART_BUFFER_LENGTH;
        // Buffer empty?
        if ( TxBufferPointerStartUART == TxBufferPointerEndUART )
          TXIE = FALSE;
        // TX the byte
        TXREG = *FSR0;
      }

      // Return value does not matter
      return Carry;


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
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver = 0x0000;

DpaHandleReturnTRUE:
        return TRUE;
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
          // Check command
          switch ( _PCMD )
          {
            // ------------------------------
            // Write to UART
            case 0:
              // There must be some data to write
              if ( _DpaDataLength == 0 )
              {
_ERROR_DATA_LEN:
                DpaApiReturnPeripheralError( ERROR_DATA_LEN );
              }

              // Pointer to data (setFRS1( _FSR_RF ) can be used too to save code)
              FSR1 = &_DpaMessage.Request.PData[0];
              do
              {
                // Send one byte
                TxUART( *FSR1++ );
                // Loop all bytes
              } while ( --_DpaDataLength != 0 );

              // _DpaDataLength = 0  => no data returned
              goto DpaHandleReturnTRUE;

              // ------------------------------
              // Read from UART
            case 1:
              // No data must be sent
              if ( _DpaDataLength != 0 )
                goto _ERROR_DATA_LEN;

              // Pointer to the output buffer  (setFRS1( _FSR_RF ) can be used too to save code)
              FSR1 = &_DpaMessage.Response.PData[0];
              // Initial count of received bytes
              _DpaDataLength = 0;
              // Loop while there is enough space and some byte to read
              while ( _DpaDataLength < sizeof( _DpaMessage.Response.PData ) && RxUART() )
              {
                // Store read byte
                setINDF1( W );
                // Move pointer
                FSR1++;
                // Count next byte
                _DpaDataLength++;
              }

              // Return number of read bytes
              goto DpaHandleReturnTRUE;
          }
        }
      }
      break;

      // -------------------------------------------------
    case DpaEvent_Init:
      // Do a one time initialization before main loop starts

      // Connected TR pins (e.g. TR72D)?
      moduleInfo();
      if ( !bufferINFO[5].7 )
      {
        // Set them as inputs
        TRISC.5 = 1;
        TRISA.5 = 1;
        TRISB.4 = 1;
      }

      // RX input
      TRISC.7 = 1;
      // TX output
      TRISC.6 = 0;

      // Set baud rate
      SPBRGL = UART_SPBRG_VALUE( UART_BAUD ) & 0xff;
      SPBRGH = UART_SPBRG_VALUE( UART_BAUD ) >> 8;
      // baud rate control setup: BRG16 = 1
      BAUDCON = 0b0000.1.000;

      // CSRC TX9 TXEN SYNC SENDB BRGH TRMT TX9D
      // TXEN = 1
      // BRGH = 1
      // async UART, high speed, 8 bit, TX enabled
      TXSTA = 0b0010.0100;

      // SPEN RX9 SREN CREN ADDEN FERR OERR RX9D
      // SPEN = 1
      // CREN = 1
      // Continuous receive, enable port, 8 bit
      RCSTA = 0b1001.0000;
      // Enable UART RX interrupt
      RCIE = TRUE;

      break;

      // -------------------------------------------------
    case DpaEvent_AfterSleep:
      // Called after woken up after sleep

      TXIE = wasTXIE;
      RCIE = TRUE;
      break;

      // -------------------------------------------------
    case DpaEvent_BeforeSleep:
      // Called before going to sleep

      // -------------------------------------------------
    case DpaEvent_DisableInterrupts:
      // Called when device needs all hardware interrupts to be disabled (before Reset, Restart, LoadCode, Remove bond, and Run RFPGM)

      wasTXIE = TXIE;
      TXIE = FALSE;
      RCIE = FALSE;
      break;
  }

  return FALSE;
}

//############################################################################################
// Note: make sure the parameter does not overlap another variable as the function is ready to be called from (timer) interrupt too
static uns8 _data;
void TxUART( uns8 data @ _data )
//############################################################################################
{
  // Wait for a space in the buffer
  while ( TXIE && TxBufferPointerStartUART == TxBufferPointerEndUART );

  // Disable TX interrupt
  TXIE = FALSE;
  // Compute pointer
  FSR0 = TxBufferUART + TxBufferPointerEndUART;
  // Optimization: TxBufferPointerEndUART = ( TxBufferPointerEndUART + 1 ) % UART_BUFFER_LENGTH;
  TxBufferPointerEndUART++;
  TxBufferPointerEndUART &= ~UART_BUFFER_LENGTH;
  // Store byte
  setINDF0( _data );
  // Start transmitting
  TXIE = TRUE;
}

//############################################################################################
// W = read byte, Carry = result
bit RxUART()
//############################################################################################
{
  // Buffer empty?
  if ( RxDataLengthUART == 0 )
    return FALSE;

  // Disable RX interrupt
  RCIE = FALSE;
  // Get byte from the circular buffer
  FSR0 = RxBufferUART + RxBufferPointerStartUART;
  // Optimization: RxBufferPointerStartUART = ( RxBufferPointerStartUART + 1 ) % UART_BUFFER_LENGTH;
  RxBufferPointerStartUART++;
  RxBufferPointerStartUART &= ~UART_BUFFER_LENGTH;
  // One byte less
  RxDataLengthUART--;
  // Returned byte
  W = *FSR0;
  // Enable RX interrupt
  RCIE = TRUE;
  // TRUE => byte was read
  return TRUE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################