// ********************************************************************************
//   Custom DPA Handler code example - Shows custom bonding using remapped button *
// ********************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-BondingButton.c,v $
// Version: $Revision: 1.2 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2018/??/??  Release for DPA 3.03
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

// This code illustrates bonding using a button connected to the custom pin.
// Except going to sleep the code behaves the same way as the standard bonding procedure.

// Custom bonding button. Change to the pin and an active level of your choice.
#define	IsButton  ( !PORTA.0 )

// The example will not work at demo DPA version because demo DPA does not support the following Custom DPA Handler events: DpaEvent_Interrupt, DpaEvent_BeforeSleep, DpaEvent_AfterSleep, DpaEvent_Reset, DpaEvent_DisableInterrupts, DpaEvent_ReceiveDpaResponse, DpaEvent_IFaceReceive, DpaEvent_ReceiveDpaRequest, DpaEvent_BeforeSendingDpaResponse

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

DpaHandleReturnTRUE:
      return TRUE;

      // -------------------------------------------------
    case DpaEvent_BondingButton:
      // Called to allow a bonding button customization 

      userReg1.0 = 0;
      if ( IsButton )
        userReg1.0 = 1;

      return TRUE;
  }

  return FALSE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
