// *******************************************************************************
//   Custom DPA Handler code example - Demonstrates using of custom FRC commands *
// *******************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-FRC-minimalistic.c,v $
// Version: $Revision: 1.10 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2017/03/13  Release for DPA 3.00
//   2015/08/05  Release for DPA 2.20
//
// *******************************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Default IQRF header
#include "IQRF.h"

// Default DPA headers
#include "DPA.h"
#include "DPAcustomHandler.h"

// Demonstrates the smallest FRC handler

//############################################################################################
bit CustomDpaHandler()
{
  // Handler presence mark
  clrwdt();

  // Return 1 if IQRF button is pressed
  if ( GetDpaEvent() == DpaEvent_FrcValue && _PCMD == FRC_USER_BIT_FROM && buttonPressed )
    responseFRCvalue.1 = 1;

  return FALSE;
}

//############################################################################################
#include "DPAcustomHandler.h"
//############################################################################################
