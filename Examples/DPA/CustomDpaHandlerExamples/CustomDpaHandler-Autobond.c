// *********************************************************************
//   Custom DPA Handler code example - Shows autobonding after reset   *
// *********************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-Autobond.c,v $
// Version: $Revision: 1.32 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2019/06/03  Release for DPA 4.02
//   2018/10/25  Release for DPA 3.03
//   2017/03/13  Release for DPA 3.00
//   2015/08/05  Release for DPA 2.20
//   2014/10/31  Release for DPA 2.10
//   2014/04/30  Release for DPA 2.00
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

// If the node is not bonded, then it tries to bond itself while paying attention not to cause RF collision with other nodes/coordinator

// Random value
static uns8 rand;
// Generates next random value
uns8 Rand();

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
    case DpaEvent_Reset:
      // Called after module is reset

      // Do default unbonding procedure but override bonding one
      if ( !amIBonded() )
      {
        // Indicate start of the bonding by a few rapid LEDG pulses lasting 2s
        pulsingLEDG();
        waitDelay( 200 );
        stopLEDG();

        // Generate non-zero random number (seed)
        moduleInfo();
        rand = bufferINFO[0];
        if ( rand == 0 )
          rand.0 = 1;

        // Gap size doubles with every attempt to run bonding
        uns8 gapSize = 1;
        for ( ;; )
        {
          // Wait random time
          waitMS( Rand() );
          // Indicate the bonding attempt by LEDG
          pulseLEDG();

          // There must be a long no-network traffic interval to try bonding (listen before talk algorithm)
          uns8 loop = 200;
          do
          {
            Rand();
            waitMS( 1 );
            // Check RFIC
            if ( wasRFICrestarted() )
              DpaApiSetRfDefaults();
          } while ( !checkRF( 20 ) && --loop != 0 );

          // Was there no traffic?
          if ( loop == 0 )
          {
            // Yes, try bonding indicated by LEDR
            pulseLEDR();

            // Bonding using 3 service channels
            _3CHTX = TRUE;
            bondRequestAdvanced();
          }

          // Are we bonded?
          if ( amIBonded() )
          {
            // Optionally get the user data
            //copyMemoryBlock( hostUserDataReceived, PeripheralRam, sizeof( hostUserDataReceived ) );
            // Yes, do a long LEDG pulse
            setLEDG();
            waitDelay( 25 );
            stopLEDG();
            // and return 
            goto DpaHandleReturnTRUE;
          }

          // Randomly wait for the next attempt to bond
          loop = gapSize;
          uns8 rndTime = ( Rand() & 0b11111 ) + 10;
          do
          {
            waitMS( rndTime );
          } while ( --loop != 0 );

          // Next gap size
          Carry = 1;
          gapSize = rl( gapSize );
        }
      }

      break;
  }

  return FALSE;
}


//############################################################################################
uns8 Rand()
//############################################################################################
{
  rand = lsr( rand );
  W = 0b10111000;	// x^8 + x^6 + x^5 + x^4 + 1
  if ( Carry )
    rand ^= W;

  return rand;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
