// *********************************************************************
//   Custom DPA Handler - Bridge using UART                            *
// *********************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-Bridge-UART.c,v $
// Version: $Revision: 1.16 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2019/03/07  Release for DPA 4.01
//   2018/10/25  Release for DPA 3.03
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

//############################################################################################
/*
This handler forwards [1] DpaEvent_DpaRequest and [2] DpaEvent_FrcValue events via UART to the external device. See UART_BAUD for the baud rate setting.
The external device is responsible for preparing the proper responses to these events. It sends the response back to the handler via UART.
Handler then responses with the response prepared by the external device back to the IQRF network.

There is an example CustomDpaHandler-Bridge-UART.ino for Arduino that shows how the external device prepares the response. In this case the device behaves as Standard IQRF Sensor.
Use an IQRF Breakout board IQRF-BB-01 or IQRF-SHIELD-02 containing the level shifters to connect TR module (3.3 V logic) to the Arduino board (5 V logic):
#IQRF-BB-01    Arduino
 GND           GND
 +3V3/+5V      3.3V
 MISO (C8=RX)  TX -> Digital 1
 SS (C5=TX)    RX <- Digital 0

This handler and the external device exchange data (packets) using the HDLC protocol the same way the DPA UART Interface except the CRC is not used.
Please see https://www.iqrf.org/DpaTechGuide/#2.3.2%20UART for details.

The handler and the external device pack a different set of information to the packet depending on the event.

[1] DpaEvent_DpaRequest
The packet (data part) sent to the device has the following structure:
#Byte  Content         Notes
 0     0               = DpaEvent_DpaRequest
 1     _DpaDataLength
 2     NADR
 3     NADRhigh
 4     PNUM
 5     PCMD
 6     HWPID.low8
 7     HWPID.high8
 8...  PDATA           Optional DPA Request data.

Please note that _DpaDataLength does not have to equal (in case of enumeration DPA Requests) length of PDATA.
The handler waits maximum RESPONSE_TIMEOUT ms for the response from the external device otherwise ERROR_FAIL is returned.

The device responses with a packet of the following structure:
#Byte  Content         Notes
 0     see Notes       = DpaEvent_DpaRequest with ORed optional flags. When bit7 set, the handler returns TRUE otherwise returns FALSE. When bit6 is set, the handler returns DPA error specified at byte #2 i.e. PDATA[0]
 1     _DpaDataLength
 2...  PDATA           Optional DPA Response data.

[2] DpaEvent_FrcValue
The 32 bytes long packet (data part) sent to the device has the following structure:
#Byte   Content        Notes
 0      10             = DpaEvent_FrcValue
 1      RFC Command
 2...31 FRC user data

The handler waits maximum FRC_RESPONSE_TIMEOUT ms for the response from the external device otherwise the FRC is not handled and the default DPA value (bit0=1) is returned.

The device responses with a 5 bytes long packet of the following structure:
#Byte  Content         Notes
 0     10              DpaEvent_DpaRequest
 1...4 FRC value       Content will be written to the responseFRCvalue, responseFRCvalue2B, responseFRCvalue4B variables. Coded using Little Endian.
*/
//############################################################################################

// UART baud rate
#define UART_BAUD                 115200

// DPA response timeout from external device [ms]
#define RESPONSE_TIMEOUT          ( 30 / 10 ) 
// FRC value response timeout from external device [ms]
#define FRC_RESPONSE_TIMEOUT      ( 20 / 10 )
// DPA reported fixed FRC response time
#define _FRC_RESPONSE_TIME_FIXED  _FRC_RESPONSE_TIME_40_MS

//############################################################################################

// Division macro with rounding
#define DIV(Dividend,Divisor) (((Dividend+((Divisor)>>1))/(Divisor)))
// PIC baud register computation
#define UART_SPBRG_VALUE( Baud )  ( DIV( F_OSC, ( ( ( uns24 )4  ) * ( Baud ) ) ) - 1 )

// HDLC byte stuffing bytes
// Flag Sequence
#define   HDLC_FRM_FLAG_SEQUENCE    0x7e
// Asynchronous Control Escape
#define   HDLC_FRM_CONTROL_ESCAPE   0x7d
// Asynchronous transparency modifier
#define   HDLC_FRM_ESCAPE_BIT       0x20

// Flag to DpaEvent_DpaRequest event value to indicate return TRUE not FALSE
#define EVENT_RETURN_TRUE           0x80
// Flag to DpaEvent_DpaRequest event value to report error, error value is the 1st data byte
#define EVENT_RESPONSE_ERROR        0x40

//############################################################################################

// Buffer used for exchange data with external device
#define RxBuffer  bufferCOM
// Sends byte to the external device
void TxByte( uns8 data );
// Sends HDLC byte to the external device
void TxHdlcByte( uns8 data );
// Receives data from external device, returns length, 0 if timeout occurred
uns8 RxPacket( uns8 timeout );
// Initialization
void Init();

//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // Handler presence mark
  clrwdt();

  // Detect DPA event to handle (unused event handlers can be commented out or even deleted)
  switch ( GetDpaEvent() )
  {
    // -------------------------------------------------
    // Handler these unused events as fast as possible
    case DpaEvent_Idle:
      Init();

    case DpaEvent_Interrupt:
      return Carry;

      // -------------------------------------------------
    case DpaEvent_FrcValue:
      // Called to get FRC value

      // Initialize HW if needed
      Init();
      // Start measuring timeout for RxPacket() function
      startCapture();

      // Start packet
      TxByte( HDLC_FRM_FLAG_SEQUENCE );
      // Send event value
      TxHdlcByte( DpaEvent_FrcValue );
      // Send FRC command
      TxHdlcByte( _PCMD );
      // Now send all FRC user data
      uns8 loop = sizeof( DataOutBeforeResponseFRC );
      FSR0 = &DataOutBeforeResponseFRC[0];
      do
      {
        TxHdlcByte( *FSR0++ );
      } while ( --loop != 0 );
      // Stop packet
      TxByte( HDLC_FRM_FLAG_SEQUENCE );

      // Receive the FRC value from the device via UART, length must equal to the event value + 4 FRC value bytes
      if ( RxPacket( FRC_RESPONSE_TIMEOUT ) == ( sizeof( uns8 ) + sizeof( uns32 ) ) && RxBuffer[0] == DpaEvent_FrcValue )
      {
        // Return FRC values to DPA
#if IQRFOS >= 403
        responseFRCvalue4B.low8 = RxBuffer[1];
        responseFRCvalue4B.midL8 = RxBuffer[2];
        responseFRCvalue4B.midH8 = RxBuffer[3];
        responseFRCvalue4B.high8 = RxBuffer[4];
#else
        responseFRCvalue = RxBuffer[1];
        responseFRCvalue2B.low8 = RxBuffer[1];
        responseFRCvalue2B.high8 = RxBuffer[2];
#endif
      }

      break;

      // -------------------------------------------------
    case DpaEvent_FrcResponseTime:
      // Called to get FRC response time
      // ToDo - Improve, make value dynamic?
      responseFRCvalue = _FRC_RESPONSE_TIME_FIXED;
      break;

      // -------------------------------------------------
    case DpaEvent_DpaRequest:
      // Called to interpret DPA request for peripherals

      // Initialize HW if needed
      Init();
      // Pass the request to the connected device via UART (must not modify FSR0 till it is used)
      // Start packet
      TxByte( HDLC_FRM_FLAG_SEQUENCE );
      // Send event value
      TxHdlcByte( DpaEvent_DpaRequest );
      // Send DPA variable data length (must not equal to the actual data length sent)
      TxHdlcByte( _DpaDataLength );
      // Send DPA message fields
      TxHdlcByte( _NADR );
      TxHdlcByte( _NADRhigh );
      TxHdlcByte( _PNUM );
      TxHdlcByte( _PCMD );
      TxHdlcByte( _HWPID.low8 );
      TxHdlcByte( _HWPID.high8 );

      // How much data to pass to the device?
      uns8 dataLength;
      if ( IsDpaEnumPeripheralsRequest() )
        dataLength = sizeof( _DpaMessage.EnumPeripheralsAnswer );
      else if ( IsDpaPeripheralInfoRequest() )
        dataLength = sizeof( _DpaMessage.PeripheralInfoAnswer );
      else
        // Same amount as available
        dataLength = _DpaDataLength;

      // FSRx might have been destroyed by Init()
      setFSR01( _FSR_RF, _FSR_RF );
      // Now send the data byte by byte
      for ( ; dataLength != 0; dataLength-- )
        TxHdlcByte( *FSR0++ );
      // Stop packet
      TxByte( HDLC_FRM_FLAG_SEQUENCE );

      // Start measuring timeout for RxPacket() function
      startCapture();

      // Receive the response from the device via UART
      dataLength = RxPacket( RESPONSE_TIMEOUT );
      // Check for timeout and correct event
      if ( dataLength == 0 || ( RxBuffer[0] & ~( EVENT_RETURN_TRUE | EVENT_RESPONSE_ERROR ) ) != DpaEvent_DpaRequest )
        DpaApiReturnPeripheralError( ERROR_FAIL );

      // Report DPA error?
      if ( ( RxBuffer[0] & EVENT_RESPONSE_ERROR ) != 0 )
        DpaApiReturnPeripheralError( RxBuffer[2] );

      // Get DPA data length field
      _DpaDataLength = RxBuffer[1];
      // Copy DPA response data (all data minus event value and _DpaDataLength value)
      copyMemoryBlock( &RxBuffer[2], &_DpaMessage.Response.PData[0], dataLength - 2 );

      // Return TRUE or FALSE
#if EVENT_RETURN_TRUE != 0x80
#error Cannot optimize
#endif
      // Carry = TRUE of FALSE to return, got from EVENT_RETURN_TRUE part of the 1st byte which is header
      W = rl( RxBuffer[0] );
      return Carry;

      // -------------------------------------------------
    case DpaEvent_DisableInterrupts:
      // Called when device needs all hardware interrupts to be disabled (before Reset, Restart, LoadCode, Remove bond and run RFPGM)

      // Disable UART so on the next boot it will be open again
      SPEN = FALSE;
      break;
  }

  return FALSE;
}

//############################################################################################
uns8 RxByteValue;
// RxByteValue = read byte, Carry = result
bit RxByte()
//############################################################################################
{
#pragma updateBank exit = UserBank_01

  if ( FERR || OERR )
  {
    W = RCREG;

    if ( OERR )
    {
      CREN = 0;
      CREN = 1;
    }

_returnFALSE:
    return FALSE;
  }

  if ( !RCIF )
    goto _returnFALSE;

  RxByteValue = RCREG;
  return TRUE;
}

//############################################################################################
uns8 RxPacket( uns8 timeout )
//############################################################################################
{
#define MIN_RX_PACKET_DATA_LENGTH  1
#define MAX_RX_PACKET_DATA_LENGTH  sizeof( RxBuffer )

  typedef enum { RXstateWaitHead, RXstatePacket, RXstateEscape } TState;

  // Make sure buffered UART RX is empty
  RxByte();
  RxByte();

  TState state = RXstateWaitHead;
  uns8 rxLength;
  for ( ; ; )
  {
    clrwdt();
    // Timeout?
    captureTicks();   // Note: must not modify FSR0
    if ( param3 > timeout )
      return 0;

    // If anything received via UART
    if ( RxByte() )
    {
      // HDLC machine
      skip( (uns8)state );
#pragma computedGoto 1
      goto _RXstateWaitHead;
      goto _RXstatePacket;
      goto _RXstateEscape;
#pragma computedGoto 0
      ;
      // ---------------------------
_RXstateEscape:
      switch ( RxByteValue )
      {
        case HDLC_FRM_FLAG_SEQUENCE:
          goto _SetRXstatePacket;

        case HDLC_FRM_CONTROL_ESCAPE:
_SetRXstateWaitHead:
          state = RXstateWaitHead;
          continue;
      }

      state--; // RXstatePacket
      RxByteValue ^= HDLC_FRM_ESCAPE_BIT;
      goto _StoreByte;

      // ---------------------------
_RXstatePacket:
      switch ( RxByteValue )
      {
        case HDLC_FRM_CONTROL_ESCAPE:
          // RXstateEscape
          state++;
          continue;

        case HDLC_FRM_FLAG_SEQUENCE:
        {
          if ( rxLength >= MIN_RX_PACKET_DATA_LENGTH )
            return rxLength;

          goto _SetRXstatePacket;
        }
      }

_StoreByte:
      if ( rxLength == ( MAX_RX_PACKET_DATA_LENGTH + 2 ) )
        goto _SetRXstateWaitHead;

      setINDF0( RxByteValue );
      FSR0++;
      rxLength++;
      continue;

      // ---------------------------
_RXstateWaitHead:
      if ( RxByteValue == HDLC_FRM_FLAG_SEQUENCE )
      {
_SetRXstatePacket:
        rxLength = 0;
        FSR0 = RxBuffer;
        state = RXstatePacket;
      }

      continue;
    }
  }
}

//############################################################################################
void TxByte( uns8 data )
//############################################################################################
{
  while ( !TXIF );
  TXREG = data;
}

//############################################################################################
void TxHdlcByte( uns8 data )
//############################################################################################
{
  switch ( data )
  {
    default:
      TxByte( data );
      return;

    case HDLC_FRM_FLAG_SEQUENCE:
    case HDLC_FRM_CONTROL_ESCAPE:
    {
      TxByte( HDLC_FRM_CONTROL_ESCAPE );
      TxByte( data ^ HDLC_FRM_ESCAPE_BIT );
      return;
    }
  }
}

//############################################################################################
void Init()
//############################################################################################
{
  // Initialize UART
  if ( !SPEN )
  {
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
  }
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
