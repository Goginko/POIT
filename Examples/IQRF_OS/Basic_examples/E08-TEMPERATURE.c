// *********************************************************************
// *                       E08 - TEMPERATURE                           *
// *                          TEMPERATURE                              *
// *********************************************************************
// Temperature measurement.
//
// Intended for:
//   HW: - (DC)TR-72DT + CK-USB-04x development kit
//
//   OS: - v4.03D
//
// Description:
//   Temperature is measured and sent via SPI in 5s period.
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    E08-TEMPERATURE.c
// Version: v2.05                                   Revision: 21/09/2018
//
// Revision history:
//   v2.05: 21/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.03: 17/07/2017  Returned value type of getTemperature was changed from usn8 to int8.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//   v2.01: 22/02/2016  Tested for OS 3.08D.
//   v2.00: 02/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************

void APPLICATION()
{
uns16 temperature, tmp;
uns8 i, tenths;

    enableSPI();

    while (1)
    {
 		if (getTemperature() != -128)			// -128 indicates an error in communication with temperature sensor
		{										// Temperature reading OK
			temperature = param3;
			
			bufferCOM[0] = 'T';          		// Prepare data for RF
			bufferCOM[1] = '=';
			bufferCOM[2] = ' ';
			bufferCOM[5] = '.';
			bufferCOM[7] = 0xB0;				// '°'
			bufferCOM[8] = 'C';

			if (temperature.high8 & 0x08)   	// Is it negative?
			{                               	// Yes
				temperature  = ~temperature;
				temperature++;
				bufferCOM[2] = '-';
			}

			tenths = temperature.low8 & 0x0F;	// Save bits 0 - 3 (tenths)
			temperature >>= 4;

			i = temperature.low8 / 10;
			bufferCOM[3] = i + '0';          	// Tens

			i = temperature.low8 % 10;
			bufferCOM[4] = i + '0';          	// Ones

			tmp = tenths * 625;
			i = tmp / 1000;
			tmp %= 1000;

			if ((tmp >= 500) && (i < 9))    	// Rounding
				i++;
			
			bufferCOM[6] = i +'0';          	// Tenths
			
			startSPI(9);
		}
		else
		{										// Temperature reading error
			bufferCOM[0] = 'T';          		
			bufferCOM[1] = '-';
			bufferCOM[2] = 'e';
			bufferCOM[3] = 'r';
			bufferCOM[4] = 'r';
			startSPI(5);
		}
		
        waitDelay(250);
		waitDelay(250);
    }
}

// *********************************************************************
