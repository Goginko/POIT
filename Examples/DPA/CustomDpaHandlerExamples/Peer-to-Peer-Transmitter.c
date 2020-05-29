// *********************************************************************
//   DPA peer-to-peer transmitter example                              *
// *********************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: Peer-to-Peer-Transmitter.c,v $
// Version: $Revision: 1.19 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2019/01/10  Release for DPA 4.00
//   2017/03/13  Release for DPA 3.00
//   2015/08/05  Release for DPA 2.20
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"

// This example transmits peer-to-peer packets to CustomDpaHandler-Peer-to-Peer.c example
// If the RF channel is set to the secondary channel of the main network, then example shows using peer-to-peer packets without build-in DPA support
// !!! It is highly recommended to use additional security techniques (e.g. encryption, rolling code, checksum, CRC) against packet sniffing, spoofing and eavesdropping. 
// !!!  It is also recommended to use listen-before-talk technique to minimize the risk of RF collision that might cause the main network RF traffic to fail.

// Address of the node to receive the peer-to-peer packet (modify according to your setup)
#define	RECEIVER_ADDRESS  1

//############################################################################################
void APPLICATION()
//############################################################################################
{
  // Set RF mode to STD TX, use _TX_LP when used at LP mode!
  setRFmode( _TX_STD | _STDL );
  // setRFmode( _TX_LP );

  // Use the very low RF power
  setRFpower( 0 );

  // Infinite loop
  for ( ;; )
  {
    // Sleep till button is pressed
    sleepWOC();
    if ( buttonPressed )
    {
      // Check if the button is pressed for more than 0.5s
      uns8 loop = 500 / 10;
      do
      {
        // 10 ms wait
        waitDelay( 1 );
      } while ( --loop != 0 && buttonPressed );

      // Prepare default PIN
      PIN = 0;

      // Was just a short (< 0.5s) button press?
      if ( loop != 0 )
      {
        // Indicate green LED
        pulseLEDG();

        // Prepare "DPA" peer-to-peer packet

        // DPA packet fields will be used
#pragma warning Remove this branch
        _DPAF = TRUE;

        // Fill in PNUM and PCMD
        _PNUM = PNUM_LEDG;
        _PCMD = CMD_LED_PULSE;
        // No DPA Data
        _DpaDataLength = 0;
        // DPaParams DPA field will be misused for addressing as the TX and RX field are not available at non-networking packets
        _DpaParams = RECEIVER_ADDRESS;
      }
      else
      {
        // Indicate red LED
        pulseLEDR();

        // Prepare pure peer-to-peer packet 

        // Prepare packet content
        // Header
        bufferRF[0] = 'P';
        // Address
        bufferRF[1] = RECEIVER_ADDRESS;
        // Packet length
        DLEN = 2;
      }

      // Transmit the prepared packet
      RFTXpacket();

      // Wait to allow LED pulse and to debounce button
      waitMS( 40 );
      // Wait till the button is released
      while ( buttonPressed );
    }
  }
}
//############################################################################################

