// **********************************************************************************
//   Custom DPA Handler code example - Nodes polling                                *
// **********************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-Coordinator-PollNodes.c,v $
// Version: $Revision: 1.20 $
// Date:    $Date: 2020/02/20 17:18:58 $
//
// Revision history:
//   2017/03/13  Release for DPA 3.00
//   2015/08/05  Release for DPA 2.20
//   2014/10/31  Release for DPA 2.10
//
// **********************************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Implement Custom DPA Handler for Coordinator
#define COORDINATOR_CUSTOM_HANDLER

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

// Application periodically sends a request to every node and then it reports a received response to its interface master
// If PeripehralRam[0].0 is set to 1, polling is disabled

// Machine states
typedef enum
{
  // Send request to the node
  state_SendRequest,
  // Wait for the response from node
  state_WaitResponse,
} TState;

// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // Handler presence mark
  clrwdt();

  // Currently polled node
  static uns8 nodeAddr;
  // Machine state
  static uns8 state;

  // Detect DPA event to handle
  switch ( GetDpaEvent() )
  {
    // -------------------------------------------------
    case DpaEvent_Init:
      // Do a one time initialization before main loop starts

      // Next node address will be 1
      nodeAddr = MAX_ADDRESS;
      // Next state is to send request to the next nodeAddr
      state = state_SendRequest;

      // Give GSM GW time to start SPI master
      waitDelay( 100 );

      break;

      // -------------------------------------------------
    case DpaEvent_Idle:
      // Do a quick background work when RF packet is not received

      // What state is the application at?
      switch ( state )
      {
        // Send request to the node
        case state_SendRequest:
          // If bit 0 of PeripheralRam[0] is set, nodes are not polled
          if ( !PeripheralRam[0].0 )
          {
            // Store previous node address
            uns8 oldAddr = nodeAddr;
            for ( ;; )
            {
              // Generate next address, handle overflow
              if ( ++nodeAddr == ( MAX_ADDRESS + 1 ) )
                nodeAddr = 1;

              // Is the node address bonded to the network?
              if ( isBondedNode( nodeAddr ) )
              {
                // Next state is to wait for the response
                state = state_WaitResponse;

                // Send request to the node
                _NADR = nodeAddr;
                _NADRhigh = 0;
                // Use OS peripheral
                _PNUM = PNUM_OS;
                // Read OS
                _PCMD = CMD_OS_READ;
                // Any HWPID
                _HWPID = HWPID_DoNotCheck;
                // This DPA request has no data
                _DpaDataLength = 0;
                // Send the DPA request
                uns16 hopsBack = DpaApiRfTxDpaPacketCoordinator();

                // Indicate DPA Request
                pulseLEDG();

                // Disable interrupts to prevent background change of DpaTicks variable
                GIE = FALSE;
                // Compute total number of hops C>N + N>C + 2
                DpaTicks = (uns16)RTHOPS + hopsBack + 2;
                // Response returns 11 bytes, so it will have the same slot length as request
                DpaTicks *= RTTSLOT;
                // Add extra 0.5s to the timeout
                DpaTicks += 500 / 10;
                // Enable interrupts again
                GIE = TRUE;
                // Exit the loop
                break;
              }

              // If the loop is back, then there is no bonded node
              if ( nodeAddr == oldAddr )
              {
                // Wait for a response that will not happen
                state = state_WaitResponse;
                // and after approx. 10s try to find bonded node again
                GIE = FALSE;
                DpaTicks = 1024;
                GIE = TRUE;
                // Exit the loop
                break;
              }
            }
          }
          break;

        case state_WaitResponse:
          // Timeout occurred?
          if ( DpaTicks.15 != 0 )
            // Yes, request the next node
            state = state_SendRequest;
          break;
      }
      break;

      // -------------------------------------------------
    case DpaEvent_ReceiveDpaResponse:
      // Called after DPA response was received at coordinator

      // Is it a response from the requested node sent by me?
      if ( _NADR == nodeAddr && _PNUM == PNUM_OS && _PCMD == ( CMD_OS_READ | RESPONSE_FLAG ) && NetDepth == 1 )
      {
        // Response received indication
        pulseLEDR();
        // Yes, forward the result to the interface master, DPA value is 0
        DpaApiSendToIFaceMaster( 0, 0 );
        state = state_SendRequest;
        // Request was handled.
        return TRUE;
      }
      break;

      // -------------------------------------------------
    case DpaEvent_DpaRequest:
      // Called to interpret DPA request for peripherals
      // -------------------------------------------------
      // Peripheral enumeration
      if ( IsDpaEnumPeripheralsRequest() )
      {
        _DpaMessage.EnumPeripheralsAnswer.HWPID = 0x000F;
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver = 0xABCD;
        return TRUE;
      }

      break;
  }

  return FALSE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
