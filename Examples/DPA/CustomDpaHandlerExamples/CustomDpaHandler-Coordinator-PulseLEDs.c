// ********************************************************************************
//   Custom DPA Handler code example - Coordinator pulses all LEDs in the network *
// ********************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-Coordinator-PulseLEDs.c,v $
// Version: $Revision: 1.26 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2017/03/13  Release for DPA 3.00
//   2015/08/05  Release for DPA 2.20
//   2014/10/31  Release for DPA 2.10
//   2014/04/30  Release for DPA 2.00
//
// ********************************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Implement Custom DPA Handler for Coordinator
#define COORDINATOR_CUSTOM_HANDLER

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

// This coordinator example periodically pulses all LEDs at the network if the coordinator is not connected to its master

// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
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
    case DpaEvent_Idle:
      // Do a quick background work when RF packet is not received

      // The following block of code demonstrates autonomous sending of packets if the [C] is not connected to the interface master
      // (if the highest bit is set, then DpaTicks overflown from 0 to 0xffff
      if ( IFaceMasterNotConnected && DpaTicks.15 == 1 )
      {
        // Disable interrupts to make sure 16b variable DpaTicks access is atomic
        GIE = FALSE;
        // Setup "timer" for 10 seconds
        DpaTicks = 100L * 10 - 1;
        GIE = TRUE;

        // DPA request is broadcast
        _NADR = BROADCAST_ADDRESS;
        _NADRhigh = 0;
        // Use IO peripheral to work with LEDs even if LED peripherals are not present
        _PNUM = PNUM_IO;
        // Make a both LEDs pulse
        _PCMD = CMD_IO_SET;
        // Any HWPID
        _HWPID = HWPID_DoNotCheck;

        // LEDG=1 => Set PORTB.7 to 1
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[0].Port = PNUM_IO_PORTB; // PORTB
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[0].Mask = 0b1000.0000;	// bit 7
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[0].Value = 0b1000.0000;	// bit 7 = 1

        // LEDR=1 => Set PORTA.2 to 1
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[1].Port = PNUM_IO_PORTA; // PORTA
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[1].Mask = 0b0000.0100;	// bit 2
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[1].Value = 0b0000.0100;	// bit 2 = 1

        // 64 ms delay
        _DpaMessage.PerIoDirectionAndSet_Request.Delays[2].Header = PNUM_IO_DELAY;  // delay
        _DpaMessage.PerIoDirectionAndSet_Request.Delays[2].Delay = 64;				// 64

        // LEDR=0 => Set PORTA.2 to 0
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[3].Port = PNUM_IO_PORTA;	// PORTA
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[3].Mask = 0b0000.0100;	// bit 2
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[3].Value = 0b0000.0000;	// bit 2 = 0

        // LEDG=0 => Set PORTB.7 to 0
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[4].Port = PNUM_IO_PORTB;	// PORTB
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[4].Mask = 0b1000.0000;	// bit 7
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[4].Value = 0b0000.0000;	// bit 7 = 0

        // Setup the correct length of data
        _DpaDataLength = 5 * sizeof( _DpaMessage.PerIoDirectionAndSet_Request.Triplets[0] );
        // Send the DPA request
        DpaApiRfTxDpaPacketCoordinator();

        // Do local a LEDG pulse
        pulseLEDG();
      }

      break;

    case DpaEvent_Init:	// -------------------------------------------------
      // Enable LED during special system actions (discovery, FRC)
      _systemLEDindication = TRUE;
      break;
  }

  return FALSE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
