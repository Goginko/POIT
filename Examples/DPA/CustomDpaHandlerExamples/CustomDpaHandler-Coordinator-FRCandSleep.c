// **********************************************************************************
//   Custom DPA Handler code example - Reading by FRC                               *
// **********************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-Coordinator-FRCandSleep.c,v $
// Version: $Revision: 1.24 $
// Date:    $Date: 2020/01/03 13:56:50 $
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

// Application periodically sends a FRC request 0x81 and waits till nodes wake up from sleep
// Result is send to the Interface master

// Sleep time between FRC requests, unit is 2.097
#define	SLEEP_TIME	5

// Calculation of the 10ms measurements from the C point of view, add 5% gap
#define	SleepIn10msPlusGap	( ( ( (uns24)SLEEP_TIME ) * ( 2097 * 1.05 ) ) / 10 )

#if SleepIn10msPlusGap >= 0x8000
#error Too long sleep time to be measured by DpaTicks. 
#error Maximum time is 312 s.
#endif

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
    case DpaEvent_Init:

      // Give GSM GW time to start SPI master
      waitDelay( 100 );

      break;

      // -------------------------------------------------
    case DpaEvent_Idle:
      // Do a quick background work when RF packet is not received

      // Time undercount?
      if ( DpaTicks.15 == 1 )
      {
        pulseLEDG();
        // Use FRC peripheral
        _PNUM = PNUM_FRC;
        // Run FRC
        _PCMD = CMD_FRC_SEND;
        // FRC = FRC_AcknowledgedBroadcastBytes
        _DpaMessage.PerFrcSend_Request.FrcCommand = FRC_AcknowledgedBroadcastBytes;
        // Length of DPA Request
        // DPA request length
        _DpaMessage.PerFrcSend_Request.UserData[0] = 8;
        // OS_SLEEP request
        _DpaMessage.PerFrcSend_Request.UserData[1] = PNUM_OS;
        _DpaMessage.PerFrcSend_Request.UserData[2] = CMD_OS_SLEEP;
        // Any HWPID
        _DpaMessage.PerFrcSend_Request.UserData[3] = ( HWPID_DoNotCheck >> 0 ) & 0xFF;
        _DpaMessage.PerFrcSend_Request.UserData[4] = ( HWPID_DoNotCheck >> 8 ) & 0xFF;
        // Sleep time
        _DpaMessage.PerFrcSend_Request.UserData[5] = ( SLEEP_TIME >> 0 ) & 0xFF;;
        _DpaMessage.PerFrcSend_Request.UserData[6] = ( SLEEP_TIME >> 8 ) & 0xFF;
        // LEDG pulse ofter wake up
        _DpaMessage.PerFrcSend_Request.UserData[7] = 0b0.0100;
        // Total length of data
        _DpaDataLength = 8 + sizeof( _DpaMessage.PerFrcSend_Request.FrcCommand );
        // Run FRC
        DpaApiLocalRequest();
        // Send to IFace master
        _NADR = COORDINATOR_ADDRESS;
        _NADRhigh = 0;
        DpaApiSendToIFaceMaster( 0, 0 );

        // Use FRC peripheral
        _PNUM = PNUM_FRC;
        // Run FRC extra result
        _PCMD = CMD_FRC_EXTRARESULT;
        // Total length of data
        _DpaDataLength = 0;
        // Run FRC extra result
        DpaApiLocalRequest();
        // Send to IFace master
        _NADR = COORDINATOR_ADDRESS;
        _NADRhigh = 0;
        DpaApiSendToIFaceMaster( 0, 0 );

        pulseLEDR();

        // Disable interrupts to make sure 16b variable DpaTicks access is atomic
        GIE = FALSE;
        // Setup "timer" for sleep time, unit is 10ms, add 5% safe gap
        DpaTicks = (uns16)SleepIn10msPlusGap;
        GIE = TRUE;
      }
      break;
  }

  return FALSE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
