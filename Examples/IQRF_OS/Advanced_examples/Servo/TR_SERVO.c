// *********************************************************************
//                          Servo control via RF
//                             Demo example
// *********************************************************************
//
// Intended for:
//   HW: - (DC)TR-72D + DK-EVAL-04x development kit with standard servo
//         connected and supplied from power adapter
//
//   OS: - v4.03D
//
// Description:
//   - this example receives RF commands and controls connected servo.
//   - use example E03-TR as a transmiter 
//	 - use IQRF IDE 4.47 or higher - Terminal (Use Numbers must be checked, see Help)
//   - import prepared macros file servo_macros.iqrfmcr to the IQRF IDE
//
//   Commands to be sent from the transmiter: 'S'<pulse width>
//		<pulse width>: values 22 ~ 76 => 700 ~ 2500 us
//		(e.g. "S#048" -> pulse width cca 1536 us)
//
// ---------------------------------------------------------------------
// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
//
// Copyright (c) IQRF Tech s.r.o.
//
// File:    TR_SERVO.c
// Version: v2.05                                   Revision: 24/09/2017
//
// Revision history:
//   v2.05: 24/09/2018  Tested for OS 4.03D.
//   v2.04: 23/08/2017  Tested for OS 4.02D.
//   v2.02: 03/03/2017  Tested for OS 4.00D.
//						Preamble quality test added.
//   v2.01: 23/02/2016  Order of _enableUserInterrupt = 1 and TMR6IE = 1 swapped.
//                      Tested for OS 3.08D.
//   v2.00: 02/07/2015  First release for (DC)TR-7xD.
//
// *********************************************************************
#include "../../../../Development/include/IQRF_OS/IQRF.h"	// System header files

// *********************************************************************
#define SG_TRIS         TRISA.0         // C1 - pin connected to servo signal input
#define SG_IO           LATA.0

// Timer 6 setting (MCU clock 8 MHz):
//  - prescaler 1:64
//  - postscaler see below
//  - PR6:
//   = servo idle period with postscaler 1:16
#define IDLE            70              // cca 18.5 ms
//   = servo pulse period with postscaler 1:2 (tune values according to used servo)
#define LOW_LIMIT       22              // 704 us
#define NEUTRAL         48              // 1536 us
#define HIGH_LIMIT      76              // 2432 us

#define	RX_FILTER		0

// *********************************************************************
void timer6Init(void);

// *********************************************************************
uns8 servo_pulse;

// *********************************************************************

void APPLICATION()
{
    SG_IO = 0;
    SG_TRIS = 0;                    // As output
    setLEDR();
    waitDelay(50);                          // Startup indication
    stopLEDR();
    servo_pulse = NEUTRAL;
    timer6Init();
	setRFmode(_WPE | _RX_STD | _TX_STD);
	_checkRFcfg_PQT = 1;			// Enable preamble quality test
    toutRF = 1;                     // Wait Packed End active so the toutRF can be set to minimum
   _enableUserInterrupt = 1;
   TMR6IE = 1;                      // Timer 6 interrupt enable
    
    while (1)
    {
        if (checkRF(RX_FILTER))
        {
            if (RFRXpacket())
            {
                pulseLEDR();

                if (bufferRF[0] == 'S')
                {
                    if      (bufferRF[1] < LOW_LIMIT)   servo_pulse = LOW_LIMIT;
                    else if (bufferRF[1] > HIGH_LIMIT)  servo_pulse = HIGH_LIMIT;
                    else                                servo_pulse = bufferRF[1];
                }
            }
        }
    }
}
// *********************************************************************

void timer6Init(void)
{
    T6CON = 0x7F;					// Prescaler: x64, Postscaler: x16, Timer: on
	PR6 = IDLE;						
	TMR6IF = 0;
}
// *********************************************************************

// This function is invoked whenever any interrupt occurs (even OS)
// and the _enableUserInterrupt flag is set to 1.
// OS generates an interrupt in the period of system ticks.
// The user interrupt routine must start from address 0x3F00 (__USER_INTERRUPT macro). 
// The user interrupt routine must take less than 50 µs. 
// User interrupts must not be called in period shorter then 800 µs.
// IQRF OS functions except of setINDFx and getINDFx are not allowed.
// IQRF OS variables must not be used.
// Only global or local static user variables are allowed.
// For more information see IQRF OS User's Guide.

#pragma origin __USER_INTERRUPT
#pragma library 0
void userIntRoutine(void)			// User interrupt service routine
{	
	if (TMR6IF)
	{														
        if (SG_IO)
        {
            SG_IO = 0;
            T6CON |= 0x70;          // Postscaler 1:16
            PR6 = IDLE;             
        }
        else
        {
            SG_IO = 1;
            T6CON &= 0x0F;          // Postscaler 1:2
            PR6 = servo_pulse;
        }
        
		TMR6 = 0;					// Timer reset
        TMR6IF = 0;
	}	
}
// *********************************************************************

