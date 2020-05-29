// *********************************************************************
//   Custom DPA Handler code example - Using calibrated TMR6 MCU timer *
// *********************************************************************
// Copyright (c) IQRF Tech s.r.o.
//
// File:    $RCSfile: CustomDpaHandler-TimerCalibrated.c,v $
// Version: $Revision: 1.11 $
// Date:    $Date: 2020/02/20 17:18:58 $
//
// Revision history:
//   2018/10/25  Release for DPA 3.03
//
// *********************************************************************

// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/

// Default IQRF include (modify the path according to your setup)
#include "IQRF.h"

// Default DPA header (modify the path according to your setup)
#include "DPA.h"
// Default Custom DPA Handler header (modify the path according to your setup)
#include "DPAcustomHandler.h"

// This example initializes and calibrates TMR6 timer.
// By pressing the standard IQRF button connected to GPIO pin PB.4 (e.g. at DK-EVAL-04x) the timer can be switched between calibrated and uncalibrated states to see the difference (use logic analyzer to measure the pulse at the demo pin).
// The example code pulses GPIO pin PA.0 every 1000 timer interrupts. At the same time it pulses Green LED when timer is calibrated or Red LED when timer is not calibrated respectively.

// This example works only at STD mode, not at LP mode

//############################################################################################

// Timer prescaler value. Can be one of 1, 4, 16, 64. Postscaler cannot be use.
#define TMR_PRESCALER       16

// PR6 register value minus 1. This value is used for further timer division from the PIC MCU fOsc/4 value. 
// E.g. fOsc=16 MHz, Prescaler=16, PR6=249 gives timer frequency 16_000_000 / 4 / 16 / ( 249 + 1 ) = 1000 Hz and period 1 ms
#define PR_VALUE            ( 250 - 1 )

// Calibration time [1 ms]. Calibration time must be multiple of 10. Longer calibration time provides higher calibration accuracy.
// Calibration accuracy is 1 / ( ( PR6 + 1 ) * NumberOfTimerTicksPerCalibration ). 
// I.e. for the previous settings and 100 ms calibration period the accuracy is 1 / ( 250 * 100 ) = 0.00004 = 0.004 % = 0.04 ‰ = 40 ppm
// Number of timer periods per calibration time must not be above 98 % of 0xFF if variable "timerTicks" is type of "uns8". If the variable is "uns16" then 98 % of 0xFfFf is maximum. The maximum number is checked automatically.
// The calibration function measures the number of prescaled timer ticks per the calibration period precisely measured by the Xtal driven IQRS OS ticks. Then Bresenham algorithm is used to let the timer interrupt generate the expected
// number of interrupts during the calibration time while the PIC timer regularly counts the measured number of timer ticks.
#define CALIB_TIME_1ms      100

// Type of the timer interrupt counter variable. Can be uns8 or uns16. It is also used for calibration.
#define TIMER_COUNTER_TYPE  uns8

//############################################################################################

// Timer tick time [1 ms]
#define TICK_TIME_1ms     ( 1000.0 / ( F_OSC / 4.0 / TMR_PRESCALER / ( PR_VALUE + 1 ) ) )

// Define timer 6 initialization register value according to the prescaler. Postscaler cannot be used.
#if TMR_PRESCALER == 1
#define T6CONvalue        0b0.0000.1.00;
#elif TMR_PRESCALER == 4 
#define T6CONvalue        0b0.0000.1.01;
#elif TMR_PRESCALER == 16
#define T6CONvalue        0b0.0000.1.10;
#elif TMR_PRESCALER == 64
#define T6CONvalue        0b0.0000.1.11;
#else
#error Invalid timer prescaler value!
#endif

// Calibration time must be multiple of 10
#if CALIB_TIME_1ms % 10 != 0
#error Calibration time must by multiple of 10!
#endif

// Check number of timer ticks per calibration
#if ( ( CALIB_TIME_1ms + TICK_TIME_1ms / 2 ) / TICK_TIME_1ms ) > (TIMER_COUNTER_TYPE)( 0.98 * ( (TIMER_COUNTER_TYPE)-1 ) )
#error Too many timer ticks per calibration!
#endif

// Number of timer ticks per calibration
#define TICK_PER_CALIB    (TIMER_COUNTER_TYPE)( ( CALIB_TIME_1ms + TICK_TIME_1ms / 2 ) / TICK_TIME_1ms )

//############################################################################################

// Timer interrupt ticks counter
TIMER_COUNTER_TYPE timerTicks;
// Normal calibrated PR6 value. Value +1 is used to slow down the timer when Bresenham algorithm underflow when subtracting BresenhamSmaller occurs.
uns8 PRcalib;
// Bresenham algorithm variable to subtract from the sum variable (smaller "line" size)
uns8 BresenhamSmaller;

//############################################################################################

void CalibrateTimer();

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
      // ! It is desirable that this event is handled with immediate return even if it is not used by the custom handler because the Interrupt event is raised on every MCU interrupt and the “empty” return handler ensures the shortest possible interrupt routine response time.
      // ! Only global variables or local ones marked by static keyword can be used to allow reentrancy.
      // ! Make sure race condition does not occur when accessing those variables at other places.
      // ! Make sure( inspect.lst file generated by C compiler ) compiler does not create any hidden temporary local variable( occurs when using division, multiplication or bit shifts ) at the event handler code.The name of such variable is usually Cnumbercnt.
      // ! Do not call any OS functions except setINDFx().
      // ! Do not use any OS variables especially for writing access.
      // ! All above rules apply also to any other function being called from the event handler code, although calling any function from Interrupt event is not recommended because of additional MCU stack usage.

    {
      // Timer6 interrupt occurred?
      if ( !TMR6IF )
        // No, do not handle the timer interrupt
        return Carry; // Note: Optimization (avoid slow "goto to return")

      // Reset the timer interrupt flag
      TMR6IF = 0;

      // Add next timer interrupt tick
      timerTicks++;

      // Bresenham algorithm sum variable
      static uns8 bresenhamSum;
      // Subtract smaller Bresenham value from the sum variable
      bresenhamSum -= BresenhamSmaller;
      // Prepare normal optimized PR6 value to the W in case the sum is not negative
      W = PRcalib; // Note: must not modify Carry
      // If the previous subtraction result was negative (underflow)
      if ( !Carry )
      {
        // Add longer "line" size value to the Bresenham sum variable
        bresenhamSum += TICK_PER_CALIB;
        // Use +1 PR6 value to slow down the timer
        W = PRcalib + 1;
      }
      // Set PR6 value
      PR6 = W;

      // Demo output = make pulse every 1000 timer ticks
      static uns16 demoCounter;
      // Time to pulse?
      if ( ++demoCounter == 1000 )
      {
        // Reset demo counter
        demoCounter = 0;
        // Make a GPIO output pulse
        LATA.0 = !LATA.0;
        // Make a LED pulse
        if ( BresenhamSmaller != 0 )
        {
          // Green pulse when timer is calibrated
          _LEDG = !_LEDG;
          _LEDR = 0;
        }
        else
        {
          // Red pulse when timer is NOT calibrated
          _LEDR = !_LEDR;
          _LEDG = 0;
        }
      }

      return Carry;
    }

    // -------------------------------------------------
    case DpaEvent_Idle:
      // Do a quick background work when RF packet is not received
    {
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
        // Is timer calibrated?
        if ( BresenhamSmaller != 0 )
          // UNcalibrate the timer
          BresenhamSmaller = 0;
        else
          // Calibrate the timer
          CalibrateTimer();
      }
      break;
    }

    // -------------------------------------------------
    case DpaEvent_Init:
      // Do a one time initialization before main loop starts

      // Setup digital output
      TRISA.0 = 0;

      // Initialize the timer
      T6CON = T6CONvalue;
      TMR6IE = TRUE;

      // Calibrate the timer
      CalibrateTimer();

      break;

      // -------------------------------------------------
    case DpaEvent_AfterSleep:
      // Called on wake-up from sleep
      TMR6IE = TRUE;
      TMR6ON = TRUE;
      break;

      // -------------------------------------------------
    case DpaEvent_BeforeSleep:
      // Called before going to sleep	(the same handling as DpaEvent_DisableInterrupts event)
      // To minimize the power consumption, no MCU pin must be left as a digital input without defined input level value.
      // So, unused pins in given hardware should be set as outputs.
      // See https://www.iqrf.org/DpaTechGuide/302/examples/CustomDpaHandler-Timer.c.html for an example.

      // -------------------------------------------------
    case DpaEvent_DisableInterrupts:
      // Called when device needs all hardware interrupts to be disabled (before Reset, Restart, LoadCode, Remove bond, and Run RFPGM)
      // Must not use TMR6 any more
      TMR6ON = FALSE;
      TMR6IE = FALSE;
      break;
  }

  return FALSE;
}

//############################################################################################
// Calibrates the timer. Assumes previous RF activity or explicit previous setRFready() call in order to have OS ticks connected to the RF Xtal.
void CalibrateTimer()
//############################################################################################
{
  // Switch off the timer
  TMR6ON = FALSE;
  // Reset timer
  TMR6 = 0;

  // Reset timer counter with a little optimization in case the counter is already 16 bit wide
#if ( (TIMER_COUNTER_TYPE)-1) == 0xFF
  timerTicks = 0;
#else
  timerTicks = -TICK_PER_CALIB;
#endif

  // Disable Bresenham algorithm to used constant and default uncalibrated PR6 value
  BresenhamSmaller = 0;

  // Synchronize OS ticks already "connected" to the RF Xtal
  waitNewTick();
  // Set default uncalibrated PR6
  PR6 = PRcalib = PR_VALUE;
  // Start timer
  LATA.0 = 1;
  TMR6ON = TRUE;
  // Wait calibration time precisely
  waitDelay( CALIB_TIME_1ms / 10 );
  // Switch off the timer
  TMR6ON = FALSE;
  LATA.0 = 0;

  // Variable tmr6 will store number of HW ticks (after prescaler ticks) above/below the ideal state at the end of the computation.
  // Compute a signed difference from ideal number of timer ticks per calibration time
#if ( (TIMER_COUNTER_TYPE)-1) == 0xFF
  // When timer counter is 8 bit we must declare and use another 16 bit variable.
  int16 tmr6 = (int16)timerTicks - TICK_PER_CALIB;
#else
  // When timer counter is 16 bit we can use it as a computation variable that already was initialized to -TICK_PER_CALIB above.
  int16 tmr6 @ timerTicks;
#endif
  // Multiply value by the PR6 value to get number of HW timer ticks above/below the ideal state
  tmr6 *= (uns16)( PR_VALUE + 1 );
  // And add more HW ticks that were above
  tmr6 += TMR6;

  // Number of HW ticks must not be negative
  while ( tmr6 < 0 )
  {
    // When negative, add number of timer ticks per calibration
    tmr6 += TICK_PER_CALIB;
    // Decrease PR6 to make timer it faster (timer was slower as the number of ticks was negative)
    PRcalib--;
  }

  // Final number of above HW timer ticks must be smaller (small Bresenham "line" value) than number of timer ticks per calibration (long Bresenham "line" value)
  while ( tmr6 > TICK_PER_CALIB )
  {
    // Subtract number of HW timer ticks per calibration
    tmr6 -= TICK_PER_CALIB;
    // Make timer slower
    PRcalib++;
  }

  // Smaller Bresenham value. The algorithm actually spreads fixed number of timer ticks into precisely measured number of HW timer ticks
  BresenhamSmaller = tmr6.low8;
  // Enable timer again
  TMR6ON = TRUE;
}

//############################################################################################
// Default Custom DPA Handler header; 2nd include to implement Code bumper to detect too long code of the Custom DPA Handler (modify the path according to your setup) 
#include "DPAcustomHandler.h"
//############################################################################################
