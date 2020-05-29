// ***********************************************************************
//   Custom DPA Handler code example - External device connected to UART *
// ***********************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-UART.c,v $
// Version: $Revision: 1.17 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2019/12/11  Release for DPA 4.11
//   2018/10/25  Release for DPA 3.03
//   2017/03/13  Release for DPA 3.00
//   2016/02/08  Release for DPA 2.26
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

// The application demonstrates simple handler used to connect external device using UART

// Before uploading this example make sure:
// 1. Peripheral UART is enabled at HWP Configuration. It will be automatically opened.

//############################################################################################

// HW Profile ID
#define	HWPid	  0x123F
// HW Profile ID Version
#define	HWPidVer  0x0000

// Bonding button, must be often (depends on used DCTR module) redefined because it might conflict with UART TX signal
#define	IsButton  ( !PORTA.0 ) 

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

      return Carry;

      // Define custom button (un)bonding only when a button is redefined
#ifdef IsButton
      // -------------------------------------------------
    case DpaEvent_BondingButton:
      // Called to allow a bonding button customization 

      userReg1.0 = 0;
      if ( IsButton )
        userReg1.0 = 1;

      goto DpaHandleReturnTRUE;
#endif

      // -------------------------------------------------
    case DpaEvent_DpaRequest:
      // Called to interpret DPA request for peripherals
      // -------------------------------------------------
      // Peripheral enumeration
      if ( IsDpaEnumPeripheralsRequest() )
      {
        // We do not implement user peripheral
        //_DpaMessage.EnumPeripheralsAnswer.UserPerNr = 0;
        // Report HWPID
        _DpaMessage.EnumPeripheralsAnswer.HWPID = HWPid;
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver = HWPidVer;

DpaHandleReturnTRUE:
        return TRUE;
      }
  }

  return FALSE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################