// *********************************************************************
//   Custom DPA Handler code example - Standard IQRF DALI - template   *
// *********************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: 5802_DALItemplate.c,v $
// Version: $Revision: 1.4 $
// Date:    $Date: 2020/02/20 17:18:59 $
//
// Revision history:
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/
// IQRF Standards documentation https://www.iqrfalliance.org/iqrf-interoperability/

// This example implements IQRF DALI standard

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"
// IQRF standards header (modify the path according to your setup)
#include "IQRFstandard.h"
#include "IQRF_HWPID.h"

//############################################################################################

// Dali TX and RX pins assignment
#define DALI_TXpin		LATC.6
#define DALI_TXtris		TRISC.6

#define DALI_RXpin		PORTC.7
#define DALI_RXtris		TRISC.7

// Macro to setup DALI GPIOs 
#define SETUP_DALI() do { \
    DALI_RXtris = 1; /* RX is input */ \
    DALI_TXtris = 0; /* TX is output */ \
    DALI_TX1();      /* set DALI idle state */ \
} while( FALSE );

// Macros for accessing the DALI signals
// Note: negative logic is used in this example because the external DALI HW interface inverts the TTL signals 
#define DALI_TX0()      DALI_TXpin = 1
#define DALI_TX1()      DALI_TXpin = 0
#define DALI_RX()       (!DALI_RXpin)

// Sends DALI commands from FSR1, then length must be non-zero even number
void SendDALIs( uns8 length );

// Place to store DALI commands for asynchronous send
// Note: as a side-effect the DALI answers are available at PeripheralRam after PCMD_STD_DALI_SEND_COMMANDS_ASYNC
#define   DALIbuffer      PeripheralRam

// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // Handler presence mark
  clrwdt();

  // Length of the DALI commands to be send asynchronously, 0 if none
  static uns8 asyncDpaDataLength;

  // Detect DPA event to handle
  switch ( GetDpaEvent() )
  {
    // -------------------------------------------------
    case DpaEvent_Interrupt:
      // Do an extra quick background interrupt work
      return Carry;

      // -------------------------------------------------
    case DpaEvent_Init:
      // Do a one time initialization before main loop starts

      // Setup DALI
      SETUP_DALI();
      break;

      // -------------------------------------------------
    case DpaEvent_DpaRequest:
      // Called to interpret DPA request for peripherals
      // -------------------------------------------------
      // Peripheral enumeration
      if ( IsDpaEnumPeripheralsRequest() )
      {
        // We implement 1 standard peripheral
        _DpaMessage.EnumPeripheralsAnswer.UserPerNr = 1;
        FlagUserPer( _DpaMessage.EnumPeripheralsAnswer.UserPer, PNUM_STD_DALI );
        _DpaMessage.EnumPeripheralsAnswer.HWPID = HWPID_IQRF_TECH__DALI_TEMPLATE;
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver = 0x0000;

DpaHandleReturnTRUE:
        return TRUE;
      }
      // -------------------------------------------------
      // Get information about peripheral
      else if ( IsDpaPeripheralInfoRequest() )
      {
        if ( _PNUM == PNUM_STD_DALI )
        {
          _DpaMessage.PeripheralInfoAnswer.PerT = PERIPHERAL_TYPE_STD_DALI;
          _DpaMessage.PeripheralInfoAnswer.PerTE = PERIPHERAL_TYPE_EXTENDED_READ_WRITE;
          // Set standard version
          _DpaMessage.PeripheralInfoAnswer.Par1 = 0;
          goto DpaHandleReturnTRUE;
        }

        break;
      }
      // -------------------------------------------------
      else
      {
        // Handle peripheral command

        // Supported peripheral number?
        if ( _PNUM == PNUM_STD_DALI )
        {
          // Supported commands?
          switch ( _PCMD )
          {
            // Invalid command
            default:
              // Return error
              DpaApiReturnPeripheralError( ERROR_PCMD );

              // Supported commands. They are handled almost the same way
            case PCMD_STD_DALI_SEND_COMMANDS:
            case PCMD_STD_DALI_SEND_COMMANDS_ASYNC:
            {
              // Check the correct data length (non-zero, even and limited by the buffer size)
              if ( _DpaDataLength == 0 || _DpaDataLength > sizeof( DALIbuffer ) || _DpaDataLength % 2 != 0 )
                DpaApiReturnPeripheralError( ERROR_DATA_LEN );

              // Synchronous DALI sending?
              if ( _PCMD == PCMD_STD_DALI_SEND_COMMANDS )
                // Send and receive now, response will always have the same size as request
                SendDALIs( _DpaDataLength );
              else
              {
                // Asynchronous DALI sending
                // Remember the DALI commands for later
                copyMemoryBlock( FSR0, DALIbuffer, sizeof( DALIbuffer ) );
                // and their length
                asyncDpaDataLength = _DpaDataLength;
                // But now return no data
                _DpaDataLength = 0;
              }
              // DPA command was handled
              goto DpaHandleReturnTRUE;
            }
          }
        }

        break;
      }

    case DpaEvent_AfterRouting:
      // Called after Notification and after routing of the DPA response was finished

      // Send DALI asynchronously?
      if ( asyncDpaDataLength != 0 )
      {
        // Send command from the storage buffer
        FSR1 = DALIbuffer;
        SendDALIs( asyncDpaDataLength );
        // Reset the length (asynchronous flag)
        asyncDpaDataLength = 0;
      }
      break;

      // -------------------------------------------------
    case DpaEvent_FrcValue:
      // Called to get FRC value

      // FSR1 for optimization purposes (avoid MOVLB) will be used to point to DataOutBeforeResponseFRC[1...] i.e. to the DALI command
      FSR1 = &DataOutBeforeResponseFRC[1];
      // Check for the correct FRC command and FRC user data
      if ( _PCMD == FRC_STD_DALI_SEND && FSR1[-1] /*DataOutBeforeResponseFRC[0]*/ == PNUM_STD_DALI && FSR1[2] /*DataOutBeforeResponseFRC[3]*/ == 0 )
      {
        // Send one DALI command from FRS1[0...1]
        SendDALIs( sizeof( uns16 ) ); // Note: does FSR1 += 2 internally
        // Copy the answer to the FRC value
        responseFRCvalue2B.high8 = FSR1[0 - sizeof( uns16 )];
        responseFRCvalue2B.low8 = FSR1[1 - sizeof( uns16 )];
      }
      break;

      // -------------------------------------------------
    case DpaEvent_FrcResponseTime:
      // Called to get FRC response time

      if ( DataOutBeforeResponseFRC[0] == FRC_STD_DALI_SEND )
        responseFRCvalue = _FRC_RESPONSE_TIME_40_MS;
      break;
  }

DpaHandleReturnFALSE:
  return FALSE;
}

//############################################################################################
// Resets the timer and its interrupt flag
void ResetTMR()
//############################################################################################
{
  TMR6 = 0;
  TMR6IF = FALSE;
}

//############################################################################################
// Scans DALI RX for the number of changes within the specified time interval
// Returns number of changes and bit7 indicates timeout
uns8 ScanDaliRx( uns8 minPers @ param3.low8, uns8 maxPers @ param3.high8 )
//############################################################################################
{
  TMR6IF = FALSE;

  // Last RX state
  bit lastRX @ param4.low8;
  lastRX = DALI_RX();

  uns8 result @ param4.high8;
  // Number of changes
  result = 0;
  for ( ;; )
  {
    // Timeout occurred?
    if ( TMR6IF || TMR6 > maxPers )
      return result | 0x80;

    // DALI RX value...
    bit daliRx = DALI_RX();
    // ...changed?
    if ( lastRX != daliRx )
    {
      // Remember the RX value
      lastRX = daliRx;
      // Increase number of changes
      result++;
      // If the change was within the time window ...
      if ( TMR6 > minPers )
      {
        // ... reset the timer for the next scan and return
        ResetTMR();
        return result;
      }
    }
  }
}

//############################################################################################
// Wait one Manchester half bit
// Note: must not modify W
void WaitDaliTxHalfBit( void )
//############################################################################################
{
  // delay 416us
  TMR6IF = FALSE;
  TMR6ON = TRUE;
  while ( TMR6IF == 0 );
  TMR6IF = FALSE;
}

//############################################################################################
// Sends Manchester encoded byte 
void DaliTx( uns8 txData @ W )
//############################################################################################
{
  // Note: the whole routine except when working with txData@W must not modify W
  uns8 loop @ param3.high8;
  // Initialize the loop variable without modifying W
  loop = 0;
  loop.3 = 1; // = 8;
  do
  {
    // Send one bit
    if ( txData.7 )
    {
      // Send 1
      DALI_TX0();
      // Wait 1st half bit
      WaitDaliTxHalfBit();
      DALI_TX1();
    }
    else
    {
      // Send 0
      DALI_TX1();
      // Wait 1st half bit
      WaitDaliTxHalfBit();
      DALI_TX0();
    }

    // Wait 2nd half bit
    WaitDaliTxHalfBit();
    // Prepare next bit
    txData <<= 1;
    // More bits?
  } while ( --loop != 0 );
}

//############################################################################################
// Note: length must not be 0
void SendDALIs( uns8 length )
//############################################################################################
{
  for ( ;;)
  {
    clrwdt();

    // ---- TX ----
    // Setup TMR6 for TX
    // prescaler=1, postscaler=8, PRx=208-1, stop timer
    T6CON = 0b0.0111.0.00;
    // interrupt every 416us
    PR6 = 208 - 1;
    TMR6IE = FALSE;
    ResetTMR();

    // TX 2x8 bit data
    // START bit start
    DALI_TX0();
    WaitDaliTxHalfBit();
    // START bit end
    DALI_TX1();
    WaitDaliTxHalfBit();
    // 1st DALI command byte
    DaliTx( FSR1[0] );
    // 2nd DALI command byte
    DaliTx( FSR1[1] );
    // STOP bits start
    DALI_TX1();
    WaitDaliTxHalfBit();
    WaitDaliTxHalfBit();
    WaitDaliTxHalfBit();
    WaitDaliTxHalfBit();

    // ---- RX ----
    // 1st answer byte
    TSTD_DALI_Answer0 answer0;
    answer0 = 0;
    answer0.Always1 = TRUE;

    // 2nd answer byte
    uns8 answer1 = 0;

    // Setup TMR6 for RX or a gap after TX
    // prescaler=64, postscaler=1, PRx=128-1, start timer
    T6CON = 0b0.0000.1.11;
    // interrupt every 2.048ms
    PR6 = 128 - 1;

    // Wait for start bit (must come until 22x double bit rate bit time + 10% error = 10 ms).
    userReg0 = ( 10 / 2.048 + 1 );
    ResetTMR();
    do
    {
      if ( TMR6IF )
      {
        TMR6IF = FALSE;
        if ( --userReg0 == 0 )
          // No start bit => no DALI answer
          goto _NoDaliRx;
      }
      // Start bit?
    } while ( DALI_RX() );

    // End of start bit
    ResetTMR();

    // There is going to be answer
    answer0.Answer = TRUE;

    // Ideal and +-10% scan interval times (Note: values must be below PR6 value)
#define TMRperIdeal		( F_OSC / 4 / ( 1200 * 2 ) / 64 )
#define TMRperMin		( ( TMRperIdeal * 90 ) / 100 )
#define TMRperMax		( ( TMRperIdeal * 110 ) / 100 )

    // One change only
    if ( ScanDaliRx( TMRperMin, TMRperMax ) != 1 )
      // Even when there is an answer error, continue with answer receiving in order to keep timing (gap between DALI forward/backward packets)
      answer0.AnswerError = TRUE;

    // Read data bits
    userReg0 = 8;
    do
    {
      // Next bit? (max. 2 changes)
      if ( ScanDaliRx( 2 * TMRperMin, 2 * TMRperMax ) > 2 )
        // Even when there is an answer error, continue with answer receiving in order to keep timing (gap between DALI forward/backward packets)
        answer0.AnswerError = TRUE;

      // Next data bit read
      userReg1 <<= 1;
      if ( DALI_RX() )
        userReg1.0 = 1;
      // More bits?
    } while ( --userReg0 != 0 );

    // Check for stop bits (max. 1 change + timeout + correct level)
    if ( ScanDaliRx( 4 * TMRperMin, 4 * TMRperMax ) > ( 0x80 | 1 ) || DALI_RX() != 1 )
      answer0.AnswerError = TRUE;

    // There was an OK answer
    if ( !answer0.AnswerError )
      answer1 = userReg1;

_NoDaliRx:

    // Copy both answer bytes and forward the pointer FSR1 to the next possible DALI command
    setINDF1( (uns8)answer0 );
    FSR1++;
    setINDF1( answer1 );
    FSR1++;

    // Decrement data counter
    length -= sizeof( uns16 );
    // More DALI commands to send?
    if ( length == 0 )
      // No, return
      return;

    // Make the gap before the next DALI command only when there was an answer otherwise the gap was already made by waiting for the answer start bit
    if ( answer0.Answer )
    {
      // Wait for 22x double bit rate bit time + 10% error = 10 ms.
      userReg0 = ( 10 / 2.048 + 1 );
      ResetTMR();
      for ( ;; )
        if ( TMR6IF )
        {
          TMR6IF = FALSE;
          if ( --userReg0 == 0 )
            break;
        }
    }
  }
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
