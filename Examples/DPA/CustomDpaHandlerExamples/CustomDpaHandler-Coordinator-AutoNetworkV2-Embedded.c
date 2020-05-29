// *********************************************************************************************
//   Custom DPA Handler code example - Auto network V2 example - coordinator version, embedded *
// *********************************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-Coordinator-AutoNetworkV2-Embedded.c,v $
// Version: $Revision: 1.16 $
// Date:    $Date: 2020/01/03 13:56:50 $
//
// Revision history:
//   2019/03/07  Release for DPA 4.01
//   2019/01/10  Release for DPA 4.00
//   2018/10/25  Release for DPA 3.03
//
// *********************************************************************

// ! IMPORTANT - Please note the Autonetwork process is complex. Before using it in your production environment please understand the process in detail and do thorough test in the lab.
// Please also read on-line DPA documentation https://doc.iqrf.org/DpaTechGuide/ (chapter "8.6.2 AutoNetwork & Coordinator-AutoNetwork-Embedded").

// This example implements automatic network construction from the coordinator point of view. No Custom DPA Handler is needed at Node.

// * Control
// PeripheralRam[0]	= Number of autonetwork waves the handler should run. Zero means stop at the next run. So writing non-zero values starts automatic network construction process.
// PeripheralRam[0].7 = If set, then PeripheralRam[0] is cleared and the autonetwork process is terminated immediately. This might cause the C and Ns are left in the undesired state.
//                      We recommend to stop Autonetwork by writing 0 to PeripheralRam[0] so the currently running wave is nicely finished.
// Note: Writing to PeripheralRam when Autonetwork process runs is not possible when SPI interface is used as the DPA Request to write to Ram is overwritten by AUtonetwrk reports sent to the same interface
//       When UART interface is used, them writing is possible.

// * Configuration:
// PeripheralEEPROM[0] = Discovery TX power <0,7>
// PeripheralEEPROM[1] = Maximum number of empty waves (when no new node was bonded to the network) when the autonetwork process is automatically stopped by writing 0 to PeripheralRam[0]. Value 0 means 256.

// ! Make sure not to write to the other EEPROM locations as there are constant data used by the handler. See below. !

// The application reports its state to the coordinator interface in the form of asynchronous packets with NADR=0x0000, PNUM=0x20, PCMD=0x00 and HWPID=thisHWPID
// 1st data byte is the current state of the algorithm, value corresponds to the TState enumeration
// 2nd and 3rd bytes are optional and at some state specify additional information
// See members of TState enumeration for reporting details

// MID and bonding user data are read from [C] or [N] at the state S_ApproveMid and 4 bytes of MID and 4 bytes of user data (totally 9 bytes) are reported
// to the interface and interface master can (dis)approve new node with temporary address for the future authorization by responding within APPROVE_TIMEOUT timeout.
// Interface master approves node by responding NADR=0, PNUM=0x20, PCMD=<non-zero>, HWPID=0x123F_or_0xFFFF or by not responding within the timeout.
// Interface master disapproves node by responding NADR=0, PNUM=0x20, PCMD=0x00, HWPID=0x123F_or_0xFFFF.

// Notes:
// - Code often ignores error codes from DPA requests to save code Flash space.

// Fixed timeout for the interface master to approve prebonded Nodes for future authorization
#define APPROVE_TIMEOUT	0   // ( 2000 / 10 )

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// This handler works at coordinator only
#define COORDINATOR_CUSTOM_HANDLER
// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

// Reports state (and bytes) to the IFace
void ReportState();
void ReportStateAndBytes( uns8 byte1, uns8 byte2 );
// Returns number of bonded nodes
uns8 BondedNodesCnt();
// Executes local request to PNUM_COORDINATOR
void DpaApiLocalRequestPNUM_COOR( uns8 pcmd, uns8 dataLen );
// Saves starting timeout time
void TimeoutFromNow();
// Prepares data for the interface report
void PrepareReport();
// DpaApiSendToIFaceMaster with zeros
void DpaApiSendToIFaceMaster0_0();
// Decreases 4 bytes unsigned little endian integer pointed by FSR0 
void DecUInt32AtFRS0();

// Number of remaining waves set by user
uns8 RemainingWaves @ PeripheralRam[0];

// Settings stored at EEPROM peripheral from address 0
typedef struct
{
  // Discovery TX power
  uns8	DiscoveryTxPower;
  // Maximum number of empty waves when the autonetwork is stopped automatically
  uns8 MaxEmptyWaves;
} TSettings;

// Initialize parameters to:
//	DiscoveryTxPower = 6
//  MaxEmptyWaves = 2
#pragma cdata[ 0 + __EESTART + PERIPHERAL_EEPROM_START ] = 6, 2

// Unbond the node batch command
#define	UNBOND_EE_ADDR ( sizeof( TSettings ) + PERIPHERAL_EEPROM_START )
#define	NODE_UNBOND_BATCH_LENGTH ( 5 + 5 + 1 )
#pragma cdata[ UNBOND_EE_ADDR + __EESTART ] = \
/* 1st command */ \
5,  /* Remove bond (command length) */ \
PNUM_NODE, \
CMD_NODE_REMOVE_BOND, \
HWPID_DoNotCheck & 0xff, \
HWPID_DoNotCheck >> 8, \
\
/* 2nd command */ \
5, /* Restart node (command length) [not needed at DPA 4.00+ as CMD_NODE_REMOVE_BOND makes restart automatically ]*/ \  
PNUM_OS, \
CMD_OS_RESTART, \
HWPID_DoNotCheck & 0xff, \
HWPID_DoNotCheck >> 8, \
\
0 /* 3rd part is end of batch */

// Macros to read settings from EEPROM
#define	get_DiscoveryTxPower()  eeReadByte( PERIPHERAL_EEPROM_START + offsetof( TSettings, DiscoveryTxPower ) )
#define	get_MaxEmptyWaves()	    eeReadByte( PERIPHERAL_EEPROM_START + offsetof( TSettings, MaxEmptyWaves ) )

// Finite machine state, see TState
uns8 state; // = S_Start 
// Starting timeout time and timeout value
uns16 timeoutFrom, timeoutLength;

// My HWPID
#define	myHWPID	0x123F

// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // States of the state finite machine
  typedef enum
  {
    // Reports state, number of nodes in the network and number of remaining waves (stored at RemainingWaves)
    S_Start = 0x00,
    // Reports state, number of detected prebonded nodes, FRC ID
    S_PrebondedAlive,
    S_PrebondedMIDs,
    // Reports state, 4B MID to approve for every prebonded node
    S_ApproveMID,
    S_WaitApproveMid,
    // Reports state, authorized address, total number of bonded nodes
    S_ApproveMidNext,
    // Reports state, address of the unaccessible unbonded node, total number of bonded nodes
    S_CheckAuthorized,
    S_WaitUnbondNodes,
    // Reports state 
    // Reports state, nodes in the network and number of discovered nodes 
    S_Discovery,
    // Reports state 
    S_Stop
  } TState;

  // Handler presence mark
  clrwdt();

  // Number of new nodes added at one wave
  static uns8 newNodes;
  // Counts waves that did not add any new nodes
  static uns8 emptyWaves;
  // ID used for autonetwork FRCs
  static uns8 frcID;
  // Number of prebonded nodes
  static uns8 prebondedCnt;
  // Offset used for the autonetwork FRC used to read MIDs
  static uns8 frcOffset;
  // Index in the buffer of read MIDs for the approval
  static uns8 approveIndex;
  // TRUE if the MID was disapproved by the master device
  static bit disapproved;
  // Bitmap of the authorized addresses
  static uns8 newNodesBitmap[sizeof( _DpaMessage.PerFrcSendSelective_Request.SelectedNodes )];
  // Bitmap of accessible prebonded nodes read using FRC
  uns8 frcAlive[sizeof( _DpaMessage.PerFrcSendSelective_Request.SelectedNodes )] @ PeripheralRam[1];

  // Detect DPA event to handle (unused event handlers can be commented out or even deleted)
  switch ( GetDpaEvent() )
  {
    // -------------------------------------------------
    case DpaEvent_Idle:
    {
      // Do a quick background work when RF packet is not received

      // If abort bit
      if ( RemainingWaves.7 )
      {
_StopMachine:
        state = S_Stop;
        ReportState();
        // Reset bit and wave counter
        RemainingWaves = 0;
        // Reset machine
        goto _ResetMachine;
      }

      // Run finite machine in the Idle event, jump to the machine state handler
      skip( state );
#pragma computedGoto 1
      goto	_S_Start;
      goto  _S_PrebondedAlive;
      goto  _S_PrebondedMIDs;
      goto  _S_ApproveMID;
      goto  _S_WaitApproveMid;
      goto  _S_ApproveMidNext;
      goto  _S_CheckAuthorized;
      goto  _S_WaitUnbondNodes;
      goto	_S_Discovery;
#pragma computedGoto 0
      ;
      // -------------------------------------------------
_S_Discovery:; // Note: this state handler is placed here, although discovery is the latest state, to save some code as the very last goto @ skip can be avoided
{
  // And finally run discovery only if there are new nodes from the current wave
  if ( newNodes != 0 )
  {
    ReportState();
    // Prepare discovery parameters
    _DpaMessage.PerCoordinatorDiscovery_Request.TxPower = get_DiscoveryTxPower();
    _DpaMessage.PerCoordinatorDiscovery_Request.MaxAddr = 0;
    DpaApiLocalRequestPNUM_COOR( CMD_COORDINATOR_DISCOVERY, sizeof( _DpaMessage.PerCoordinatorDiscovery_Request ) );
    // Report number of nodes
    ReportStateAndBytes( BondedNodesCnt(), _DpaMessage.PerCoordinatorDiscovery_Response.DiscNr );
    // Start counting empty waves
    emptyWaves = 0;
  }
  else
    // Another empty wave
    emptyWaves++;

  // Too many empty waves or no more waves to run?
  if ( get_MaxEmptyWaves() == emptyWaves || RemainingWaves == 0 )
  {
    // Start counting empty waves again next time the autonetwork starts
    emptyWaves = 0;
    goto _StopMachine;
  }

  // Reset machine to the initial state
_ResetMachine:
  W = S_Start;

_W2state:
  // Set machine state to W
  state = W;
  // Escape from the machine
  goto _ExitMachine;
}

// -------------------------------------------------
// All waiting states. They wait till the timeout is over and then switch to the next state.
_S_WaitApproveMid:
_S_WaitUnbondNodes:
{
  // Compute time difference
  GIE = FALSE;
  uns16 timeDiff = timeoutFrom - DpaTicks;
  GIE = TRUE;

  // If timeout is over
  if ( timeDiff >= timeoutLength )
    // Go to the next state
    goto _NextState;

  // Exit machine without state change
  goto _ExitMachine;
}

// -------------------------------------------------
// Machine start state
_S_Start:
{
  // Continue only, when a non-zero number of waves is at RemainingWaves
  if ( RemainingWaves == 0 )
    goto _ExitMachine;

  // Report state, number of nodes, number of remaining waves
  ReportStateAndBytes( BondedNodesCnt(), --RemainingWaves );

  // If network is full, we must stop (instead of a fixed number MAX_ADDRESS a parameter at EEPROM could be used too if needed)
  if ( BondedNodesCnt() >= MAX_ADDRESS )
    goto _StopMachine;

  // Make sure hops optimization is set to DOM
  _DpaMessage.PerCoordinatorSetHops_Request_Response.RequestHops = 0xff;
  _DpaMessage.PerCoordinatorSetHops_Request_Response.ResponseHops = 0xff;
  DpaApiLocalRequestPNUM_COOR( CMD_COORDINATOR_SET_HOPS, sizeof( _DpaMessage.PerCoordinatorSetHops_Request_Response ) );

  // Make sure diagnostics is off
  _DpaMessage.PerCoordinatorSetDpaParams_Request_Response.DpaParam = 0b0000.0.0.00;
  DpaApiLocalRequestPNUM_COOR( CMD_COORDINATOR_SET_DPAPARAMS, sizeof( _DpaMessage.PerCoordinatorSetDpaParams_Request_Response ) );

  // Use SmartConnect for the prebonding
  clearBufferRF();
  _DpaMessage.PerCoordinatorSmartConnect_Request.ReqAddr = TEMPORARY_ADDRESS;
  DpaApiLocalRequestPNUM_COOR( CMD_COORDINATOR_SMART_CONNECT, sizeof( _DpaMessage.PerCoordinatorSmartConnect_Request ) );

  // Go to the next state+1
_NextState:
  state++;

  // Exit machine without state change
_ExitMachine:
  break;
}

// -------------------------------------------------
_S_PrebondedAlive:
{
  // Initialize various indexes, counters, ..
  newNodes = 0;
  frcOffset = 0;
  approveIndex = 0;
  // Empty the bitmap of the authorized nodes
  clearBufferINFO();
  copyMemoryBlock( bufferINFO, newNodesBitmap, sizeof( newNodesBitmap ) );

  // Generate non-zero FRC ID
  if ( ++frcID == 0 )
    frcID++;

  // Use FRC to detected prebonded nodes
  _PNUM = PNUM_FRC;
  _PCMD = CMD_FRC_SEND;
  _DpaMessage.PerFrcSend_Request.UserData[0] = frcID;
  _DpaMessage.PerFrcSend_Request.FrcCommand = FRC_PrebondedAlive;
  _DpaDataLength = sizeof( _DpaMessage.PerFrcSend_Request ) - sizeof( _DpaMessage.PerFrcSend_Request.UserData ) + 2 /* bytes of FRC user data */;
  DpaApiLocalRequest();
  // Remember the bitmap for the 2nd selective FRC 
  copyMemoryBlock( _DpaMessage.PerFrcSend_Response.FrcData, frcAlive, sizeof( frcAlive ) );
  // Remember number of prebonded nodes
  prebondedCnt = _DpaMessage.PerFrcSend_Response.Status;
  // Report the status
  ReportStateAndBytes( prebondedCnt, frcID );
  // If no prebonded nodes, go to discovery (it will actually not start discovery as there are no new nodes)
  if ( prebondedCnt == 0 )
  {
_DoDiscovery:
    // Do discovery
    W = S_Discovery;
    goto _W2state;
  }

  goto _NextState;
}

// -------------------------------------------------
_S_PrebondedMIDs:
{
  // All prebonded MIDs were read?
  if ( frcOffset >= prebondedCnt )
  {
    // Then check they were successfully authorized
    W = S_CheckAuthorized;
    goto _W2state;
  }

  // Read a block of maximum 15 MIDs using the bitmap from the 1st FRC as the selective one
  _PNUM = PNUM_FRC;
  _PCMD = CMD_FRC_SEND_SELECTIVE;
  copyMemoryBlock( frcAlive, _DpaMessage.PerFrcSendSelective_Request.SelectedNodes, sizeof( _DpaMessage.PerFrcSendSelective_Request.SelectedNodes ) );
  _DpaMessage.PerFrcSendSelective_Request.FrcCommand = FRC_PrebondedMemoryReadPlus1;
  _DpaMessage.PerFrcSendSelective_Request.UserData[0] = frcID;
  _DpaMessage.PerFrcSendSelective_Request.UserData[1] = frcOffset;
  // Move offset for the next read
  frcOffset += 15;
  // Address to read 4B from the response of CMD_OS_READ
  _DpaMessage.PerFrcSendSelective_Request.UserData[2] = &_DpaMessage.PerOSRead_Response.MID[0] & 0xFF;
  _DpaMessage.PerFrcSendSelective_Request.UserData[3] = &_DpaMessage.PerOSRead_Response.MID[0] >> 8;
  // Peripheral 
  _DpaMessage.PerFrcSendSelective_Request.UserData[4] = PNUM_OS;
  // Command
  _DpaMessage.PerFrcSendSelective_Request.UserData[5] = CMD_OS_READ;
  // No data
  _DpaMessage.PerFrcSendSelective_Request.UserData[6] = 0;
  _DpaDataLength = sizeof( _DpaMessage.PerFrcSendSelective_Request ) - sizeof( _DpaMessage.PerFrcSendSelective_Request.UserData ) + 2 /* bytes of FRC user data */;
  DpaApiLocalRequest();
  // Remember MIDs at bufferAUX
  copyMemoryBlock( _DpaMessage.PerFrcSend_Response.FrcData, bufferAUX, sizeof( _DpaMessage.PerFrcSend_Response.FrcData ) );
  // Read rest of MIDs and store them at the tail of AUX
  _PCMD = CMD_FRC_EXTRARESULT;
  _DpaDataLength = 0;
  DpaApiLocalRequest();
  copyMemoryBlock( _DpaMessage.Response.PData, &bufferAUX[sizeof( _DpaMessage.PerFrcSend_Response.FrcData )], sizeof( bufferAUX ) - sizeof( _DpaMessage.PerFrcSend_Response.FrcData ) );

  // Initialize approval index
  approveIndex = (uns8)-sizeof( uns32 );
  goto _NextState;
}

// -------------------------------------------------
_S_ApproveMID:
{
  // Approve every MID
  do
  {
    // Compute pointer to MID @ AUX
    setFSR01( _FSR_AUX, _FSR_RF );
    // Prepare index
    approveIndex += sizeof( uns32 );
    FSR0L += approveIndex;
    // MID was read?
    if ( FSR0[0] != 0 || FSR0[1] != 0 || FSR0[2] != 0 || FSR0[3] != 0 )
    {
      // FRC returned MID+1, we must decrease 1
      DecUInt32AtFRS0();
      // Not disapproved yet
      disapproved = FALSE;
      // Prepare report and interface message for the approval
      PrepareReport();
      FSR1 += 1;
      // Copy MID to the report buffer
      copyMemoryBlock( FSR0, FSR1, sizeof( uns32 ) );
      // Size of MID and status byte
      _DpaDataLength = sizeof( uns32 ) + 1;
      // Send message to the interface
      DpaApiSendToIFaceMaster0_0();

      // Preset optional timeout
      timeoutLength = APPROVE_TIMEOUT;
      TimeoutFromNow();
      // Wait for the approval
      goto _NextState;
    }
    // More MIDs to process?
  } while ( approveIndex <= sizeof( bufferAUX ) - sizeof( uns32 ) );
  // Read next block of maximum 15 MIDs 
  W = S_PrebondedMIDs;
  goto _W2state;
}

// -------------------------------------------------
_S_ApproveMidNext:
{
  // Disapproved? We will make the to self unbond and restart by using BROADCAST_ADDRESS as a parameter of CMD_COORDINATOR_AUTHORIZE_BOND
  W = BROADCAST_ADDRESS;
  if ( !disapproved )
  {
    // Approved, it will be authorized
    newNodes++;
    // We will authorize for the next free address
    W = 0;
  }

  // Authorize or unbond+restart. Set the address.
  _DpaMessage.PerCoordinatorAuthorizeBond_Request.ReqAddr = W;
  copyMemoryBlock( bufferAUX + approveIndex, _DpaMessage.PerCoordinatorAuthorizeBond_Request.MID, sizeof( _DpaMessage.PerCoordinatorAuthorizeBond_Request.MID ) );
  DpaApiLocalRequestPNUM_COOR( CMD_COORDINATOR_AUTHORIZE_BOND, sizeof( _DpaMessage.PerCoordinatorAuthorizeBond_Request ) );

  // If authorized, then store the assigned address in the bitmap
  if ( !disapproved )
  {
    addressBitmap( _DpaMessage.PerCoordinatorAuthorizeBond_Response.BondAddr );
    FSR0 = newNodesBitmap;
    FSR0L += bitmapByteIndex; // Note: FSR0L will not overflow
    setINDF0( *FSR0 | bitmapBitMask );
  }
  //  Do report
  ReportStateAndBytes( _DpaMessage.PerCoordinatorAuthorizeBond_Response.BondAddr, _DpaMessage.PerCoordinatorAuthorizeBond_Response.DevNr );
  // Approve and authorize next MID
  W = S_ApproveMID;
  goto _W2state;
}

// -------------------------------------------------
_S_CheckAuthorized:
{
  // Any new authorized nodes to check?
  if ( newNodes != 0 )
  {
    // Detect authorized living nodes using selective FRC
    _PNUM = PNUM_FRC;
    _PCMD = CMD_FRC_SEND_SELECTIVE;
    copyMemoryBlock( newNodesBitmap, _DpaMessage.PerFrcSendSelective_Request.SelectedNodes, sizeof( _DpaMessage.PerFrcSendSelective_Request.SelectedNodes ) );
    _DpaMessage.PerFrcSendSelective_Request.FrcCommand = FRC_Ping;
    _DpaDataLength = sizeof( _DpaMessage.PerFrcSendSelective_Request ) - sizeof( _DpaMessage.PerFrcSendSelective_Request.UserData ) + 2;
    DpaApiLocalRequest();

    // Now find the difference between bitmaps using XOR => create inaccessible Nodes bitmap
    FSR0 = newNodesBitmap;
    FSR1 = _DpaMessage.PerFrcSend_Response.FrcData;
    uns8 loop = sizeof( newNodesBitmap );
    do
    {
      setINDF0( *FSR0 ^ *FSR1++ );
      FSR0++;
    } while ( --loop != 0 );

    // Unbond every unaccessible node at coordinator first
    bit wasUnbonded = FALSE;
    // Address-1 to check
    loop = 0;
    do
    {
      addressBitmap( ++loop );
      FSR0 = newNodesBitmap;
      FSR0L += bitmapByteIndex; // Note: FSR0L will not overflow
      // Node with this address was not accessible?
      if ( *FSR0 & bitmapBitMask )
      {
        // Then unbond it at coordinator
        _DpaMessage.PerCoordinatorRemoveBond_Request.BondAddr = loop;
        DpaApiLocalRequestPNUM_COOR( CMD_COORDINATOR_REMOVE_BOND, sizeof( _DpaMessage.PerCoordinatorRemoveBond_Request ) );
        ReportStateAndBytes( loop, _DpaMessage.PerCoordinatorRemoveBond_Response.DevNr );
        wasUnbonded = TRUE;
        newNodes--;
      }
    } while ( loop != MAX_ADDRESS );

    // If some node was unbonded then unbond all inaccessible Nodes at once and restart them using selective broadcast
    if ( wasUnbonded )
    {
      _NADRhigh = 0;
      _NADR = BROADCAST_ADDRESS;
      _HWPID = HWPID_DoNotCheck;
      _PNUM = PNUM_OS;
      _PCMD = CMD_OS_SELECTIVE_BATCH;
      // Selective bitmap = bitmap of inaccessible Nodes
      copyMemoryBlock( newNodesBitmap, _DpaMessage.PerOSSelectiveBatch_Request.SelectedNodes, sizeof( _DpaMessage.PerOSSelectiveBatch_Request.SelectedNodes ) );
      // Batch to unbond and restart
      eeReadData( UNBOND_EE_ADDR, NODE_UNBOND_BATCH_LENGTH );
      copyMemoryBlock( bufferINFO, _DpaMessage.PerOSSelectiveBatch_Request.Requests, NODE_UNBOND_BATCH_LENGTH );
      _DpaDataLength = NODE_UNBOND_BATCH_LENGTH + sizeof( _DpaMessage.PerOSSelectiveBatch_Request ) - sizeof( _DpaMessage.PerOSSelectiveBatch_Request.Requests );
      // Send request
      DpaApiRfTxDpaPacketCoordinator();

      // Sets timeout to the RTTSLOT * ( RTHOPS + 1 ), i.e forward routing
      timeoutLength = 0;
      RTHOPS++;
      do
      {
        timeoutLength += RTTSLOT;
      } while ( --RTHOPS != 0 );

      TimeoutFromNow();
      goto _NextState;
    }
  }

  goto _DoDiscovery;
}
    }

    // -------------------------------------------------
    case DpaEvent_DpaRequest:
    {
      // Called to interpret DPA request for peripherals
      // -------------------------------------------------
      // Peripheral enumeration
      IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest()
      {
        // We implement 1 user peripheral
        _DpaMessage.EnumPeripheralsAnswer.UserPerNr = 1;
        FlagUserPer( _DpaMessage.EnumPeripheralsAnswer.UserPer, PNUM_USER + 0 );
        _DpaMessage.EnumPeripheralsAnswer.HWPID = myHWPID;
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver = 0x0001;

DpaHandleReturnTRUE:
        return TRUE;
      }
      else
      // -------------------------------------------------
      // Get information about peripheral
      {
      if ( _PNUM == PNUM_USER + 0 )
      {
        _DpaMessage.PeripheralInfoAnswer.PerT |= PERIPHERAL_TYPE_USER_AREA;
        _DpaMessage.PeripheralInfoAnswer.PerTE |= PERIPHERAL_TYPE_EXTENDED_WRITE;
        goto DpaHandleReturnTRUE;
      }

      break;
      }
      // -------------------------------------------------
      // Handle peripheral command
      if ( _PNUM == PNUM_USER + 0 && state == S_WaitApproveMid )
      {
        // Check that there is no data
        if ( _DpaDataLength != 0 )
          DpaApiReturnPeripheralError( ERROR_DATA_LEN );

        // MID not approved
        if ( _PCMD == 0 )
          disapproved = TRUE;

        // Next state to finish waiting for the approval and approve next MID
        state++;

        // Return no response data (there were no request data too)
        goto DpaHandleReturnTRUE;
      }
      break;
    }
  }

  return FALSE;
}

//############################################################################################
// Prepares constant part of the IFace report
// ! DESTROYS existing DPA request/response !
void PrepareReport()
//############################################################################################
{
  _NADR = COORDINATOR_ADDRESS;
  _NADRhigh = 0;
  _PNUM = PNUM_USER;
  _PCMD = 0 | RESPONSE_FLAG;
  _HWPID = myHWPID;
  _DpaMessage.Response.PData[0] = state;
}

//############################################################################################
// Reports state to the IFace
void ReportState()
//############################################################################################
{
  PrepareReport();
  _DpaDataLength = 1;
  DpaApiSendToIFaceMaster0_0();
}

//############################################################################################
// Reports state and 2 extra bytes to the IFace
void ReportStateAndBytes( uns8 byte1 @ param4.low8, uns8 byte2 @ W )
//############################################################################################
{
  _DpaMessage.Response.PData[2] = byte2;
  _DpaMessage.Response.PData[1] = byte1;
  PrepareReport();
  _DpaDataLength = 3;
  DpaApiSendToIFaceMaster0_0();
}

//############################################################################################
// Returns number of bonded nodes
uns8 BondedNodesCnt()
//############################################################################################
{
  return eeReadByte( 0x00 );
}

//############################################################################################
// Execute local DPA request to the coordinator peripheral
void DpaApiLocalRequestPNUM_COOR( uns8 pcmd @ param2, uns8 dataLen @ W )
//############################################################################################
{
  _DpaDataLength = dataLen;
  _PNUM = PNUM_COORDINATOR;
  _PCMD = pcmd;
  DpaApiLocalRequest();
}

//############################################################################################
// Sets timeout start time to the current time (uses 10ms DpaTicks driven by TMR6)
void TimeoutFromNow()
//############################################################################################
{
  GIE = FALSE;
  timeoutFrom = DpaTicks;
  GIE = TRUE;
}

//############################################################################################
void DpaApiSendToIFaceMaster0_0()
//############################################################################################
{
  DpaApiSendToIFaceMaster( 0, 0 );
}

//############################################################################################
void DecUInt32AtFRS0()
//############################################################################################
{
  uns8 prevByte = 0xFF;
  uns8 loop = sizeof( uns32 );
  do
  {
    if ( prevByte == 0xFF )
    {
      setINDF0( *FSR0 - 1 );
      prevByte = *FSR0;
    }
    FSR0++;
  } while ( --loop != 0 );
  FSR0 -= sizeof( uns32 );
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
