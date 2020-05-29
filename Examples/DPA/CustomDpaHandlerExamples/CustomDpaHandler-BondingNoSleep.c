// ***************************************************************************
//   Custom DPA Handler code example - Disables sleep during default bonding *
// ***************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-BondingNoSleep.c,v $
// Version: $Revision: 1.5 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2018/10/25  Release for DPA 3.03
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

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
      return Carry;

      // -------------------------------------------------
    case DpaEvent_BondingButton:
      // Called to allow a bonding button customization 

      BondingSleepCountdown = 0;
      break;

    case DpaEvent_DpaRequest:
      // Called to interpret DPA request for peripherals
      // -------------------------------------------------
      // Peripheral enumeration
      if ( IsDpaEnumPeripheralsRequest() )
      {
        // We implement no user peripheral
        _DpaMessage.EnumPeripheralsAnswer.HWPID = 0xabcF;
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver = 0x9876;
        return TRUE;
      }
  }

  return FALSE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
