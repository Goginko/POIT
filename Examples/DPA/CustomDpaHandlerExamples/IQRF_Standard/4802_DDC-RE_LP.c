// *************************************************************************************
//   Custom DPA Handler code example - Standard Binary output - DDC-RE-01 - LP version *
// *************************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: 4802_DDC-RE_LP.c,v $
// Version: $Revision: 1.6 $
// Date:    $Date: 2020/02/20 17:18:59 $
//
// Revision history:
//   2019/03/07  Release for DPA 4.01
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/
// IQRF Standards documentation https://www.iqrfalliance.org/iqrf-interoperability/

// This example also implements 2 binary outputs according to the IQRF Binary Outputs standard
// Index 0 i.e. 1st output is Relay #1 @ DDC-RE-01
// Index 1 i.e. 2nd output is Relay #2 @ DDC-RE-01

// This example must be compiled without a "-bu" compiler switch in order to fit into available Flash memory

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// We can save more instructions if needed by the symbol below
// #define	PARAM_CHECK_LEVEL 1

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"
// IQRF standards header (modify the path according to your setup)
#include "IQRFstandard.h"
#include "IQRF_HWPID.h"

//############################################################################################

// Define useful macro that saves some code but not preset at DPA < 3.01
#if DPA_VERSION_MASTER	< 0x0301
// Optimized macro for both testing enumeration peripherals ELSE peripherals information. See examples
#define	IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequestNoSize() if ( _PCMD == CMD_GET_PER_INFO ) if ( _PNUM == PNUM_ENUMERATION )

#if PARAM_CHECK_LEVEL >= 2
#define IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest() if ( _DpaDataLength == 0 && _PCMD == CMD_GET_PER_INFO ) if ( _PNUM == PNUM_ENUMERATION )
#else
#define	IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest() IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequestNoSize()
#endif
#endif

//############################################################################################

// ms per ticks
#define	TICKS_LEN  10

// Number of implemented binary outputs
#define	OUTPUTS_COUNT 2

// Sets and Gets state of the indexed binary output
void SetOutput( uns8 state, uns8 index );
bit GetOutput( uns8 index );

// DDC-RE-01 relay pins
//  C.5 = C8 = Relay#1
#define	RELAY1_LAT	LATC.5 
#define	RELAY1_TRIS	TRISC.5
//  C.2 = C2 = Relay#2
#define	RELAY2_LAT	LATC.2 
#define	RELAY2_TRIS	TRISC.2

// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // This forces CC5X to wisely use MOVLB instructions (doc says:  The 'default' bank is used by the compiler for loops and labels when the algorithm gives up finding the optimal choice)
#pragma updateBank default = UserBank_01

  // Handler presence mark
  clrwdt();

  // Timers for outputs. The space must be long enough to fit them all. 2+2 bytes per one binary output. 
  //  2B timeout
  //  2B startTicks
  static uns16	Timers[OUTPUTS_COUNT * 2];

  // Detect DPA event to handle
  switch ( GetDpaEvent() )
  {
    // -------------------------------------------------
    case DpaEvent_Interrupt:
      // Do an extra quick background interrupt work

      return Carry;

      // -------------------------------------------------
    case DpaEvent_Idle:
      // Do a quick background work when RF packet is not received

      // Check binary output timers
    {
      // Pointer to the timers array
      FSR1 = (uns16)&Timers[0];
      // Output index
      uns8 index;
      index = 0;
      do
      {
        // Is timer running (is non-zero)?
        if ( ( FSR1[1] | INDF1 ) != 0 )
        {
          // Get timer value
          uns16 timer;
          timer.low8 = FSR1[0];
          timer.high8 = FSR1[1];
          // Get start time
          uns16 timerStart;
          timerStart.low8 = FSR1[2];
          timerStart.high8 = FSR1[3];
          // Measure elapsed time
          captureTicks(); // Note: must not modify FSR1
          param3 -= timerStart;
          // It time over?
          if ( param3 > timer )
          {
            // Set output to OFF
            SetOutput( 0, index );
            // Reset timer
            setINDF1( 0 );
            FSR1++;
            setINDF1( 0 );
            FSR1--;
          }
        }
        // Next timer
        FSR1 += 2 * sizeof( Timers[0] );
        // Next index
      } while ( ++index < OUTPUTS_COUNT );
    }
    break;

    // -------------------------------------------------
    case DpaEvent_Init:
      // Do a one time initialization before main loop starts

      // Initialize ticks
      startCapture();

      // Initialize relays @ DDC-RE
      RELAY1_LAT = 0;
      RELAY2_LAT = 0;
      RELAY1_TRIS = 0;
      RELAY2_TRIS = 0;

      break;

      // -------------------------------------------------
    case DpaEvent_DpaRequest:
      // Called to interpret DPA request for peripherals
      // -------------------------------------------------
      // Peripheral enumeration
      IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest()
      {
        // We implement 2 standard peripherals
        _DpaMessage.EnumPeripheralsAnswer.UserPerNr = 1;
        FlagUserPer( _DpaMessage.EnumPeripheralsAnswer.UserPer, PNUM_STD_BINARY_OUTPUTS );
        _DpaMessage.EnumPeripheralsAnswer.HWPID = HWPID_IQRF_TECH__DEMO_DDC_RE01_LP;
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver |= 0x0000;

DpaHandleReturnTRUE:
        return TRUE;
      }
      // -------------------------------------------------
      // Get information about peripherals
else
      {
      switch ( _DpaMessage.PeripheralInfoAnswer.PerT = _PNUM )
      {
        case PNUM_STD_BINARY_OUTPUTS:
          // Set standard version
          _DpaMessage.PeripheralInfoAnswer.Par1 = 4;
          _DpaMessage.PeripheralInfoAnswer.PerTE = PERIPHERAL_TYPE_EXTENDED_READ_WRITE;
          goto DpaHandleReturnTRUE;
      }

      break;
      }

      {
      // -------------------------------------------------
      // Handle peripheral command

      // Supported peripheral number?
      switch ( _PNUM )
      {
        case PNUM_STD_BINARY_OUTPUTS:
        {
          // Supported commands?
          switch ( _PCMD )
          {
            // Invalid command
            default:
              // Return error
_ERROR_PCMD:
              W = ERROR_PCMD;
              goto _ERROR_W;

              // Outputs enumeration
            case PCMD_STD_ENUMERATE:
              if ( _DpaDataLength != 0 )
                goto _ERROR_DATA_LEN;

              // Return number of outputs
              _DpaMessageIqrfStd.PerStdBinaryOutputEnumerate_Response.Count = OUTPUTS_COUNT;
              W = sizeof( _DpaMessageIqrfStd.PerStdBinaryOutputEnumerate_Response );
_W2_DpaDataLength:
              _DpaDataLength = W;
              goto DpaHandleReturnTRUE;

              // Supported commands.
            case PCMD_STD_BINARY_OUTPUTS_SET:
            {
              // Pointers FSR01 to data are already set at the DPA

              // As this template implements < 9 outputs the working bitmap is uns8, if more outputs are implemented then uns16, ..., uns32 must be used
#if OUTPUTS_COUNT < 9
              uns8 inBitmap = *FSR0--;
              uns8 outBitmap @ _DpaMessageIqrfStd.PerStdBinaryOutputSet_Request.Bitmap[0];
              uns8 bitmapMask = 0b1;
#else
#error Not implemented
#endif

              // Number of selected outputs + bitmap length
              uns8 outputsCount = sizeof( _DpaMessageIqrfStd.PerStdBinaryOutputSet_Request.Bitmap );
              // Loop bitmap
              uns8 index = sizeof( _DpaMessageIqrfStd.PerStdBinaryOutputSet_Request.Bitmap );
              do
              {
                // Count bits of next byte
                uns8 byte = *++FSR0;
                if ( byte != 0 )
                {
                  // Brian Kernighan's Algorithm for counting set bits 
                  do
                  {
                    outputsCount++;
                    byte &= byte - 1;
                  } while ( byte != 0 );
                }

                // Reset bitmap
                setINDF0( 0 );
              } while ( --index != 0 );

              // Check data length
              if ( _DpaDataLength != outputsCount )
              {
_ERROR_DATA_LEN:
                W = ERROR_DATA_LEN;
_ERROR_W:
                DpaApiReturnPeripheralError( W );
              }

              // Pointer to the timers array
              FSR1 = (uns16)&Timers[0];
              // Output index
              index = 0;
              do
              {
                // Output was set?
                if ( GetOutput( index ) )
                  // Yes, set in the output bitmap
                  outBitmap |= bitmapMask;

                // Implemented output selected? Set the state.
                if ( inBitmap.0 )
                {
                  // Default is timer off
                  uns16 time = 0;
                  // Desired state
                  uns8 state = *++FSR0;
                  if ( state > 1 )
                  {
                    // Get time in units s/min
                    time = state & 0x7F;
                    if ( time == 0 )
                    {
                      // Invalid time
                      W = ERROR_FAIL;
_ERROR_FAIL:
                      goto _ERROR_W;
                    }

                    // Conversion coefficient, ready for seconds
                    uns16 coef = 1000 / TICKS_LEN;
                    if ( !state.7 )
                    {
                      // Check for the maximum supported time because of captureTicks method
                      if ( time.low8 > ( (uns24)0xFFFF * TICKS_LEN / 1000 / 60 ) )
                        goto _ERROR_FAIL;

                      // Convert from minutes
                      uns16 coef = 60 * ( 1000 / TICKS_LEN );
                    }

                    // Convert to 250 ms
                    time *= coef;
                    // Set ON
                    state = 1;
                  }

                  // Set output
                  SetOutput( state, index );

                  // Set timer but preserve pointer
                  setINDF1( time.low8 );
                  FSR1++;
                  setINDF1( time.high8 );
                  FSR1++;
                  // Get start time
                  captureTicks(); //Note: must not destroy FSR1
                  setINDF1( param3.low8 );
                  FSR1++;
                  setINDF1( param3.high8 );
                  FSR1 -= 3;
                }

                // Pointer to the next timer
                FSR1 += 2 * sizeof( Timers[0] );
                // Next bits
                bitmapMask <<= 1;
                inBitmap >>= 1;
                // Next index
              } while ( ++index < OUTPUTS_COUNT );

              // Return bitmap
_DpaDataLength4:
              W = sizeof( _DpaMessageIqrfStd.PerStdBinaryOutputSet_Response.PreviousStates );
              goto _W2_DpaDataLength;
            }
          }
        }
      }

      break;
      }
  }
DpaHandleReturnFALSE:
  return FALSE;
}

//############################################################################################
void SetOutput( uns8 state, uns8 index @ W )
//############################################################################################
{
  // Note: FSRs must not be modified
  // Note: This method is called in the interrupt too!

  skip( index );
#pragma computedGoto 1
  goto set0;
  goto set1;
#pragma computedGoto 0
  ;
  // --------------------------------------
set1:
  if ( !state.0 )
    RELAY2_LAT = 0;
  else
    RELAY2_LAT = 1;

  return;
  // --------------------------------------
set0:
  if ( !state.0 )
    RELAY1_LAT = 0;
  else
    RELAY1_LAT = 1;

  return;
  // --------------------------------------
}

//############################################################################################
bit GetOutput( uns8 index @ W )
//############################################################################################
{
  Carry = FALSE; // Note: must not modify W

  // Note: all below must not modify Carry except when needed
  skip( index );
#pragma computedGoto 1
  goto get0;
  goto get1;
#pragma computedGoto 0
  ;
  // --------------------------------------
get1:
  if ( RELAY2_LAT )
    Carry = TRUE;
  goto _return;
  // --------------------------------------
get0:
  if ( RELAY1_LAT )
    Carry = TRUE;
  goto _return;
  // --------------------------------------

_return:
  return Carry;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
