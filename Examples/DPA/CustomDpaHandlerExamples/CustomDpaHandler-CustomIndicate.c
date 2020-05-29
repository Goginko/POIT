// *********************************************************************
//   Custom DPA Handler code example - Custom indication               *
// *********************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-CustomIndicate.c,v $
// Version: $Revision: 1.4 $
// Date:    $Date: 2020/02/27 13:17:18 $
//
// Revision history:
//   2020/02/27  Release for DPA 4.13
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

  // Detect DPA event to handle (unused event handlers can be commented out or even deleted)
  switch ( GetDpaEvent() )
  {
    // -------------------------------------------------
    case DpaEvent_Interrupt:
      // Do an extra quick background interrupt work
      return Carry;

      // -------------------------------------------------
    case DpaEvent_Init:
      // Do a one time initialization before main loop starts

      // Set custom indication GPIO to output
      TRISA.0 = 0;
      break;

      // -------------------------------------------------
    case DpaEvent_Indicate:
      // Called to allow a customization of the device indication

      // userReg1 = Control byte with only valid bit.0-1, bit.2-7 = 0

      // Control == 0 (indication off) ? 
      if ( userReg1 == 0 )
      {
_IndicateOff:
        stopLEDR();
        LATA.0 = 0;
      }
      else
      {
        // Indication on
        setLEDR();
        LATA.0 = 1;

        // Timed indication?
        if ( userReg1.1 )
        {
          // 1 s
          W = 10 * 2;
          // Control != 2
          if ( userReg1.0 )
            // 10 s
            userReg1 = W;
          do
          {
            // 0.5 s step
            waitDelay( 50 );
            clrwdt();
          } while ( --userReg1 != 0 );

          // Switch off the indication
          goto _IndicateOff;
        }
      }

      // return TRUE to skip default indication by LEDR and LEDG (userReg1 might be thus modified)
      return TRUE;

      // -------------------------------------------------
    case DpaEvent_BeforeSleep:
      // Called before going to sleep

      // Switch off custom indication via GPIO
      goto _IndicateOff;
  }

  return FALSE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
