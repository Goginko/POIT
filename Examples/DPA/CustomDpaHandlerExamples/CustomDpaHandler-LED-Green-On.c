// *******************************************************************************
//   Custom DPA Handler code example - LEDG on                                   *
// *******************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-LED-Green-On.c,v $
// Version: $Revision: 1.15 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2018/10/25  Release for DPA 3.03
//   2017/03/13  Release for DPA 3.00
//   2016/02/03  Release for DPA 2.26
//
// *******************************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Default IQRF header
#include "IQRF.h"

// Default DPA headers
#include "DPA.h"
#include "DPAcustomHandler.h"

// Small diagnostic handler, useful for testing of uploading handlers using CMD_OS_LOAD_CODE

//############################################################################################
bit CustomDpaHandler()
{
  // Handler presence mark
  clrwdt();

  if ( GetDpaEvent() == DpaEvent_Idle )
    setLEDG();
  else if ( GetDpaEvent() == DpaEvent_DpaRequest && IsDpaEnumPeripheralsRequest() )
  {
    _DpaMessage.EnumPeripheralsAnswer.HWPID = 0xEEEF;
    _DpaMessage.EnumPeripheralsAnswer.HWPIDver = 0xEEEF;
    return TRUE;
  }

  return FALSE;
}

//############################################################################################
#include "DPAcustomHandler.h"
//############################################################################################
