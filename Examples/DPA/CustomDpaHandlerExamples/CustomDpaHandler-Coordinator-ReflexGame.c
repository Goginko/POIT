// **********************************************************************************
//   Custom DPA Handler code example - Simple reflex game controlled by coordinator *
// **********************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-Coordinator-ReflexGame.c,v $
// Version: $Revision: 1.38 $
// Date:    $Date: 2020/02/20 17:18:58 $
//
// Revision history:
//   2019/01/10  Release for DPA 4.00
//   2018/10/25  Release for DPA 3.03
//   2017/03/13  Release for DPA 3.00
//   2015/08/05  Release for DPA 2.20
//   2014/10/31  Release for DPA 2.10
//   2014/04/30  Release for DPA 2.00
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

// Application randomly chooses one of the gaming nodes to test the player's reaction
// It tests whether the button (use e.g. at DK-EVAL-04) of the node is not pressed in advance
// If so it tries to find another node with the button not being pressed
// After a random time it pulses node's LEDR
// If the player presses button within a certain time then LEDG at coordinator goes ON
// If the player does not press the button then LEDR at coordinator goes ON
// The game repeats again
// This example works only at STD mode, not at LP mode

// Max. number of gaming nodes
// The application uses nodes with VRN=1,2, ... ,NODES to play with
#define	NODES			4

// Machine states
typedef enum
{
  // Send button off test
  state_TestOff,
  // Receive button off test
  state_TestOffResponse,
  // Pulse LED
  state_Pulse,
  // Send button on test
  state_Test,
  // Receive button on test
  state_TestResponse,
} TState;

// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // Handler presence mark
  clrwdt();

  // Addresses indexed by VRNs
  static uns8	nodes[NODES];
  // Number of VRNs found
  static uns8	nodesCnt;
  // Random value
  static uns8	rand;
  // Machine state
  static uns8	state;
  // Tested node VRN
  static uns8	nodeVrn;
  // Tested node address
  static uns8	nodeAddr;

  // Detect DPA event to handle
  switch ( GetDpaEvent() )
  {
    // -------------------------------------------------
    case DpaEvent_Init:
      // Do a one time initialization before main loop starts

      // Find nodes with VRN starting 1 up to NODES, break if node not found
      for ( nodesCnt = 0; nodesCnt < NODES; nodesCnt++ )
      {
        // Try to find VRN among all nodes
        for ( RX = 0; RX <= MAX_ADDRESS; RX++ )
        {
          // Node is bonded and discovered
          if ( isBondedNode( RX ) && isDiscoveredNode( RX ) )
          {
            optimizeHops( 0xFF );
            // if VRN matches
            if ( RTHOPS - 1 == nodesCnt )
            {
              // Store its address
              writeToRAM( nodes + nodesCnt, RX );
              // and try to find next VRN
              goto nextVrn;
            }
          }
        }

        // Node with requested VRN not found
        break;

nextVrn:
      }

      // Initialize random seed
      rand = lastRSSI ^ TMR1L;
      if ( rand == 0 )
        rand.0 = 1;

      // Set starting state
      state = state_TestOff;

      // Set starting delay 2s
      GIE = FALSE;
      DpaTicks = 100L * 2;
      GIE = TRUE;

      break;

      // -------------------------------------------------
    case DpaEvent_Idle:
      // Do a quick background work when RF packet is not received

      // Game runs only if interface master is not connected
      if ( IFaceMasterNotConnected )
      {
        // In the meantime generate new and new random values
        rand = lsr( rand );
        W = 0b10111000;
        if ( Carry )
          rand ^= W;

        // We run state machine within Idle event handler
        switch ( state )
        {
          case state_TestOff:
            // Timer is over?
            if ( DpaTicks.15 == 0 )
              break;

            // If no nodes found, then restart pulsing LEDR every 1s
            if ( nodesCnt == 0 )
            {
              pulsingLEDR();
              GIE = FALSE;
              DpaTicks = 1L * 100;
              GIE = TRUE;
              break;
            }

            // Generate random VRN to test
            nodeVrn = rand % nodesCnt;
            nodeVrn++;
            // Get its address
            FSR0 = &nodes[nodeVrn - 1];
            nodeAddr = *FSR0;
            // fall through!

          case state_Test:
            // Wait for the testing DPA request
            if ( DpaTicks.15 == 0 )
              break;

            // DPA request to test the button pin
            _NADR = nodeAddr;
            _NADRhigh = 0;
            // Use IO peripheral
            _PNUM = PNUM_IO;
            // Read GPIOs
            _PCMD = CMD_IO_GET;
            // Any HWPID
            _HWPID = HWPID_DoNotCheck;
            // This DPA request has no data
            _DpaDataLength = 0;
            // Send the DPA request
            DpaApiRfTxDpaPacketCoordinator();
            // Setup safe 1s timeout to recover from not receiving the DPA response at state state_Response
            GIE = FALSE;
            DpaTicks = 1 * 100L;
            GIE = TRUE;
            // Next state
            state++;
            break;

          case state_Pulse:
            // DPA request to pulse LEDR in the tested node by batch (this is done synchronously in the precise time)
            _NADR = nodeAddr;
            _NADRhigh = 0;
            // Use IO peripheral to work with LEDs even if LED peripherals are not present
            _PNUM = PNUM_IO;
            // Make a LEDR pulse
            _PCMD = CMD_IO_SET;
            // Any HWPID
            _HWPID = HWPID_DoNotCheck;

            // LEDR=1 => Set PORTA.2 to 1
            _DpaMessage.PerIoDirectionAndSet_Request.Triplets[0].Port = PNUM_IO_PORTA; // PORTA
            _DpaMessage.PerIoDirectionAndSet_Request.Triplets[0].Mask = 0b0000.0100;	// bit 2
            _DpaMessage.PerIoDirectionAndSet_Request.Triplets[0].Value = 0b0000.0100;	// bit 2 = 1

            // 64 ms pulse
            _DpaMessage.PerIoDirectionAndSet_Request.Delays[1].Header = PNUM_IO_DELAY;  // delay
            _DpaMessage.PerIoDirectionAndSet_Request.Delays[1].Delay = 64;			 // 64

            // LEDR=0 => Set PORTA.2 to 0
            _DpaMessage.PerIoDirectionAndSet_Request.Triplets[2].Port = PNUM_IO_PORTA; // PORTA
            _DpaMessage.PerIoDirectionAndSet_Request.Triplets[2].Mask = 0b0000.0100;	// bit 2
            _DpaMessage.PerIoDirectionAndSet_Request.Triplets[2].Value = 0b0000.0000;	// bit 2 = 0

            // Setup the correct length of data
            _DpaDataLength = 3 * sizeof( _DpaMessage.PerIoDirectionAndSet_Request.Triplets[0] );

            // Send the DPA request
            DpaApiRfTxDpaPacketCoordinator();

            // Set timeout in the way that the testing of the button will be at the same time as possible independently on the node VRN
            GIE = FALSE;
            DpaTicks = 35L + nodeVrn * MIN_STD_TIMESLOT - 2 * MIN_STD_TIMESLOT;
            GIE = TRUE;
            // Next state
            state++;
            break;

          case state_TestOffResponse:
          case state_TestResponse:
            // Did we get DPA response within timeout?
            if ( DpaTicks.15 )
              // No, go to the 1st state
              state = state_TestOff;

            break;
        }
      }

      break;

      // -------------------------------------------------
    case DpaEvent_ReceiveDpaResponse:
      // Called after DPA response was received at coordinator

      // Did we get the testing response?
      if ( _NADR == nodeAddr && _PNUM == PNUM_IO && _PCMD == ( CMD_IO_GET | RESPONSE_FLAG ) )
      {
        if ( state == state_TestResponse )
        {
          // Long pulse
          setOnPulsingLED( 30 );
          //  and the color depends on the state of the button PIN @ PORTA.5
          if ( _DpaMessage.Response.PData[0].5 )
            pulseLEDR();
          else
            pulseLEDG();

startMachine:
          // Set the starting state
          state = state_TestOff;
          // With random time to start new test
          GIE = FALSE;
          DpaTicks = ( 2 * 100L ) + rand / 2;
          GIE = TRUE;

          // By returning true consume the DPA request
DpaHandleReturnTRUE:
          return TRUE;
        }

        if ( state == state_TestOffResponse )
        {
          // Button @ PORTA.5 must be off
          if ( !_DpaMessage.Response.PData[0].5 )
            goto startMachine;

          state = state_Pulse;

          // By returning true consume the DPA request
          goto DpaHandleReturnTRUE;
        }
      }

      break;
  }

  return FALSE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
