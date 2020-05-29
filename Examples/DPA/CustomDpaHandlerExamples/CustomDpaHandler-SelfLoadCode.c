// *********************************************************************
//   Custom DPA Handler code example - Self Load Code                  *
// *********************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-SelfLoadCode.c,v $
// Version: $Revision: 1.5 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2018/10/25  Release for DPA 3.03
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// This example demonstrates loading other Custom DPA Handler from the existing one.
// It exists in two variants. The "red" or "green" (see RED below). Based on the variant the corresponding LED is on all the time.
// When a button is pressed the other Custom DPA Handler variant is loaded and started. 
// The example expects that both variants of the example are preloaded at the external EEPROM in the way they can be loaded by CMD_OS_LOAD_CODE command.
// Also, the example expects that there are parameters for CMD_OS_LOAD_CODE for loading the handlers stored at the external EEPROM too.
// Parameters for the "red: variant must be stored at address 0 and for the "green" variant at the address 6 respectively.
// Each set of parameters takes 6 bytes. Parameters have the following format (see https://www.iqrf.org/DpaTechGuide/#3.4.13%20LoadCode for details):
// offset   name          value
// 0        addrLow       lower byte of the corresponding driver .hex image in the external EEPROM
// 1        addrHigh      same as above but higher byte
// 2        lengthLow     lower byte of the corresponding driver .hex image length in the external EEPROM
// 3        lengthHigh    same as above but higher byte
// 4        checksumLow   lower byte of the corresponding driver .hex image checksum in the external EEPROM
// 5        checksumHigh  same as above but higher byte
// 
// IQRF IDE Tools / IQRF Network Manager / Control / Upload can be used to upload both .hex images into the external EEPROM.
// Then Terminal in DPA mode is used to write LoadCode parameters into external EEPROM too.

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

//############################################################################################

// Defined for the "RED" variant otherwise "GREEN" variant is compiled
#define RED

// Based on the variant define the variant specific symbols 
#ifdef RED
#define setLED()              setLEDR()
#define pulseOtherLED()       pulseLEDG()
#define OTHER_HANDLER_ADDRESS 6

#else // #ifdef RED
#define setLED()              setLEDG()
#define pulseOtherLED()       pulseLEDR()
#define OTHER_HANDLER_ADDRESS 0

#endif

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
    case DpaEvent_Idle:
    {
      // Do a quick background work when RF packet is not received

      // Light your LED all the time
      setLED();

      // Variable to save last 8 states of the IQRF button to avoid "pressing" button at CK-USB-04x or a button bouncing in general
      static uns8 lastButton;
      // bit.0 will hold the current button state, so let's make a room for it
      lastButton <<= 1;
      // Set bit.0 if the button is pressed
      if ( buttonPressed )
        lastButton |= 1;

      // Was the button kept pressed for the last 7 Idle events?
      if ( lastButton == ( (uns8)-1 >> 1 ) )
      {
        // Read CMD_OS_LOAD_CODE parameters (address, length, checksum) into bufferINFO for loading the other driver
        eeeReadData( OTHER_HANDLER_ADDRESS );
        // Copy CMD_OS_LOAD_CODE parameters to the correct offset at the DPA request
        memoryOffsetTo = offsetof( TPerOSLoadCode_Request, Address );
        // Copy loaded parameters from bufferINFO into bufferRF where actually _DpaMessage is
        copyBufferINFO2RF();
        // We load custom DPA handler stored at .hex format and want to really load it
        _DpaMessage.PerOSLoadCode_Request.Flags = 0x01;
        // Set peripheral and its command for LoadCode
        _PNUM = PNUM_OS;
        _PCMD = CMD_OS_LOAD_CODE;
        // Length of the DPA request
        _DpaDataLength = sizeof( _DpaMessage.PerOSLoadCode_Request );
        // Perform LoadCode, if it succeeds the device resets
        DpaApiLocalRequest();
        // If the LoadCode failed (reason: .hex not loaded at the external EEPROM memory or the LoadCode parameters stored at external EEPROM are incorrect) ...
        if ( !_DpaMessage.Response.PData[0].0 )
          // Pulse the other LED to indicate error when loading other handler
          pulseOtherLED();
      }

      break;
    }
  }

  return FALSE;
}
//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
