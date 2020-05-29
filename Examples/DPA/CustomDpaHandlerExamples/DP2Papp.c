// $Header: /volume1/cvs/cvsroot/microrisc/iqrf/DPA/Examples/DP2Papp.c,v 1.10 2019/10/30 08:53:37 hynek Exp $
//############################################################################################
// Example DP2P TR module application
//############################################################################################

#include "../includes/IQRF.h"
#include "Dpa.h"

// DP2P special (fake PNUM and PCMD) DPA values to indicate that DP2P parameters are specified
#define	DP2P_PNUM	0xDD
#define	DP2P_PCMD	0xDD

// Incoming SPI DP2P packet request
typedef union
{
  // Used to send DP2P requests, then only PNUM+PCMD(+_zero_) fields from the next union field are used to specify DPA request PCMD+PNUN

  /* IQRF IDE Terminal example
   $07$03$00$FE$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$07$00$FF$FF
     PNUM = PNUM_LEDG = $07
     PCMD = CMD_LED_PULSE = $03
     Nodes #1-7 = $FE + 29 x $00
     SlotLength = 0x00 (default values)
     Response RF power = $07
     HWPID 0xFFFF = $FF$FF
  */
  TDP2Prequest DP2Prequest;

  // Used to specify PNUM and PCMD of the DP2P request or to setup DP2P parameters (when PNUM and PCMD equal DP2P_PNUM and DP2P_PCMD respectively)
  struct
  {
    // PNUM
    uns8  PNUM;
    // PCMD
    uns8  PCMD;
    // Zero
    uns8  _zero_;
    // Previous fields overlap Header part of the TDP2Prequest

    // Next fields are used only when PNUM and PCMD equal DP2P_PNUM and DP2P_PCMD respectively

    /* IQRF IDE Terminal example
     $DD$DD$00$01$23$07$05$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00$00
       STD+LP network = $01
       channel 35 = $23
       Request RF power 7 = $07
       RX filter 5 = $05
       AccessPassword = 16 x $00 (i.e. empty, not recommended to use!)
    */

    // Non-zero value when STD+LP network is used, otherwise STD
    uns8  STD_LP_network;
    // Channel to communicate DP2P packets
    uns8  RfChannel;
    // Power to send DP2P request
    uns8  ReqTxPower;
    // Filter to receive DP2P responses
    uns8  RespRxFilter;
    // Access Password to use for de/encryption
    uns8  AccessPassword[sizeof( _DpaMessage.PerOSSetSecurity_Request.Data )];
  } DP2PrequestParams;
} TSPI_TDP2Prequest;

// Outgoing SPI DP2P packet invite (fields must exactly overlap TDP2Invite)
typedef struct
{
  uns8  Header[3];  // 0x000001 (0x00 is used to detect TSPI_TDP2Pinvite, 0xFF specifies TSPI_TDP2Presponse - see below)
  uns8  NADR;
} TSPI_TDP2Pinvite;

// Outgoing SPI DP2P packet response
typedef union
{
  // Real DP2P response but PNUM and PCMD is overlapped, see the next union field
  TDP2Presponse DP2Presponse;

  struct
  {
    // PNUM
    uns8  PNUM;
    // PCMD
    uns8  PCMD;
    // 0xFF (remainder from the DP2P Response, used to detect Response)
    uns8 _0xFF_;
  } DP2PresponseParams;
} TSPI_TDP2Presponse;

/*
IQRF IDE terminal log example
 <> Length Data (comment)
 Tx 23     DD.DD.00.01.23.07.05.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00. (Sets DP2P parameters, see details above)
 Tx 38     07.03.00.FE.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.07.00.FF.FF. (Nodes 1-7 will pulse the LEDG, see details above)
 Rx 4      01.00.00.01. (Invite from Node #1)
 Rx 4      07.83.FF.01. (Response from Node #1)
 Rx 4      01.00.00.02. (Invite from Node #2)
 Rx 4      07.83.FF.02. (Response from Node #2)
 Rx 4      01.00.00.03. (Invite from Node #3)
 Rx 4      07.83.FF.03. (Response from Node #3)
 Rx 4      01.00.00.04. (Invite from Node #4)
 Rx 4      07.83.FF.04. (Response from Node #4)
 Rx 4      01.00.00.06. (Invite from Node #5)
 Rx 4      07.83.FF.05. (Response from Node #5)
 Rx 4      01.00.00.06. (Invite from Node #6)
 Rx 4      07.83.FF.06. (Response from Node #6)
 Rx 4      01.00.00.07. (Invite from Node #7)
 Rx 4      07.83.FF.07. (Response from Node #7)
*/

//############################################################################################
void APPLICATION()
//############################################################################################
{
  // Indicate start
  pulseLEDR();
  pulseLEDG();

  // We use SPI
  enableSPI();

  // Filter used to receive DP2P responses
  uns8 RespRxFilter;
  // RF mode to send DP2P Requests
  uns8 ReqRfMode;

  // Loop forever
  for ( ;; )
  {
    // Anything at SPI received?
    if ( !getStatusSPI() && _SPIRX )
    {
      // No CRC error
      if ( _SPICRCok )
      {
        // Stop SPI
        stopSPI();

        // SPI incoming packet variable
        TSPI_TDP2Prequest SPI_TDP2Prequest @ bufferCOM;
        // Check the packet a little
        if ( SPI_TDP2Prequest.DP2PrequestParams._zero_ == 0 )
        {
          // DP2P parameters to set?
          if ( SPI_TDP2Prequest.DP2PrequestParams.PNUM == DP2P_PNUM && SPI_TDP2Prequest.DP2PrequestParams.PCMD == DP2P_PCMD )
          {
            // Check the length
            if ( SPIpacketLength == sizeof( SPI_TDP2Prequest.DP2PrequestParams ) )
            {
              // Indicate reception
              pulseLEDR();

              // Set RF mode and toutRF according to the network type
              if ( SPI_TDP2Prequest.DP2PrequestParams.STD_LP_network != 0 )
                // STD+LP network
                ReqRfMode = _WPE | _RX_STD | _TX_LP;
              else
                // STD network
                ReqRfMode = _WPE | _RX_STD | _TX_STD | _STDL;

              // Set RF channel
              setRFchannel( SPI_TDP2Prequest.DP2PrequestParams.RfChannel );
              // Set RF power
              setRFpower( SPI_TDP2Prequest.DP2PrequestParams.ReqTxPower );
              // Remember RX filter
              RespRxFilter = SPI_TDP2Prequest.DP2PrequestParams.RespRxFilter;
              // Set Access Password
              copyMemoryBlock( SPI_TDP2Prequest.DP2PrequestParams.AccessPassword, bufferINFO, sizeof( SPI_TDP2Prequest.DP2PrequestParams.AccessPassword ) );
              setAccessPassword();
            }
          }
          else
          {
            // Check the length
            if ( SPIpacketLength >= offsetof( TDP2Prequest, PDATA ) )
            {
              // Send DP2P request

              // Prepare DPA Request
              // Get PNUN
              _PNUM = SPI_TDP2Prequest.DP2PrequestParams.PNUM;
              // Get PCMD
              _PCMD = SPI_TDP2Prequest.DP2PrequestParams.PCMD;
              // DP2P Request variable
              TDP2Prequest DP2Prequest @ bufferRF;
              // Make sure header is correct (there were PNUM and PMCD stored)
              SPI_TDP2Prequest.DP2Prequest.Header[0] = 0;
              SPI_TDP2Prequest.DP2Prequest.Header[1] = 0;
              // Set data length
              PPAR = SPIpacketLength - offsetof( TDP2Prequest, PDATA );
              // Set the DP2P packet length
              DLEN = sizeof( DP2Prequest );
              // Use Access Password for encryption
              encryptByAccessPassword = TRUE;
              // Encrypt the DP2P Request
              copyBufferCOM2RF();
              encryptBufferRF( sizeof( DP2Prequest ) / 16 );
              // Set RF Flags
              PIN = _DPAF_MASK;
              // And finally send DP2P Request
              setRFmode( ReqRfMode );
              RFTXpacket();
              // Indicate reception
              pulseLEDR();
            }
          }
        }
      }
      // Start SPI again
      startSPI( 0 );
    }

    // Ready to receive STD packets
    toutRF = 1;
    setRFmode( _WPE | _RX_STD | _TX_STD );
    // Anything from RF received?
    if ( // There is a signal
         checkRF( RespRxFilter ) &&
         // Packet was received
         RFRXpacket() &&
         // DPA flags are used
         _DPAF &&
         // There is enough data (at least one AES block)
         DLEN >= 16 &&
         // Real packet has a correct AES-128 block length
         ( ( PPAR + 15 ) & ~15 ) == DLEN )
    {
      // Decrypt the packet
      encryptByAccessPassword = TRUE;
      decryptBufferRF( DLEN / 16 );

      // DP2P Invite variable
      TDP2Invite DP2Invite @ bufferRF;
      // Check the DP2P Invite validity
      if ( PPAR == sizeof( TDP2Invite ) && DP2Invite.Header[0] == 1 && ( DP2Invite.Header[1] | DP2Invite.Header[2] ) == 0 )
      {
        // Save N address
        uns8 addr = DP2Invite.NADR;

        // Prepare DP2P Confirm
        TDP2Confirm DP2Confirm @ bufferRF;
        DP2Confirm.Header[0] = 0x03;
        // Encrypt the DP2P Confirm
        encryptByAccessPassword = TRUE;
        encryptBufferRF( sizeof( TDP2Confirm ) / 16 );
        // And finally send DP2P Confirm
        RFTXpacket();
        // Indicate reception
        pulseLEDG();

        // Finish SPI work
        while ( getStatusSPI() );
        stopSPI();
        // Prepare SPI Invite
        TSPI_TDP2Pinvite SPI_TDP2Pinvite @ bufferCOM;
        SPI_TDP2Pinvite.NADR = addr;
        SPI_TDP2Pinvite.Header[0] = 0x01;
        SPI_TDP2Pinvite.Header[1] = 0x00;
        SPI_TDP2Pinvite.Header[2] = 0x00;
        // Send SPI outgoing Invite packet
        startSPI( sizeof( TSPI_TDP2Pinvite ) );
      }
      else
      {
        // DP2P Response variable
        TDP2Presponse DP2Presponse @ bufferRF;
        // Check the DP2P Response validity
        if ( PPAR >= offsetof( TDP2Presponse, PDATA ) && ( DP2Presponse.Header[0] & DP2Presponse.Header[1] & DP2Presponse.Header[2] ) == 0xFF )
        {
          // SPI outgoing Response packet variable
          TSPI_TDP2Presponse SPI_TDP2Presponse @ bufferCOM;
          // Finish SPI work
          while ( getStatusSPI() );
          stopSPI();
          // Copy to COM buffer
          copyBufferRF2COM();
          // Report PNUM and PCMD back via SPI
          SPI_TDP2Presponse.DP2PresponseParams.PNUM = _PNUM;
          SPI_TDP2Presponse.DP2PresponseParams.PCMD = _PCMD;
          // Send response to SPI
          startSPI( PPAR );
          // Indicate reception
          pulseLEDR();
          pulseLEDG();
        }
      }
    }
  }
}

//############################################################################################
