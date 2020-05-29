// ****************************************************************************
//   Custom DPA Handler code example - Sending asynchronous request from Node *
// ****************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-AsyncRequest.c,v $
// Version: $Revision: 1.33 $
// Date:    $Date: 2020/01/31 13:28:19 $
//
// Revision history:
//   2020/01/09  Release for DPA 4.12
//   2017/03/13  Release for DPA 3.00
//   2015/08/05  Release for DPA 2.20
//   2014/10/31  Release for DPA 2.10
//   2014/04/30  Release for DPA 2.00
//
// ****************************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Uncomment to implement Custom DPA Handler for Coordinator (actually not needed as we do not used EEPROMs)
//#define COORDINATOR_CUSTOM_HANDLER

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

// This example shows how to send asynchronous request from Node to the coordinator or its master device

// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // Handler presence mark
  clrwdt();

  // Packet ID
  static uns8 pid;

  // Detect DPA event to handle
  switch ( GetDpaEvent() )
  {
#ifdef DpaEvent_Interrupt
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

      return TRUE;
#endif

      // -------------------------------------------------
    case DpaEvent_Idle:
    {
      // Do a background work when RF packet is not received
      uns16 btnCnt = 1000 + 1;
      while ( buttonPressed && --btnCnt != 0 )
        waitMS( 1 );

      // Button was pressed for at least the 100 ms
      if ( btnCnt < 1000 - 100 )
      {
        // Packet ID
        PID = ++pid;
        // Number of hops = my VRN
        RTHOPS = ntwVRN;
        // Fake the last sender in case there was no communication at all from the network before
        bank0 uns8 lastTX @ usedBank0[0x13];
        lastTX = ntwVRN;

        // No DPA Params used
        _DpaParams = 0;

        // Data is sent to the Coordinator
        if ( btnCnt == 0 )
        {
          // To the coordinator master if button pressed > 0.5s
          _NADR = COORDINATOR_ADDRESS;
          pulseLEDR();
        }
        else
        {
          // Otherwise to coordinator the local device
          _NADR = LOCAL_ADDRESS;
          pulseLEDG();
        }
        _NADRhigh = 0;

        // Use IO peripheral to work with LEDs even if LED peripherals are not present
        _PNUM = PNUM_IO;
        // Make a LEDR pulse
        _PCMD = CMD_IO_SET;
        // Any HWPID
        _HWPID = HWPID_DoNotCheck;

        // LEDR=1 => Set PORTA.2 to 1
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[0].Port = PNUM_IO_PORTA;	// PORTA
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[0].Mask = 0b0000.0100;	// bit 2
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[0].Value = 0b0000.0100;	// bit 2 = 1

        // 64 ms pulse
        _DpaMessage.PerIoDirectionAndSet_Request.Delays[1].Header = PNUM_IO_DELAY;  // delay
        _DpaMessage.PerIoDirectionAndSet_Request.Delays[1].Delay = 64;				// 64

        // LEDR=0 => Set PORTA.2 to 0
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[2].Port = PNUM_IO_PORTA;	// PORTA
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[2].Mask = 0b0000.0100;	// bit 2
        _DpaMessage.PerIoDirectionAndSet_Request.Triplets[2].Value = 0b0000.0000;	// bit 2 = 0

        // Setup the correct length of data
        _DpaDataLength = 3 * sizeof( _DpaMessage.PerIoDirectionAndSet_Request.Triplets[0] );

        // Transmit DPA message with DPA Value equal the lastRSSI (can be any other value)
        DpaApiRfTxDpaPacket( lastRSSI, 0x7F );

        // Wait for the button to be released (could be done in cleaner way w/o active waiting at this point of code ...)
        while ( buttonPressed )
          clrwdt();
      }
    }
    break;
  }

DpaHandleReturnFALSE:
  return FALSE;
}
//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################