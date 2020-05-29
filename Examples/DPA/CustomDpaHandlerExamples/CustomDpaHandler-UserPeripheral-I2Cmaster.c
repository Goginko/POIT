// *********************************************************************************
//   Custom DPA Handler code example - User peripheral implementation - I2C master *
// *********************************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-UserPeripheral-I2Cmaster.c,v $
// Version: $Revision: 1.6 $
// Date:    $Date: 2020/02/20 17:18:58 $
//
// Revision history:
//   2020/01/02  Release for DPA 4.11
//   2019/10/09  Release for DPA 4.10
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// This example implements Master I2C as a user peripheral

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"
// I2C Master library
#include "lib/I2Cmaster.c"

/*********************************************************************
 I2C master is controlled by DPA Request data sent to the user peripheral PNUM=0x20, PCMD=0x00.
 The command allows to (optionally) write bytes to the I2C bus and then (optionally) read bytes from the bus.

 *** DPA Request format ***
  Byte index | Description
  ---------------------------------------------------------------------------------------------------------------------------
  0          | I2C 8-bit device address to write/read data to/from (bit.0 of the address is set when reading from the I2C bus).
  1          | Number of bytes to read from I2C after data are written to the I2C bus.
  2-55       | Optional bytes to write to the I2C before data are read from I2C bus.

  *** DPA Response format ***
  Byte index | Description
  ---------------------------------------------------------------------------------------------------------------------------
  0-55       | Byte read from I2C bus. If no bytes are read, no data is returned i.e. DPA Response is empty.


  *** Example #1 ***
  Configuring MCP9802 temperature sensor in the DDC-SE-01 connected to the DK-EVAL-04x for the 12-bit ADC, i.e. 0.0625 °C resolution.

  DPA Request:
  Byte index | Value [hex] | Description
  ---------------------------------------------------------------------------------------------------------------------------
  0          | 96          | MCP9802 I2C 8-bit address
  1          | 00          | Read no data
  2          | 01          | Write MCP9802 configuration register address
  3          | 60          | Write MCP9802 configuration register value: 12-bit ADC, i.e. 0.0625°C resolution

  DPA Response:
  Empty as no data is read.


  *** Example #2 ***
  Reading temperature from  MCP9802 temperature sensor in the DDC-SE-01 connected to the DK-EVAL-04x.

  DPA Request:
  Byte index | Value [hex] | Description
  ---------------------------------------------------------------------------------------------------------------------------
  0          | 96          | MCP9802 I2C 8-bit address
  1          | 02          | Read 2 bytes with the temperature value
  2          | 00          | Write MCP9802 ambient temperature register address

  DPA Response:
  Byte index | Value [hex] | Description
  ---------------------------------------------------------------------------------------------------------------------------
  0          | 17          | Upper half of the temperature register
  1          | E0          | Lower half of the temperature register (0x17E0 = 23.875 °C)

*********************************************************************

  I2C signals to PIN assignment:

    TR module pin | DK-EVAL-04x pin | I2C
    -------------------------------------
    C7            | 2               | SDA
    C6            | 3               | SCL
    C4            | 7               | GND

  ! Do not forget to connect pull-up resistors (e.g. 10k) between SDA and SCL and Vcc = 3 Volts preferably at I2C master side!

*********************************************************************/

// I2C SCL frequency [Hz]
#define I2Cfrequency                100000

// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
//############################################################################################
bit CustomDpaHandler()
//############################################################################################
{
  // I2C master peripheral command data structure
  typedef struct
  {
    uns8  I2Caddress;
    uns8  ReadDataCount;
    uns8  WriteData[DPA_MAX_DATA_LENGTH - 2 * sizeof( uns8 )];
  } TI2Crequest;

  // Handler presence mark
  clrwdt();

  // Detect DPA event to handle
  switch ( GetDpaEvent() )
  {
    // -------------------------------------------------
    case DpaEvent_Interrupt:
      // Do an extra quick background interrupt work
      return Carry;

      // -------------------------------------------------
    case DpaEvent_DpaRequest:
      // Called to interpret DPA request for peripherals
      // -------------------------------------------------
      // Peripheral enumeration
      if ( IsDpaEnumPeripheralsRequest() )
      {
        // We implement 1 user peripheral
        _DpaMessage.EnumPeripheralsAnswer.UserPerNr |= 1;
        FlagUserPer( _DpaMessage.EnumPeripheralsAnswer.UserPer, PNUM_USER + 0 );
        _DpaMessage.EnumPeripheralsAnswer.HWPID |= 0x123F;
        _DpaMessage.EnumPeripheralsAnswer.HWPIDver |= 0xABCD;

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
          _DpaMessage.PeripheralInfoAnswer.PerTE = PERIPHERAL_TYPE_EXTENDED_READ_WRITE;
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
          // Check DPA command value
          if ( _PCMD != 0 )
            DpaApiReturnPeripheralError( ERROR_PCMD );

          // Check for minimum request data length
          if ( _DpaDataLength < offsetof( TI2Crequest, WriteData ) )
            DpaApiReturnPeripheralError( ERROR_DATA_LEN );

          // Looping variable
          uns8 loop;
          // Count of bytes to write to I2C
          loop = _DpaDataLength - offsetof( TI2Crequest, WriteData );
          // Anything to write to I2C?
          if ( loop != 0 )
          {
            // Start I2C writing
            i2c_start( FSR0[offsetof( TI2Crequest, I2Caddress )] );
            // Pointer to the data to write
            FSR1 += offsetof( TI2Crequest, WriteData );
            // Now write byte by byte to the I2C
            do
            {
              i2c_write( *FSR1++ );
            } while ( --loop != 0 );
            // Stop I2C
            i2c_stop();
          }

          // FSR1 points to the DPA response/request data buffer
          FSR1 = _DpaMessage.Response.PData;
          // Number of bytes to read from I2C
          loop = _DpaDataLength = FSR1[offsetof( TI2Crequest, ReadDataCount )];
          // Anything to read from I2C?
          if ( loop == 0 )
          {
            // No, return with empty DPA response
            goto DpaHandleReturnTRUE;
          }

          // Start I2C reading
          i2c_start( FSR1[offsetof( TI2Crequest, I2Caddress )] | 1 );
          // Read byte by byte from I2C
          do
          {
            // Only very last read byte is not acknowledged
            setINDF1( i2c_read( loop != 1 ) );
            FSR1++;
          } while ( --loop != 0 );
          // Stop I2C
          i2c_stop();

          goto DpaHandleReturnTRUE;
        }
      }

      // -------------------------------------------------
    case DpaEvent_Init:
      // Do a one time initialization before main loop starts
      // Fall through!
      // -------------------------------------------------
    case DpaEvent_AfterSleep:
      // Called after woken up after sleep
      // Enable I2C master
      i2c_init();
      return Carry;

      // -------------------------------------------------
    case DpaEvent_BeforeSleep:
      // Called before going to sleep
      // Disable I2C master
      i2c_shutdown();
      return Carry;
  }

  return FALSE;
}

//############################################################################################
// I2C Master library
#include "lib/I2Cmaster.c"
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
