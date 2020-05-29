// *********************************************************************
//   Custom DPA Handler code example - Multiple response example       *
// *********************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-MultiResponse.c,v $
// Version: $Revision: 1.26 $
// Date:    $Date: 2020/02/20 17:18:58 $
//
// Revision history:
//   2019/01/10  Release for DPA 4.00
//   2017/03/13  Release for DPA 3.00
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// This is an !EXPERIMENTAL EXAMPLE! that shows how to receive responses from more nodes to the one request.
// This example works only at STD mode.
// Because of used uns24 type it requires an extended version of CC5X compiler.

// The example allows to send DPA request to multiple selected nodes (addressed by VRN but NOT by logical address). Addressed nodes then send one by one their responses.
// Please see TRequest structure below. Just broadcast this structure at PDATA top the PNUM=0x20 and PCMD=0x00.
// Only addressed, discovered and routing nodes will respond.

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"
// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

//############################################################################################

typedef struct
{
  // Length of the response slot in 10 ms units. The length must be big enough to fit the longest response from any addressed node.
  // Please see MIN_TIMESLOT and MAX_TIMESLOT.
  uns8	ResponseSlot;
  // Bitmap of the addressed nodes. Each bit represent one node. VrnBitmap[0].1 is VRN=1, VrnBitmap[0].2 is VRN=2, ..., VrnBitmap[29].7 is VRN=239
  // Mapping from logical address to VRN is stored at Coordinator external EEPROM table at address 0x5000. See IQRF OS documentation for more information.
  uns8	VrnBitmap[( MAX_ADDRESS + 7 ) / 8];
  // PNUM of the request
  uns8	PNUM;
  // PCMD of the request
  uns8	PCMD;
  // Optional PDATA 
  uns8	PDATA[0];
} TMultiRequest;

// Request stored at bufferAUX. We can use bufferAUX because the Batch, that internally uses bufferAUX, is not used.
TMultiRequest MultiRequest @ bufferAUX;

#if &_DpaMessage.Request.PData[0] != &bufferRF[0] || &_DpaMessage.Response.PData[0] != &bufferRF[0]
#error We assume DPA data @ bufferRF
#endif

// Indicate response will be sent back
static bit sendResponse;

void CancelResponse();

#define	  myHWPID 0xaAaF

// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // Handler presence mark
  clrwdt();

  // Saved DPA request/response parameters
  static uns8 saveDpaDataLength, savePNUM, savePCMD;
  // Indicate multi-request was send
  static bit multiResponseRequested;
  // Delay to send the response back
  static uns24 responseDelay;

  // Detect DPA event to handle (unused event handlers can be commented out or even deleted)
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

      //  If TMR6 interrupt occurred
      if ( TMR6IF )
      {
        // Unmask interrupt
        TMR6IF = 0;
        // Decrement tick count
        if ( responseDelay != 0 )
          responseDelay--;
        else
          TMR6ON = FALSE;
      }

      return Carry;

      // -------------------------------------------------
    case DpaEvent_Idle:
      // Do a quick background work when RF packet is not received

      // Ready to send the response?
      if ( sendResponse && !TMR6ON )
      {
        // Cancel response
        CancelResponse();
        // Number of hops = my VRN
        RTHOPS = ntwVRN;
        // Just generate new PID
        ++PID;
        // No DPA Params used
        _DpaParams = 0;
        // Send response to the coordinator
        _NADR = COORDINATOR_ADDRESS;
        // Recall stored parameters
        _PNUM = savePNUM;
        _PCMD = savePCMD;
        // Copy stored response data to the right place 
        swapBufferINFO();
        copyBufferINFO2RF();
        // Prepare data length
        _DpaDataLength = saveDpaDataLength;
        // My HWPID
        _HWPID = myHWPID;
        // And finally send the response
        DpaApiRfTxDpaPacket( lastRSSI, 0xff );
      }
      break;

      // -------------------------------------------------
    case DpaEvent_Init:
      // Do a one time initialization before main loop starts

      // Setup TMR6 to generate ticks on the background (ticks every 10ms)
      PR6 = 250 - 1;
      // Prescaler 16, Postscaler 10, 16 * 10 * 250 = 40000 = 4MHz * 10ms
      T6CON = 0b0.1001.0.10;
      TMR6IE = TRUE;
      break;

      // -------------------------------------------------
    case DpaEvent_AfterSleep:
      // Called on wake-up from sleep

      // "Start" timer
      TMR6IE = TRUE;
      break;

      // -------------------------------------------------
    case DpaEvent_BeforeSleep:
      // Called before going to sleep	(the same handling as DpaEvent_DisableInterrupts event)
    case DpaEvent_DisableInterrupts:
      // Called when device needs all hardware interrupts to be disabled (before Reset, Restart, LoadCode, Remove bond, and Run RFPGM)

      // Must not use TMR6 any more
      TMR6ON = FALSE;
      TMR6IE = FALSE;

      // -------------------------------------------------
    case DpaEvent_FrcResponseTime:
      // Called to get FRC response time
    case DpaEvent_FrcValue:
      // Called to get FRC value
    case DpaEvent_ReceiveDpaRequest:
      // Called after DPA request was received

      // Events that will cancel sending response
      CancelResponse();
      break;

      // -------------------------------------------------
    case DpaEvent_AfterRouting:
      // Called after Notification and after routing of the DPA response was finished

      // Is there a multi-response?
      if ( multiResponseRequested )
      {
        // Reset the flag
        multiResponseRequested = FALSE;
        // Delay for the 1st VRN
        responseDelay = MIN_STD_TIMESLOT;
        // Bitmap bit mask
        uns8 mask = 0;
        // Bitmap byte pointer
#if &MultiRequest.VrnBitmap[-1] != &bufferAUX[0]
        FSR0 = &MultiRequest.VrnBitmap[-1];
#else
        setFSR0( _FSR_AUX );
#endif
        // Checked VRN
        uns8 vrn;
        for ( vrn = 0; vrn <= ntwVRN; vrn++ )
        {
          // Next mask
          mask <<= 1;
          // Start with new mask and next byte?
          if ( mask == 0 )
          {
            // New mask
            mask.0 = 1;
            // Next byte
            FSR0++;
          }

          // Is the VRN addressed?
          if ( ( *FSR0 & mask ) != 0 )
          {
            // Is it me?
            if ( vrn == ntwVRN )
            {
              // I am addressed :-), so I will run the request and store a response for later sending
              // Prepare the request
              _PNUM = MultiRequest.PNUM;
              _PCMD = MultiRequest.PCMD;
              setFSR1( _FSR_RF );
              copyMemoryBlock( MultiRequest.PDATA, FSR1, _DpaDataLength = saveDpaDataLength - sizeof( MultiRequest ) );
              // Run the request. It should take the same time at all nodes.
              DpaApiLocalRequest();
              // Store the response parameters
              savePNUM = _PNUM;
              savePCMD = _PCMD;
              saveDpaDataLength = _DpaDataLength;
              // Store the response data to bufferAUX
              copyBufferRF2INFO();
              swapBufferINFO();
              // Delay to send the response is on
              sendResponse = TRUE;
              // Synchronize and start timer
              TMR6 = 0;
              TMR6ON = TRUE;
              break;
            }
            else
            {
              // Delay for the current VRN
              uns8 loop = MultiRequest.ResponseSlot;
              // Loop is faster and shorter than multiplication
              do
              {
                responseDelay += vrn;
              } while ( --loop != 0 );
              // Add some extra time for every addressed VRN before me
              responseDelay += MIN_STD_TIMESLOT;
            }
          }
        }
        // Delay is computed, but I was not addressed :-(
      }
      break;

      // -------------------------------------------------
    case DpaEvent_DpaRequest:
      // Called to interpret DPA request for peripherals

      // Cancel sending multi response
      CancelResponse();

      // -------------------------------------------------
      // Peripheral enumeration
      if ( IsDpaEnumPeripheralsRequest() )
      {
        _DpaMessage.EnumPeripheralsAnswer.UserPerNr = 1;
        FlagUserPer( _DpaMessage.EnumPeripheralsAnswer.UserPer, PNUM_USER + 0 );
        _DpaMessage.EnumPeripheralsAnswer.HWPID = myHWPID;

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
          // Check the command value and other stuff
          if ( _PCMD == 0 &&
               // Data length is enough?
               _DpaDataLength >= sizeof( MultiRequest ) &&
               // Broadcast?
               _NADR == BROADCAST_ADDRESS &&
               // Am I routing?
               ( DpaApiReadConfigByte( CFGIND_DPA_FLAGS ) & 0b1000 ) == 0 &&
               // I do not have temporary address (i.e. no VRN)?
               ntwADDR != TEMPORARY_ADDRESS &&
               // I have a non-zero VRN (I am or I was discovered)
               ntwVRN != 0 &&
               // I am discovered with the last discovery?
               ntwDID == RTDID )
            // Then I can response back
          {
            // Save data length
            saveDpaDataLength = _DpaDataLength;
            // We will process the request after routing is over
            multiResponseRequested = TRUE;
            // Store TMultiRequest to the bufferAUX for lated processing
            copyBufferRF2INFO();
            swapBufferINFO();
            // Request was just OK (because of broadcast the response is not sent)
            goto DpaHandleReturnTRUE;
          }

          // Invalid command for some reason from above
          DpaApiReturnPeripheralError( ERROR_FAIL );
        }
      }

      break;
  }

  return FALSE;
}
//############################################################################################
void CancelResponse()
//############################################################################################
{
  TMR6ON = FALSE;
  sendResponse = FALSE;
}
//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################