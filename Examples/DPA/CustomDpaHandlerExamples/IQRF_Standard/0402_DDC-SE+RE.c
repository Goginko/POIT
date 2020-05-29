// **********************************************************************************************
//   Custom DPA Handler code example - Standard Sensors + Binary output - DDC-SE-01 + DDC-RE-01 *
// **********************************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: 0402_DDC-SE+RE.c,v $
// Version: $Revision: 1.28 $
// Date:    $Date: 2020/02/20 17:18:58 $
//
// Revision history:
//   2020/01/02  Release for DPA 4.11
//   2017/11/16  Release for DPA 3.02
//   2017/08/14  Release for DPA 3.01
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/
// IQRF Standards documentation https://www.iqrfalliance.org/iqrf-interoperability/

// This example implements 3 sensors according to the IQRF Sensors standard
// Index 0 i.e. 1st sensor is either Dallas 18B20 or MCP9802 temperature sensor at DDC-SE-01 board according to the HW jumper position and symbol DALLASnotMCP.
// Index 1 i.e. 2nd sensor is light intensity indicator at DDC-SE-01 board (value range is 0[max light]-127[max dark]).
// Index 2 i.e. 3rd sensor is potentiometer value at DDC-SE-01 board (value range is 0[left stop]-127[right stop]).

// This example also implements 4 binary outputs according to the IQRF Binary Outputs standard
// Index 0 i.e. 1st output is Relay #1 @ DDC-RE-01
// Index 1 i.e. 2nd output is Relay #2 @ DDC-RE-01
// Index 2 i.e. 3rd output is Red LED
// Index 3 i.e. 4th output is Green LED

// This example must be compiled without a "-bu" compiler switch in order to fit into available Flash memory

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// We can save more instructions if needed by the symbol below
// #define	PARAM_CHECK_LEVEL 1

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"
// IQRF standards header (modify the path according to your setup)
#include "IQRFstandard.h"
#include "IQRF_HWPID.h"
// I2C Master library
#include "lib/I2Cmaster.c"

// If defined then the handler is compiled for Dallas otherwise for MCP9802
#define	DALLASnotMCP

//############################################################################################

// Define useful macro that saves some code but not preset at DPA < 3.01
#if DPA_VERSION_MASTER	< 0x0301
// Optimized macro for both testing enumeration peripherals ELSE peripherals information. See examples
#define	IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequestNoSize() if ( _PCMD == CMD_GET_PER_INFO ) if ( _PNUM == PNUM_ENUMERATION )

#if PARAM_CHECK_LEVEL >= 2
#define IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest() if ( _DpaDataLength == 0 && _PCMD == CMD_GET_PER_INFO ) if ( _PNUM == PNUM_ENUMERATION )
#else
#define	IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest() IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequestNoSize()
#endif
#endif

//############################################################################################

// Number of implemented sensors
#define	SENSORS_COUNT 3

// Variable to store sensor value at Get?_????() methods. This example implements sensors returning maximum 2 bytes of data.
uns16 sensorValue @ param3;

// Reads sensor value to the sensorValue variable and to responseFRCvalue(2B) variable
bit Get0_Temperature();
bit Get1_BinaryData_Light();
bit Get2_BinaryData_Potentiometer();

// Stores sensor value byte(s) to the FSR1[+1...], in case of PCMD_STD_SENSORS_READ_TYPES_AND_VALUES sensor type is stored before value byte(s)
void StoreValue( uns8 sensorType );

#ifdef DALLASnotMCP
// Sensor connected to PORT C.3 (compatible with DDC-SE-01)
#define OneWire_TRIS         TRISC.3
#define OneWire_IO_IN        PORTC.3
#define OneWire_IO_OUT       LATC.3

// Writes sensor configuration (resolution)
bit Ds18B20WriteConfig( uns8 value );

// Resets OneWire
bit OneWireReset();
// Reads OneWire byte
uns8 OneWireReadByte();
// Writes OneWire byte
void OneWireWriteByte( uns8 byte );

// DS18B20 commands
#define CMD_READROM		  0x33
#define CMD_CONVERTTEMP	  0x44
#define CMD_CPYSCRATCHPAD 0x48
#define CMD_WSCRATCHPAD	  0x4e
#define CMD_MATCHROM	  0x55
#define CMD_RPWRSUPPLY	  0xb4
#define CMD_RECEEPROM	  0xb8
#define CMD_RSCRATCHPAD	  0xbe
#define CMD_SKIPROM		  0xcc
#define CMD_ALARMSEARCH	  0xec
#define CMD_SEARCHROM	  0xf0

// Final DS18B20 temperature value read by state machine
uns16 temperature;

#else // DALLASnotMCP

// I2C SCL frequency [Hz]
#define I2Cfrequency      50000

// Own implementation with timeout
#define i2c_waitForIdle_REDEFINE

// TRUE if I2C timeout occurred
bit i2cTimeout;

// MCP9802 address
#define I2C_ADR         	0b10010110
// Power pin
#define PWR_SENSOR_TRIS 	TRISC.7
#define PWR_SENSOR_IO   	LATC.7

#endif

//############################################################################################

// Number of implemented binary outputs
#define	OUTPUTS_COUNT 4

// Sets and Gets state of the indexed binary output
void SetOutput( uns8 state, uns8 index );
bit GetOutput( uns8 index );

// DDC-RE-01 relay pins
//  C.5 = C8 = Relay#1
#define	RELAY1_LAT	LATC.5 
#define	RELAY1_TRIS	TRISC.5
//  C.2 = C2 = Relay#2
#define	RELAY2_LAT	LATC.2 
#define	RELAY2_TRIS	TRISC.2

// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // This forces CC5X to wisely use MOVLB instructions (doc says:  The 'default' bank is used by the compiler for loops and labels when the algorithm gives up finding the optimal choice)
#pragma updateBank default = UserBank_01

#ifdef DALLASnotMCP
  // Finite machine states
  typedef enum
  {
    S_ResetConvertT = 0,
    S_SkipRomConvertT,
    S_CmdConvertT,

    S_WaitConvertT,

    S_ResetReadTemp,
    S_SkipRomReadTemp,
    S_CmdReadTemp,
    S_Byte1ReadTemp,
    S_Byte2ReadTemp
  } TState;
#endif

  // Handler presence mark
  clrwdt();

  // Sleeping parameters, valid when Time != 0
  static TPerOSSleep_Request PerOSSleep_Request;

#ifdef DALLASnotMCP
  // Finite machine state
  static uns8 state; // = S_ResetConvertT = 0
  // Pre-read lower temperature byte
  static uns8 temperatureByteLow;
  // Conversion timeout counter
  static uns8 timeout;
#endif

  // Timers for outputs. The space must be long enough to fit them all. 2 bytes per one binary output.
  static uns16	Timers[OUTPUTS_COUNT];

  // Detect DPA event to handle
  switch ( GetDpaEvent() )
  {
    // -------------------------------------------------
    case DpaEvent_Interrupt:
      // Do an extra quick background interrupt work
      // ! The time spent handling this event is critical.If there is no interrupt to handle return immediately otherwise keep the code as fast as possible.
      // ! Make sure the event is the 1st case in the main switch statement at the handler routine.This ensures that the event is handled as the 1st one.
      // ! It is desirable that this event is handled with immediate return even if it is not used by the custom handler because the Interrupt event is raised on every MCU interrupt and the “empty” return handler ensures the shortest possible interrupt routine response time.
      // ! Only global variables or local ones marked by static keyword can be used to allow reentrancy.
      // ! Make sure race condition does not occur when accessing those variables at other places.
      // ! Make sure( inspect.lst file generated by C compiler ) compiler does not create any hidden temporary local variable( occurs when using division, multiplication or bit shifts ) at the event handler code.The name of such variable is usually Cnumbercnt.
      // ! Do not call any OS functions except setINDFx().
      // ! Do not use any OS variables especially for writing access.
      // ! All above rules apply also to any other function being called from the event handler code, although calling any function from Interrupt event is not recommended because of additional MCU stack usage.

      // ms per TMR6 interrupt
#define	TICKS_LEN  10

      //  If TMR6 interrupt occurred
      if ( TMR6IF )
      {
        // Unmask interrupt
        TMR6IF = 0;
#ifdef DALLASnotMCP
        // Decrement count
        if ( timeout != 0 )
          timeout--;
#endif

        // Count 250 ms from 10 ms micro ticks
        static uns8 count250ms;
        if ( ++count250ms == ( 250 / 10 ) )
        {
          // 250 ms
          count250ms = 0;

          // Pointer to the timers array
          FSR1 = (uns16)&Timers[0];
          // Output index
          static uns8 index;
          index = 0;
          do
          {
            // Is timer running (is non-zero)?
            if ( ( FSR1[1] | INDF1 ) != 0 )
            {
              // Get time
              static uns16 time;
              time.low8 = *FSR1++;
              time.high8 = *FSR1;
              // Is timer over?
              if ( --time == 0 )
                // Set output to OFF
                SetOutput( 0, index );

              // Store new time
              setINDF1( time.high8 );
              FSR1--;
              setINDF1( time.low8 );
            }
            // Next timer
            FSR1 += sizeof( Timers[0] );
            // Next index
          } while ( ++index < OUTPUTS_COUNT );
        }
      }
      return Carry;

      // -------------------------------------------------
    case DpaEvent_Idle:
      // Do a quick background work when RF packet is not received

      // Should go to sleep?
      if ( PerOSSleep_Request.Time != 0 )
      {
        // Copy sleep parameters to the DPA request
        _DpaMessage.PerOSSleep_Request.Time = PerOSSleep_Request.Time;
        _DpaMessage.PerOSSleep_Request.Control = PerOSSleep_Request.Control;
        // Finalize OS Sleep DPA Request
        _DpaDataLength = sizeof( _DpaMessage.PerOSSleep_Request );
        _PNUM = PNUM_OS;
        _PCMD = CMD_OS_SLEEP;
        // Perform local DPA Request to go to sleep
        DpaApiLocalRequest();
        // Switch off sleeping time=flag
        PerOSSleep_Request.Time = 0;
      }

#ifdef DALLASnotMCP
      // Run finite state machine to read temperature from DS18B20 at background so the temperature value is immediately ready for FRC 

      // Make sure 1Wire data pin at LATX.y is low as it might be set by another PORTX.? pin manipulation
      OneWire_IO_OUT = 0;

      skip( state );
#pragma computedGoto 1
      goto _S_ResetConvertT;
      goto _S_SkipRomConvertT;
      goto _S_CmdConvertT;
      goto _S_WaitConvertT;
      goto _S_ResetReadTemp;
      goto _S_SkipRomReadTemp;
      goto _S_CmdReadTemp;
      goto _S_Byte1ReadTemp;
      goto _S_Byte2ReadTemp;
#pragma computedGoto 0
      ;
      // --------------
_S_Byte2ReadTemp:
      temperature.high8 = OneWireReadByte();
      temperature.low8 = temperatureByteLow;

ResetMachine:
      state = S_ResetConvertT;
      goto ExitMachine;

      // --------------
_S_ResetConvertT:
_S_ResetReadTemp:
      if ( !OneWireReset() )
      {
_S_Error_Reset:
        STD_SENSOR_TYPE_TEMPERATURE_SET_ERROR( temperature );
        goto ResetMachine;
      }
      goto NextState;

      // --------------
_S_SkipRomConvertT:
_S_SkipRomReadTemp:
      // OneWire: Skip ROM
      OneWireWriteByte( CMD_SKIPROM );
      goto NextState;

      // --------------
_S_CmdConvertT:
      // OneWire: Convert temperature
      OneWireWriteByte( CMD_CONVERTTEMP );
      // Setup timeout for approx 750 ms (the longest conversion time)
      timeout = 2 + 750 / TICKS_LEN;
      goto NextState;

      // --------------
_S_WaitConvertT:
      if ( OneWireReadByte() == 0xff )
        goto NextState;

      // Timeout?
      if ( timeout == 0 )
        goto _S_Error_Reset;

      goto ExitMachine;

      // --------------
_S_CmdReadTemp:
      // OneWire: Read scratchpad
      OneWireWriteByte( CMD_RSCRATCHPAD );
      goto NextState;

      // --------------
_S_Byte1ReadTemp:
      temperatureByteLow = OneWireReadByte();
      goto NextState;

      // --------------
NextState:
      ++state;

ExitMachine:
#endif
      break;

      // -------------------------------------------------
    case DpaEvent_Init:
      // Do a one time initialization before main loop starts

      // Initialize relays @ DDC-RE
      RELAY1_LAT = 0;
      RELAY2_LAT = 0;
      RELAY1_TRIS = 0;
      RELAY2_TRIS = 0;

      // Initialize sensors
      // C5 (AN4) as input 
      moduleInfo();
      // Connected TR pins?
      if ( !bufferINFO[5].7 )
      {
        TRISC.6 = 1;
        TRISB.4 = 1;
      }
      TRISA.5 = 1;

      // C1 (AN0) as input 
      TRISA.0 = 1;

      // Setup TMR6 to generate ticks on the background (ticks every 10ms)
#if F_OSC == 16000000
      PR6 = 250 - 1;
      T6CON = 0b0.1001.1.10;	// Prescaler 16, Postscaler 10, 16 * 10 * 250 = 40000 = 4MHz * 10ms
#else
#error Unsupported oscillator frequency
#endif

#ifdef DALLASnotMCP
      // Setup DS18B20 for 9bit precision, conversion takes 94ms (see datasheet)
      Ds18B20WriteConfig( 0b0.00.00000 );
#else
      // Expect MCP9802 is enabled
      i2c_init();
#endif
      break;

      // -------------------------------------------------
    case DpaEvent_AfterSleep:
      // Called after woken up after sleep
#ifndef DALLASnotMCP
      i2c_init();
#endif

      // Called on wake-up from sleep
      TMR6IE = TRUE;
      TMR6ON = TRUE;
      break;

      // -------------------------------------------------
    case DpaEvent_BeforeSleep:
      // Called before going to sleep
#ifndef DALLASnotMCP
      i2c_shutdown();
#endif

      // -------------------------------------------------
    case DpaEvent_DisableInterrupts:
      // Called when device needs all hardware interrupts to be disabled (before Reset, Restart, LoadCode, Remove bond, and Run RFPGM)
      // Must not use TMR6 any more
      TMR6ON = FALSE;
      TMR6IE = FALSE;
      break;

      // -------------------------------------------------
    case DpaEvent_DpaRequest:
      // Called to interpret DPA request for peripherals
      // -------------------------------------------------
      // Peripheral enumeration
      IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest()
      {
        // We implement 2 standard peripherals
        _DpaMessage.EnumPeripheralsAnswer.UserPerNr = 2;
        FlagUserPer( _DpaMessage.EnumPeripheralsAnswer.UserPer, PNUM_STD_SENSORS );
        FlagUserPer( _DpaMessage.EnumPeripheralsAnswer.UserPer, PNUM_STD_BINARY_OUTPUTS );
        _DpaMessage.EnumPeripheralsAnswer.HWPID = HWPID_IQRF_TECH__DEMO_DDC_SE01_RE01;
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver = 0x0002;

DpaHandleReturnTRUE:
        return TRUE;
      }
      // -------------------------------------------------
      // Get information about peripherals
      else
      {
#if PERIPHERAL_TYPE_STD_SENSORS != PNUM_STD_SENSORS || PERIPHERAL_TYPE_STD_BINARY_OUTPUTS != PNUM_STD_BINARY_OUTPUTS
#error
#endif
        switch ( _DpaMessage.PeripheralInfoAnswer.PerT = _PNUM )
        {
          case PNUM_STD_SENSORS:
            // Set standard version
            W = 13;
            goto Par1toVersion;

          case PNUM_STD_BINARY_OUTPUTS:
            // Set standard version
            W = 4;
Par1toVersion:
            _DpaMessage.PeripheralInfoAnswer.Par1 = W;
            _DpaMessage.PeripheralInfoAnswer.PerTE = PERIPHERAL_TYPE_EXTENDED_READ_WRITE;
            goto DpaHandleReturnTRUE;
        }

        break;
      }

      {
        // -------------------------------------------------
        // Handle peripheral command

        // Supported peripheral number?
        switch ( _PNUM )
        {
          case PNUM_STD_SENSORS:
          {
            // Supported commands?
            switch ( _PCMD )
            {
              // Invalid command
              default:
              {
                // Return error
_ERROR_PCMD:
                W = ERROR_PCMD;
_ERROR_W:
                DpaApiReturnPeripheralError( W );
              }

              // Sensor enumeration
              case PCMD_STD_ENUMERATE:
                if ( _DpaDataLength != 0 )
                  goto _ERROR_DATA_LEN;

                _DpaMessage.Response.PData[0] = STD_SENSOR_TYPE_TEMPERATURE;
                _DpaMessage.Response.PData[1] = STD_SENSOR_TYPE_BINARYDATA7;
                _DpaMessage.Response.PData[2] = STD_SENSOR_TYPE_BINARYDATA7;
                W = SENSORS_COUNT;
                goto _W2_DpaDataLength;

                // Supported commands. They are handled the same way except one "if" at StoreValue() method
              case PCMD_STD_SENSORS_READ_VALUES:
              case PCMD_STD_SENSORS_READ_TYPES_AND_VALUES:
              {
                // No sensor bitmap specified? W = _DpaDataLength. Note: W is used to avoid MOVLB at next if
                W = _DpaDataLength;
                if ( W == 0 )	// Note: must not modify W
                {
                  // Actually clears the bitmap
#if &_DpaMessage.Request.PData[0] != &bufferRF[0]
#error
#endif
                  clearBufferRF();
                  // Simulate 1st only sensor in the bitmap (states of the other unimplemented sensors do not care)
                  _DpaMessage.Request.PData[0].0 = 1;
                  // Bitmap is 32 bits long = 4
                  _DpaDataLength = W = 4;
                }

                // Invalid bitmap (data) length (W = _DpaDataLength)?
                if ( W != 4 )
                  goto _ERROR_DATA_LEN;

                // Now read the sensors

                // Prepare pointer (minus 1, see below) to store sensor (types and) values to
                // Note: 4 sensors at this example cannot return more than DPA_MAX_DATA_LENGTH bytes of data, so it does not have to be checked...
                // ... If it would be the case, then ERROR_FAIL must be returned
                FSR1 = &_DpaMessage.Response.PData[-1];

                // Store bitmap of sensors to get values from
                uns8 sensorsBitmap = FSR1[1];

                // 1st sensor (index 0) selected?
                if ( sensorsBitmap.0 )
                {
                  Get0_Temperature();
                  StoreValue( STD_SENSOR_TYPE_TEMPERATURE );
                }

                // 2nd sensor (index 1) selected?
                if ( sensorsBitmap.1 )
                {
                  Get1_BinaryData_Light();
                  StoreValue( STD_SENSOR_TYPE_BINARYDATA7 );
                }

                // 3rd sensor (index 2) selected?
                if ( sensorsBitmap.2 )
                {
                  Get2_BinaryData_Potentiometer();
                  StoreValue( STD_SENSOR_TYPE_BINARYDATA7 );
                }

                // Compute returned data bytes count
                W = FSR1L - ( (uns16)&_DpaMessage.Response.PData[0] & 0xFF ) + 1;
                // Optimization: return W long block of bytes at response
_W2_DpaDataLength:
                _DpaDataLength = W;
                goto DpaHandleReturnTRUE;
              }
            }
          }

          case PNUM_STD_BINARY_OUTPUTS:
          {
            // Supported commands?
            switch ( _PCMD )
            {
              // Invalid command
              default:
                // Return error
                goto _ERROR_PCMD;

                // Outputs enumeration
              case PCMD_STD_ENUMERATE:
                if ( _DpaDataLength != 0 )
                  goto _ERROR_DATA_LEN;

                // Return number of outputs
                _DpaMessage.Response.PData[0] = OUTPUTS_COUNT;
                W = 1;
                goto _W2_DpaDataLength;

                // Supported commands.
              case PCMD_STD_BINARY_OUTPUTS_SET:
              {
                // Pointers FSR01 to data are already set at the DPA

                // As this template implements < 9 outputs the working bitmap is uns8, if more outputs are implemented then uns16, ..., uns32 must be used
#if OUTPUTS_COUNT < 9
                uns8 inBitmap = *FSR0--;
                uns8 outBitmap @ _DpaMessage.Response.PData[0];
                uns8 bitmapMask = 0b1;
#else
#error Not implemented
#endif

              // Number of selected outputs + bitmap length
                uns8 outputsCount = 4;
                // Loop bitmap
                uns8 index = 4;
                do
                {
                  // Count bits of next byte
                  uns8 byte = *++FSR0;
                  if ( byte != 0 )
                  {
                    // Brian Kernighan's Algorithm for counting set bits 
                    do
                    {
                      outputsCount++;
                      byte &= byte - 1;
                    } while ( byte != 0 );
                  }

                  // Reset bitmap
                  setINDF0( 0 );
                } while ( --index != 0 );

                // Check data length
                if ( _DpaDataLength != outputsCount )
                {
_ERROR_DATA_LEN:
                  W = ERROR_DATA_LEN;
                  goto _ERROR_W;
                }

                // Pointer to the timers array
                FSR1 = (uns16)&Timers[0];
                // Output index
                index = 0;
                do
                {
                  // Output was set?
                  if ( GetOutput( index ) )
                    // Yes, set in the output bitmap
                    outBitmap |= bitmapMask;

                  // Implemented output selected? Set the state.
                  if ( inBitmap.0 )
                  {
                    // Default is timer off
                    uns16 time = 0;
                    // Desired state
                    uns8 state = *++FSR0;
                    if ( state > 1 )
                    {
                      // Get time in units s/min
                      time = state & 0x7F;
                      if ( time == 0 )
                      {
                        // Invalid time
                        W = ERROR_FAIL;
                        goto _ERROR_W;
                      }

                      // Conversion coefficient, ready for minutes to 250 ms
                      uns8 coef = 60 * ( 1000 / 250 );
                      if ( state.7 )
                        // Convert from seconds
                        coef = 1000 / 250;

                      // Convert to 250 ms
                      time *= coef;
                      // Set ON
                      state = 1;
                    }

                    // Set output
                    SetOutput( state, index );

                    // Set timer and preserve pointer
                    GIE = FALSE;
                    setINDF1( time.low8 );
                    FSR1++;
                    setINDF1( time.high8 );
                    FSR1--;
                    GIE = TRUE;
                  }

                  // Next pointer to the timer
                  FSR1 += sizeof( Timers[0] );
                  // Next bits
                  bitmapMask <<= 1;
                  inBitmap >>= 1;
                  // Next index
                } while ( ++index < OUTPUTS_COUNT );

                // Return bitmap
_DpaDataLength4:
                W = 4;
                goto _W2_DpaDataLength;
              }
            }
          }
        }

        break;
      }

      // -------------------------------------------------
    case DpaEvent_FrcValue:
      // Called to get FRC value

      // FSR1 for optimization purposes (avoid MOVLB) will be used to point to DataOutBeforeResponseFRC[0...]
      FSR1 = &DataOutBeforeResponseFRC[0];
      // Check for correct FRC user data
      if ( *FSR1++ /*DataOutBeforeResponseFRC[0]*/ == PNUM_STD_SENSORS )
      {
        // Actually used sensor index
        uns8 sensorIndex = FSR1[1] /*DataOutBeforeResponseFRC[2]*/ & 0x1f;
        // Test sensor type
        switch ( *FSR1++ /*DataOutBeforeResponseFRC[1]*/ )
        {
          default:
            goto DpaHandleReturnFALSE;

            // No type specified, use specified index value
          case 0x00:
            goto _KeepSensorIndex;

            // For other types make the index value based on the requested index value and sensor type
          case STD_SENSOR_TYPE_TEMPERATURE:
            if ( sensorIndex > 0 )
              goto DpaHandleReturnFALSE;
            W = 0 + sensorIndex;
            break;

          case STD_SENSOR_TYPE_BINARYDATA7:
            if ( sensorIndex > 1 )
              goto DpaHandleReturnFALSE;
            W = 1 + sensorIndex;
            break;
        }

        // New sensor index based on type and requested index
        sensorIndex = W;
_KeepSensorIndex:

        // Test for supported FRC commands
        switch ( _PCMD )
        {
          default:
            goto DpaHandleReturnFALSE;

          case FRC_STD_SENSORS_BIT:
          case FRC_STD_SENSORS_1B:
          case FRC_STD_SENSORS_2B:
            switch ( sensorIndex )
            {
              default:
                goto DpaHandleReturnFALSE;

              case 0:
                Carry = Get0_Temperature();
                break;

              case 1:
                Carry = Get1_BinaryData_Light();
                break;

              case 2:
                Carry = Get2_BinaryData_Potentiometer();
                break;
            }

            // This type of FRC is not valid for the specified sensor
            if ( !Carry )
              goto DpaHandleReturnFALSE;

            break;
        }

        // Some sensor was measured by FRC, check if there is a sleep request
        FSR1++;
        if ( INDF1.0 ) // Note: same as DataOutBeforeResponseFRC[3].0
        {
          // Remember sleep parameters to go to sleep at the Idle event later
          PerOSSleep_Request.Time.low8 = FSR1[4 - 3]; // Note: same as DataOutBeforeResponseFRC[4];
          PerOSSleep_Request.Time.high8 = FSR1[5 - 3]; // Note: same as DataOutBeforeResponseFRC[5];
          PerOSSleep_Request.Control = FSR1[6 - 3]; // Note: same as DataOutBeforeResponseFRC[6];
        }
      }

      break;

      // -------------------------------------------------
    case DpaEvent_FrcResponseTime:
      // Called to get FRC response time

      // In this example the FRC commands are fast 
      switch ( DataOutBeforeResponseFRC[0] )
      {
        case FRC_STD_SENSORS_BIT:
        case FRC_STD_SENSORS_1B:
        case FRC_STD_SENSORS_2B:
          responseFRCvalue = _FRC_RESPONSE_TIME_40_MS;
          break;
      }
      break;
  }
DpaHandleReturnFALSE:
  return FALSE;
}

//############################################################################################
bit returnTRUE()
//############################################################################################
{
  return TRUE;
}

//############################################################################################
bit returnFALSE()
//############################################################################################
{
  return FALSE;
}

//############################################################################################
// Increases FSR1 and then stores the byte
void setPlusPlusINDF1( uns8 data @ W )
//############################################################################################
{
  FSR1++; // Note: must not modify W
  setINDF1( data );
}

//############################################################################################
// Stores measured sensor value byte(s) and optionally sensor type to the FSR[+1...]
void StoreValue( uns8 sensorType )
//############################################################################################
{
  // Is the sensor type to be stored too?
  if ( _PCMD == PCMD_STD_SENSORS_READ_TYPES_AND_VALUES )
    setPlusPlusINDF1( sensorType );

  // Store lower value byte
  setPlusPlusINDF1( sensorValue.low8 );

  // No more value bytes to store?
  if ( sensorType.7 != 0 )
    return;

  // Store higher value byte
  setPlusPlusINDF1( sensorValue.high8 );

  // Note: this example implements sensors returning only 1 or 2 bytes of data. If another data widths are returned, then it must be implemented explicitly.
}

//############################################################################################
bit setFRCerror()
//############################################################################################
{
  responseFRCvalue2B = 2;
  return returnTRUE();
}

//############################################################################################
bit sensorError;
bit AdjustFrcTemperature()
//############################################################################################
{
  // Test for supported FRC commands
  switch ( _PCMD )
  {
    default:
      return returnFALSE();

    case FRC_STD_SENSORS_1B:
      // Return sensor FRC value 1B
      // Check for out of limits
      if ( sensorError || (int16)sensorValue > (int16)( 105.5 * 16 ) || (int16)sensorValue < ( (int16)-20 * 16 ) )
        return setFRCerror();

      // Convert to the "F = ( T + 22 ) * 2 " from 1/16 resolution
      uns16 _sensorValue = sensorValue + 4;	// Note: do rounding when /8
      responseFRCvalue = (uns8)( _sensorValue / 8 ) + 44;
      break;

    case FRC_STD_SENSORS_2B:
      // Return sensor FRC value 2B
      if ( sensorError )
        return setFRCerror();

      responseFRCvalue2B = sensorValue ^ 0x8000;
      break;
  }

  return returnTRUE();
}

//############################################################################################
// Sensor index 1: measure temperature using one of the DDC-SE-01 sensors
bit Get0_Temperature()
//############################################################################################
{
  // Make sure FSR1 is not modified

  // Measure temperature using DDC-SE-01 sensors
  // Read temperature and check for an error

  // Reads temperature from an enabled sensor
#ifdef DALLASnotMCP
  sensorError = FALSE;
  // Temperature is ready at the background
  sensorValue = temperature;
  // When error, return standard (FRC) error value(s)
  if ( STD_SENSOR_TYPE_TEMPERATURE_IS_ERROR( sensorValue ) )
    sensorError = TRUE;
#else
  sensorError = TRUE;
  // Temperature value must be read from I2C sensor
  STD_SENSOR_TYPE_TEMPERATURE_SET_ERROR( sensorValue );
  // MCP9802 address
  i2c_start( I2C_ADR );
  if ( !i2cTimeout )
  {
    // pointer: 1 = configuration register
    i2c_write( 0x01 );
    // configuration: 9-bit ADC
    i2c_write( 0x00 );
    i2c_stop();

    // MCP9802 address
    i2c_start( I2C_ADR );
    // pointer: 0 = temperature
    i2c_write( 0 );
    i2c_stop();

    // MCP9802 address + read
    i2c_start( I2C_ADR | 1 );
    // store the result
    sensorValue.high8 = i2c_read( TRUE );
    sensorValue.low8 = i2c_read( FALSE );
    i2c_stop();

    sensorValue += 0x10 / 2;
    sensorValue /= 0x10;

    sensorError = FALSE;
  }
#endif

  // FrcValues
  return AdjustFrcTemperature();
}

//############################################################################################
// Sensor index 1: returns light intensity indicator value using DDC-SE-01
bit Get_BinaryData_Final( uns8 _ADCON0 @ W )
//############################################################################################
{
  ADCON0 = _ADCON0;
  // Read ADC

  // ADC result - left justified, Fosc/8
  ADCON1 = 0b0001.0000;
  // Do a smallest delay for ADC ACQUISITION TIME
  waitMS( 1 );
  // start ADC
  GO = 1;
  // wait for ADC finish
  while ( GO );
  // Get ADC value
  sensorValue.low8 = ADRESH / 2;

  // Return sensor FRC value

  // Test for supported FRC commands
  switch ( _PCMD )
  {
    default:
      return returnFALSE();

    case FRC_STD_SENSORS_BIT:
      // If there is a sensor error, 2-bit FRC cannot indicate it, it returns [01]

      // Number of shifts to get the bit out of the return value
      uns8 bitLoop = ( INDF1 >> 5 ) + 1;
      // Value to get the bit from
      W = sensorValue.low8;
      do
      {
        // Get the bit to Carry
        W = rr( W );
        // Next bit
      } while ( --bitLoop != 0 ); // Note: must not modify W and Carry
      // Current (prepared by DPA) FRC value is [01], change it to [11] (means bit is set)
      responseFRCvalue.1 = 1; // Note: must not modify Carry
      // Is bit set?
      if ( !Carry )
        // Bit is NOT set, return [10]
        responseFRCvalue.0 = 0;
      break;

    case FRC_STD_SENSORS_1B:
      responseFRCvalue = sensorValue.low8 + 4;
      break;
  }

  return returnTRUE();
}

//############################################################################################
// Sensor index 1: returns light intensity indicator value using DDC-SE-01
bit Get1_BinaryData_Light()
//############################################################################################
{
  // Make sure FSR1 is not modified

  // ADC initialization (for more info see PIC datasheet) pin C1 (AN0) as analog input 
  ANSELA.0 = 1;
  // ADC setting (AN0 channel)
  return Get_BinaryData_Final( 0b0.00000.01 );
}

//############################################################################################
// Sensor index 2: returns potentiometer value using DDC-SE-01
bit Get2_BinaryData_Potentiometer()
//############################################################################################
{
  // Make sure FSR1 is not modified

  // ADC initialization (for more info see PIC datasheet) pin C5 (AN4) as analog input 
  ANSELA.5 = 1;
  // ADC setting (AN4 channel)
  return Get_BinaryData_Final( 0b0.00100.01 );
}

#ifdef DALLASnotMCP
//############################################################################################
// OneWire and Dallas 18B20 routines
//############################################################################################

//############################################################################################
void Delay5us( uns8 val @ W ) // Absolutely precise timing but val != 0
//############################################################################################
{
  // 16 MHz
  // + 0.75us ( W=val, Call )
  for ( ;; )
  {         // loop time
    nop2();	// 0.50us
    nop2();	// 1.00us
    nop2();	// 1.50us
    nop2();	// 2.00us
    nop2();	// 2.50us
    nop2();	// 3.00us
    nop();	// 3.25us
    if ( --val == 0 ) // + 0.75us (W--, BTFS ) 
      return;         // + 0.25us 
    nop2();	// 4.50us
  }         // 5.00us (Goto)
}
//############################################################################################

#define	OneWireData0()	{ OneWire_TRIS = 0; }	  // 0.5us @ 16MHz
#define OneWireData1()  { OneWire_TRIS = 1; }	  // 0.5us @ 16MHz

//############################################################################################
void OneWireWriteByte( uns8 byte )
//############################################################################################
{
  uns8 bitLoop = 8;
  do
  {
    // Next sequence is time precision critical
    GIE = FALSE;

    OneWireData0();
    nop2();	        // 1 us [0.5 us]
    nop2();	        // [1.0 us]
    if ( byte.0 )	// 2.5 us [1.75us]
      OneWireData1();

    // End of time precision critical sequence
    GIE = TRUE;

    // 60us minimum in total, does not have to be precise
    Delay5us( ( 60 - 3 ) / 5 + 1 );

    OneWireData1();

    byte >>= 1;
  } while ( --bitLoop != 0 );
}

//############################################################################################
uns8 OneWireReadByte()
//############################################################################################
{
  uns8 result;
  uns8 bitLoop = 8;
  do
  {
    // Next sequence is time precision critical
    GIE = FALSE;

    OneWireData0();
    nop2();	        // 1 us [0.5 us]
#if F_OSC == 16000000
    nop2();	        // [1.0 us]
#endif
    OneWireData1();			// 2 us [1.5 us]
    Delay5us( 15 / 5 );		// 17 us [16.5 us]

    Carry = 0;				// 17.5 us [16.75 us]
    if ( OneWire_IO_IN )	// 18.5 us [ 17.25 us] (condition must not modify Carry)
      Carry = 1;

    // End of time precision critical sequence
    GIE = TRUE;				// must not modify Carry
    result = rr( result );

    // 60us minimum in total, does not have to be precise
    Delay5us( ( 60 - 20 ) / 5 + 1 );
  } while ( --bitLoop != 0 );

  return result;
}

//############################################################################################
bit OneWireReset()
//############################################################################################
{
  // Setting the pin once to low is enough
  OneWire_IO_OUT = 0;
  // Reset pulse
  OneWireData0();
  Delay5us( 500 / 5 );
  // Reset pulse end
  OneWireData1();
  // Next sequence is time precision critical
  GIE = FALSE;
  // Wait for presence pulse
  Delay5us( 70 / 5 );
  // End of time precision critical sequence
  GIE = TRUE;
  // Presence pulse?
  if ( OneWire_IO_IN )
  {
    // No presence, finish initialization sequence
    Delay5us( ( 500 - 70 ) / 5 );
    return returnFALSE();
  }
  else
  {
    // Presence OK, finish initialization sequence
    Delay5us( ( 500 - 70 ) / 5 );
    return returnTRUE();
  }
}

//############################################################################################
void OneWireCmd( uns8 cmd )
//############################################################################################
{
  // OneWire: Skip ROM
  OneWireWriteByte( CMD_SKIPROM );
  // OneWire: Send command
  OneWireWriteByte( cmd );
}

//############################################################################################
bit Ds18B20WriteConfig( uns8 value )
//############################################################################################
{
  if ( OneWireReset() )
  {
    // Write Scratchpad
    OneWireCmd( CMD_WSCRATCHPAD );

    // Write TL = ? (we dot not care the value)
    OneWireWriteByte( W );
    // Write TH = ? (we dot not care the value)
    OneWireWriteByte( W );
    // Write Config byte
    OneWireWriteByte( value );

    if ( OneWireReset() )
    {
      //  Copy Scratchpad
      OneWireCmd( CMD_CPYSCRATCHPAD );
      return returnTRUE();
    }
  }
  return returnFALSE();
}

#else // DALLASnotMCP

//############################################################################################
void i2c_waitForIdle()
//############################################################################################
{
  i2cTimeout = FALSE;
  uns8 timeout;
  // Wait for idle and not writing
  timeout = 0;
  while ( ( SSPCON2 & 0b0001.1111 ) != 0 || RW_ )
    if ( ++timeout == 0 )
    {
      i2cTimeout = TRUE;
      break;
    }
}

#endif

//############################################################################################
// Other routines
//############################################################################################

//############################################################################################
static uns8 _state;
void SetOutput( uns8 state @ _state, uns8 index @ W )
//############################################################################################
{
  // Note: FSRs must not be modified
  // Note: This method is called in the interrupt too!

  skip( index );
#pragma computedGoto 1
  goto set0;
  goto set1;
  goto set2;
  goto set3;
#pragma computedGoto 0
  ;
  // --------------------------------------
set3:
  if ( !state.0 )
    _LEDG = 0;
  else
    _LEDG = 1;

  return;
  // --------------------------------------
set2:
  if ( !state.0 )
    _LEDR = 0;
  else
    _LEDR = 1;

  return;
  // --------------------------------------
set1:
  if ( !state.0 )
    RELAY2_LAT = 0;
  else
    RELAY2_LAT = 1;

  return;
  // --------------------------------------
set0:
  if ( !state.0 )
    RELAY1_LAT = 0;
  else
    RELAY1_LAT = 1;

  return;
  // --------------------------------------
}

//############################################################################################
bit GetOutput( uns8 index @ W )
//############################################################################################
{
  Carry = FALSE; // Note: must not modify W

  // Note: all below must not modify Carry except when needed
  skip( index );
#pragma computedGoto 1
  goto get0;
  goto get1;
  goto get2;
  goto get3;
#pragma computedGoto 0
  ;
  // --------------------------------------
get3:
  if ( _LEDG )
    Carry = TRUE;
  goto _return;
  // --------------------------------------
get2:
  if ( _LEDR )
    Carry = TRUE;
  goto _return;
  // --------------------------------------
get1:
  if ( RELAY2_LAT )
    Carry = TRUE;
  goto _return;
  // --------------------------------------
get0:
  if ( RELAY1_LAT )
    Carry = TRUE;
  goto _return;
  // --------------------------------------

_return:
  return Carry;
}

//############################################################################################
#ifndef DALLASnotMCP
// I2C Master library
#include "lib/I2Cmaster.c"
#endif

// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
