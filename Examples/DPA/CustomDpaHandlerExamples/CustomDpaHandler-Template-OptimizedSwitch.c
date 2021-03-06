// *********************************************************************
//   Custom DPA Handler code template                                  *
// *********************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-Template-OptimizedSwitch.c,v $
// Version: $Revision: 1.26 $
// Date:    $Date: 2020/02/27 13:17:18 $
//
// Revision history:
//   2020/02/27  Release for DPA 4.13
//   2019/01/10  Release for DPA 4.00
//   2017/08/14  Release for DPA 3.01
//   2017/03/13  Release for DPA 3.00
//   2015/08/05  Release for DPA 2.20
//   2014/10/31  Release for DPA 2.10
//   2014/04/30  Release for DPA 2.00
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Uncomment to implement Custom DPA Handler for Coordinator
//#define COORDINATOR_CUSTOM_HANDLER

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

//############################################################################################

// Place for global variables shared among CustomDpaHandler() and other function, otherwise local [static] variables are recommended
// example: uns8 globalCounter;

// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // Handler presence mark
  clrwdt();

  // Place for local static variables used only within CustomDpaHandler() among more events
  // example: static bit interruptOccured;

  // Detect DPA event to handle
  W = DpaEvent_LAST - GetDpaEvent();
  if ( !Carry )
    goto DpaHandleReturnFALSE;

  skip( W );
#pragma computedGoto 1
  // Reverse order of events because of subtraction
  goto _DpaEvent_Indicate;
  goto _DpaEvent_BondingButton;
  goto _DpaEvent_FrcResponseTime;
  goto _DpaEvent_UserDpaValue;
  goto DpaHandleReturnFALSE; // Formerly DpaEvent_AuthorizePreBonding
  goto _DpaEvent_PeerToPeer;
  goto _DpaEvent_BeforeSendingDpaResponse;
  goto _DpaEvent_ReceiveDpaRequest;
  goto _DpaEvent_IFaceReceive;
  goto _DpaEvent_ReceiveDpaResponse;
  goto _DpaEvent_FrcValue;
  goto _DpaEvent_DisableInterrupts;
  goto _DpaEvent_Reset;
  goto _DpaEvent_AfterSleep;
  goto _DpaEvent_BeforeSleep;
  goto _DpaEvent_AfterRouting;
  goto _DpaEvent_Notification;
  goto _DpaEvent_Init;
  goto _DpaEvent_Idle;
  goto _DpaEvent_Interrupt;
  goto _DpaEvent_DpaRequest;
#pragma computedGoto 0
  ;
  // -------------------------------------------------
_DpaEvent_DpaRequest:
  // Called to interpret DPA request for peripherals
  // -------------------------------------------------
  // Peripheral enumeration
  IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest()
  {
    _DpaMessage.EnumPeripheralsAnswer.UserPerNr = 0; // ?
    // FlagUserPer( _DpaMessage.EnumPeripheralsAnswer.UserPer, PNUM_USER + 0 ); // ?
    _DpaMessage.EnumPeripheralsAnswer.HWPID = 0x000F; // ????
    _DpaMessage.EnumPeripheralsAnswer.HWPIDver = 0; // ????

DpaHandleReturnTRUE:
    return TRUE;
  }
  // -------------------------------------------------
  // Get information about peripheral
  else
  {
    if ( _PNUM == PNUM_USER + 0 ) // ?
    {
      _DpaMessage.PeripheralInfoAnswer.PerT = 0; // PERIPHERAL_TYPE_?
      _DpaMessage.PeripheralInfoAnswer.PerTE = 0; // PERIPHERAL_TYPE_EXTENDED_?
      _DpaMessage.PeripheralInfoAnswer.Par1 = 0; // ?
      _DpaMessage.PeripheralInfoAnswer.Par2 = 0; // ?
      goto DpaHandleReturnTRUE;
    }

    goto DpaHandleReturnFALSE;
  }
  // -------------------------------------------------
    // Handle peripheral command
  if ( _PNUM == PNUM_USER + 0 ) // ?
  {
    if ( _PCMD == 0 ) // ????
    {
      goto DpaHandleReturnTRUE;
    }
  }

  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_Interrupt:
#ifdef DpaEvent_Interrupt
  // Do an extra quick background interrupt work
  // ! The time spent handling this event is critical.If there is no interrupt to handle return immediately otherwise keep the code as fast as possible.
  // ! Make sure the event is the 1st _in the main switch statement at the handler routine.This ensures that the event is handled as the 1st one.
  // ! It is desirable that this event is handled with immediate return even if it is not used by the custom handler because the Interrupt event is raised on every MCU interrupt and the �empty� return handler ensures the shortest possible interrupt routine response time.
  // ! Only global variables or local ones marked by static keyword can be used to allow reentrancy.
  // ! Make sure race condition does not occur when accessing those variables at other places.
  // ! Make sure( inspect.lst file generated by C compiler ) compiler does not create any hidden temporary local variable( occurs when using division, multiplication or bit shifts ) at the event handler code.The name of such variable is usually Cnumbercnt.
  // ! Do not call any OS functions except setINDFx().
  // ! Do not use any OS variables especially for writing access.
  // ! All above rules apply also to any other function being called from the event handler code, although calling any function from Interrupt event is not recommended because of additional MCU stack usage.

  return Carry;
#endif
  // -------------------------------------------------
_DpaEvent_Idle:
  // Do a quick background work when RF packet is not received
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_Reset:
  // Called after module is reset
  //goto DpaHandleReturnTRUE; // return TRUE only if you handle node bonding/unbonding
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_BondingButton:
  // Called to allow a bonding button customization 
  //goto DpaHandleReturnTRUE; // return TRUE to handle bonding button
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_Indicate:
  // Called to allow a customization of the device indication
  //goto DpaHandleReturnTRUE; // return TRUE to skip default indication 
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_Init:
  // Do a one time initialization before main loop starts
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_ReceiveDpaRequest:
  // Called after DPA request was received
  //goto DpaHandleReturnTRUE; // return TRUE to skip default processing
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_BeforeSendingDpaResponse:
  // Called before sending DPA response back to originator of DPA response
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_Notification:
  // Called after DPA request was processed and after DPA response was sent
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_AfterRouting:
  // Called after Notification and after routing of the DPA response was finished
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_FrcValue:
#ifdef DpaEvent_FrcValue
  // Called to get FRC value
#endif
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_FrcResponseTime:
#ifdef DpaEvent_FrcResponseTime
  // Called to get FRC response time
#endif
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_BeforeSleep:
#ifdef DpaEvent_BeforeSleep
  // Called before going to sleep
#endif
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_AfterSleep:
#ifdef DpaEvent_AfterSleep
  // Called after woken up after sleep
#endif
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_DisableInterrupts:
  // Called when device needs all hardware interrupts to be disabled (before Reset, Restart, LoadCode, Remove bond and run RFPGM)
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_ReceiveDpaResponse:
#ifdef DpaEvent_ReceiveDpaResponse
  // Called after DPA response was received at coordinator
  //goto DpaHandleReturnTRUE; // return TRUE to skip default processing
#endif
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_IFaceReceive:
#ifdef DpaEvent_IFaceReceive
  // Called after DPA request from interface master was received at coordinator
  //goto DpaHandleReturnTRUE; // return TRUE to skip default processing
#endif
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_PeerToPeer:
  // Called when peer-to-peer (non-networking) packet is received
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------
_DpaEvent_UserDpaValue:
  // Called when DPA is required to return User defined DPA value in the response
  goto DpaHandleReturnFALSE;

  // -------------------------------------------------

DpaHandleReturnFALSE:
  return FALSE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
