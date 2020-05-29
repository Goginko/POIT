// *********************************************************************
//   Custom DPA Handler - XLP long term sleep and wake up              *
// *********************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-XLPstandBy.c,v $
// Version: $Revision: 1.4 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2019/01/10  Release for DPA 4.00
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

//############################################################################################

//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // Handler presence mark
  clrwdt();

  static bit doXLPstandBy;

  // Detect DPA event to handle (unused event handlers can be commented out or even deleted)
  switch ( GetDpaEvent() )
  {
    // -------------------------------------------------
    case DpaEvent_Interrupt:
      // Do an extra quick background interrupt work

      return Carry;

      // -------------------------------------------------
    case DpaEvent_Notification:
      // Called after DPA request was processed and after DPA response was sent

      if ( doXLPstandBy )
      {
        doXLPstandBy = FALSE;

        setNonetMode();
        setRFmode( _WPE | _RX_XLP | _TX_XLP );

        setLEDG();
        setLEDR();
        waitDelay( 100 );

        uns8 cnt;
        for ( ;; )
        {
          cnt++;
          if ( cnt.0 )
            setLEDG();
          else
            setLEDR();
          waitMS( 1 );
          stopLEDR();
          stopLEDG();

          toutRF = 2 * 10;
          if ( RFRXpacket() && DLEN == 1 && bufferRF[0] == 'W' )
          {
            setLEDG();
            setLEDR();
            waitDelay( 100 );
            stopLEDR();
            stopLEDG();
            break;
          }
        }

        DpaApiSetRfDefaults();
        setNodeMode();
      }
      break;

      // -------------------------------------------------
    case DpaEvent_DpaRequest:
      // Called to interpret DPA request for peripherals
      IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest()
      {
        // -------------------------------------------------
        // Peripheral enumeration

        _DpaMessage.EnumPeripheralsAnswer.UserPerNr = 1;
        FlagUserPer( _DpaMessage.EnumPeripheralsAnswer.UserPer, PNUM_USER + 0 );
        _DpaMessage.EnumPeripheralsAnswer.HWPID = 0x581F;

DpaHandleReturnTRUE:
        return TRUE;
      }
      else
      {
        // -------------------------------------------------
        // Get information about peripheral

        if ( _PNUM == PNUM_USER + 0 )
        {
          _DpaMessage.PeripheralInfoAnswer.PerT = PERIPHERAL_TYPE_USER_AREA;
          _DpaMessage.PeripheralInfoAnswer.PerTE = PERIPHERAL_TYPE_EXTENDED_WRITE;
          goto DpaHandleReturnTRUE;
        }

        break;
      }

      // -------------------------------------------------
      // Handle peripheral command

      if ( _PNUM == PNUM_USER + 0 && _PCMD == 0 && _DpaDataLength == 0 )
      {
        if ( ( DpaApiReadConfigByte( CFGIND_DPA_PERIPHERALS + PNUM_COORDINATOR / 8 ) & ( 1 << ( PNUM_COORDINATOR % 8 ) ) ) != 0 )
        {
          // Coordinator
          setNonetMode();
          setRFpower( 7 );
          setRFmode( _TX_XLP );

          PIN = 0;
          DLEN = 1;
          bufferRF[0] = 'W';
          RFTXpacket();

          DpaApiSetRfDefaults();
          setCoordinatorMode();
        }
        else
        {
          // Node
          doXLPstandBy = TRUE;
        }

        goto DpaHandleReturnTRUE;
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
