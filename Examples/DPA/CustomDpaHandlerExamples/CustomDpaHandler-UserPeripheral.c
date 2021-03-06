// *********************************************************************
//   Custom DPA Handler code example - User peripheral implementation  *
// *********************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-UserPeripheral.c,v $
// Version: $Revision: 1.33 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2018/10/25  Release for DPA 3.03
//   2017/03/13  Release for DPA 3.00
//   2015/08/05  Release for DPA 2.20
//   2014/10/31  Release for DPA 2.10
//   2014/04/30  Release for DPA 2.00
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// This example implements the user peripheral.
// It is actually a double color LED peripheral using the same commands as default DPA LED peripherals

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Uncomment to implement Custom DPA Handler for Coordinator
//#define COORDINATOR_CUSTOM_HANDLER

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

      // -------------------------------------------------
    case DpaEvent_DpaRequest:
      // Called to interpret DPA request for peripherals
      // -------------------------------------------------
      // Peripheral enumeration
      IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest()
      {
        // We implement 1 user peripheral
        _DpaMessage.EnumPeripheralsAnswer.UserPerNr = 1;
        FlagUserPer( _DpaMessage.EnumPeripheralsAnswer.UserPer, PNUM_USER + 0 );
        _DpaMessage.EnumPeripheralsAnswer.HWPID = 0x000F;
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver = 0x1234;

DpaHandleReturnTRUE:
        return TRUE;
      }
      // -------------------------------------------------
      // Get information about peripheral
      else
      {
        if ( _PNUM == PNUM_USER + 0 )
        {
          _DpaMessage.PeripheralInfoAnswer.PerT = PERIPHERAL_TYPE_LED;
          _DpaMessage.PeripheralInfoAnswer.PerTE = PERIPHERAL_TYPE_EXTENDED_READ_WRITE;
          _DpaMessage.PeripheralInfoAnswer.Par1 = LED_COLOR_UNKNOWN;
          goto DpaHandleReturnTRUE;
        }

        break;
      }
      // -------------------------------------------------

      // Handle peripheral command
      if ( _PNUM == PNUM_USER + 0 )
      {
        // Check that there is no data
        if ( _DpaDataLength != 0 )
          DpaApiReturnPeripheralError( ERROR_DATA_LEN );

        // Process commands
        switch ( _PCMD )
        {
          // LEDs ON
          case CMD_LED_SET_ON:
            setLEDR();
            setLEDG();
            // Make sure LED is visible at LP mode
            waitMS( 10 );
WriteNoError:
            _DpaDataLength = 0;
            goto DpaHandleReturnTRUE;

            // LEDs OFF
          case CMD_LED_SET_OFF:
            stopLEDR();
            stopLEDG();
            goto WriteNoError;

            // Pulse LEDs 
          case CMD_LED_PULSE:
            pulseLEDR();
            pulseLEDG();
            // Make sure LED is visible at LP mode
            waitMS( 10 );
            goto WriteNoError;

            // Flashing LEDs 
          case CMD_LED_FLASHING:
            pulsingLEDR();
            pulsingLEDG();
            goto WriteNoError;

            // Invalid command
          default:
            DpaApiReturnPeripheralError( ERROR_PCMD );
        }
      }
  }

  return FALSE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
