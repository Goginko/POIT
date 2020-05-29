// *********************************************************************
// *                           DDC-SE-01-i2c			               *
// *                            Demo example						   *
// *********************************************************************
// Temperature measurement with sensor MCP9802 via I2C bus.
//
// Intended for:
//   HW: - (DC)TR-72D or DDC-76D-01 + DK-EVAL-04x development kit with DDC-SE-01 connected
//
//   OS: - v4.03D
//
// Description:
//   Temperature is measured and sent via RF in 5 s period. It can be
//   received by example E03-TR.c and displayed in terminal of IQRF IDE.
//   Jumper MCP9802 must be connected (DDC-SE-01).
//
// Note:
//	 SSPCON1 and SSPCON2 registers can not be accessed directly for writing
//	 due to OS restriction. writeToRAM function must be used instead. See code below.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    DDC-SE-01-i2c.c
// Version: v2.06                                   Revision: 21/09/2018
//
// Revision history:
//   v2.06: 21/09/2018  Tested for OS 4.03D.
//   v2.05: 22/08/2017  Tested for OS 4.02D.
//   v2.03: 03/03/2017  Tested for OS 4.00D.
//   v2.02: 23/02/2016  Tested for OS 3.08D.
//   v2.01: 17/09/2015  Tested with DDC-76D-01.
//   v2.00: 01/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
#define PWR_SENSOR_TRIS 	TRISC.7
#define PWR_SENSOR_IO   	LATC.7

#define I2C_ADR         	0b10010110
#define ACK             	1
#define NO_ACK          	0

// *********************************************************************
void read_MCP9802();

// I2C functions
void i2c_init();
void i2c_waitForIdle();
void i2c_start();
void i2c_repStart();
void i2c_stop();
uns8 i2c_read(uns8 ack);
uns8 i2c_write(uns8 i2cWriteData);

// *********************************************************************
uns16 temperature, tmp;
uns8 i, tenths;

// *********************************************************************

void APPLICATION()
{
	i2c_init();
    setWDTon_4s();
    PWR_SENSOR_IO = 1;                  // I2C sensor power ON
	pulseLEDG();						// reset indication
	waitDelay(10);

    while (1)
    {
        clrwdt();
        read_MCP9802();                 // result in temperature register

        bufferRF[0] = 'T';              // prepare data for RF
        bufferRF[1] = '=';
        bufferRF[5] = '.';
        bufferRF[7] = 0xB0;				// '°'
        bufferRF[8] = 'C';
	                                    // see sensor datasheet for temperature coding
        if (temperature.high8 & 0x80)   // is it negative?
        {                               // yes
            temperature  = ~temperature;
            temperature++;
            bufferRF[2] = '-';
        }
        else
            bufferRF[2] = ' ';          // no

        temperature >>= 4;
        tenths = temperature.low8 & 0x0F;   // save bits 0 - 3 (tenths)
        temperature >>= 4;

        i = temperature.low8 / 10;
        bufferRF[3] = i + '0';          // tens

        i = temperature.low8 % 10;
        bufferRF[4] = i + '0';          // ones

        tmp = tenths * 625;
        i = tmp / 1000;
        tmp %= 1000;

        if ((tmp >= 500) && (i < 9))    // rounding
            i++;

        bufferRF[6] = i +'0';           // tenths
	
        DLEN = 9;
        PIN = 0;
        RFTXpacket();                   // send data via RF
        pulseLEDR();

        waitDelay(250);
        clrwdt();
        waitDelay(250);
    }
}
// *********************************************************************

void read_MCP9802()
{
    i2c_start();
    i2c_write(I2C_ADR);                 // MCP9802 address
    i2c_write(0x01);                    // pointer: 1 = configuration register
    i2c_write(0x60);                    // config: 12-bit ADC
    i2c_stop();
    i2c_start();
    i2c_write(I2C_ADR);                 // MCP9802 address
    i2c_write(0);                       // pointer: 0 = temperature
    i2c_stop();
    i2c_start();
    i2c_write(I2C_ADR | 1);             // MCP9802 address + read
    temperature.high8 = i2c_read(ACK);  // store the result
    temperature.low8 = i2c_read(NO_ACK);
    i2c_stop();
}
//----------------------------------------------------------------------

void i2c_init(void)
{
    PORTC = 0x80;                       // port

    TRISC.3 = 1;                        // SCL as input (SIM C6)
    TRISC.4 = 1;                        // SDA as input (SIM C7)

    PWR_SENSOR_TRIS = 0;                // sensor power as output (SIM C8)
    TRISC.5 = 1;                        // shared with SIM C8

    TRISA.5 = 1;                        // sensor ALERT as input (SIM C5)
    TRISB.4 = 1;                        // shared with SIM C5
    TRISC.6 = 1;                        // shared with SIM C5

	writeToRAM(&SSPCON1, 0x38);			// I2C master mode     SSPCON  = 0b00111000
	writeToRAM(&SSPCON2, 0x00);
                                        // 50 kHz SCL frequency
    SSPADD = (F_OSC / (50000L * 4)) - 1;

    SMP = 1;                            // disable slew rate control
}
//----------------------------------------------------------------------

void i2c_waitForIdle()
{
    while ( SSPCON2 & 0x1F );           // wait for idle and not writing
    while ( RW_ );                      // wait for idle and not writing
}
//----------------------------------------------------------------------

void i2c_start()
{
    i2c_waitForIdle();
 	writeToRAM(&SSPCON2, SSPCON2 | 0x01);		// SEN = 1
}
//----------------------------------------------------------------------

void i2c_repStart()
{
    i2c_waitForIdle();
	writeToRAM(&SSPCON2, SSPCON2 | 0x02);		// RSEN = 1
}
//----------------------------------------------------------------------

void i2c_stop()
{
    i2c_waitForIdle();
	writeToRAM(&SSPCON2, SSPCON2 | 0x04);		// PEN = 1
}
//----------------------------------------------------------------------

uns8 i2c_read(uns8 ack)
{
    uns8 i2cReadData;

    i2c_waitForIdle();
 	writeToRAM(&SSPCON2, SSPCON2 | 0x08);		// RCEN = 1

    i2c_waitForIdle();
    i2cReadData = SSPBUF;

    i2c_waitForIdle();

    if (ack)
		writeToRAM(&SSPCON2, SSPCON2 & 0xDF);	// acknowledge, ACKDT = 0
    else
		writeToRAM(&SSPCON2, SSPCON2 | 0x20);	// not acknowledge, ACKDT = 1 

	writeToRAM(&SSPCON2, SSPCON2 | 0x10);		// send acknowledge sequence, ACKEN = 1 
    return(i2cReadData);
}
//----------------------------------------------------------------------

uns8 i2c_write(uns8 i2cWriteData)
{
    i2c_waitForIdle();
    SSPBUF = i2cWriteData;
    i2c_waitForIdle();
	return(!(readFromRAM(&SSPCON2) & 0x40)); 	// returns 1 if transmission is acknowledged, return(!ACKSTAT);
}
// *********************************************************************

