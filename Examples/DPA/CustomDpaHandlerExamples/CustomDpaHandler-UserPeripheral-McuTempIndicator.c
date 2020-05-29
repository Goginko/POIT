// ************************************************************************************************
//   Custom DPA Handler code example - User peripheral implementation - MCU temperature indicator *
// ************************************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-UserPeripheral-McuTempIndicator.c,v $
// Version: $Revision: 1.13 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2017/03/13  Release for DPA 3.00
//   2016/09/12  Release for DPA 2.28
//   2015/08/05  Release for DPA 2.20
//   2014/05/26  Release for DPA 2.11
//   2014/10/31  Release for DPA 2.10
//   2014/05/26  Release for DPA 2.01
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// This example implements the user peripheral reading PIC MCU temperature indicator

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

//############################################################################################

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
    case DpaEvent_DpaRequest:
      // Called to interpret DPA request for peripherals
      // -------------------------------------------------
      // Peripheral enumeration
      if ( IsDpaEnumPeripheralsRequest() )
      {
        // We implement 1 user peripheral
        _DpaMessage.EnumPeripheralsAnswer.UserPerNr = 1;
        FlagUserPer( _DpaMessage.EnumPeripheralsAnswer.UserPer, PNUM_USER + 0 );
        _DpaMessage.EnumPeripheralsAnswer.HWPID = 0x000F;
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver = 0xabcd;

DpaHandleReturnTRUE:
        return TRUE;
      }
      // -------------------------------------------------
      // Get information about peripheral
      else if ( IsDpaPeripheralInfoRequest() )
      {
        if ( _PNUM == PNUM_USER + 0 )
        {
          _DpaMessage.PeripheralInfoAnswer.PerT = PERIPHERAL_TYPE_USER_AREA;
          _DpaMessage.PeripheralInfoAnswer.PerTE = PERIPHERAL_TYPE_EXTENDED_READ;
          goto DpaHandleReturnTRUE;
        }

        break;
      }
      // -------------------------------------------------
      else
      {
        // Handle peripheral command
        if ( _PNUM == PNUM_USER + 0 )
        {
          // Check command
          switch ( _PCMD )
          {
            case 0:
              // -------------------------------------------------
              // Read temperature
              if ( _DpaDataLength != 0 )
                DpaApiReturnPeripheralError( ERROR_DATA_LEN );

              uns16 temperature  @ _DpaMessage.Response.PData;

              // TSRNG: Temperature Indicator Range Selection bit(
              TSRNG = 0;
              // TSEN: Temperature Indicator Enable bit
              TSEN = 1;
              // ADC result 
              // - right justified
              // - Fosc/64
              // - VREF- is connected to VSS
              // - VREF+ is connected to VDD
              ADCON1 = 0b1.110.0.0.00;
              // ADC setting (temp indicator)
              ADCON0 = 0b0.11101.01;
              waitMS( 1 );
              // start ADC
              GO = 1;
              // wait for ADC finish
              while ( GO );

              temperature.low8 = ADRESL;
              temperature.high8 = ADRESH;

              TSEN = 0;

              _DpaDataLength = sizeof( temperature );
              goto DpaHandleReturnTRUE;

            default:
              // -------------------------------------------------
              // Invalid command
              DpaApiReturnPeripheralError( ERROR_PCMD );
          }
        }
      }
  }

  return FALSE;
}

//############################################################################################

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
