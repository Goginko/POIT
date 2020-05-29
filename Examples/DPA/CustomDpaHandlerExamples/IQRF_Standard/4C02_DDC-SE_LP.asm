
; CC5X Version 3.7D, Copyright (c) B Knudsen Data
; C compiler for the PICmicro family
; ************  28. Mar 2020  11:18  *************

        processor  16F1938
        radix  DEC

INDF1       EQU   0x01
FSR1        EQU   0x06
FSR1L       EQU   0x06
WREG        EQU   0x09
Carry       EQU   0
Zero_       EQU   2
PORTC       EQU   0x0E
TRISA       EQU   0x8C
TRISB       EQU   0x8D
TRISC       EQU   0x8E
ADRESH      EQU   0x9C
ADCON0      EQU   0x9D
ADCON1      EQU   0x9E
LATC        EQU   0x10E
ANSELA      EQU   0x18C
GIE         EQU   7
GO          EQU   1
bufferINFO  EQU   0x320
userReg0    EQU   0x70
param3      EQU   0x74
responseFRCvalue EQU   0x5BC
responseFRCvalue2B EQU   0x5BC
DLEN        EQU   0x2A1
PNUM        EQU   0x2AE
PCMD        EQU   0x2AF
DataOutBeforeResponseFRC EQU   0x4C0
DpaRfMessage EQU   0x4A0
par1        EQU   0x73
_DpaMessageIqrfStd EQU   0x4A0
sensorValue EQU   0x74
temperature EQU   0x5C5
PerOSSleep_Request EQU   0x5C7
state       EQU   0x5CA
temperatureByteLow EQU   0x5CB
timeoutStart EQU   0x5CC
sensorsBitmap EQU   0x5C0
sensorIndex EQU   0x5C0
sensorType  EQU   0x5C1
sensorError EQU   0
_sensorValue EQU   0x5C1
C1cnt       EQU   0x5C3
bitLoop     EQU   0x5C1
byte        EQU   0x5C2
bitLoop_2   EQU   0x5C3
result      EQU   0x5C0
bitLoop_3   EQU   0x5C1
cmd         EQU   0x5C1
value_4     EQU   0x5C0

        GOTO main

  ; FILE ../../../../Development/include/IQRF_OS/IQRF.h
                        ;// *********************************************************************
                        ;// *                   IQRF OS basic include file                      *
                        ;// *********************************************************************
                        ;//
                        ;// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
                        ;//
                        ;// Copyright (c) IQRF Tech s.r.o.
                        ;//
                        ;// Intended for:
                        ;//	 HW: TR-72D, TR-76D, TR-77D, TR-78D, TR-75D
                        ;//   OS: v4.03D
                        ;//
                        ;// File:    IQRF.h
                        ;// Version: v1.00                                   Revision: 10/05/2018
                        ;//
                        ;// Revision history:
                        ;//	 v1.00: 10/05/2018 	First release for OS 4.03D.
                        ;//
                        ;// *********************************************************************
                        ;
                        ;#if IQRFOS != 403
                        ;    #error Invalid IQRF OS version, V4.03 is expected. Make sure matching header files and project IQRF OS version setting are used.
                        ;#endif
                        ;
                        ;#if defined TR72D
                        ;    #message Compilation for TR-72D modules (PIC16LF1938) and IQRF OS 4.03D.
                        ;    #define TR7xD
                        ;#elif defined TR76D
                        ;    #message Compilation for TR-76D modules (PIC16LF1938) and IQRF OS 4.03D.
                        ;    #define TR7xD
                        ;#elif defined TR77D
                        ;    #message Compilation for TR-77D modules (PIC16LF1938) and IQRF OS 4.03D.
                        ;    #define TR7xD
                        ;#elif defined TR78D
                        ;    #message Compilation for TR-78D modules (PIC16LF1938) and IQRF OS 4.03D.
                        ;    #define TR7xD
                        ;#elif defined TR75D
                        ;    #message Compilation for TR-75D modules (PIC16LF1938) and IQRF OS 4.03D.
                        ;    #define TR7xD
                        ;#else
                        ;    #error Unsupported TR module type.
                        ;#endif
                        ;
                        ;#pragma chip PIC16F1938
                        ;
                        ;#if __CC5X__ < 3701
                        ;    #warning Insufficient CC5X compiler version, V3.7A is recommended.
                        ;#endif
                        ;
                        ;#pragma origin 0x100
        ORG 0x0100
                        ;#pragma update_RP 0
                        ;
                        ;void APPLICATION();
                        ;
                        ;void main()                                	// Skipped during Upload
                        ;{
main
                        ;    APPLICATION();
        MOVLP 0x38
        CALL  APPLICATION
        MOVLP 0x00
                        ;}
        SLEEP
        GOTO main

  ; FILE ../../../../Development/include/IQRF_OS/IQRF-functions.h
                        ;// *********************************************************************
                        ;//      				   IQRF OS functions						   *
                        ;// *********************************************************************
                        ;// Intended for:
                        ;//    OS: v4.03D
                        ;//
                        ;// Online IQRF OS Reference Guide: http://www.iqrf.org/IQRF-OS-Reference-guide/
                        ;//
                        ;// Copyright (c) IQRF Tech s.r.o.
                        ;//
                        ;// File:    IQRF-functions.h
                        ;// Version: v1.00                                   Revision: 27/06/2018
                        ;//
                        ;// Revision history:
                        ;//   v1.00: 27/06/2018  First release for OS 4.03D.
                        ;//
                        ;// *********************************************************************
                        ; 
                        ;#pragma optimize 0
                        ;#pragma update_PAGE 0
                        ;#pragma update_RP 0
                        ;
                        ;#define	dummy_address	0x3810
                        ;#pragma origin dummy_address
        ORG 0x3810
                        ;void dummy()
                        ;{
dummy
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	iqrfSleep_address	0x3816
                        ;#pragma origin iqrfSleep_address
        ORG 0x3816
                        ;void iqrfSleep()
                        ;{
iqrfSleep
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	_debug_address	0x3819
                        ;#pragma origin _debug_address
        ORG 0x3819
                        ;void _debug()
                        ;{
_debug
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define debug()	    \
                        ;	do {			\
                        ;		_debug();	\
                        ;        nop();      \
                        ;	} while (0)
                        ;
                        ;#define	moduleInfo_address	0x381c
                        ;#pragma origin moduleInfo_address
        ORG 0x381C
                        ;void moduleInfo()
                        ;{
moduleInfo
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	pulsingLEDR_address	0x3822
                        ;#pragma origin pulsingLEDR_address
        ORG 0x3822
                        ;void pulsingLEDR()
                        ;{
pulsingLEDR
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	pulseLEDR_address	0x3825
                        ;#pragma origin pulseLEDR_address
        ORG 0x3825
                        ;void pulseLEDR()
                        ;{
pulseLEDR
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	stopLEDR_address	0x3828
                        ;#pragma origin stopLEDR_address
        ORG 0x3828
                        ;void stopLEDR()
                        ;{
stopLEDR
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	pulsingLEDG_address	0x382b
                        ;#pragma origin pulsingLEDG_address
        ORG 0x382B
                        ;void pulsingLEDG()
                        ;{
pulsingLEDG
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	pulseLEDG_address	0x382e
                        ;#pragma origin pulseLEDG_address
        ORG 0x382E
                        ;void pulseLEDG()
                        ;{
pulseLEDG
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	stopLEDG_address	0x3831
                        ;#pragma origin stopLEDG_address
        ORG 0x3831
                        ;void stopLEDG()
                        ;{
stopLEDG
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setOnPulsingLED_address	0x3834
                        ;#pragma origin setOnPulsingLED_address
        ORG 0x3834
                        ;void setOnPulsingLED(uns8 ticks @ W)
                        ;{
setOnPulsingLED
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setOffPulsingLED_address	0x3837
                        ;#pragma origin setOffPulsingLED_address
        ORG 0x3837
                        ;void setOffPulsingLED(uns8 ticks @ W)
                        ;{
setOffPulsingLED
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	eeReadByte_address	0x383a
                        ;#pragma origin eeReadByte_address
        ORG 0x383A
                        ;uns8 eeReadByte(uns8 address @ W)
                        ;{
eeReadByte
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return 1;
        RETLW 0x01
                        ;}
                        ;
                        ;#define	eeReadData_address	0x383d
                        ;#pragma origin eeReadData_address
        ORG 0x383D
                        ;bit eeReadData(uns8 address @ param2, uns8 length @ W)
                        ;{
eeReadData
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	eeWriteByte_address	0x3840
                        ;#pragma origin eeWriteByte_address
        ORG 0x3840
                        ;void eeWriteByte(uns8 address @ param2, uns8 data @ W)
                        ;{
eeWriteByte
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	eeWriteData_address	0x3843
                        ;#pragma origin eeWriteData_address
        ORG 0x3843
                        ;void eeWriteData(uns8 address @ param2, uns8 length @ W)
                        ;{
eeWriteData
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	readFromRAM_address	0x3846
                        ;#pragma origin readFromRAM_address
        ORG 0x3846
                        ;uns8 readFromRAM(uns16 address @ FSR0)
                        ;{
readFromRAM
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return 1;
        RETLW 0x01
                        ;}
                        ;
                        ;#define	clearBufferINFO_address	0x384c
                        ;#pragma origin clearBufferINFO_address
        ORG 0x384C
                        ;void clearBufferINFO()
                        ;{
clearBufferINFO
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	swapBufferINFO_address	0x384f
                        ;#pragma origin swapBufferINFO_address
        ORG 0x384F
                        ;void swapBufferINFO()
                        ;{
swapBufferINFO
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	compareBufferINFO2RF_address	0x3852
                        ;#pragma origin compareBufferINFO2RF_address
        ORG 0x3852
                        ;bit compareBufferINFO2RF(uns8 length @ W)
                        ;{
compareBufferINFO2RF
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	copyBufferINFO2COM_address	0x3855
                        ;#pragma origin copyBufferINFO2COM_address
        ORG 0x3855
                        ;void copyBufferINFO2COM()
                        ;{
copyBufferINFO2COM
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	copyBufferINFO2RF_address	0x3858
                        ;#pragma origin copyBufferINFO2RF_address
        ORG 0x3858
                        ;void copyBufferINFO2RF()
                        ;{
copyBufferINFO2RF
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	copyBufferRF2COM_address	0x385b
                        ;#pragma origin copyBufferRF2COM_address
        ORG 0x385B
                        ;void copyBufferRF2COM()
                        ;{
copyBufferRF2COM
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	copyBufferRF2INFO_address	0x385e
                        ;#pragma origin copyBufferRF2INFO_address
        ORG 0x385E
                        ;void copyBufferRF2INFO()
                        ;{
copyBufferRF2INFO
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	copyBufferCOM2RF_address	0x3861
                        ;#pragma origin copyBufferCOM2RF_address
        ORG 0x3861
                        ;void copyBufferCOM2RF()
                        ;{
copyBufferCOM2RF
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	copyBufferCOM2INFO_address	0x3864
                        ;#pragma origin copyBufferCOM2INFO_address
        ORG 0x3864
                        ;void copyBufferCOM2INFO()
                        ;{
copyBufferCOM2INFO
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	copyMemoryBlock_address	0x3867
                        ;#pragma origin copyMemoryBlock_address
        ORG 0x3867
                        ;void copyMemoryBlock(uns16 from @ FSR0, uns16 to @ FSR1, uns8 length @ W)
                        ;{
copyMemoryBlock
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ; #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	startDelay_address	0x386a
                        ;#pragma origin startDelay_address
        ORG 0x386A
                        ;void startDelay(uns8 ticks @ W)
                        ;{
startDelay
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	startLongDelay_address	0x386d
                        ;#pragma origin startLongDelay_address
        ORG 0x386D
                        ;void startLongDelay(uns16 ticks @ param3)
                        ;{
startLongDelay
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	isDelay_address	0x3870
                        ;#pragma origin isDelay_address
        ORG 0x3870
                        ;bit isDelay()
                        ;{
isDelay
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	waitDelay_address	0x3873
                        ;#pragma origin waitDelay_address
        ORG 0x3873
                        ;void waitDelay(uns8 ticks @ W)
                        ;{
waitDelay
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	waitMS_address	0x3876
                        ;#pragma origin waitMS_address
        ORG 0x3876
                        ;void waitMS(uns8 ms @ W)
                        ;{
waitMS
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	startCapture_address	0x3879
                        ;#pragma origin startCapture_address
        ORG 0x3879
                        ;void startCapture()
                        ;{
startCapture
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	captureTicks_address	0x387c
                        ;#pragma origin captureTicks_address
        ORG 0x387C
                        ;void captureTicks()
                        ;{
captureTicks
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	waitNewTick_address	0x3882
                        ;#pragma origin waitNewTick_address
        ORG 0x3882
                        ;void waitNewTick()
                        ;{
waitNewTick
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	enableSPI_address	0x3885
                        ;#pragma origin enableSPI_address
        ORG 0x3885
                        ;void enableSPI()
                        ;{
enableSPI
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	disableSPI_address	0x3888
                        ;#pragma origin disableSPI_address
        ORG 0x3888
                        ;void disableSPI()
                        ;{
disableSPI
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	startSPI_address	0x388b
                        ;#pragma origin startSPI_address
        ORG 0x388B
                        ;void startSPI(uns8 length @ W)
                        ;{
startSPI
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	stopSPI_address	0x388e
                        ;#pragma origin stopSPI_address
        ORG 0x388E
                        ;void stopSPI()
                        ;{
stopSPI
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	restartSPI_address	0x3891
                        ;#pragma origin restartSPI_address
        ORG 0x3891
                        ;void restartSPI()
                        ;{
restartSPI
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	getStatusSPI_address	0x3894
                        ;#pragma origin getStatusSPI_address
        ORG 0x3894
                        ;bit getStatusSPI()
                        ;{
getStatusSPI
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	setRFpower_address	0x3897
                        ;#pragma origin setRFpower_address
        ORG 0x3897
                        ;void setRFpower(uns8 level @ W)
                        ;{
setRFpower
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setLEDG_address	0x389a
                        ;#pragma origin setLEDG_address
        ORG 0x389A
                        ;void setLEDG()
                        ;{
setLEDG
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setRFchannel_address	0x389d
                        ;#pragma origin setRFchannel_address
        ORG 0x389D
                        ;void setRFchannel(uns8 channel @ W)
                        ;{
setRFchannel
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setRFmode_address	0x38a0
                        ;#pragma origin setRFmode_address
        ORG 0x38A0
                        ;void setRFmode(uns8 mode @ W)
                        ;{
setRFmode
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setRFspeed_address	0x38a3
                        ;#pragma origin setRFspeed_address
        ORG 0x38A3
                        ;void setRFspeed(uns8 speed @ W)
                        ;{
setRFspeed
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setRFsleep_address	0x38a6
                        ;#pragma origin setRFsleep_address
        ORG 0x38A6
                        ;void setRFsleep()
                        ;{
setRFsleep
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setRFready_address	0x38a9
                        ;#pragma origin setRFready_address
        ORG 0x38A9
                        ;void setRFready()
                        ;{
setRFready
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	RFTXpacket_address	0x38ac
                        ;#pragma origin RFTXpacket_address
        ORG 0x38AC
                        ;void RFTXpacket()
                        ;{
RFTXpacket
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	RFRXpacket_address	0x38af
                        ;#pragma origin RFRXpacket_address
        ORG 0x38AF
                        ;bit RFRXpacket()
                        ;{
RFRXpacket
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	checkRF_address	0x38b2
                        ;#pragma origin checkRF_address
        ORG 0x38B2
                        ;bit checkRF(uns8 level @ W)
                        ;{
checkRF
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	amIBonded_address	0x38b8
                        ;#pragma origin amIBonded_address
        ORG 0x38B8
                        ;bit amIBonded()
                        ;{
amIBonded
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	removeBond_address	0x38bb
                        ;#pragma origin removeBond_address
        ORG 0x38BB
                        ;void removeBond()
                        ;{
removeBond
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	bondNewNode_address	0x38be
                        ;#pragma origin bondNewNode_address
        ORG 0x38BE
                        ;bit bondNewNode(uns8 address @ W)
                        ;{
bondNewNode
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	isBondedNode_address	0x38c1
                        ;#pragma origin isBondedNode_address
        ORG 0x38C1
                        ;bit isBondedNode(uns8 node @ W)
                        ;{
isBondedNode
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	removeBondedNode_address	0x38c4
                        ;#pragma origin removeBondedNode_address
        ORG 0x38C4
                        ;void removeBondedNode(uns8 node @ W)
                        ;{
removeBondedNode
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	rebondNode_address	0x38c7
                        ;#pragma origin rebondNode_address
        ORG 0x38C7
                        ;bit rebondNode(uns8 node @ W)
                        ;{
rebondNode
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	clearAllBonds_address	0x38ca
                        ;#pragma origin clearAllBonds_address
        ORG 0x38CA
                        ;void clearAllBonds()
                        ;{
clearAllBonds
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setNonetMode_address	0x38cd
                        ;#pragma origin setNonetMode_address
        ORG 0x38CD
                        ;void setNonetMode()
                        ;{
setNonetMode
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setCoordinatorMode_address	0x38d0
                        ;#pragma origin setCoordinatorMode_address
        ORG 0x38D0
                        ;void setCoordinatorMode()
                        ;{
setCoordinatorMode
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setNodeMode_address	0x38d3
                        ;#pragma origin setNodeMode_address
        ORG 0x38D3
                        ;void setNodeMode()
                        ;{
setNodeMode
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setNetworkFilteringOn_address	0x38d6
                        ;#pragma origin setNetworkFilteringOn_address
        ORG 0x38D6
                        ;void setNetworkFilteringOn()
                        ;{
setNetworkFilteringOn
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setNetworkFilteringOff_address	0x38d9
                        ;#pragma origin setNetworkFilteringOff_address
        ORG 0x38D9
                        ;void setNetworkFilteringOff()
                        ;{
setNetworkFilteringOff
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	getNetworkParams_address	0x38dc
                        ;#pragma origin getNetworkParams_address
        ORG 0x38DC
                        ;uns8 getNetworkParams()
                        ;{
getNetworkParams
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return 1;
        RETLW 0x01
                        ;}
                        ;
                        ;#define	setRoutingOn_address	0x38df
                        ;#pragma origin setRoutingOn_address
        ORG 0x38DF
                        ;void setRoutingOn()
                        ;{
setRoutingOn
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setRoutingOff_address	0x38e2
                        ;#pragma origin setRoutingOff_address
        ORG 0x38E2
                        ;void setRoutingOff()
                        ;{
setRoutingOff
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	answerSystemPacket_address	0x38e8
                        ;#pragma origin answerSystemPacket_address
        ORG 0x38E8
                        ;void answerSystemPacket()
                        ;{
answerSystemPacket
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	discovery_address	0x38eb
                        ;#pragma origin discovery_address
        ORG 0x38EB
                        ;uns8 discovery(uns8 MaxNodeAddress @ W)
                        ;{
discovery
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return 1;
        RETLW 0x01
                        ;}
                        ;
                        ;#define	wasRouted_address	0x38ee
                        ;#pragma origin wasRouted_address
        ORG 0x38EE
                        ;bit wasRouted()
                        ;{
wasRouted
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	optimizeHops_address	0x38f1
                        ;#pragma origin optimizeHops_address
        ORG 0x38F1
                        ;bit optimizeHops(uns8 method @ W)
                        ;{
optimizeHops
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	getSupplyVoltage_address	0x38f4
                        ;#pragma origin getSupplyVoltage_address
        ORG 0x38F4
                        ;uns8 getSupplyVoltage()
                        ;{
getSupplyVoltage
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return 1;
        RETLW 0x01
                        ;}
                        ;
                        ;#define	getTemperature_address	0x38f7
                        ;#pragma origin getTemperature_address
        ORG 0x38F7
                        ;int8 getTemperature()
                        ;{
getTemperature
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return W;
        RETURN
                        ;}
                        ;
                        ;#define	clearBufferRF_address	0x38fa
                        ;#pragma origin clearBufferRF_address
        ORG 0x38FA
                        ;void clearBufferRF()
                        ;{
clearBufferRF
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	isDiscoveredNode_address	0x3910
                        ;#pragma origin isDiscoveredNode_address
        ORG 0x3910
                        ;bit isDiscoveredNode(uns8 address @ W)
                        ;{
isDiscoveredNode
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	enableRFPGM_address	0x3913
                        ;#pragma origin enableRFPGM_address
        ORG 0x3913
                        ;void enableRFPGM()
                        ;{
enableRFPGM
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	disableRFPGM_address	0x3916
                        ;#pragma origin disableRFPGM_address
        ORG 0x3916
                        ;void disableRFPGM()
                        ;{
disableRFPGM
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setupRFPGM_address	0x3919
                        ;#pragma origin setupRFPGM_address
        ORG 0x3919
                        ;void setupRFPGM(uns8 x @ W)
                        ;{
setupRFPGM
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	runRFPGM_address	0x391c
                        ;#pragma origin runRFPGM_address
        ORG 0x391C
                        ;void runRFPGM()
                        ;{
runRFPGM
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	iqrfDeepSleep_address	0x391f
                        ;#pragma origin iqrfDeepSleep_address
        ORG 0x391F
                        ;void iqrfDeepSleep()
                        ;{
iqrfDeepSleep
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	wasRFICrestarted_address	0x3922
                        ;#pragma origin wasRFICrestarted_address
        ORG 0x3922
                        ;uns8 wasRFICrestarted()
                        ;{
wasRFICrestarted
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return 1;
        RETLW 0x01
                        ;}
                        ;
                        ;#define	eeeWriteData_address	0x3925
                        ;#pragma origin eeeWriteData_address
        ORG 0x3925
                        ;bit eeeWriteData(uns16 address @ param3)
                        ;{
eeeWriteData
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	eeeReadData_address	0x3928
                        ;#pragma origin eeeReadData_address
        ORG 0x3928
                        ;bit eeeReadData(uns16 address @ param3)
                        ;{
eeeReadData
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	setINDF0_address	0x3931
                        ;#pragma origin setINDF0_address
        ORG 0x3931
                        ;void setINDF0(uns8 value @ W)
                        ;{
setINDF0
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setINDF1_address	0x3934
                        ;#pragma origin setINDF1_address
        ORG 0x3934
                        ;void setINDF1(uns8 value @ W)
                        ;{
setINDF1
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	getRSSI_address	0x3937
                        ;#pragma origin getRSSI_address
        ORG 0x3937
                        ;uns8 getRSSI()
                        ;{
getRSSI
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return 1;
        RETLW 0x01
                        ;}
                        ;
                        ;#define	removeBondAddress_address	0x393a
                        ;#pragma origin removeBondAddress_address
        ORG 0x393A
                        ;void removeBondAddress()
                        ;{
removeBondAddress
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	sendFRC_address	0x393d
                        ;#pragma origin sendFRC_address
        ORG 0x393D
                        ;uns8 sendFRC(uns8 command @ W)
                        ;{
sendFRC
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return 1;
        RETLW 0x01
                        ;}
                        ;
                        ;#define	responseFRC_address	0x3940
                        ;#pragma origin responseFRC_address
        ORG 0x3940
                        ;void responseFRC()
                        ;{
responseFRC
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	bondRequestAdvanced_address	0x3943
                        ;#pragma origin bondRequestAdvanced_address
        ORG 0x3943
                        ;bit bondRequestAdvanced()
                        ;{
bondRequestAdvanced
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	prebondNodeAtNode_address	0x3946
                        ;#pragma origin prebondNodeAtNode_address
        ORG 0x3946
                        ;bit prebondNodeAtNode()
                        ;{
prebondNodeAtNode
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	nodeAuthorization_address	0x3949
                        ;#pragma origin nodeAuthorization_address
        ORG 0x3949
                        ;bit nodeAuthorization(uns8 address @ W)
                        ;{
nodeAuthorization
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	dummy01_address	0x394c
                        ;#pragma origin dummy01_address
        ORG 0x394C
                        ;void dummy01()
                        ;{
dummy01
                        ;  #asm
                        ;  DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setAccessPassword_address	0x3958
                        ;#pragma origin setAccessPassword_address
        ORG 0x3958
                        ;void setAccessPassword()
                        ;{
setAccessPassword
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	setUserKey_address	0x395b
                        ;#pragma origin setUserKey_address
        ORG 0x395B
                        ;void setUserKey()
                        ;{
setUserKey
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	amIRecipientOfFRC_address	0x3961
                        ;#pragma origin amIRecipientOfFRC_address
        ORG 0x3961
                        ;bit amIRecipientOfFRC()
                        ;{
amIRecipientOfFRC
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	setLEDR_address	0x3964
                        ;#pragma origin setLEDR_address
        ORG 0x3964
                        ;void setLEDR()
                        ;{
setLEDR
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	encryptBufferRF_address	0x3967
                        ;#pragma origin encryptBufferRF_address
        ORG 0x3967
                        ;void encryptBufferRF(uns8 blocks @ W)
                        ;{
encryptBufferRF
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	decryptBufferRF_address	0x396a
                        ;#pragma origin decryptBufferRF_address
        ORG 0x396A
                        ;void decryptBufferRF(uns8 blocks @ W)
                        ;{
decryptBufferRF
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	prebondNodeAtCoordinator_address	0x396d
                        ;#pragma origin prebondNodeAtCoordinator_address
        ORG 0x396D
                        ;bit prebondNodeAtCoordinator(uns8 address @ W)
                        ;{
prebondNodeAtCoordinator
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	setFSRs_address	0x3970
                        ;#pragma origin setFSRs_address
        ORG 0x3970
                        ;uns8 setFSRs(uns8 fsrs @ W)
                        ;{
setFSRs
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return 1;
        RETLW 0x01
                        ;}
                        ;
                        ;// For internal usage only
                        ;#define	updateCRC16_address	0x3973
                        ;#pragma origin updateCRC16_address
        ORG 0x3973
                        ;void updateCRC16(uns8 value @ W)
                        ;{
updateCRC16
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;}
        RETURN
                        ;
                        ;#define	smartConnect_address	0x3976
                        ;#pragma origin smartConnect_address
        ORG 0x3976
                        ;bit smartConnect(uns8 address @ W)
                        ;{
smartConnect
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return Carry;
        RETURN
                        ;}
                        ;
                        ;#define	addressBitmap_address	0x3979
                        ;#pragma origin addressBitmap_address
        ORG 0x3979
                        ;uns8 addressBitmap(uns8 bitIndex @ W)
                        ;{
addressBitmap
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return 1;
        RETLW 0x01
                        ;}
                        ;
                        ;#define	setServiceChannel_address	0x397c
                        ;#pragma origin setServiceChannel_address
        ORG 0x397C
                        ;bit setServiceChannel(uns8 channelNumber @ W)
                        ;{
setServiceChannel
                        ;  #asm
                        ;    DW 0x2000
        DW    0x2000
                        ;  #endasm
                        ;  #pragma updateBank exit=UserBank_01
                        ;  return 1;
        BSF   0x03,Carry
        RETURN
                        ;}
                        ;
                        ;#pragma optimize 1
                        ;#pragma update_RP 1
                        ;#pragma update_PAGE 1
                        ;#pragma origin __APPLICATION_ADDRESS
        ORG 0x3A00

  ; FILE ../../../../Development/include/DPA/DPA.h
                        ;// *********************************************************************
                        ;//   General public DPA header file                                    *
                        ;// *********************************************************************
                        ;// Copyright (c) IQRF Tech s.r.o.
                        ;//
                        ;// File:    $RCSfile: DPA.h,v $
                        ;// Version: $Revision: 1.275 $
                        ;// Date:    $Date: 2020/02/27 13:17:17 $
                        ;//
                        ;// Revision history:
                        ;//   2020/02/27  Release for DPA 4.13
                        ;//   2020/01/09  Release for DPA 4.12
                        ;//   2019/12/11  Release for DPA 4.11
                        ;//   2019/10/09  Release for DPA 4.10
                        ;//   2019/06/12  Release for DPA 4.03
                        ;//   2019/06/03  Release for DPA 4.02
                        ;//   2019/03/07  Release for DPA 4.01
                        ;//   2019/01/10  Release for DPA 4.00
                        ;//   2018/10/25  Release for DPA 3.03
                        ;//   2017/11/16  Release for DPA 3.02
                        ;//   2017/08/14  Release for DPA 3.01
                        ;//   2017/03/13  Release for DPA 3.00
                        ;//   2016/09/12  Release for DPA 2.28
                        ;//   2016/04/14  Release for DPA 2.27
                        ;//   2016/03/03  Release for DPA 2.26
                        ;//   2016/01/21  Release for DPA 2.25
                        ;//   2015/12/01  Release for DPA 2.24
                        ;//   2015/10/23  Release for DPA 2.23
                        ;//   2015/09/25  Release for DPA 2.22
                        ;//   2015/09/03  Release for DPA 2.21
                        ;//   2015/08/05  Release for DPA 2.20
                        ;//   2014/10/31  Release for DPA 2.10
                        ;//   2014/04/30  Release for DPA 2.00
                        ;//   2013/10/03  Release for DPA 1.00
                        ;//
                        ;// *********************************************************************
                        ;
                        ;// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/
                        ;// IQRF Standards documentation https://www.iqrfalliance.org/iqrf-interoperability/
                        ;
                        ;#ifndef _DPA_HEADER_
                        ;#define _DPA_HEADER_
                        ;
                        ;//############################################################################################
                        ;
                        ;// DPA version
                        ;#define	DPA_VERSION_MASTER			0x0413
                        ;
                        ;#ifdef __CC5X__
                        ;// Compiled only at CC5X
                        ;#if __CC5X__ < 3703
                        ;#error Insufficient CC5X compiler version, V3.7C is minimum
                        ;#endif
                        ;
                        ;#if IQRFOS < 403
                        ;#error IQRF OS 4.03+ is required
                        ;#endif
                        ;
                        ;// Bank for custom variables
                        ;#pragma rambank = UserBank_01
                        ;
                        ;// Main DPA API entry address (also start of the licensed FLASH) 
                        ;#define	DPA_API_ADDRESS				__LICENSED_FLASH
                        ;
                        ;// Main DPA entry address
                        ;#define	MAIN_DPA_ADDRESS			( DPA_API_ADDRESS + 4 )
                        ;
                        ;// Main DPA API entry address stub
                        ;#define	DPA_API_ADDRESS_ENTRY		0x3A08
                        ;
                        ;// Address of the DPA Custom Handler
                        ;#define	CUSTOM_HANDLER_ADDRESS		0x3A20
                        ;
                        ;// Address of the DPA Custom Handler end + 1
                        ;#define	CUSTOM_HANDLER_ADDRESS_END	0x3D80
                        ;
                        ;// DPA API entry function
                        ;uns8  DpaApiEntry( uns8 par1, uns8 par2, uns8 apiIndex );
                        ;
                        ;// DPA API codes
                        ;#define	DPA_API_RFTX_DPAPACKET				0
                        ;#define	DPA_API_READ_CONFIG_BYTE			1
                        ;#define	DPA_API_SEND_TO_IFACEMASTER			2
                        ;#define	DPA_API_COORDINATOR_RFTX_DPAPACKET	3
                        ;#define	DPA_API_LOCAL_REQUEST				4
                        ;#define	DPA_API_SET_PERIPHERAL_ERROR		5
                        ;#define	DPA_API_SET_RF_DEFAULTS				6
                        ;
                        ;// Used buffer size symbols
                        ;#define	sizeofBufferRF						sizeof( bufferRF )
                        ;#define	sizeofBufferAUX						sizeof( bufferAUX )
                        ;#define	sizeofBufferCOM						sizeof( bufferCOM )
                        ;
                        ;#define	STRUCTATTR
                        ;
                        ;#else //__CC5X__
                        ;// Not compiled at CC5X
                        ;
                        ;// Define CC5X types
                        ;typedef uint8_t	  uns8;
                        ;typedef uint16_t  uns16;
                        ;
                        ;// Define some types missing at Arduino
                        ;#ifdef Arduino_h
                        ;typedef int8_t  int8;
                        ;typedef int16_t int16;
                        ;#endif
                        ;
                        ;// Fake buffer sizes
                        ;#define	sizeofBufferRF  64
                        ;#define	sizeofBufferAUX 64
                        ;#define	sizeofBufferCOM 64
                        ;
                        ;// Disables alignment of members of structures
                        ;#define	STRUCTATTR	__attribute__((packed))
                        ;
                        ;#endif	// __CC5X__
                        ;
                        ;// Indexes of configuration bytes used by DpaApiReadConfigByte( index )
                        ;// Checksum
                        ;#define	CFGIND_CHECKSUM			0x00
                        ;// Embedded peripherals
                        ;#define	CFGIND_DPA_PERIPHERALS	0x01
                        ;// DPA configuration flags #0
                        ;#define	CFGIND_DPA_FLAGS		0x05
                        ;// Main RF channel, used by the subordinate network
                        ;#define	CFGIND_CHANNEL_2ND_A	0x06
                        ;// Second RF channel, used by the subordinate network
                        ;#define	CFGIND_CHANNEL_2ND_B	0x07
                        ;// TX power
                        ;#define	CFGIND_TXPOWER 			0x08
                        ;// RX filter used by checkRF()
                        ;#define	CFGIND_RXFILTER			0x09
                        ;// toutRF for LP mode
                        ;#define	CFGIND_DPA_LP_TOUTRF	0x0A
                        ;// UART interface baud rate
                        ;#define	CFGIND_DPA_UART_IFACE_SPEED 0x0B
                        ;// Alternate DSM channel
                        ;#define	CFGIND_ALTERNATE_DSM_CHANNEL 0x0C
                        ;// Main RF channel
                        ;#define	CFGIND_CHANNEL_A		0x11
                        ;// Second RF channel
                        ;#define	CFGIND_CHANNEL_B		0x12
                        ;
                        ;// 0: checks only mandatory precondition in order to prevent critical side-effects
                        ;// 1: as above plus checks meaningful parameter conditions
                        ;// 2: full implemented parameter checking (default)
                        ;#ifndef PARAM_CHECK_LEVEL
                        ;#define	PARAM_CHECK_LEVEL 2
                        ;#endif
                        ;
                        ;// "foursome" at IFace structure
                        ;typedef struct
                        ;{
                        ;  // Node address low byte
                        ;  uns8	NADRlow;
                        ;  // Node address high byte
                        ;  uns8	NADRhigh;
                        ;  // Peripheral number
                        ;  uns8	PNUM;
                        ;  // Peripheral command
                        ;  uns8	PCMD;
                        ;  // HWPID
                        ;  uns16	HWPID;
                        ;} STRUCTATTR TDpaIFaceHeader;
                        ;
                        ;// Maximum command PCMD value (except reserved 0x3F = CMD_GET_PER_INFO)
                        ;#define	PCMD_MAX					0x7f
                        ;// Bit mask at PCMD that indicates DPA Response message
                        ;#define	RESPONSE_FLAG				0x80
                        ;
                        ;// IQMESH coordinator address
                        ;#define COORDINATOR_ADDRESS			0x00
                        ;// IQMESH broadcast address
                        ;#define BROADCAST_ADDRESS			0xff
                        ;// IQMESH temporary address, assigned by pre-bonding before authorization is done
                        ;#define TEMPORARY_ADDRESS			0xfe
                        ;// Address of the local device addressed by IFace
                        ;#define LOCAL_ADDRESS				0xfc
                        ;// Maximum IQMESH network device address
                        ;#define MAX_ADDRESS					( 240 - 1 )
                        ;
                        ;// Time slots lengths in 10 ms
                        ;#define	MIN_STD_TIMESLOT	4
                        ;#define	MAX_STD_TIMESLOT	6
                        ;
                        ;#define	MIN_LP_TIMESLOT		8
                        ;#define	MAX_LP_TIMESLOT		10
                        ;
                        ;// Maximum number of DPA PData bytes ( minus 8 = 6B foursome + 8b error code + 8b DpaValue )
                        ;#define DPA_MAX_DATA_LENGTH			( sizeofBufferCOM - sizeof( TDpaIFaceHeader ) - 2 * sizeof( uns8 ) )
                        ;
                        ;// Maximum number of peripherals info that can fit in the message
                        ;#define	MAX_PERIPHERALS_PER_BLOCK_INFO	( DPA_MAX_DATA_LENGTH / sizeof( TPeripheralInfoAnswer ) )
                        ;
                        ;// Embedded peripheral numbers
                        ;#define	PNUM_COORDINATOR	0x00
                        ;#define	PNUM_NODE			0x01
                        ;#define	PNUM_OS				0x02
                        ;#define	PNUM_EEPROM			0x03
                        ;#define	PNUM_EEEPROM		0x04
                        ;#define	PNUM_RAM			0x05
                        ;#define	PNUM_LEDR			0x06
                        ;#define	PNUM_LEDG			0x07
                        ;#define	PNUM_SPI			0x08
                        ;#define	PNUM_IO				0x09
                        ;#define	PNUM_THERMOMETER	0x0A
                        ;#define	PNUM_UART			0x0C
                        ;#define	PNUM_FRC			0x0D
                        ;
                        ;// Number of the 1st user peripheral
                        ;#define	PNUM_USER			0x20
                        ;// Number of the last user peripheral
                        ;#define	PNUM_USER_MAX		0x3E
                        ;// Maximum peripheral number
                        ;#define	PNUM_MAX			0x7F
                        ;
                        ;// Fake peripheral number used to flag DPA response with error sent by RF
                        ;#define	PNUM_ERROR_FLAG		0xFE
                        ;// Special peripheral used for enumeration
                        ;#define	PNUM_ENUMERATION	0xFF
                        ;
                        ;// DPA Commands for embedded peripherals
                        ;#define	CMD_COORDINATOR_ADDR_INFO  0
                        ;#define	CMD_COORDINATOR_DISCOVERED_DEVICES 1
                        ;#define	CMD_COORDINATOR_BONDED_DEVICES 2
                        ;#define	CMD_COORDINATOR_CLEAR_ALL_BONDS 3
                        ;#define	CMD_COORDINATOR_BOND_NODE 4
                        ;#define	CMD_COORDINATOR_REMOVE_BOND 5
                        ;#define	CMD_COORDINATOR_DISCOVERY 7
                        ;#define	CMD_COORDINATOR_SET_DPAPARAMS 8
                        ;#define	CMD_COORDINATOR_SET_HOPS 9
                        ;#define	CMD_COORDINATOR_BACKUP 11
                        ;#define	CMD_COORDINATOR_RESTORE 12
                        ;#define	CMD_COORDINATOR_AUTHORIZE_BOND 13
                        ;#define	CMD_COORDINATOR_BRIDGE 14
                        ;#define	CMD_COORDINATOR_SMART_CONNECT 18
                        ;#define	CMD_COORDINATOR_SET_MID 19
                        ;
                        ;#define	CMD_NODE_READ 0
                        ;#define	CMD_NODE_REMOVE_BOND 1
                        ;#define	CMD_NODE_BACKUP 6
                        ;#define	CMD_NODE_RESTORE 7
                        ;#define	CMD_NODE_VALIDATE_BONDS 8
                        ;
                        ;#define	CMD_OS_READ 0
                        ;#define	CMD_OS_RESET 1
                        ;#define	CMD_OS_READ_CFG 2
                        ;#define	CMD_OS_RFPGM 3
                        ;#define	CMD_OS_SLEEP 4
                        ;#define	CMD_OS_BATCH 5
                        ;#define	CMD_OS_SET_SECURITY 6
                        ;#define	CMD_OS_INDICATE 7
                        ;#define	CMD_OS_RESTART 8
                        ;#define	CMD_OS_WRITE_CFG_BYTE 9
                        ;#define	CMD_OS_LOAD_CODE 10
                        ;#define	CMD_OS_SELECTIVE_BATCH 11
                        ;#define	CMD_OS_TEST_RF_SIGNAL 12
                        ;#define	CMD_OS_FACTORY_SETTINGS 13
                        ;#define	CMD_OS_WRITE_CFG 15
                        ;
                        ;#define	CMD_RAM_READ 0
                        ;#define	CMD_RAM_WRITE 1
                        ;#define	CMD_RAM_READ_ANY    15
                        ;
                        ;#define	CMD_EEPROM_READ CMD_RAM_READ
                        ;#define	CMD_EEPROM_WRITE CMD_RAM_WRITE
                        ;
                        ;#define	CMD_EEEPROM_XREAD ( CMD_RAM_READ + 2 )
                        ;#define	CMD_EEEPROM_XWRITE ( CMD_RAM_WRITE + 2 )
                        ;
                        ;#define	CMD_LED_SET_OFF 0
                        ;#define	CMD_LED_SET_ON 1
                        ;#define	CMD_LED_PULSE 3
                        ;#define	CMD_LED_FLASHING 4
                        ;
                        ;#define	CMD_SPI_WRITE_READ 0
                        ;
                        ;#define	CMD_IO_DIRECTION  0
                        ;#define	CMD_IO_SET	1
                        ;#define	CMD_IO_GET	2
                        ;
                        ;#define	CMD_THERMOMETER_READ 0
                        ;
                        ;#define	CMD_UART_OPEN 0
                        ;#define	CMD_UART_CLOSE 1
                        ;#define	CMD_UART_WRITE_READ 2
                        ;#define	CMD_UART_CLEAR_WRITE_READ 3
                        ;
                        ;#define	CMD_FRC_SEND 0
                        ;#define	CMD_FRC_EXTRARESULT 1
                        ;#define	CMD_FRC_SEND_SELECTIVE 2
                        ;#define	CMD_FRC_SET_PARAMS 3
                        ;
                        ;#define	CMD_GET_PER_INFO  0x3f
                        ;
                        ;// DPA peripheral type
                        ;typedef enum
                        ;{
                        ;  PERIPHERAL_TYPE_DUMMY = 0x00,
                        ;  PERIPHERAL_TYPE_COORDINATOR = 0x01,
                        ;  PERIPHERAL_TYPE_NODE = 0x02,
                        ;  PERIPHERAL_TYPE_OS = 0x03,
                        ;  PERIPHERAL_TYPE_EEPROM = 0x04,
                        ;  PERIPHERAL_TYPE_BLOCK_EEPROM = 0x05,
                        ;  PERIPHERAL_TYPE_RAM = 0x06,
                        ;  PERIPHERAL_TYPE_LED = 0x07,
                        ;  PERIPHERAL_TYPE_SPI = 0x08,
                        ;  PERIPHERAL_TYPE_IO = 0x09,
                        ;  PERIPHERAL_TYPE_UART = 0x0a,
                        ;  PERIPHERAL_TYPE_THERMOMETER = 0x0b,
                        ;  PERIPHERAL_TYPE_ADC = 0x0c,
                        ;  PERIPHERAL_TYPE_PWM = 0x0d,
                        ;  PERIPHERAL_TYPE_FRC = 0x0e,
                        ;  // Starts peripheral type number interval for user peripherals
                        ;  PERIPHERAL_TYPE_USER_AREA = 0x80
                        ;} TDpaPeripheralType;
                        ;
                        ;// Peripheral extended information
                        ;typedef enum
                        ;{
                        ;  PERIPHERAL_TYPE_EXTENDED_DEFAULT = 0x00,
                        ;  PERIPHERAL_TYPE_EXTENDED_READ = 0x01,
                        ;  PERIPHERAL_TYPE_EXTENDED_WRITE = 0x02,
                        ;  PERIPHERAL_TYPE_EXTENDED_READ_WRITE = PERIPHERAL_TYPE_EXTENDED_READ | PERIPHERAL_TYPE_EXTENDED_WRITE
                        ;} TDpaPeripheralTypeExtended;
                        ;
                        ;// Response packet error codes
                        ;typedef enum
                        ;{
                        ;  // No error
                        ;  STATUS_NO_ERROR = 0,
                        ;
                        ;  // General fail
                        ;  ERROR_FAIL = 1,
                        ;  // Incorrect PCMD
                        ;  ERROR_PCMD = 2,
                        ;  // Incorrect PNUM or PCMD
                        ;  ERROR_PNUM = 3,
                        ;  // Incorrect Address value when addressing memory type peripherals
                        ;  ERROR_ADDR = 4,
                        ;  // Incorrect Data length
                        ;  ERROR_DATA_LEN = 5,
                        ;  // Incorrect Data
                        ;  ERROR_DATA = 6,
                        ;  // Incorrect HWPID used
                        ;  ERROR_HWPID = 7,
                        ;  // Incorrect NADR
                        ;  ERROR_NADR = 8,
                        ;  // IFACE data consumed by Custom DPA Handler
                        ;  ERROR_IFACE_CUSTOM_HANDLER = 9,
                        ;  // Custom DPA Handler is missing
                        ;  ERROR_MISSING_CUSTOM_DPA_HANDLER = 10,
                        ;
                        ;  // Beginning of the user code error interval
                        ;  ERROR_USER_FROM = 0x20,
                        ;  // End of the user code error interval
                        ;  ERROR_USER_TO = 0x3f,
                        ;
                        ;  // Bit/flag reserved for a future use
                        ;  STATUS_RESERVED_FLAG = 0x40,
                        ;  // Bit to flag asynchronous DPA Response from [N]
                        ;  STATUS_ASYNC_RESPONSE = 0x80,
                        ;  // Error code used to mark DPA Confirmation
                        ;  STATUS_CONFIRMATION = 0xff
                        ;} TErrorCodes;
                        ;
                        ;// Embedded FRC commands
                        ;typedef enum
                        ;{
                        ;  // 2 bits
                        ;  FRC_Ping = 0x00,
                        ;  FRC_AcknowledgedBroadcastBits = 0x02,
                        ;  FRC_PrebondedAlive = 0x03,
                        ;  FRC_SupplyVoltage = 0x04,
                        ;  // 1 byte
                        ;  FRC_Temperature = 0x80,
                        ;  FRC_AcknowledgedBroadcastBytes = 0x81,
                        ;  FRC_MemoryRead = 0x82,
                        ;  FRC_MemoryReadPlus1 = 0x83,
                        ;  FRC_FrcResponseTime = 0x84,
                        ;  FRC_TestRFsignal = 0x85,
                        ;  // 4 bytes
                        ;  FRC_PrebondedMemoryReadPlus1 = 0xF8,
                        ;  FRC_MemoryRead4B = 0xFA
                        ;} TFRCommands;
                        ;
                        ;// Intervals of user FRC codes
                        ;#define	FRC_USER_BIT_FROM	  0x40
                        ;#define	FRC_USER_BIT_TO		  0x7F
                        ;#define	FRC_USER_BYTE_FROM	  0xC0
                        ;#define	FRC_USER_BYTE_TO	  0xDF
                        ;#define	FRC_USER_2BYTE_FROM	  0xF0
                        ;#define	FRC_USER_2BYTE_TO	  0xF7
                        ;#define	FRC_USER_4BYTE_FROM	  0xFC
                        ;#define	FRC_USER_4BYTE_TO	  0xFF
                        ;
                        ;// No HWPID specified
                        ;#define HWPID_Default         0x0000
                        ;// Use this type to override HWPID check
                        ;#define HWPID_DoNotCheck      0xFfFf
                        ;
                        ;// RAM peripheral block definitions
                        ;#define	PERIPHERAL_RAM_LENGTH		48
                        ;
                        ;// Start address of EEPROM peripheral in the real EEPROM
                        ;#ifndef COORDINATOR_CUSTOM_HANDLER // Node
                        ;#define	PERIPHERAL_EEPROM_START		( (uns8)0x00 )
                        ;#else // Coordinator
                        ;#define	PERIPHERAL_EEPROM_START		( (uns8)0x80 )
                        ;#endif
                        ;
                        ;// Length of the readable area of serial EEEPROM from the EEEPROM DPA peripheral write point of view. 
                        ;#define	EEEPROM_READ_LENGTH					0x8000
                        ;// Length of the writable area of serial EEEPROM from the EEEPROM DPA peripheral write point of view. 
                        ;#define	EEEPROM_WRITE_LENGTH				0x4000
                        ;
                        ;// Starting address of the Autoexec DPA storage at external EEPROM
                        ;#define	AUTOEXEC_EEEPROM_ADDR				0x0000
                        ;// Length of the autoexec memory block
                        ;#define	AUTOEXEC_LENGTH						sizeofBufferAUX
                        ;
                        ;// Starting address of the IO Setup DPA storage at external EEPROM
                        ;#define	IOSETUP_EEEPROM_ADDR				( AUTOEXEC_EEEPROM_ADDR + AUTOEXEC_LENGTH )
                        ;// Length of the IO setup memory block
                        ;#define	IOSETUP_LENGTH						sizeofBufferAUX
                        ;
                        ;// ---------------------------------------------------------
                        ;
                        ;// Enumerate peripherals structure
                        ;typedef struct
                        ;{
                        ;  uns16	DpaVersion;
                        ;  uns8	UserPerNr;
                        ;  uns8	EmbeddedPers[PNUM_USER / 8];
                        ;  uns16	HWPID;
                        ;  uns16	HWPIDver;
                        ;  uns8	Flags;
                        ;  uns8	UserPer[( PNUM_MAX - PNUM_USER + 1 + 7 ) / 8];
                        ;} STRUCTATTR TEnumPeripheralsAnswer;
                        ;
                        ;#define	FlagUserPer(UserPersArray,UserPerNumber)	UserPersArray[((UserPerNumber)-PNUM_USER) / 8] |= (uns8)0x01 << (((UserPerNumber)-PNUM_USER) % 8);
                        ;
                        ;// Get peripheral info structure (CMD_GET_PER_INFO)
                        ;typedef struct
                        ;{
                        ;  uns8	PerTE;
                        ;  uns8	PerT;
                        ;  uns8	Par1;
                        ;  uns8	Par2;
                        ;} STRUCTATTR TPeripheralInfoAnswer;
                        ;
                        ;// Error DPA response (PNUM_ERROR_FLAG)
                        ;typedef struct
                        ;{
                        ;  uns8	ErrN;
                        ;  uns8	PNUMoriginal;
                        ;} STRUCTATTR TErrorAnswer;
                        ;
                        ;// Structure returned by CMD_COORDINATOR_ADDR_INFO
                        ;typedef struct
                        ;{
                        ;  uns8	DevNr;
                        ;  uns8	DID;
                        ;} STRUCTATTR TPerCoordinatorAddrInfo_Response;
                        ;
                        ;// Structure for CMD_COORDINATOR_BOND_NODE
                        ;typedef struct
                        ;{
                        ;  uns8	ReqAddr;
                        ;  uns8	BondingTestRetries;
                        ;} STRUCTATTR TPerCoordinatorBondNode_Request;
                        ;
                        ;// Structure returned by CMD_COORDINATOR_BOND_NODE or CMD_COORDINATOR_SMART_CONNECT
                        ;typedef struct
                        ;{
                        ;  uns8	BondAddr;
                        ;  uns8	DevNr;
                        ;} STRUCTATTR TPerCoordinatorBondNodeSmartConnect_Response;
                        ;
                        ;// Structure for CMD_COORDINATOR_REMOVE_BOND
                        ;typedef struct
                        ;{
                        ;  uns8	BondAddr;
                        ;} STRUCTATTR TPerCoordinatorRemoveBond_Request;
                        ;
                        ;// Structure returned by CMD_COORDINATOR_REMOVE_BOND
                        ;typedef struct
                        ;{
                        ;  uns8	DevNr;
                        ;} STRUCTATTR TPerCoordinatorRemoveBond_Response;
                        ;
                        ;// Structure for CMD_COORDINATOR_DISCOVERY
                        ;typedef struct
                        ;{
                        ;  uns8	TxPower;
                        ;  uns8	MaxAddr;
                        ;} STRUCTATTR TPerCoordinatorDiscovery_Request;
                        ;
                        ;// Structure returned by CMD_COORDINATOR_DISCOVERY
                        ;typedef struct
                        ;{
                        ;  uns8	DiscNr;
                        ;} STRUCTATTR TPerCoordinatorDiscovery_Response;
                        ;
                        ;// Structure for and also returned by CMD_COORDINATOR_SET_DPAPARAMS
                        ;typedef struct
                        ;{
                        ;  uns8	DpaParam;
                        ;} STRUCTATTR TPerCoordinatorSetDpaParams_Request_Response;
                        ;
                        ;// Structure for and also returned by CMD_COORDINATOR_SET_HOPS
                        ;typedef struct
                        ;{
                        ;  uns8	RequestHops;
                        ;  uns8	ResponseHops;
                        ;} STRUCTATTR TPerCoordinatorSetHops_Request_Response;
                        ;
                        ;// Structure for CMD_COORDINATOR_BACKUP and CMD_NODE_BACKUP
                        ;typedef struct
                        ;{
                        ;  uns8	Index;
                        ;} STRUCTATTR TPerCoordinatorNodeBackup_Request;
                        ;
                        ;// Structure returned by CMD_COORDINATOR_BACKUP and CMD_NODE_BACKUP
                        ;typedef struct
                        ;{
                        ;  uns8	NetworkData[49];
                        ;} STRUCTATTR TPerCoordinatorNodeBackup_Response;
                        ;
                        ;// Structure for CMD_COORDINATOR_RESTORE and CMD_NODE_RESTORE
                        ;typedef struct
                        ;{
                        ;  uns8	NetworkData[49];
                        ;} STRUCTATTR TPerCoordinatorNodeRestore_Request;
                        ;
                        ;// Structure for CMD_COORDINATOR_AUTHORIZE_BOND
                        ;typedef struct
                        ;{
                        ;  uns8	ReqAddr;
                        ;  uns8	MID[4];
                        ;} STRUCTATTR TPerCoordinatorAuthorizeBond_Request;
                        ;
                        ;// Structure returned by CMD_COORDINATOR_AUTHORIZE_BOND
                        ;typedef struct
                        ;{
                        ;  uns8	BondAddr;
                        ;  uns8	DevNr;
                        ;} STRUCTATTR TPerCoordinatorAuthorizeBond_Response;
                        ;
                        ;// Structure for CMD_COORDINATOR_BRIDGE
                        ;typedef struct
                        ;{
                        ;  TDpaIFaceHeader subHeader;
                        ;  uns8	subPData[DPA_MAX_DATA_LENGTH - sizeof( TDpaIFaceHeader )];
                        ;} STRUCTATTR TPerCoordinatorBridge_Request;
                        ;
                        ;// Structure returned by CMD_COORDINATOR_BRIDGE
                        ;typedef struct
                        ;{
                        ;  TDpaIFaceHeader subHeader;
                        ;  uns8	subRespCode;
                        ;  uns8	subDpaValue;
                        ;  uns8	subPData[DPA_MAX_DATA_LENGTH - sizeof( TDpaIFaceHeader ) - 2 * sizeof( uns8 )];
                        ;} STRUCTATTR TPerCoordinatorBridge_Response;
                        ;
                        ;// Structure for CMD_COORDINATOR_SMART_CONNECT
                        ;typedef struct
                        ;{
                        ;  uns8  ReqAddr;
                        ;  uns8  BondingTestRetries;
                        ;  uns8  IBK[16];
                        ;  uns8  MID[4];
                        ;  uns8  reserved0[2];
                        ;  uns8  VirtualDeviceAddress;
                        ;  uns8  reserved1[9];
                        ;  uns8	UserData[4];
                        ;} STRUCTATTR TPerCoordinatorSmartConnect_Request;
                        ;
                        ;// Structure for CMD_COORDINATOR_SET_MID
                        ;typedef struct
                        ;{
                        ;  uns8  MID[4];
                        ;  uns8	BondAddr;
                        ;} STRUCTATTR TPerCoordinatorSetMID_Request;
                        ;
                        ;// Structure returned by CMD_NODE_READ
                        ;typedef struct
                        ;{
                        ;  uns8  ntwADDR;
                        ;  uns8  ntwVRN;
                        ;  uns8  ntwZIN;
                        ;  uns8  ntwDID;
                        ;  uns8  ntwPVRN;
                        ;  uns16 ntwUSERADDRESS;
                        ;  uns16 ntwID;
                        ;  uns8  ntwVRNFNZ;
                        ;  uns8  ntwCFG;
                        ;  uns8  Flags;
                        ;} STRUCTATTR TPerNodeRead_Response;
                        ;
                        ;// Structures for CMD_NODE_VALIDATE_BONDS
                        ;typedef struct
                        ;{
                        ;  uns8	Address;
                        ;  uns8  MID[4];
                        ;} STRUCTATTR TPerNodeValidateBondsItem;
                        ;
                        ;// Structure for CMD_NODE_VALIDATE_BONDS
                        ;typedef struct
                        ;{
                        ;  TPerNodeValidateBondsItem Bonds[DPA_MAX_DATA_LENGTH / sizeof( TPerNodeValidateBondsItem )];
                        ;} STRUCTATTR TPerNodeValidateBonds_Request;
                        ;
                        ;// Structure returned by CMD_OS_READ
                        ;typedef struct
                        ;{
                        ;  uns8	MID[4];
                        ;  uns8	OsVersion;
                        ;  uns8	McuType;
                        ;  uns16	OsBuild;
                        ;  uns8	Rssi;
                        ;  uns8	SupplyVoltage;
                        ;  uns8	Flags;
                        ;  uns8	SlotLimits;
                        ;  uns8  IBK[16];
                        ;  // Enumerate peripherals part, variable length because of UserPer field
                        ;  uns16	DpaVersion;
                        ;  uns8	UserPerNr;
                        ;  uns8	EmbeddedPers[PNUM_USER / 8];
                        ;  uns16	HWPID;
                        ;  uns16	HWPIDver;
                        ;  uns8	FlagsEnum;
                        ;  uns8	UserPer[( PNUM_MAX - PNUM_USER + 1 + 7 ) / 8];
                        ;} STRUCTATTR TPerOSRead_Response;
                        ;
                        ;// Structure returned by CMD_OS_READ_CFG
                        ;typedef struct
                        ;{
                        ;  uns8	Checksum;
                        ;  uns8	Configuration[31];
                        ;  uns8	RFPGM;
                        ;  uns8	Undocumented[1];
                        ;} STRUCTATTR TPerOSReadCfg_Response;
                        ;
                        ;// Structure for CMD_OS_WRITE_CFG
                        ;typedef struct
                        ;{
                        ;  uns8	Undefined;
                        ;  uns8	Configuration[31];
                        ;  uns8	RFPGM;
                        ;} STRUCTATTR TPerOSWriteCfg_Request;
                        ;
                        ;// Structures for CMD_OS_WRITE_CFG_BYTE
                        ;typedef struct
                        ;{
                        ;  uns8	Address;
                        ;  uns8	Value;
                        ;  uns8	Mask;
                        ;} STRUCTATTR TPerOSWriteCfgByteTriplet;
                        ;
                        ;// Structure for CMD_OS_WRITE_CFG_BYTE
                        ;typedef struct
                        ;{
                        ;  TPerOSWriteCfgByteTriplet Triplets[DPA_MAX_DATA_LENGTH / sizeof( TPerOSWriteCfgByteTriplet )];
                        ;} STRUCTATTR TPerOSWriteCfgByte_Request;
                        ;
                        ;// Structure for CMD_OS_SET_SECURITY
                        ;typedef struct
                        ;{
                        ;  uns8	Type;
                        ;  uns8	Data[16];
                        ;} STRUCTATTR TPerOSSetSecurity_Request;
                        ;
                        ;// Structure for CMD_OS_LOAD_CODE
                        ;typedef struct
                        ;{
                        ;  uns8	Flags;
                        ;  uns16	Address;
                        ;  uns16	Length;
                        ;  uns16	CheckSum;
                        ;} STRUCTATTR TPerOSLoadCode_Request;
                        ;
                        ;// Structure for CMD_OS_SLEEP
                        ;typedef struct
                        ;{
                        ;  uns16	Time;
                        ;  uns8	Control;
                        ;} STRUCTATTR TPerOSSleep_Request;
                        ;
                        ;// Structure for CMD_OS_SELECTIVE_BATCH
                        ;typedef struct
                        ;{
                        ;  uns8	SelectedNodes[30];
                        ;  uns8	Requests[DPA_MAX_DATA_LENGTH - 30];
                        ;} STRUCTATTR TPerOSSelectiveBatch_Request;
                        ;
                        ;// Structure for CMD_OS_TEST_RF_SIGNAL request
                        ;typedef struct
                        ;{
                        ;  uns8  Channel;
                        ;  uns8  RXfilter;
                        ;  uns16 Time;
                        ;} STRUCTATTR TPerOSTestRfSignal_Request;
                        ;
                        ;// Structure for CMD_OS_TEST_RF_SIGNAL response
                        ;typedef struct
                        ;{
                        ;  uns8  Counter;
                        ;} STRUCTATTR TPerOSTestRfSignal_Response;
                        ;
                        ;// Structure for CMD_OS_INDICATE request
                        ;typedef struct
                        ;{
                        ;  uns8  Control;
                        ;} STRUCTATTR TPerOSIndicate_Request;
                        ;
                        ;// Structure for general memory request
                        ;typedef struct
                        ;{
                        ;  // Address of data to write or read
                        ;  uns8	Address;
                        ;
                        ;  union
                        ;  {
                        ;    // Memory read request
                        ;    struct
                        ;    {
                        ;      // Length of data to read
                        ;      uns8	Length;
                        ;    } Read;
                        ;
                        ;    // Size of Address field
                        ;#define	MEMORY_WRITE_REQUEST_OVERHEAD	( sizeof( uns8 ) )
                        ;
                        ;    // Memory write request
                        ;    struct
                        ;    {
                        ;      uns8	PData[DPA_MAX_DATA_LENGTH - MEMORY_WRITE_REQUEST_OVERHEAD];
                        ;    } Write;
                        ;
                        ;  } ReadWrite;
                        ;} STRUCTATTR TPerMemoryRequest;
                        ;
                        ;// Structure for general extended memory request
                        ;typedef struct
                        ;{
                        ;  // Address of data to write or read
                        ;  uns16	Address;
                        ;
                        ;  union
                        ;  {
                        ;    // Memory read request
                        ;    struct
                        ;    {
                        ;      // Length of data to read
                        ;      uns8	Length;
                        ;    } Read;
                        ;
                        ;    // Size of Address field
                        ;#define	XMEMORY_WRITE_REQUEST_OVERHEAD	( sizeof( uns16 ) )
                        ;
                        ;    // Memory write request
                        ;    struct
                        ;    {
                        ;      uns8	PData[DPA_MAX_DATA_LENGTH - XMEMORY_WRITE_REQUEST_OVERHEAD];
                        ;    } Write;
                        ;
                        ;  } ReadWrite;
                        ;} STRUCTATTR TPerXMemoryRequest;
                        ;
                        ;// Structure for CMD_IO requests
                        ;typedef struct
                        ;{
                        ;  uns8  Port;
                        ;  uns8  Mask;
                        ;  uns8  Value;
                        ;} STRUCTATTR TPerIOTriplet;
                        ;
                        ;typedef struct
                        ;{
                        ;  uns8  Header;	// == PNUM_IO_DELAY
                        ;  uns16 Delay;
                        ;} STRUCTATTR TPerIODelay;
                        ;
                        ;// Union for CMD_IO_SET and CMD_IO_DIRECTION requests
                        ;typedef union
                        ;{
                        ;  TPerIOTriplet Triplets[DPA_MAX_DATA_LENGTH / sizeof( TPerIOTriplet )];
                        ;  TPerIODelay   Delays[DPA_MAX_DATA_LENGTH / sizeof( TPerIODelay )];
                        ;} STRUCTATTR TPerIoDirectionAndSet_Request;
                        ;
                        ;// Structure returned by CMD_THERMOMETER_READ
                        ;typedef struct
                        ;{
                        ;  int8  IntegerValue;
                        ;  int16 SixteenthValue;
                        ;} STRUCTATTR TPerThermometerRead_Response;
                        ;
                        ;// Structure for CMD_UART_OPEN
                        ;typedef struct
                        ;{
                        ;  uns8  BaudRate;
                        ;} STRUCTATTR TPerUartOpen_Request;
                        ;
                        ;// Structure for CMD_UART_[CLEAR_]WRITE_READ and CMD_SPI_WRITE_READ
                        ;typedef struct
                        ;{
                        ;  uns8  ReadTimeout;
                        ;  uns8	WrittenData[DPA_MAX_DATA_LENGTH - sizeof( uns8 )];
                        ;} STRUCTATTR TPerUartSpiWriteRead_Request;
                        ;
                        ;// Structure for CMD_FRC_SEND
                        ;typedef struct
                        ;{
                        ;  uns8  FrcCommand;
                        ;  uns8	UserData[30];
                        ;} STRUCTATTR TPerFrcSend_Request;
                        ;
                        ;// Structure for CMD_FRC_SEND_SELECTIVE
                        ;typedef struct
                        ;{
                        ;  uns8  FrcCommand;
                        ;  uns8	SelectedNodes[30];
                        ;  uns8	UserData[25];
                        ;} STRUCTATTR TPerFrcSendSelective_Request;
                        ;
                        ;// Structure returned by CMD_FRC_SEND and CMD_FRC_SEND_SELECTIVE
                        ;typedef struct
                        ;{
                        ;  uns8  Status;
                        ;  uns8	FrcData[DPA_MAX_DATA_LENGTH - sizeof( uns8 )];
                        ;} STRUCTATTR TPerFrcSend_Response;
                        ;
                        ;// Structure for request and response of CMD_FRC_SET_PARAMS
                        ;typedef struct
                        ;{
                        ;  uns8	FRCresponseTime;
                        ;} STRUCTATTR TPerFrcSetParams_RequestResponse;
                        ;
                        ;// Interface and CMD_COORDINATOR_BRIDGE confirmation structure
                        ;typedef struct
                        ;{
                        ;  // Number of hops
                        ;  uns8  Hops;
                        ;  // Time slot length in 10ms
                        ;  uns8  TimeSlotLength;
                        ;  // Number of hops for response
                        ;  uns8  HopsResponse;
                        ;} STRUCTATTR TIFaceConfirmation;
                        ;
                        ;// ---------------------------------------------------------
                        ;
                        ;// DPA Message data structure (packet w/o NADR, PNUM, PCMD, HWPID)
                        ;typedef union
                        ;{
                        ;  // General DPA request
                        ;  struct
                        ;  {
                        ;    uns8	PData[DPA_MAX_DATA_LENGTH];
                        ;  } Request;
                        ;
                        ;  // General DPA response
                        ;  struct
                        ;  {
                        ;    uns8	PData[DPA_MAX_DATA_LENGTH];
                        ;  } Response;
                        ;
                        ;  // Enumerate peripherals structure
                        ;  TEnumPeripheralsAnswer EnumPeripheralsAnswer;
                        ;
                        ;  // Get peripheral info structure (CMD_GET_PER_INFO)
                        ;  TPeripheralInfoAnswer PeripheralInfoAnswer;
                        ;
                        ;  // Get peripheral info structure (CMD_GET_PER_INFO) for more peripherals
                        ;  TPeripheralInfoAnswer PeripheralInfoAnswers[MAX_PERIPHERALS_PER_BLOCK_INFO];
                        ;
                        ;  // Error DPA response (PNUM_ERROR_FLAG)
                        ;  TErrorAnswer ErrorAnswer;
                        ;
                        ;  // Structure returned by CMD_COORDINATOR_ADDR_INFO
                        ;  TPerCoordinatorAddrInfo_Response PerCoordinatorAddrInfo_Response;
                        ;
                        ;  // Structure for CMD_COORDINATOR_BOND_NODE
                        ;  TPerCoordinatorBondNode_Request PerCoordinatorBondNode_Request;
                        ;
                        ;  // Structure returned by CMD_COORDINATOR_BOND_NODE or CMD_COORDINATOR_SMART_CONNECT
                        ;  TPerCoordinatorBondNodeSmartConnect_Response PerCoordinatorBondNodeSmartConnect_Response;
                        ;
                        ;  // Structure for CMD_COORDINATOR_REMOVE_BOND
                        ;  TPerCoordinatorRemoveBond_Request PerCoordinatorRemoveBond_Request;
                        ;
                        ;  // Structure returned by CMD_COORDINATOR_REMOVE_BOND 
                        ;  TPerCoordinatorRemoveBond_Response PerCoordinatorRemoveBond_Response;
                        ;
                        ;  // Structure for CMD_COORDINATOR_DISCOVERY
                        ;  TPerCoordinatorDiscovery_Request PerCoordinatorDiscovery_Request;
                        ;
                        ;  // Structure returned by CMD_COORDINATOR_DISCOVERY
                        ;  TPerCoordinatorDiscovery_Response PerCoordinatorDiscovery_Response;
                        ;
                        ;  // Structure for and also returned by CMD_COORDINATOR_SET_DPAPARAMS
                        ;  TPerCoordinatorSetDpaParams_Request_Response PerCoordinatorSetDpaParams_Request_Response;
                        ;
                        ;  // Structure for and also returned by CMD_COORDINATOR_SET_HOPS
                        ;  TPerCoordinatorSetHops_Request_Response PerCoordinatorSetHops_Request_Response;
                        ;
                        ;  // Structure for CMD_COORDINATOR_BACKUP and CMD_NODE_BACKUP
                        ;  TPerCoordinatorNodeBackup_Request PerCoordinatorNodeBackup_Request;
                        ;
                        ;  // Structure returned by CMD_COORDINATOR_BACKUP and CMD_NODE_BACKUP
                        ;  TPerCoordinatorNodeBackup_Response PerCoordinatorNodeBackup_Response;
                        ;
                        ;  // Structure for CMD_COORDINATOR_RESTORE and CMD_NODE_RESTORE
                        ;  TPerCoordinatorNodeRestore_Request PerCoordinatorNodeRestore_Request;
                        ;
                        ;  // Structure for CMD_COORDINATOR_AUTHORIZE_BOND
                        ;  TPerCoordinatorAuthorizeBond_Request PerCoordinatorAuthorizeBond_Request;
                        ;
                        ;  // Structure returned by CMD_COORDINATOR_AUTHORIZE_BOND
                        ;  TPerCoordinatorAuthorizeBond_Response PerCoordinatorAuthorizeBond_Response;
                        ;
                        ;  // Structure for CMD_COORDINATOR_BRIDGE
                        ;  TPerCoordinatorBridge_Request PerCoordinatorBridge_Request;
                        ;
                        ;  // Structure returned by CMD_COORDINATOR_BRIDGE
                        ;  TPerCoordinatorBridge_Response PerCoordinatorBridge_Response;
                        ;
                        ;  // Structure for CMD_COORDINATOR_SMART_CONNECT
                        ;  TPerCoordinatorSmartConnect_Request PerCoordinatorSmartConnect_Request;
                        ;
                        ;  // Structure for CMD_COORDINATOR_SET_MID
                        ;  TPerCoordinatorSetMID_Request PerCoordinatorSetMID_Request;
                        ;
                        ;  // Structure returned by CMD_NODE_READ
                        ;  TPerNodeRead_Response PerNodeRead_Response;
                        ;
                        ;  // Structure for CMD_NODE_VALIDATE_BONDS
                        ;  TPerNodeValidateBonds_Request PerNodeValidateBonds_Request;
                        ;
                        ;  // Structure returned by CMD_OS_READ
                        ;  TPerOSRead_Response PerOSRead_Response;
                        ;
                        ;  // Structure returned by CMD_OS_READ_CFG
                        ;  TPerOSReadCfg_Response PerOSReadCfg_Response;
                        ;
                        ;  // Structure for CMD_OS_WRITE_CFG
                        ;  TPerOSWriteCfg_Request PerOSWriteCfg_Request;
                        ;
                        ;  // Structure for CMD_OS_WRITE_CFG_BYTE
                        ;  TPerOSWriteCfgByte_Request PerOSWriteCfgByte_Request;
                        ;
                        ;  // Structure for CMD_OS_SET_SECURITY
                        ;  TPerOSSetSecurity_Request PerOSSetSecurity_Request;
                        ;
                        ;  // Structure for CMD_OS_LOAD_CODE
                        ;  TPerOSLoadCode_Request PerOSLoadCode_Request;
                        ;
                        ;  // Structure for CMD_OS_SLEEP
                        ;  TPerOSSleep_Request PerOSSleep_Request;
                        ;
                        ;  // Structure for CMD_OS_SELECTIVE_BATCH
                        ;  TPerOSSelectiveBatch_Request PerOSSelectiveBatch_Request;
                        ;
                        ;  // Structure for CMD_OS_TEST_RF_SIGNAL request
                        ;  TPerOSTestRfSignal_Request PerOSTestRfSignal_Request;
                        ;
                        ;  // Structure for CMD_OS_INDICATE request
                        ;  TPerOSIndicate_Request PerOSIndicate_Request;
                        ;
                        ;  // Structure for CMD_OS_TEST_RF_SIGNAL response
                        ;  TPerOSTestRfSignal_Response PerOSTestRfSignal_Response;
                        ;
                        ;  // Structure for general memory request
                        ;  TPerMemoryRequest MemoryRequest;
                        ;
                        ;  // Structure for general extended memory request
                        ;  TPerXMemoryRequest XMemoryRequest;
                        ;
                        ;  // Structure for CMD_IO requests
                        ;  TPerIoDirectionAndSet_Request PerIoDirectionAndSet_Request;
                        ;
                        ;  // Structure returned by CMD_THERMOMETER_READ
                        ;  TPerThermometerRead_Response PerThermometerRead_Response;
                        ;
                        ;  // Structure for CMD_UART_OPEN
                        ;  TPerUartOpen_Request PerUartOpen_Request;
                        ;
                        ;  // Structure for CMD_UART_[CLEAR_]WRITE_READ and CMD_SPI_WRITE_READ
                        ;  TPerUartSpiWriteRead_Request PerUartSpiWriteRead_Request;
                        ;
                        ;  // Structure for CMD_FRC_SEND
                        ;  TPerFrcSend_Request PerFrcSend_Request;
                        ;
                        ;  // Structure returned by CMD_FRC_SEND and CMD_FRC_SEND_SELECTIVE
                        ;  TPerFrcSend_Response PerFrcSend_Response;
                        ;
                        ;  // Structure for CMD_FRC_SEND_SELECTIVE
                        ;  TPerFrcSendSelective_Request PerFrcSendSelective_Request;
                        ;
                        ;  // Structure for request and response of CMD_FRC_SET_PARAMS
                        ;  TPerFrcSetParams_RequestResponse PerFrcSetParams_RequestResponse;
                        ;
                        ;  // Interface and CMD_COORDINATOR_BRIDGE confirmation structure
                        ;  TIFaceConfirmation IFaceConfirmation;
                        ;} TDpaMessage;
                        ;
                        ;// Custom DPA Handler events
                        ;#define	DpaEvent_DpaRequest				  0
                        ;#define	DpaEvent_Interrupt				  1
                        ;#define	DpaEvent_Idle					  2
                        ;#define	DpaEvent_Init					  3
                        ;#define	DpaEvent_Notification			  4
                        ;#define	DpaEvent_AfterRouting			  5
                        ;#define	DpaEvent_BeforeSleep			  6
                        ;#define	DpaEvent_AfterSleep				  7
                        ;#define	DpaEvent_Reset					  8
                        ;#define	DpaEvent_DisableInterrupts		  9
                        ;#define	DpaEvent_FrcValue				  10
                        ;#define	DpaEvent_ReceiveDpaResponse		  11
                        ;#define	DpaEvent_IFaceReceive			  12
                        ;#define	DpaEvent_ReceiveDpaRequest		  13
                        ;#define	DpaEvent_BeforeSendingDpaResponse 14
                        ;#define	DpaEvent_PeerToPeer				  15
                        ;#define	DpaEvent_UserDpaValue			  17
                        ;#define	DpaEvent_FrcResponseTime		  18
                        ;#define	DpaEvent_BondingButton			  19
                        ;#define	DpaEvent_Indicate    			  20
                        ;
                        ;#define	DpaEvent_LAST					  DpaEvent_Indicate
                        ;
                        ;// Types of the diagnostic DPA Value that is returned inside DPA response 
                        ;typedef enum
                        ;{
                        ;  DpaValueType_RSSI = 0,
                        ;  DpaValueType_SupplyVoltage = 1,
                        ;  DpaValueType_System = 2,
                        ;  DpaValueType_User = 3
                        ;} TDpaValueType;
                        ;
                        ;// Type (color) of LED peripheral
                        ;typedef enum
                        ;{
                        ;  LED_COLOR_RED = 0,
                        ;  LED_COLOR_GREEN = 1,
                        ;  LED_COLOR_BLUE = 2,
                        ;  LED_COLOR_YELLOW = 3,
                        ;  LED_COLOR_WHITE = 4,
                        ;  LED_COLOR_UNKNOWN = 0xff
                        ;} TLedColor;
                        ;
                        ;// Baud rates
                        ;typedef enum
                        ;{
                        ;  DpaBaud_1200 = 0x00,
                        ;  DpaBaud_2400 = 0x01,
                        ;  DpaBaud_4800 = 0x02,
                        ;  DpaBaud_9600 = 0x03,
                        ;  DpaBaud_19200 = 0x04,
                        ;  DpaBaud_38400 = 0x05,
                        ;  DpaBaud_57600 = 0x06,
                        ;  DpaBaud_115200 = 0x07,
                        ;  DpaBaud_230400 = 0x08
                        ;} TBaudRates;
                        ;
                        ;// Useful PNUM_IO definitions
                        ;typedef enum
                        ;{
                        ;  PNUM_IO_PORTA = 0x00,
                        ;  PNUM_IO_TRISA = 0x00,
                        ;  PNUM_IO_PORTB = 0x01,
                        ;  PNUM_IO_TRISB = 0x01,
                        ;  PNUM_IO_PORTC = 0x02,
                        ;  PNUM_IO_TRISC = 0x02,
                        ;  PNUM_IO_PORTE = 0x04,
                        ;  PNUM_IO_TRISE = 0x04,
                        ;  PNUM_IO_WPUB = 0x11,
                        ;  PNUM_IO_DELAY = 0xff
                        ;} PNUM_IO_Definitions;
                        ;
                        ;// To test for enumeration peripherals request
                        ;#define IsDpaEnumPeripheralsRequestNoSize() ( _PNUM == PNUM_ENUMERATION && _PCMD == CMD_GET_PER_INFO )
                        ;
                        ;#if PARAM_CHECK_LEVEL >= 2
                        ;#define IsDpaEnumPeripheralsRequest() ( IsDpaEnumPeripheralsRequestNoSize() && _DpaDataLength == 0 )
                        ;#else
                        ;#define IsDpaEnumPeripheralsRequest() IsDpaEnumPeripheralsRequestNoSize()
                        ;#endif
                        ;
                        ;// To test for peripherals information request
                        ;#define IsDpaPeripheralInfoRequestNoSize()  ( _PNUM != PNUM_ENUMERATION && _PCMD == CMD_GET_PER_INFO )
                        ;
                        ;#if PARAM_CHECK_LEVEL >= 2
                        ;#define IsDpaPeripheralInfoRequest()  ( IsDpaPeripheralInfoRequestNoSize() && _DpaDataLength == 0 )
                        ;#else
                        ;#define IsDpaPeripheralInfoRequest()  IsDpaPeripheralInfoRequestNoSize()
                        ;#endif
                        ;
                        ;// Optimized macro for both testing enumeration peripherals ELSE peripherals information. See examples
                        ;#define	IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequestNoSize() if ( _PCMD == CMD_GET_PER_INFO ) if ( _PNUM == PNUM_ENUMERATION )
                        ;
                        ;#if PARAM_CHECK_LEVEL >= 2
                        ;#define IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest() if ( _DpaDataLength == 0 && _PCMD == CMD_GET_PER_INFO ) if ( _PNUM == PNUM_ENUMERATION )
                        ;#else
                        ;#define	IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest() IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequestNoSize()
                        ;#endif
                        ;
                        ;#ifdef __CC5X__
                        ;
                        ;// DPA message at bufferRF
                        ;TDpaMessage DpaRfMessage @bufferRF;
                        ;
                        ;// Actual allocation of the RAM Peripheral memory block @ UserBank_02
                        ;bank12 uns8  PeripheralRam[PERIPHERAL_RAM_LENGTH] @ 0x620;
                        ;
                        ;// Actual DPA message parameters at memory
                        ;#define	_NADR			RX
                        ;#define _NADRhigh		RTAUX
                        ;#define _PNUM			PNUM
                        ;#define _PCMD			PCMD
                        ;#define _DpaDataLength	DLEN
                        ;#define _DpaMessage		DpaRfMessage
                        ;
                        ;// Return actual DPA user routine event
                        ;#define	GetDpaEvent()	userReg0
                        ;
                        ;// Stores DPA Params inside DPA request/response
                        ;#define	_DpaParams		PPAR
                        ;// Get DPA Value type out of the DPA Params
                        ;#define	DpaValueType()	( _DpaParams & 0b11 )
                        ;
                        ;// When TRUE then encryptBufferRF/decryptBufferRF is done by AccessPassord
                        ;bit encryptByAccessPassword @ usedBank0[0x23].7;
                        ;
                        ;// DP2P response time-slot time in 10 ms
                        ;#define DP2P_TIMESLOT   11
                        ;
                        ;// DP2P request packet. Fills out the whole bufferRF.
                        ;typedef struct
                        ;{
                        ;  uns8  Header[3];  // 0x000000
                        ;  uns8  SelectedNodes[30];
                        ;  uns8  SlotLength;
                        ;  uns8  ResponseTxPower;
                        ;  uns8  Reserved;
                        ;  uns16	HWPID;
                        ;  uns8  PDATA[sizeofBufferRF - ( 3 + 30 + 1 + 1 + 1 ) * sizeof( uns8 ) - ( 1 ) * sizeof( uns16 )]; // size is 26 bytes
                        ;} STRUCTATTR TDP2Prequest;
                        ;
                        ;// DP2P invite packet.
                        ;typedef struct
                        ;{
                        ;  uns8  Header[3];  // 0x000001
                        ;  uns8  NADR;
                        ;  uns8  Rand[12];
                        ;} STRUCTATTR TDP2Invite;
                        ;
                        ;// DP2P confirm packet.
                        ;typedef struct
                        ;{
                        ;  uns8  Header[3];  // 0x000003
                        ;  uns8  NADR;
                        ;  uns8  Rand[12];
                        ;} STRUCTATTR TDP2Confirm;
                        ;
                        ;// DP2P response packet.
                        ;typedef struct
                        ;{
                        ;  uns8  Header[3];  // 0xFfFfFf
                        ;  uns8  NADR;
                        ;  uns8  PDATA[DPA_MAX_DATA_LENGTH];
                        ;} STRUCTATTR TDP2Presponse;
                        ;
                        ;// Include assembler definitions
                        ;#include "HexCodes.h"
                        ;
                        ;// Next code must start at the IQRF APPLICATION routine entry point
                        ;#pragma origin __APPLICATION_ADDRESS
        ORG 0x3A00

  ; FILE ../../../../Development/include/DPA/DPAcustomHandler.h
                        ;// *********************************************************************
                        ;//   Main Custom DPA Handler header                                    *
                        ;// *********************************************************************
                        ;// Copyright (c) IQRF Tech s.r.o.
                        ;//
                        ;// File:    $RCSfile: DPAcustomHandler.h,v $
                        ;// Version: $Revision: 1.113 $
                        ;// Date:    $Date: 2020/02/27 13:17:18 $
                        ;//
                        ;// Revision history:
                        ;//   2020/02/27  Release for DPA 4.13
                        ;//   2020/01/09  Release for DPA 4.12
                        ;//   2019/12/11  Release for DPA 4.11
                        ;//   2019/10/09  Release for DPA 4.10
                        ;//   2019/06/12  Release for DPA 4.03
                        ;//   2019/06/03  Release for DPA 4.02
                        ;//   2019/03/07  Release for DPA 4.01
                        ;//   2019/01/10  Release for DPA 4.00
                        ;//   2018/10/25  Release for DPA 3.03
                        ;//   2017/11/16  Release for DPA 3.02
                        ;//   2017/08/14  Release for DPA 3.01
                        ;//   2017/03/13  Release for DPA 3.00
                        ;//   2016/09/12  Release for DPA 2.28
                        ;//   2016/04/14  Release for DPA 2.27
                        ;//   2016/03/03  Release for DPA 2.26
                        ;//   2016/01/21  Release for DPA 2.25
                        ;//   2015/12/01  Release for DPA 2.24
                        ;//   2015/10/23  Release for DPA 2.23
                        ;//   2015/09/25  Release for DPA 2.22
                        ;//   2015/09/03  Release for DPA 2.21
                        ;//   2015/08/05  Release for DPA 2.20
                        ;//   2014/10/31  Release for DPA 2.10
                        ;//   2014/04/30  Release for DPA 2.00
                        ;//   2013/10/03  Release for DPA 1.00
                        ;//
                        ;// *********************************************************************
                        ;
                        ;// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/
                        ;// IQRF Standards documentation https://www.iqrfalliance.org/iqrf-interoperability/
                        ;
                        ;#ifndef _CUSTOM_DPA_HANDLER_
                        ;#define _CUSTOM_DPA_HANDLER_
                        ;
                        ;//############################################################################################
                        ;// 1st include 
                        ;
                        ;// Custom DPA Handler routine declaration
                        ;bit CustomDpaHandler();
                        ;
                        ;// Various DPA flags shared between DPA and Custom DPA Handler
                        ;uns8 DpaFlags @ usedBank4[0];
                        ;#ifdef COORDINATOR_CUSTOM_HANDLER
                        ;// [C] TRUE if interface master is not connected (detected)
                        ;bit IFaceMasterNotConnected @ DpaFlags.2;
                        ;#endif
                        ;// [N] [CN] DPA by interface notification is sent also when there was a "reading" like DPA request
                        ;bit EnableIFaceNotificationOnRead @ DpaFlags.3;
                        ;// [N] [NC] TRUE when node was just bonded using default bonding procedure
                        ;bit NodeWasBonded @ DpaFlags.4;
                        ;
                        ;// [C] Ticks (decrementing) counter usable for timing in the coordinator's Customer DPA Handler
                        ;uns16 DpaTicks @ usedBank4[1];
                        ;// [N] [NC] toutRF for LP mode, read from configuration memory after reset
                        ;uns8 LPtoutRF @ usedBank4[3];
                        ;// DPA Request/Response HWPID
                        ;uns16 _HWPID @ usedBank4[4];
                        ;// Identifies type of reset (stored at UserReg0 upon module reset). See Reset chapter at IQRF User's Guide for more information
                        ;uns8 ResetType @ usedBank4[6];
                        ;// User DPA Values to return
                        ;uns8 UserDpaValue @ usedBank4[7];
                        ;// Network depth of the DPA request/response, increases on bridging, decreases on back-bridging
                        ;uns8 NetDepth @ usedBank4[8];
                        ;// TRUE when node was at DPA Service Mode after last boot
                        ;bit DSMactivated @ usedBank4[9].0;
                        ;// If set to TRUE, then LP RX mode in the main loop can be terminated by pin, see _RLPMAT
                        ;bit LpRxPinTerminate @ usedBank4[9].1;
                        ;// If set to TRUE, then [C] executes asynchronous DPA requests received from [N]
                        ;bit AsyncReqAtCoordinator @ usedBank4[9].2;
                        ;// RX filter used at the DPA main loop checkRF call
                        ;uns8 RxFilter @ usedBank4[11];
                        ;// Countdown variable for button bonding before going to deep sleep 
                        ;uns16 BondingSleepCountdown @ usedBank4[12];
                        ;#define BONDING_SLEEP_COUNTDOWN_UNIT  290
                        ;// Non-zero pseudo-random value, read-only, updated on every Reset and Idle event, at [N] only.
                        ;uns16 Random @ usedBank4[14];
                        ;
                        ;// Macro to return an error from the peripheral handler. If the code size is not an issue this macro is the right choice.
                        ;#define DpaApiReturnPeripheralError(error) do { \
                        ;	DpaApiSetPeripheralError( error ); \
                        ;	return Carry; \
                        ;  } while( 0 )
                        ;
                        ;// DPA API functions, see documentation for details
                        ;#define	DpaApiRfTxDpaPacket( dpaValue, netDepthAndFlags ) DpaApiEntry( dpaValue, netDepthAndFlags, DPA_API_RFTX_DPAPACKET )
                        ;#define	DpaApiReadConfigByte( index )					  DpaApiEntry( index, param3.low8, DPA_API_READ_CONFIG_BYTE )
                        ;#define	DpaApiLocalRequest()							  DpaApiEntry( param2, param3.low8, DPA_API_LOCAL_REQUEST )
                        ;#define	DpaApiSetPeripheralError( error )				  DpaApiEntry( error, param3.low8, DPA_API_SET_PERIPHERAL_ERROR )
                        ;#define	DpaApiSendToIFaceMaster( dpaValue, flags )		  DpaApiEntry( dpaValue, flags, DPA_API_SEND_TO_IFACEMASTER )
                        ;#define	DpaApiSetRfDefaults()							  DpaApiEntry( param2, param3.low8, DPA_API_SET_RF_DEFAULTS )
                        ;
                        ;#ifdef COORDINATOR_CUSTOM_HANDLER
                        ;#define	DpaApiRfTxDpaPacketCoordinator()				  DpaApiEntry( param2, param3.low8, DPA_API_COORDINATOR_RFTX_DPAPACKET )
                        ;#endif
                        ;
                        ;#ifdef COORDINATOR_CUSTOM_HANDLER
                        ;#undef	DpaEvent_Interrupt
                        ;#undef	DpaEvent_BeforeSleep
                        ;#undef	DpaEvent_AfterSleep
                        ;#undef	DpaEvent_FrcValue
                        ;#undef	DpaEvent_FrcResponseTime
                        ;#else
                        ;#undef  DpaEvent_ReceiveDpaResponse
                        ;#undef  DpaEvent_IFaceReceive
                        ;#endif
                        ;
                        ;// To detect overlapping code in case someone would put some code before this header by mistake
                        ;#pragma origin __APPLICATION_ADDRESS
        ORG 0x3A00
                        ;#pragma updateBank 0
                        ;
                        ;//############################################################################################
                        ;// Main IQRF entry point jumps to the main DPA entry point
                        ;void APPLICATION()
                        ;//############################################################################################
                        ;{
APPLICATION
                        ;  #asm
                        ;    DW  __MOVLP( MAIN_DPA_ADDRESS >> 8 );
        DW    0x31AC
                        ;  DW  __GOTO( MAIN_DPA_ADDRESS );
        DW    0x2C04
                        ;  #endasm
                        ;
                        ;#ifndef NO_CUSTOM_DPA_HANDLER
                        ;    // Fake call to force CustomDpaHandler() compilation
                        ;    CustomDpaHandler();
        CALL  CustomDpaHandler
                        ;#endif
                        ;
                        ;  // Fake call to force DpaApiEntry() compilation
                        ;  DpaApiEntry( param2, param3.low8, W );
        BRA   DpaApiEntry
                        ;}
                        ;
                        ;//############################################################################################
                        ;// Entry stub to the real DPA API entry
                        ;#pragma origin DPA_API_ADDRESS_ENTRY
        ORG 0x3A08
                        ;uns8  DpaApiEntry( uns8 par1 @ param2, uns8 par2 @ param3.low8, uns8 apiIndex @ W )
                        ;//############################################################################################
                        ;{
DpaApiEntry
                        ;  #asm
                        ;    DW  __MOVLP( DPA_API_ADDRESS >> 8 );
        DW    0x31AC
                        ;  DW  __CALL( DPA_API_ADDRESS );
        DW    0x2400
                        ;  DW  __MOVLP( DPA_API_ADDRESS_ENTRY >> 8 );
        DW    0x31BA
                        ;  #endasm
                        ;
                        ;    return W;
        RETURN
                        ;}
                        ;
                        ;//############################################################################################
                        ;#pragma origin DPA_API_ADDRESS_ENTRY + 0x08
        ORG 0x3A10
                        ;
                        ;//############################################################################################
                        ;
                        ;#ifndef NO_CUSTOM_DPA_HANDLER
                        ;// Next comes Custom DPA handler routine
                        ;#pragma origin CUSTOM_HANDLER_ADDRESS
        ORG 0x3A20
                        ;#endif
                        ;
                        ;#pragma updateBank 1

  ; FILE D:\DP\IQRF_Startup_Package_OS403D_TR-7xD_200302\IQRF_OS403_7xD\Examples\DPA\CustomDpaHandlerExamples\IQRF_Standard\4C02_DDC-SE_LP.c
                        ;// ***********************************************************************************************************
                        ;//   Custom DPA Handler code example - Standard Sensors - DDC-SE-01 - LP version *
                        ;// ***********************************************************************************************************
                        ;// Copyright (c) IQRF Tech s.r.o.
                        ;//
                        ;// File:    $RCSfile: 4C02_DDC-SE_LP.c,v $
                        ;// Version: $Revision: 1.9 $
                        ;// Date:    $Date: 2020/02/20 17:18:59 $
                        ;//
                        ;// Revision history:
                        ;//   2020/01/02  Release for DPA 4.11
                        ;//   2019/03/07  Release for DPA 4.01
                        ;//
                        ;// *********************************************************************
                        ;
                        ;// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/
                        ;// IQRF Standards documentation https://www.iqrfalliance.org/iqrf-interoperability/
                        ;
                        ;// This example implements 3 sensors according to the IQRF Sensors standard
                        ;// Index 0 i.e. 1st sensor is either Dallas 18B20 or MCP9802 temperature sensor at DDC-SE-01 board according to the HW jumper position and symbol DALLASnotMCP.
                        ;// Index 1 i.e. 2nd sensor is light intensity indicator at DDC-SE-01 board (value range is 0[max light]-127[max dark]).
                        ;// Index 2 i.e. 3rd sensor is potentiometer value at DDC-SE-01 board (value range is 0[left stop]-127[right stop]).
                        ;
                        ;// This example must be compiled without a "-bu" compiler switch in order to fit into available Flash memory
                        ;
                        ;// Default IQRF include (modify the path according to your setup)
                        ;#include "IQRF.h"
                        ;
                        ;// We can save more instructions if needed by the symbol below
                        ;// #define	PARAM_CHECK_LEVEL 1
                        ;
                        ;// Default DPA header (modify the path according to your setup)
                        ;#include "DPA.h"
                        ;// Default Custom DPA Handler header (modify the path according to your setup)
                        ;#include "DPAcustomHandler.h"
                        ;// IQRF standards header (modify the path according to your setup)
                        ;#include "IQRFstandard.h"
                        ;#include "IQRF_HWPID.h"
                        ;// I2C Master library
                        ;#include "lib/I2Cmaster.c"
                        ;
                        ;// If defined then the handler is compiled for Dallas otherwise for MCP9802
                        ;#define	DALLASnotMCP
                        ;
                        ;//############################################################################################
                        ;
                        ;// Define useful macro that saves some code but not preset at DPA < 3.01
                        ;#if DPA_VERSION_MASTER	< 0x0301
                        ;// Optimized macro for both testing enumeration peripherals ELSE peripherals information. See examples
                        ;#define	IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequestNoSize() if ( _PCMD == CMD_GET_PER_INFO ) if ( _PNUM == PNUM_ENUMERATION )
                        ;
                        ;#if PARAM_CHECK_LEVEL >= 2
                        ;#define IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest() if ( _DpaDataLength == 0 && _PCMD == CMD_GET_PER_INFO ) if ( _PNUM == PNUM_ENUMERATION )
                        ;#else
                        ;#define	IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest() IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequestNoSize()
                        ;#endif
                        ;#endif
                        ;
                        ;//############################################################################################
                        ;
                        ;// Number of implemented sensors
                        ;#define	SENSORS_COUNT 3
                        ;
                        ;// Variable to store sensor value at Get?_????() methods. This example implements sensors returning maximum 2 bytes of data.
                        ;uns16 sensorValue @ param3;
                        ;
                        ;// Reads sensor value to the sensorValue variable and to responseFRCvalue(2B) variable
                        ;bit Get0_Temperature();
                        ;bit Get1_BinaryData_Light();
                        ;bit Get2_BinaryData_Potentiometer();
                        ;
                        ;// Stores sensor value byte(s) to the FSR1[+1...], in case of PCMD_STD_SENSORS_READ_TYPES_AND_VALUES sensor type is stored before value byte(s)
                        ;void StoreValue( uns8 sensorType );
                        ;
                        ;#ifdef DALLASnotMCP
                        ;// Sensor connected to PORT C.3 (compatible with DDC-SE-01)
                        ;#define OneWire_TRIS         TRISC.3
                        ;#define OneWire_IO_IN        PORTC.3
                        ;#define OneWire_IO_OUT       LATC.3
                        ;
                        ;// ms per ticks
                        ;#define	TICKS_LEN  10
                        ;
                        ;// Writes sensor configuration (resolution)
                        ;bit Ds18B20WriteConfig( uns8 value );
                        ;
                        ;// Resets OneWire
                        ;bit OneWireReset();
                        ;// Reads OneWire byte
                        ;uns8 OneWireReadByte();
                        ;// Writes OneWire byte
                        ;void OneWireWriteByte( uns8 byte );
                        ;
                        ;// DS18B20 commands
                        ;#define CMD_READROM		  0x33
                        ;#define CMD_CONVERTTEMP	  0x44
                        ;#define CMD_CPYSCRATCHPAD 0x48
                        ;#define CMD_WSCRATCHPAD	  0x4e
                        ;#define CMD_MATCHROM	  0x55
                        ;#define CMD_RPWRSUPPLY	  0xb4
                        ;#define CMD_RECEEPROM	  0xb8
                        ;#define CMD_RSCRATCHPAD	  0xbe
                        ;#define CMD_SKIPROM		  0xcc
                        ;#define CMD_ALARMSEARCH	  0xec
                        ;#define CMD_SEARCHROM	  0xf0
                        ;
                        ;// Final DS18B20 temperature value read by state machine
                        ;uns16 temperature;
                        ;
                        ;#else // DALLASnotMCP
                        ;
                        ;// I2C SCL frequency [Hz]
                        ;#define I2Cfrequency      50000
                        ;
                        ;// Own implementation with timeout
                        ;#define i2c_waitForIdle_REDEFINE
                        ;
                        ;// TRUE if I2C timeout occurred
                        ;bit i2cTimeout;
                        ;
                        ;// MCP9802 address
                        ;#define I2C_ADR         	0b10010110
                        ;// Power pin
                        ;#define PWR_SENSOR_TRIS 	TRISC.7
                        ;#define PWR_SENSOR_IO   	LATC.7
                        ;
                        ;#endif
                        ;
                        ;// Must be the 1st defined function in the source code in order to be placed at the correct FLASH location!
                        ;//############################################################################################
                        ;bit CustomDpaHandler()
                        ;//############################################################################################
                        ;{
CustomDpaHandler
                        ;  // This forces CC5X to wisely use MOVLB instructions (doc says:  The 'default' bank is used by the compiler for loops and labels when the algorithm gives up finding the optimal choice)
                        ;#pragma updateBank default = UserBank_01
                        ;
                        ;#ifdef DALLASnotMCP
                        ;  // Finite machine states
                        ;  typedef enum
                        ;  {
                        ;    S_ResetConvertT = 0,
                        ;    S_SkipRomConvertT,
                        ;    S_CmdConvertT,
                        ;
                        ;    S_WaitConvertT,
                        ;
                        ;    S_ResetReadTemp,
                        ;    S_SkipRomReadTemp,
                        ;    S_CmdReadTemp,
                        ;    S_Byte1ReadTemp,
                        ;    S_Byte2ReadTemp
                        ;  } TState;
                        ;#endif
                        ;
                        ;  // Handler presence mark
                        ;  clrwdt();
        CLRWDT
                        ;
                        ;  // Sleeping parameters, valid when Time != 0
                        ;  static TPerOSSleep_Request PerOSSleep_Request;
                        ;
                        ;#ifdef DALLASnotMCP
                        ;  // Finite machine state
                        ;  static uns8 state; // = S_ResetConvertT = 0
                        ;  // Pre-read lower temperature byte
                        ;  static uns8 temperatureByteLow;
                        ;  // Conversion timeout counter
                        ;  static uns16 timeoutStart;
                        ;#endif
                        ;
                        ;  // Detect DPA event to handle
                        ;  switch ( GetDpaEvent() )
        MOVF  userReg0,W
        XORLW 0x01
        BTFSC 0x03,Zero_
        BRA   m001
        XORLW 0x03
        BTFSC 0x03,Zero_
        BRA   m002
        XORLW 0x01
        BTFSC 0x03,Zero_
        BRA   m014
        XORLW 0x04
        BTFSC 0x03,Zero_
        GOTO  m043
        XORLW 0x01
        BTFSC 0x03,Zero_
        GOTO  m043
        XORLW 0x06
        BTFSC 0x03,Zero_
        BRA   m016
        XORLW 0x0A
        BTFSC 0x03,Zero_
        BRA   m031
        XORLW 0x18
        BTFSC 0x03,Zero_
        GOTO  m041
        GOTO  m043
                        ;  {
                        ;    // -------------------------------------------------
                        ;    case DpaEvent_Interrupt:
                        ;      // Do an extra quick background interrupt work
                        ;
                        ;      return Carry;
m001    RETURN
                        ;
                        ;      // -------------------------------------------------
                        ;    case DpaEvent_Idle:
                        ;      // Do a quick background work when RF packet is not received
                        ;
                        ;      // Should go to sleep?
                        ;      if ( PerOSSleep_Request.Time != 0 )
m002    MOVLB 0x0B
        MOVF  PerOSSleep_Request,W
        IORWF PerOSSleep_Request+1,W
        BTFSC 0x03,Zero_
        BRA   m003
                        ;      {
                        ;        // Copy sleep parameters to the DPA request
                        ;        _DpaMessage.PerOSSleep_Request.Time = PerOSSleep_Request.Time;
        MOVF  PerOSSleep_Request,W
        MOVLB 0x09
        MOVWF DpaRfMessage
        MOVLB 0x0B
        MOVF  PerOSSleep_Request+1,W
        MOVLB 0x09
        MOVWF DpaRfMessage+1
                        ;        _DpaMessage.PerOSSleep_Request.Control = PerOSSleep_Request.Control;
        MOVLB 0x0B
        MOVF  PerOSSleep_Request+2,W
        MOVLB 0x09
        MOVWF DpaRfMessage+2
                        ;        // Finalize OS Sleep DPA Request
                        ;        _DpaDataLength = sizeof( _DpaMessage.PerOSSleep_Request );
        MOVLW 0x03
        MOVLB 0x05
        MOVWF DLEN
                        ;        _PNUM = PNUM_OS;
        MOVLW 0x02
        MOVWF PNUM
                        ;        _PCMD = CMD_OS_SLEEP;
        MOVLW 0x04
        MOVWF PCMD
                        ;        // Perform local DPA Request to go to sleep
                        ;        DpaApiLocalRequest();
        MOVLW 0x04
        CALL  DpaApiEntry
                        ;        // Switch off sleeping time=flag
                        ;        PerOSSleep_Request.Time = 0;
        MOVLB 0x0B
        CLRF  PerOSSleep_Request
        CLRF  PerOSSleep_Request+1
                        ;      }
                        ;
                        ;#ifdef DALLASnotMCP
                        ;      // Run finite state machine to read temperature from DS18B20 at background so the temperature value is immediately ready for FRC 
                        ;
                        ;      // Make sure 1Wire data pin at LATX.y is low as it might be set by another PORTX.? pin manipulation
                        ;      OneWire_IO_OUT = 0;
m003    MOVLB 0x02
        BCF   LATC,3
                        ;
                        ;      skip( state );
        MOVLB 0x0B
        MOVF  state,W
        BRW  
                        ;#pragma computedGoto 1
                        ;      goto _S_ResetConvertT;
        BRA   m006
                        ;      goto _S_SkipRomConvertT;
        BRA   m008
                        ;      goto _S_CmdConvertT;
        BRA   m009
                        ;      goto _S_WaitConvertT;
        BRA   m010
                        ;      goto _S_ResetReadTemp;
        BRA   m006
                        ;      goto _S_SkipRomReadTemp;
        BRA   m008
                        ;      goto _S_CmdReadTemp;
        BRA   m011
                        ;      goto _S_Byte1ReadTemp;
        BRA   m012
                        ;      goto _S_Byte2ReadTemp;
                        ;#pragma computedGoto 0
                        ;      ;
                        ;      // --------------
                        ;_S_Byte2ReadTemp:
                        ;      temperature.high8 = OneWireReadByte();
m004    CALL  OneWireReadByte
        MOVLB 0x0B
        MOVWF temperature+1
                        ;      temperature.low8 = temperatureByteLow;
        MOVF  temperatureByteLow,W
        MOVWF temperature
                        ;
                        ;ResetMachine:
                        ;      state = S_ResetConvertT;
m005    MOVLB 0x0B
        CLRF  state
                        ;      goto ExitMachine;
        GOTO  m043
                        ;
                        ;      // --------------
                        ;_S_ResetConvertT:
                        ;_S_ResetReadTemp:
                        ;      if ( !OneWireReset() )
m006    CALL  OneWireReset
        BTFSC 0x03,Carry
        BRA   m013
                        ;      {
                        ;_S_Error_Reset:
                        ;        STD_SENSOR_TYPE_TEMPERATURE_SET_ERROR( temperature );
m007    MOVLB 0x0B
        CLRF  temperature
        MOVLW 0x80
        MOVWF temperature+1
                        ;        goto ResetMachine;
        BRA   m005
                        ;      }
                        ;      goto NextState;
                        ;
                        ;      // --------------
                        ;_S_SkipRomConvertT:
                        ;_S_SkipRomReadTemp:
                        ;      // OneWire: Skip ROM
                        ;      OneWireWriteByte( CMD_SKIPROM );
m008    MOVLW 0xCC
        CALL  OneWireWriteByte
                        ;      goto NextState;
        BRA   m013
                        ;
                        ;      // --------------
                        ;_S_CmdConvertT:
                        ;      // OneWire: Convert temperature
                        ;      OneWireWriteByte( CMD_CONVERTTEMP );
m009    MOVLW 0x44
        CALL  OneWireWriteByte
                        ;      // Start timeout for approx 750 ms (the longest conversion time)
                        ;      captureTicks();
        CALL  captureTicks
                        ;      // Remember start time
                        ;      timeoutStart = param3;
        MOVF  param3,W
        MOVWF timeoutStart
        MOVF  param3+1,W
        MOVWF timeoutStart+1
                        ;      goto NextState;
        BRA   m013
                        ;
                        ;      // --------------
                        ;_S_WaitConvertT:
                        ;      // Measured?
                        ;      if ( OneWireReadByte() == 0xff )
m010    CALL  OneWireReadByte
        XORLW 0xFF
        BTFSC 0x03,Zero_
                        ;        goto NextState;
        BRA   m013
                        ;
                        ;      // Timeout?
                        ;      captureTicks();
        CALL  captureTicks
                        ;      param3 -= timeoutStart;
        MOVF  timeoutStart+1,W
        SUBWF param3+1,1
        MOVF  timeoutStart,W
        SUBWF param3,1
        BTFSS 0x03,Carry
        DECF  param3+1,1
                        ;      // Yes!
                        ;      if ( param3 > ( 2 + 750 / TICKS_LEN ) )
        MOVF  param3+1,W
        BTFSS 0x03,Zero_
        BRA   m007
        MOVLW 0x4E
        SUBWF param3,W
        BTFSS 0x03,Carry
        BRA   m043
                        ;        goto _S_Error_Reset;
        BRA   m007
                        ;
                        ;      goto ExitMachine;
                        ;
                        ;      // --------------
                        ;_S_CmdReadTemp:
                        ;      // OneWire: Read scratchpad
                        ;      OneWireWriteByte( CMD_RSCRATCHPAD );
m011    MOVLW 0xBE
        CALL  OneWireWriteByte
                        ;      goto NextState;
        BRA   m013
                        ;
                        ;      // --------------
                        ;_S_Byte1ReadTemp:
                        ;      temperatureByteLow = OneWireReadByte();
m012    CALL  OneWireReadByte
        MOVLB 0x0B
        MOVWF temperatureByteLow
                        ;      goto NextState;
                        ;
                        ;      // --------------
                        ;NextState:
                        ;      ++state;
m013    MOVLB 0x0B
        INCF  state,1
                        ;
                        ;ExitMachine:
                        ;#endif
                        ;      break;
        BRA   m043
                        ;
                        ;      // -------------------------------------------------
                        ;    case DpaEvent_Init:
                        ;      // Do a one time initialization before main loop starts
                        ;
                        ;      // Initialize ticks
                        ;      startCapture();
m014    CALL  startCapture
                        ;
                        ;      // Initialize sensors
                        ;      // C5 (AN4) as input 
                        ;      moduleInfo();
        CALL  moduleInfo
                        ;      // Connected TR pins?
                        ;      if ( !bufferINFO[5].7 )
        MOVLB 0x06
        BTFSC bufferINFO+5,7
        BRA   m015
                        ;      {
                        ;        TRISC.6 = 1;
        MOVLB 0x01
        BSF   TRISC,6
                        ;        TRISB.4 = 1;
        BSF   TRISB,4
                        ;      }
                        ;      TRISA.5 = 1;
m015    MOVLB 0x01
        BSF   TRISA,5
                        ;
                        ;      // C1 (AN0) as input 
                        ;      TRISA.0 = 1;
        BSF   TRISA,0
                        ;
                        ;#ifdef DALLASnotMCP
                        ;      // Setup DS18B20 for 9bit precision, conversion takes 94ms (see datasheet)
                        ;      Ds18B20WriteConfig( 0b0.00.00000 );
        MOVLW 0x00
        CALL  Ds18B20WriteConfig
                        ;#else
                        ;      // Expect MCP9802 is enabled
                        ;      i2c_init();
                        ;#endif
                        ;      break;
        BRA   m043
                        ;
                        ;      // -------------------------------------------------
                        ;    case DpaEvent_AfterSleep:
                        ;      // Called after woken up after sleep
                        ;#ifndef DALLASnotMCP
                        ;      i2c_init();
                        ;#endif
                        ;
                        ;      break;
                        ;
                        ;      // -------------------------------------------------
                        ;    case DpaEvent_BeforeSleep:
                        ;      // Called before going to sleep
                        ;#ifndef DALLASnotMCP
                        ;      i2c_shutdown();
                        ;#endif
                        ;      break;
                        ;
                        ;      // -------------------------------------------------
                        ;    case DpaEvent_DpaRequest:
                        ;      // Called to interpret DPA request for peripherals
                        ;      // -------------------------------------------------
                        ;      // Peripheral enumeration
                        ;      IfDpaEnumPeripherals_Else_PeripheralInfo_Else_PeripheralRequest()
m016    MOVLB 0x05
        MOVF  DLEN,1
        BTFSS 0x03,Zero_
        BRA   m019
        MOVF  PCMD,W
        XORLW 0x3F
        BTFSS 0x03,Zero_
        BRA   m019
        INCFSZ PNUM,W
        BRA   m018
                        ;      {
                        ;        // We implement 2 standard peripherals
                        ;        _DpaMessage.EnumPeripheralsAnswer.UserPerNr = 1;
        MOVLW 0x01
        MOVLB 0x09
        MOVWF DpaRfMessage+2
                        ;        FlagUserPer( _DpaMessage.EnumPeripheralsAnswer.UserPer, PNUM_STD_SENSORS );
        BSF   DpaRfMessage+19,6
                        ;        _DpaMessage.EnumPeripheralsAnswer.HWPID = HWPID_IQRF_TECH__DEMO_DDC_SE01_LP;
        MOVLW 0x02
        MOVWF DpaRfMessage+7
        MOVLW 0x4C
        MOVWF DpaRfMessage+8
                        ;        _DpaMessage.EnumPeripheralsAnswer.HWPIDver |= 0x0000;
                        ;
                        ;DpaHandleReturnTRUE:
                        ;        return TRUE;
m017    BSF   0x03,Carry
        RETURN
                        ;      }
                        ;      // -------------------------------------------------
                        ;      // Get information about peripherals
                        ;      else
                        ;      {
                        ;        switch ( _DpaMessage.PeripheralInfoAnswer.PerT = _PNUM )
m018    MOVLB 0x05
        MOVF  PNUM,W
        MOVLB 0x09
        MOVWF DpaRfMessage+1
        XORLW 0x5E
        BTFSS 0x03,Zero_
        BRA   m043
                        ;        {
                        ;          case PNUM_STD_SENSORS:
                        ;            // Set standard version
                        ;            _DpaMessage.PeripheralInfoAnswer.Par1 = 15;
        MOVLW 0x0F
        MOVLB 0x09
        MOVWF DpaRfMessage+2
                        ;            _DpaMessage.PeripheralInfoAnswer.PerTE = PERIPHERAL_TYPE_EXTENDED_READ_WRITE;
        MOVLW 0x03
        MOVWF DpaRfMessage
                        ;            goto DpaHandleReturnTRUE;
        BRA   m017
                        ;        }
                        ;
                        ;        break;
                        ;      }
                        ;
                        ;      {
                        ;        // -------------------------------------------------
                        ;        // Handle peripheral command
                        ;
                        ;        // Supported peripheral number?
                        ;        switch ( _PNUM )
m019    MOVLB 0x05
        MOVF  PNUM,W
        XORLW 0x5E
        BTFSS 0x03,Zero_
        BRA   m043
                        ;        {
                        ;          case PNUM_STD_SENSORS:
                        ;          {
                        ;            // Supported commands?
                        ;            switch ( _PCMD )
        MOVLB 0x05
        MOVF  PCMD,W
        XORLW 0x3E
        BTFSC 0x03,Zero_
        BRA   m022
        XORLW 0x3E
        BTFSC 0x03,Zero_
        BRA   m025
        XORLW 0x01
        BTFSC 0x03,Zero_
        BRA   m025
                        ;            {
                        ;              // Invalid command
                        ;              default:
                        ;              {
                        ;                // Return error
                        ;_ERROR_PCMD:
                        ;                W = ERROR_PCMD;
m020    MOVLW 0x02
                        ;_ERROR_W:
                        ;                DpaApiReturnPeripheralError( W );
m021    MOVWF par1
        MOVLW 0x05
        BRA   DpaApiEntry
                        ;              }
                        ;
                        ;              // Sensor enumeration
                        ;              case PCMD_STD_ENUMERATE:
                        ;                if ( _DpaDataLength != 0 )
m022    MOVLB 0x05
        MOVF  DLEN,1
        BTFSC 0x03,Zero_
        BRA   m024
                        ;                {
                        ;_ERROR_DATA_LEN:
                        ;                  W = ERROR_DATA_LEN;
m023    MOVLW 0x05
                        ;                  goto _ERROR_W;
        BRA   m021
                        ;                }
                        ;
                        ;                _DpaMessage.Response.PData[0] = STD_SENSOR_TYPE_TEMPERATURE;
m024    MOVLW 0x01
        MOVLB 0x09
        MOVWF DpaRfMessage
                        ;                _DpaMessage.Response.PData[1] = STD_SENSOR_TYPE_BINARYDATA7;
        MOVLW 0x81
        MOVWF DpaRfMessage+1
                        ;                _DpaMessage.Response.PData[2] = STD_SENSOR_TYPE_BINARYDATA7;
        MOVLW 0x81
        MOVWF DpaRfMessage+2
                        ;                W = SENSORS_COUNT;
        MOVLW 0x03
                        ;                goto _W2_DpaDataLength;
        BRA   m030
                        ;
                        ;                // Supported commands. They are handled the same way except one "if" at StoreValue() method
                        ;              case PCMD_STD_SENSORS_READ_VALUES:
                        ;              case PCMD_STD_SENSORS_READ_TYPES_AND_VALUES:
                        ;              {
                        ;                // No sensor bitmap specified? W = _DpaDataLength. Note: W is used to avoid MOVLB at next if
                        ;                W = _DpaDataLength;
m025    MOVLB 0x05
        MOVF  DLEN,W
                        ;                if ( W == 0 )	// Note: must not modify W
        BTFSS 0x03,Zero_
        BRA   m026
                        ;                {
                        ;                  // Actually clears the bitmap
                        ;#if &_DpaMessageIqrfStd.PerStdSensorRead_Request.Bitmap[0] != &bufferRF[0]
                        ;#error Cannot use clearBufferRF for clearing bitmap
                        ;#endif
                        ;                  clearBufferRF();
        CALL  clearBufferRF
                        ;                  // Simulate 1st only sensor in the bitmap (states of the other unimplemented sensors do not care)
                        ;                  _DpaMessageIqrfStd.PerStdSensorRead_Request.Bitmap[0].0 = 1;
        MOVLB 0x09
        BSF   _DpaMessageIqrfStd,0
                        ;                  // Bitmap is 32 bits long
                        ;                  _DpaDataLength = W = sizeof( _DpaMessageIqrfStd.PerStdSensorRead_Request.Bitmap );
        MOVLW 0x04
        MOVLB 0x05
        MOVWF DLEN
                        ;                }
                        ;
                        ;                // Invalid bitmap (data) length (W = _DpaDataLength)?
                        ;                if ( W != sizeof( _DpaMessageIqrfStd.PerStdSensorRead_Request.Bitmap ) )
m026    XORLW 0x04
        BTFSS 0x03,Zero_
                        ;                  goto _ERROR_DATA_LEN;
        BRA   m023
                        ;
                        ;                // Now read the sensors
                        ;
                        ;                // Prepare pointer (minus 1, see below) to store sensor (types and) values to
                        ;                // Note: 3 sensors at this example cannot return more than DPA_MAX_DATA_LENGTH bytes of data, so it does not have to be checked...
                        ;                // ... If it would be the case, then ERROR_FAIL must be returned
                        ;                FSR1 = &_DpaMessage.Response.PData[-1];
        MOVLW 0x9F
        MOVWF FSR1
        MOVLW 0x04
        MOVWF FSR1+1
                        ;
                        ;                // Store bitmap of sensors to get values from
                        ;                uns8 sensorsBitmap = FSR1[1 + offsetof( TPerStdSensorRead_Request, Bitmap )];
        MOVIW 1[INDF1]
        MOVLB 0x0B
        MOVWF sensorsBitmap
                        ;
                        ;                // 1st sensor (index 0) selected?
                        ;                if ( sensorsBitmap.0 )
        BTFSS sensorsBitmap,0
        BRA   m027
                        ;                {
                        ;                  Get0_Temperature();
        CALL  Get0_Temperature
                        ;                  StoreValue( STD_SENSOR_TYPE_TEMPERATURE );
        MOVLW 0x01
        CALL  StoreValue
                        ;                }
                        ;
                        ;                // 2nd sensor (index 1) selected?
                        ;                if ( sensorsBitmap.1 )
m027    MOVLB 0x0B
        BTFSS sensorsBitmap,1
        BRA   m028
                        ;                {
                        ;                  Get1_BinaryData_Light();
        CALL  Get1_BinaryData_Light
                        ;                  StoreValue( STD_SENSOR_TYPE_BINARYDATA7 );
        MOVLW 0x81
        CALL  StoreValue
                        ;                }
                        ;
                        ;                // 3rd sensor (index 2) selected?
                        ;                if ( sensorsBitmap.2 )
m028    MOVLB 0x0B
        BTFSS sensorsBitmap,2
        BRA   m029
                        ;                {
                        ;                  Get2_BinaryData_Potentiometer();
        CALL  Get2_BinaryData_Potentiometer
                        ;                  StoreValue( STD_SENSOR_TYPE_BINARYDATA7 );
        MOVLW 0x81
        CALL  StoreValue
                        ;                }
                        ;
                        ;                // Compute returned data bytes count
                        ;                W = FSR1L - ( (uns16)&_DpaMessageIqrfStd & 0xFF ) + 1;
m029    MOVLW 0x9F
        SUBWF FSR1L,W
                        ;                // Optimization: return W long block of bytes at response
                        ;_W2_DpaDataLength:
                        ;                _DpaDataLength = W;
m030    MOVLB 0x05
        MOVWF DLEN
                        ;                goto DpaHandleReturnTRUE;
        BRA   m017
                        ;              }
                        ;            }
                        ;          }
                        ;        }
                        ;
                        ;        break;
                        ;      }
                        ;
                        ;      // -------------------------------------------------
                        ;    case DpaEvent_FrcValue:
                        ;      // Called to get FRC value
                        ;
                        ;      // FSR1 for optimization purposes (avoid MOVLB) will be used to point to DataOutBeforeResponseFRC[0...]
                        ;      FSR1 = (uns16)&PerStdSensorFrc;
m031    MOVLW 0xC0
        MOVWF FSR1
        MOVLW 0x04
        MOVWF FSR1+1
                        ;#if offsetof( TPerStdSensorFrc, Header ) != 0 || offsetof( TPerStdSensorFrc, SensorType ) != 1 || offsetof( TPerStdSensorFrc, Options ) != 3
                        ;#error Cannot optimize
                        ;#endif
                        ;      // Check for correct FRC user data
                        ;      if ( *FSR1++ /* PerStdSensorFrc.Header */ == PNUM_STD_SENSORS )
        MOVIW INDF1++
        XORLW 0x5E
        BTFSS 0x03,Zero_
        BRA   m043
                        ;      {
                        ;        // Actually used sensor index
                        ;        uns8 sensorIndex = FSR1[offsetof( TPerStdSensorFrc, SensorIndex ) - 1] & 0x1f;
        MOVIW 1[INDF1]
        ANDLW 0x1F
        MOVLB 0x0B
        MOVWF sensorIndex
                        ;        // Test sensor type
                        ;        switch ( *FSR1++ /* PerStdSensorFrc.SensorType */ )
        MOVIW INDF1++
        BTFSC 0x03,Zero_
        BRA   m035
        XORLW 0x01
        BTFSC 0x03,Zero_
        BRA   m032
        XORLW 0x80
        BTFSC 0x03,Zero_
        BRA   m033
        BRA   m043
                        ;        {
                        ;          default:
                        ;            goto DpaHandleReturnFALSE;
                        ;
                        ;            // No type specified, use specified index value
                        ;          case 0x00:
                        ;            goto _KeepSensorIndex;
                        ;
                        ;            // For other types make the index value based on the requested index value and sensor type
                        ;          case STD_SENSOR_TYPE_TEMPERATURE:
                        ;            if ( sensorIndex > 0 )
m032    MOVLB 0x0B
        MOVF  sensorIndex,1
        BTFSS 0x03,Zero_
                        ;              goto DpaHandleReturnFALSE;
        BRA   m043
                        ;            W = 0 + sensorIndex;
        MOVLB 0x0B
        MOVF  sensorIndex,W
                        ;            break;
        BRA   m034
                        ;
                        ;          case STD_SENSOR_TYPE_BINARYDATA7:
                        ;            if ( sensorIndex > 1 )
m033    MOVLW 0x02
        MOVLB 0x0B
        SUBWF sensorIndex,W
        BTFSC 0x03,Carry
                        ;              goto DpaHandleReturnFALSE;
        BRA   m043
                        ;            W = 1 + sensorIndex;
        MOVLB 0x0B
        INCF  sensorIndex,W
                        ;            break;
                        ;        }
                        ;
                        ;        // New sensor index based on type and requested index
                        ;        sensorIndex = W;
m034    MOVLB 0x0B
        MOVWF sensorIndex
                        ;_KeepSensorIndex:
                        ;
                        ;        // Test for supported FRC commands
                        ;        switch ( _PCMD )
m035    MOVLB 0x05
        MOVF  PCMD,W
        XORLW 0x10
        BTFSC 0x03,Zero_
        BRA   m036
        XORLW 0x80
        BTFSC 0x03,Zero_
        BRA   m036
        XORLW 0x70
        BTFSS 0x03,Zero_
        BRA   m043
                        ;        {
                        ;          default:
                        ;            goto DpaHandleReturnFALSE;
                        ;
                        ;          case FRC_STD_SENSORS_BIT:
                        ;          case FRC_STD_SENSORS_1B:
                        ;          case FRC_STD_SENSORS_2B:
                        ;            switch ( sensorIndex )
m036    MOVLB 0x0B
        MOVF  sensorIndex,W
        BTFSC 0x03,Zero_
        BRA   m037
        XORLW 0x01
        BTFSC 0x03,Zero_
        BRA   m038
        XORLW 0x03
        BTFSC 0x03,Zero_
        BRA   m039
        BRA   m043
                        ;            {
                        ;              default:
                        ;                goto DpaHandleReturnFALSE;
                        ;
                        ;              case 0:
                        ;                Carry = Get0_Temperature();
m037    CALL  Get0_Temperature
                        ;                break;
        BRA   m040
                        ;
                        ;              case 1:
                        ;                Carry = Get1_BinaryData_Light();
m038    CALL  Get1_BinaryData_Light
                        ;                break;
        BRA   m040
                        ;
                        ;              case 2:
                        ;                Carry = Get2_BinaryData_Potentiometer();
m039    CALL  Get2_BinaryData_Potentiometer
                        ;                break;
                        ;            }
                        ;
                        ;            // This type of FRC is not valid for the specified sensor
                        ;            if ( !Carry )
m040    BTFSS 0x03,Carry
                        ;              goto DpaHandleReturnFALSE;
        BRA   m043
                        ;
                        ;            break;
                        ;        }
                        ;
                        ;        // Some sensor was measured by FRC, check if there is a sleep request
                        ;        FSR1++;
        ADDFSR INDF1,1
                        ;        if ( INDF1.0 ) // Note: same as PerStdSensorFrc.Options.0
        BTFSS INDF1,0
        BRA   m043
                        ;        {
                        ;          // Remember sleep parameters to go to sleep at the Idle event later
                        ;          PerOSSleep_Request.Time.low8 = FSR1[offsetof( TPerOSSleep_Request, Time ) + 0 + offsetof( TPerStdSensorFrc, SleepParameters ) - 3];
        MOVIW 1[INDF1]
        MOVLB 0x0B
        MOVWF PerOSSleep_Request
                        ;          PerOSSleep_Request.Time.high8 = FSR1[offsetof( TPerOSSleep_Request, Time ) + 1 + offsetof( TPerStdSensorFrc, SleepParameters ) - 3];
        MOVIW 2[INDF1]
        MOVWF PerOSSleep_Request+1
                        ;          PerOSSleep_Request.Control = FSR1[offsetof( TPerOSSleep_Request, Control ) + offsetof( TPerStdSensorFrc, SleepParameters ) - 3];
        MOVIW 3[INDF1]
        MOVWF PerOSSleep_Request+2
                        ;        }
                        ;      }
                        ;
                        ;      break;
        BRA   m043
                        ;
                        ;      // -------------------------------------------------
                        ;    case DpaEvent_FrcResponseTime:
                        ;      // Called to get FRC response time
                        ;
                        ;      // In this example the FRC commands are fast 
                        ;      switch ( DataOutBeforeResponseFRC[0] )
m041    MOVLB 0x09
        MOVF  DataOutBeforeResponseFRC,W
        XORLW 0x10
        BTFSC 0x03,Zero_
        BRA   m042
        XORLW 0x80
        BTFSC 0x03,Zero_
        BRA   m042
        XORLW 0x70
        BTFSS 0x03,Zero_
        BRA   m043
                        ;      {
                        ;        case FRC_STD_SENSORS_BIT:
                        ;        case FRC_STD_SENSORS_1B:
                        ;        case FRC_STD_SENSORS_2B:
                        ;          responseFRCvalue = _FRC_RESPONSE_TIME_40_MS;
m042    MOVLB 0x0B
        CLRF  responseFRCvalue
                        ;          break;
                        ;      }
                        ;      break;
                        ;  }
                        ;DpaHandleReturnFALSE:
                        ;  return FALSE;
m043    BCF   0x03,Carry
        RETURN
                        ;}
                        ;
                        ;//############################################################################################
                        ;bit returnTRUE()
                        ;//############################################################################################
                        ;{
returnTRUE
                        ;  return TRUE;
        BSF   0x03,Carry
        RETURN
                        ;}
                        ;
                        ;//############################################################################################
                        ;bit returnFALSE()
                        ;//############################################################################################
                        ;{
returnFALSE
                        ;  return FALSE;
        BCF   0x03,Carry
        RETURN
                        ;}
                        ;
                        ;//############################################################################################
                        ;// Increases FSR1 and then stores the byte
                        ;void setPlusPlusINDF1( uns8 data @ W )
                        ;//############################################################################################
                        ;{
setPlusPlusINDF1
                        ;  FSR1++; // Note: must not modify W
        ADDFSR INDF1,1
                        ;  setINDF1( data );
        GOTO  setINDF1
                        ;}
                        ;
                        ;//############################################################################################
                        ;// Stores measured sensor value byte(s) and optionally sensor type to the FSR[+1...]
                        ;void StoreValue( uns8 sensorType )
                        ;//############################################################################################
                        ;{
StoreValue
        MOVLB 0x0B
        MOVWF sensorType
                        ;  // Is the sensor type to be stored too?
                        ;  if ( _PCMD == PCMD_STD_SENSORS_READ_TYPES_AND_VALUES )
        MOVLB 0x05
        DECFSZ PCMD,W
        BRA   m044
                        ;    setPlusPlusINDF1( sensorType );
        MOVLB 0x0B
        MOVF  sensorType,W
        CALL  setPlusPlusINDF1
                        ;
                        ;  // Store lower value byte
                        ;  setPlusPlusINDF1( sensorValue.low8 );
m044    MOVF  sensorValue,W
        CALL  setPlusPlusINDF1
                        ;
                        ;  // No more value bytes to store?
                        ;  if ( sensorType.7 != 0 )
        MOVLB 0x0B
        BTFSC sensorType,7
                        ;    return;
        RETURN
                        ;
                        ;  // Store higher value byte
                        ;  setPlusPlusINDF1( sensorValue.high8 );
        MOVF  sensorValue+1,W
        BRA   setPlusPlusINDF1
                        ;
                        ;  // Note: this example implements sensors returning only 1 or 2 bytes of data. If another data widths are returned, then it must be implemented explicitly.
                        ;}
                        ;
                        ;//############################################################################################
                        ;bit setFRCerror()
                        ;//############################################################################################
                        ;{
setFRCerror
                        ;  responseFRCvalue2B = 2;
        MOVLW 0x02
        MOVLB 0x0B
        MOVWF responseFRCvalue2B
        CLRF  responseFRCvalue2B+1
                        ;  return returnTRUE();
        BRA   returnTRUE
                        ;}
                        ;
                        ;//############################################################################################
                        ;bit sensorError;
                        ;bit AdjustFrcTemperature()
                        ;//############################################################################################
                        ;{
AdjustFrcTemperature
                        ;  // Test for supported FRC commands
                        ;  switch ( _PCMD )
        MOVLB 0x05
        MOVF  PCMD,W
        XORLW 0x90
        BTFSC 0x03,Zero_
        BRA   m045
        XORLW 0x70
        BTFSC 0x03,Zero_
        BRA   m048
        BRA   returnFALSE
                        ;  {
                        ;    default:
                        ;      return returnFALSE();
                        ;
                        ;    case FRC_STD_SENSORS_1B:
                        ;      // Return sensor FRC value 1B
                        ;      // Check for out of limits
                        ;      if ( sensorError || (int16)sensorValue > (int16)( 105.5 * 16 ) || (int16)sensorValue < ( (int16)-20 * 16 ) )
m045    MOVLB 0x0B
        BTFSC 0x5CE,sensorError
        BRA   setFRCerror
        BTFSC sensorValue+1,7
        BRA   m046
        MOVLW 0x06
        SUBWF sensorValue+1,W
        BTFSS 0x03,Carry
        BRA   m046
        BTFSS 0x03,Zero_
        BRA   setFRCerror
        MOVLW 0x99
        SUBWF sensorValue,W
        BTFSC 0x03,Carry
        BRA   setFRCerror
m046    BTFSS sensorValue+1,7
        BRA   m047
        MOVLW 0xFE
        SUBWF sensorValue+1,W
        BTFSS 0x03,Carry
        BRA   setFRCerror
        BTFSS 0x03,Zero_
        BRA   m047
        MOVLW 0xC0
        SUBWF sensorValue,W
        BTFSS 0x03,Carry
                        ;        return setFRCerror();
        BRA   setFRCerror
                        ;
                        ;      // Convert to the "F = ( T + 22 ) * 2 " from 1/16 resolution
                        ;      uns16 _sensorValue = sensorValue + 4;	// Note: do rounding when /8
m047    MOVF  sensorValue+1,W
        MOVLB 0x0B
        MOVWF _sensorValue+1
        MOVLW 0x04
        ADDWF sensorValue,W
        MOVWF _sensorValue
        BTFSC 0x03,Carry
        INCF  _sensorValue+1,1
                        ;      responseFRCvalue = (uns8)( _sensorValue / 8 ) + 44;
        RRF   _sensorValue+1,W
        MOVWF C1cnt
        RRF   _sensorValue,W
        RRF   C1cnt,1
        RRF   WREG,1
        RRF   C1cnt,1
        RRF   WREG,1
        ADDLW 0x2C
        MOVWF responseFRCvalue
                        ;      break;
        BRA   returnTRUE
                        ;
                        ;    case FRC_STD_SENSORS_2B:
                        ;      // Return sensor FRC value 2B
                        ;      if ( sensorError )
m048    MOVLB 0x0B
        BTFSC 0x5CE,sensorError
                        ;        return setFRCerror();
        BRA   setFRCerror
                        ;
                        ;      responseFRCvalue2B = sensorValue ^ 0x8000;
        MOVLW 0x80
        XORWF sensorValue+1,W
        MOVLB 0x0B
        MOVWF responseFRCvalue2B+1
        MOVF  sensorValue,W
        MOVWF responseFRCvalue2B
                        ;      break;
        BRA   returnTRUE
                        ;  }
                        ;
                        ;  return returnTRUE();
                        ;}
                        ;
                        ;//############################################################################################
                        ;// Sensor index 1: measure temperature using one of the DDC-SE-01 sensors
                        ;bit Get0_Temperature()
                        ;//############################################################################################
                        ;{
Get0_Temperature
                        ;  // Make sure FSR1 is not modified
                        ;
                        ;  // Measure temperature using DDC-SE-01 sensors
                        ;  // Read temperature and check for an error
                        ;
                        ;  // Reads temperature from an enabled sensor
                        ;#ifdef DALLASnotMCP
                        ;  sensorError = FALSE;
        MOVLB 0x0B
        BCF   0x5CE,sensorError
                        ;  // Temperature is ready at the background
                        ;  sensorValue = temperature;
        MOVF  temperature,W
        MOVWF sensorValue
        MOVF  temperature+1,W
        MOVWF sensorValue+1
                        ;  // When error, return standard (FRC) error value(s)
                        ;  if ( STD_SENSOR_TYPE_TEMPERATURE_IS_ERROR( sensorValue ) )
        MOVF  sensorValue,1
        BTFSS 0x03,Zero_
        BRA   AdjustFrcTemperature
        MOVF  sensorValue+1,W
        XORLW 0x80
        BTFSS 0x03,Zero_
        BRA   AdjustFrcTemperature
                        ;    sensorError = TRUE;
        BSF   0x5CE,sensorError
                        ;#else
                        ;  sensorError = TRUE;
                        ;  // Temperature value must be read from I2C sensor
                        ;  STD_SENSOR_TYPE_TEMPERATURE_SET_ERROR( sensorValue );
                        ;  // MCP9802 address
                        ;  i2c_start( I2C_ADR );
                        ;  if ( !i2cTimeout )
                        ;  {
                        ;    // pointer: 1 = configuration register
                        ;    i2c_write( 0x01 );
                        ;    // configuration: 9-bit ADC
                        ;    i2c_write( 0x00 );
                        ;    i2c_stop();
                        ;
                        ;    // MCP9802 address
                        ;    i2c_start( I2C_ADR );
                        ;    // pointer: 0 = temperature
                        ;    i2c_write( 0 );
                        ;    i2c_stop();
                        ;
                        ;    // MCP9802 address + read
                        ;    i2c_start( I2C_ADR | 1 );
                        ;    // store the result
                        ;    sensorValue.high8 = i2c_read( TRUE );
                        ;    sensorValue.low8 = i2c_read( FALSE );
                        ;    i2c_stop();
                        ;
                        ;    sensorValue += 0x10 / 2;
                        ;    sensorValue /= 0x10;
                        ;
                        ;    sensorError = FALSE;
                        ;  }
                        ;#endif
                        ;
                        ;  // FrcValues
                        ;  return AdjustFrcTemperature();
        BRA   AdjustFrcTemperature
                        ;}
                        ;
                        ;//############################################################################################
                        ;// Sensor index 1: returns light intensity indicator value using DDC-SE-01
                        ;bit Get_BinaryData_Final( uns8 _ADCON0 @ W )
                        ;//############################################################################################
                        ;{
Get_BinaryData_Final
                        ;  ADCON0 = _ADCON0;
        MOVLB 0x01
        MOVWF ADCON0
                        ;  // Read ADC
                        ;
                        ;  // ADC result - left justified, Fosc/8
                        ;  ADCON1 = 0b0001.0000;
        MOVLW 0x10
        MOVWF ADCON1
                        ;  // Do a smallest delay for ADC ACQUISITION TIME
                        ;  waitMS( 1 );
        MOVLW 0x01
        CALL  waitMS
                        ;  // start ADC
                        ;  GO = 1;
        MOVLB 0x01
        BSF   0x9D,GO
                        ;  // wait for ADC finish
                        ;  while ( GO );
m049    MOVLB 0x01
        BTFSC 0x9D,GO
        BRA   m049
                        ;  // Get ADC value
                        ;  sensorValue.low8 = ADRESH / 2;
        BCF   0x03,Carry
        MOVLB 0x01
        RRF   ADRESH,W
        MOVWF sensorValue
                        ;
                        ;  // Return sensor FRC value
                        ;
                        ;  // Test for supported FRC commands
                        ;  switch ( _PCMD )
        MOVLB 0x05
        MOVF  PCMD,W
        XORLW 0x10
        BTFSC 0x03,Zero_
        BRA   m050
        XORLW 0x80
        BTFSC 0x03,Zero_
        BRA   m052
        BRA   returnFALSE
                        ;  {
                        ;    default:
                        ;      return returnFALSE();
                        ;
                        ;    case FRC_STD_SENSORS_BIT:
                        ;      // If there is a sensor error, 2-bit FRC cannot indicate it, it returns [01]
                        ;
                        ;      // Number of shifts to get the bit out of the return value
                        ;      uns8 bitLoop = ( INDF1 >> 5 ) + 1;
m050    SWAPF INDF1,W
        ANDLW 0x0F
        LSRF  WREG,W
        ADDLW 0x01
        MOVLB 0x0B
        MOVWF bitLoop
                        ;      // Value to get the bit from
                        ;      W = sensorValue.low8;
        MOVF  sensorValue,W
                        ;      do
                        ;      {
                        ;        // Get the bit to Carry
                        ;        W = rr( W );
m051    RRF   WREG,1
                        ;        // Next bit
                        ;      } while ( --bitLoop != 0 ); // Note: must not modify W and Carry
        MOVLB 0x0B
        DECFSZ bitLoop,1
        BRA   m051
                        ;      // Current (prepared by DPA) FRC value is [01], change it to [11] (means bit is set)
                        ;      responseFRCvalue.1 = 1; // Note: must not modify Carry
        BSF   responseFRCvalue,1
                        ;      // Is bit set?
                        ;      if ( !Carry )
        BTFSC 0x03,Carry
        BRA   returnTRUE
                        ;        // Bit is NOT set, return [10]
                        ;        responseFRCvalue.0 = 0;
        BCF   responseFRCvalue,0
                        ;      break;
        BRA   returnTRUE
                        ;
                        ;    case FRC_STD_SENSORS_1B:
                        ;      responseFRCvalue = sensorValue.low8 + 4;
m052    MOVLW 0x04
        ADDWF sensorValue,W
        MOVLB 0x0B
        MOVWF responseFRCvalue
                        ;      break;
        BRA   returnTRUE
                        ;  }
                        ;
                        ;  return returnTRUE();
                        ;}
                        ;
                        ;//############################################################################################
                        ;// Sensor index 1: returns light intensity indicator value using DDC-SE-01
                        ;bit Get1_BinaryData_Light()
                        ;//############################################################################################
                        ;{
Get1_BinaryData_Light
                        ;  // Make sure FSR1 is not modified
                        ;
                        ;  // ADC initialization (for more info see PIC datasheet) pin C1 (AN0) as analog input 
                        ;  ANSELA.0 = 1;
        MOVLB 0x03
        BSF   ANSELA,0
                        ;  // ADC setting (AN0 channel)
                        ;  return Get_BinaryData_Final( 0b0.00000.01 );
        MOVLW 0x01
        BRA   Get_BinaryData_Final
                        ;}
                        ;
                        ;//############################################################################################
                        ;// Sensor index 2: returns potentiometer value using DDC-SE-01
                        ;bit Get2_BinaryData_Potentiometer()
                        ;//############################################################################################
                        ;{
Get2_BinaryData_Potentiometer
                        ;  // Make sure FSR1 is not modified
                        ;
                        ;  // ADC initialization (for more info see PIC datasheet) pin C5 (AN4) as analog input 
                        ;  ANSELA.5 = 1;
        MOVLB 0x03
        BSF   ANSELA,5
                        ;  // ADC setting (AN4 channel)
                        ;  return Get_BinaryData_Final( 0b0.00100.01 );
        MOVLW 0x11
        BRA   Get_BinaryData_Final
                        ;}
                        ;
                        ;#ifdef DALLASnotMCP
                        ;//############################################################################################
                        ;// OneWire and Dallas 18B20 routines
                        ;//############################################################################################
                        ;
                        ;//############################################################################################
                        ;void Delay5us( uns8 val @ W ) // Absolutely precise timing but val != 0
                        ;//############################################################################################
                        ;{
Delay5us
                        ;  // 16 MHz
                        ;  // + 0.75us ( W=val, Call )
                        ;  for ( ;; )
                        ;  {         // loop time
                        ;    nop2();	// 0.50us
m053    BRA   m054
                        ;    nop2();	// 1.00us
m054    BRA   m055
                        ;    nop2();	// 1.50us
m055    BRA   m056
                        ;    nop2();	// 2.00us
m056    BRA   m057
                        ;    nop2();	// 2.50us
m057    BRA   m058
                        ;    nop2();	// 3.00us
m058    BRA   m059
                        ;    nop();	// 3.25us
m059    NOP  
                        ;    if ( --val == 0 ) // + 0.75us (W--, BTFS ) 
        ADDLW 0xFF
        BTFSC 0x03,Zero_
                        ;      return;         // + 0.25us 
        RETURN
                        ;    nop2();	// 4.50us
        BRA   m060
                        ;  }         // 5.00us (Goto)
m060    BRA   m053
                        ;}
                        ;//############################################################################################
                        ;
                        ;#define	OneWireData0()	{ OneWire_TRIS = 0; }	  // 0.5us @ 16MHz
                        ;#define OneWireData1()  { OneWire_TRIS = 1; }	  // 0.5us @ 16MHz
                        ;
                        ;//############################################################################################
                        ;void OneWireWriteByte( uns8 byte )
                        ;//############################################################################################
                        ;{
OneWireWriteByte
        MOVLB 0x0B
        MOVWF byte
                        ;  uns8 bitLoop = 8;
        MOVLW 0x08
        MOVWF bitLoop_2
                        ;  do
                        ;  {
                        ;    // Next sequence is time precision critical
                        ;    GIE = FALSE;
m061    BCF   0x0B,GIE
                        ;
                        ;    OneWireData0();
        MOVLB 0x01
        BCF   TRISC,3
                        ;    nop2();	        // 1 us [0.5 us]
        BRA   m062
                        ;    nop2();	        // [1.0 us]
m062    BRA   m063
                        ;    if ( byte.0 )	// 2.5 us [1.75us]
m063    MOVLB 0x0B
        BTFSS byte,0
        BRA   m064
                        ;      OneWireData1();
        MOVLB 0x01
        BSF   TRISC,3
                        ;
                        ;    // End of time precision critical sequence
                        ;    GIE = TRUE;
m064    BSF   0x0B,GIE
                        ;
                        ;    // 60us minimum in total, does not have to be precise
                        ;    Delay5us( ( 60 - 3 ) / 5 + 1 );
        MOVLW 0x0C
        CALL  Delay5us
                        ;
                        ;    OneWireData1();
        MOVLB 0x01
        BSF   TRISC,3
                        ;
                        ;    byte >>= 1;
        BCF   0x03,Carry
        MOVLB 0x0B
        RRF   byte,1
                        ;  } while ( --bitLoop != 0 );
        DECFSZ bitLoop_2,1
        BRA   m061
                        ;}
        RETURN
                        ;
                        ;//############################################################################################
                        ;uns8 OneWireReadByte()
                        ;//############################################################################################
                        ;{
OneWireReadByte
                        ;  uns8 result;
                        ;  uns8 bitLoop = 8;
        MOVLW 0x08
        MOVLB 0x0B
        MOVWF bitLoop_3
                        ;  do
                        ;  {
                        ;    // Next sequence is time precision critical
                        ;    GIE = FALSE;
m065    BCF   0x0B,GIE
                        ;
                        ;    OneWireData0();
        MOVLB 0x01
        BCF   TRISC,3
                        ;    nop2();	        // 1 us [0.5 us]
        BRA   m066
                        ;#if F_OSC == 16000000
                        ;    nop2();	        // [1.0 us]
m066    BRA   m067
                        ;#endif
                        ;    OneWireData1();			// 2 us [1.5 us]
m067    MOVLB 0x01
        BSF   TRISC,3
                        ;    Delay5us( 15 / 5 );		// 17 us [16.5 us]
        MOVLW 0x03
        CALL  Delay5us
                        ;
                        ;    Carry = 0;				// 17.5 us [16.75 us]
        BCF   0x03,Carry
                        ;    if ( OneWire_IO_IN )	// 18.5 us [ 17.25 us] (condition must not modify Carry)
        MOVLB 0x00
        BTFSC PORTC,3
                        ;      Carry = 1;
        BSF   0x03,Carry
                        ;
                        ;    // End of time precision critical sequence
                        ;    GIE = TRUE;				// must not modify Carry
        BSF   0x0B,GIE
                        ;    result = rr( result );
        MOVLB 0x0B
        RRF   result,1
                        ;
                        ;    // 60us minimum in total, does not have to be precise
                        ;    Delay5us( ( 60 - 20 ) / 5 + 1 );
        MOVLW 0x09
        CALL  Delay5us
                        ;  } while ( --bitLoop != 0 );
        DECFSZ bitLoop_3,1
        BRA   m065
                        ;
                        ;  return result;
        MOVF  result,W
        RETURN
                        ;}
                        ;
                        ;//############################################################################################
                        ;bit OneWireReset()
                        ;//############################################################################################
                        ;{
OneWireReset
                        ;  // Setting the pin once to low is enough
                        ;  OneWire_IO_OUT = 0;
        MOVLB 0x02
        BCF   LATC,3
                        ;  // Reset pulse
                        ;  OneWireData0();
        MOVLB 0x01
        BCF   TRISC,3
                        ;  Delay5us( 500 / 5 );
        MOVLW 0x64
        CALL  Delay5us
                        ;  // Reset pulse end
                        ;  OneWireData1();
        BSF   TRISC,3
                        ;  // Next sequence is time precision critical
                        ;  GIE = FALSE;
        BCF   0x0B,GIE
                        ;  // Wait for presence pulse
                        ;  Delay5us( 70 / 5 );
        MOVLW 0x0E
        CALL  Delay5us
                        ;  // End of time precision critical sequence
                        ;  GIE = TRUE;
        BSF   0x0B,GIE
                        ;  // Presence pulse?
                        ;  if ( OneWire_IO_IN )
        MOVLB 0x00
        BTFSS PORTC,3
        BRA   m068
                        ;  {
                        ;    // No presence, finish initialization sequence
                        ;    Delay5us( ( 500 - 70 ) / 5 );
        MOVLW 0x56
        CALL  Delay5us
                        ;    return returnFALSE();
        BRA   returnFALSE
                        ;  }
                        ;  else
                        ;  {
                        ;    // Presence OK, finish initialization sequence
                        ;    Delay5us( ( 500 - 70 ) / 5 );
m068    MOVLW 0x56
        CALL  Delay5us
                        ;    return returnTRUE();
        BRA   returnTRUE
                        ;  }
                        ;}
                        ;
                        ;//############################################################################################
                        ;void OneWireCmd( uns8 cmd )
                        ;//############################################################################################
                        ;{
OneWireCmd
        MOVLB 0x0B
        MOVWF cmd
                        ;  // OneWire: Skip ROM
                        ;  OneWireWriteByte( CMD_SKIPROM );
        MOVLW 0xCC
        CALL  OneWireWriteByte
                        ;  // OneWire: Send command
                        ;  OneWireWriteByte( cmd );
        MOVLB 0x0B
        MOVF  cmd,W
        BRA   OneWireWriteByte
                        ;}
                        ;
                        ;//############################################################################################
                        ;bit Ds18B20WriteConfig( uns8 value )
                        ;//############################################################################################
                        ;{
Ds18B20WriteConfig
        MOVLB 0x0B
        MOVWF value_4
                        ;  if ( OneWireReset() )
        CALL  OneWireReset
        BTFSS 0x03,Carry
        BRA   returnFALSE
                        ;  {
                        ;    // Write Scratchpad
                        ;    OneWireCmd( CMD_WSCRATCHPAD );
        MOVLW 0x4E
        CALL  OneWireCmd
                        ;
                        ;    // Write TL = ? (we dot not care the value)
                        ;    OneWireWriteByte( W );
        CALL  OneWireWriteByte
                        ;    // Write TH = ? (we dot not care the value)
                        ;    OneWireWriteByte( W );
        CALL  OneWireWriteByte
                        ;    // Write Config byte
                        ;    OneWireWriteByte( value );
        MOVLB 0x0B
        MOVF  value_4,W
        CALL  OneWireWriteByte
                        ;
                        ;    if ( OneWireReset() )
        CALL  OneWireReset
        BTFSS 0x03,Carry
        GOTO  returnFALSE
                        ;    {
                        ;      //  Copy Scratchpad
                        ;      OneWireCmd( CMD_CPYSCRATCHPAD );
        MOVLW 0x48
        CALL  OneWireCmd
                        ;      return returnTRUE();
        GOTO  returnTRUE
                        ;    }
                        ;  }
                        ;  return returnFALSE();

  ; FILE ../../../../Development/include/DPA/DPAcustomHandler.h
                        ;// *********************************************************************
                        ;//   Main Custom DPA Handler header                                    *
                        ;// *********************************************************************
                        ;// Copyright (c) IQRF Tech s.r.o.
                        ;//
                        ;// File:    $RCSfile: DPAcustomHandler.h,v $
                        ;// Version: $Revision: 1.113 $
                        ;// Date:    $Date: 2020/02/27 13:17:18 $
                        ;//
                        ;// Revision history:
                        ;//   2020/02/27  Release for DPA 4.13
                        ;//   2020/01/09  Release for DPA 4.12
                        ;//   2019/12/11  Release for DPA 4.11
                        ;//   2019/10/09  Release for DPA 4.10
                        ;//   2019/06/12  Release for DPA 4.03
                        ;//   2019/06/03  Release for DPA 4.02
                        ;//   2019/03/07  Release for DPA 4.01
                        ;//   2019/01/10  Release for DPA 4.00
                        ;//   2018/10/25  Release for DPA 3.03
                        ;//   2017/11/16  Release for DPA 3.02
                        ;//   2017/08/14  Release for DPA 3.01
                        ;//   2017/03/13  Release for DPA 3.00
                        ;//   2016/09/12  Release for DPA 2.28
                        ;//   2016/04/14  Release for DPA 2.27
                        ;//   2016/03/03  Release for DPA 2.26
                        ;//   2016/01/21  Release for DPA 2.25
                        ;//   2015/12/01  Release for DPA 2.24
                        ;//   2015/10/23  Release for DPA 2.23
                        ;//   2015/09/25  Release for DPA 2.22
                        ;//   2015/09/03  Release for DPA 2.21
                        ;//   2015/08/05  Release for DPA 2.20
                        ;//   2014/10/31  Release for DPA 2.10
                        ;//   2014/04/30  Release for DPA 2.00
                        ;//   2013/10/03  Release for DPA 1.00
                        ;//
                        ;// *********************************************************************
                        ;
                        ;// Online DPA documentation https://doc.iqrf.org/DpaTechGuide/
                        ;// IQRF Standards documentation https://www.iqrfalliance.org/iqrf-interoperability/
                        ;
                        ;#ifndef _CUSTOM_DPA_HANDLER_
                        ;#define _CUSTOM_DPA_HANDLER_
                        ;
                        ;//############################################################################################
                        ;// 1st include 
                        ;
                        ;// Custom DPA Handler routine declaration
                        ;bit CustomDpaHandler();
                        ;
                        ;// Various DPA flags shared between DPA and Custom DPA Handler
                        ;uns8 DpaFlags @ usedBank4[0];
                        ;#ifdef COORDINATOR_CUSTOM_HANDLER
                        ;// [C] TRUE if interface master is not connected (detected)
                        ;bit IFaceMasterNotConnected @ DpaFlags.2;
                        ;#endif
                        ;// [N] [CN] DPA by interface notification is sent also when there was a "reading" like DPA request
                        ;bit EnableIFaceNotificationOnRead @ DpaFlags.3;
                        ;// [N] [NC] TRUE when node was just bonded using default bonding procedure
                        ;bit NodeWasBonded @ DpaFlags.4;
                        ;
                        ;// [C] Ticks (decrementing) counter usable for timing in the coordinator's Customer DPA Handler
                        ;uns16 DpaTicks @ usedBank4[1];
                        ;// [N] [NC] toutRF for LP mode, read from configuration memory after reset
                        ;uns8 LPtoutRF @ usedBank4[3];
                        ;// DPA Request/Response HWPID
                        ;uns16 _HWPID @ usedBank4[4];
                        ;// Identifies type of reset (stored at UserReg0 upon module reset). See Reset chapter at IQRF User's Guide for more information
                        ;uns8 ResetType @ usedBank4[6];
                        ;// User DPA Values to return
                        ;uns8 UserDpaValue @ usedBank4[7];
                        ;// Network depth of the DPA request/response, increases on bridging, decreases on back-bridging
                        ;uns8 NetDepth @ usedBank4[8];
                        ;// TRUE when node was at DPA Service Mode after last boot
                        ;bit DSMactivated @ usedBank4[9].0;
                        ;// If set to TRUE, then LP RX mode in the main loop can be terminated by pin, see _RLPMAT
                        ;bit LpRxPinTerminate @ usedBank4[9].1;
                        ;// If set to TRUE, then [C] executes asynchronous DPA requests received from [N]
                        ;bit AsyncReqAtCoordinator @ usedBank4[9].2;
                        ;// RX filter used at the DPA main loop checkRF call
                        ;uns8 RxFilter @ usedBank4[11];
                        ;// Countdown variable for button bonding before going to deep sleep 
                        ;uns16 BondingSleepCountdown @ usedBank4[12];
                        ;#define BONDING_SLEEP_COUNTDOWN_UNIT  290
                        ;// Non-zero pseudo-random value, read-only, updated on every Reset and Idle event, at [N] only.
                        ;uns16 Random @ usedBank4[14];
                        ;
                        ;// Macro to return an error from the peripheral handler. If the code size is not an issue this macro is the right choice.
                        ;#define DpaApiReturnPeripheralError(error) do { \
                        ;	DpaApiSetPeripheralError( error ); \
                        ;	return Carry; \
                        ;  } while( 0 )
                        ;
                        ;// DPA API functions, see documentation for details
                        ;#define	DpaApiRfTxDpaPacket( dpaValue, netDepthAndFlags ) DpaApiEntry( dpaValue, netDepthAndFlags, DPA_API_RFTX_DPAPACKET )
                        ;#define	DpaApiReadConfigByte( index )					  DpaApiEntry( index, param3.low8, DPA_API_READ_CONFIG_BYTE )
                        ;#define	DpaApiLocalRequest()							  DpaApiEntry( param2, param3.low8, DPA_API_LOCAL_REQUEST )
                        ;#define	DpaApiSetPeripheralError( error )				  DpaApiEntry( error, param3.low8, DPA_API_SET_PERIPHERAL_ERROR )
                        ;#define	DpaApiSendToIFaceMaster( dpaValue, flags )		  DpaApiEntry( dpaValue, flags, DPA_API_SEND_TO_IFACEMASTER )
                        ;#define	DpaApiSetRfDefaults()							  DpaApiEntry( param2, param3.low8, DPA_API_SET_RF_DEFAULTS )
                        ;
                        ;#ifdef COORDINATOR_CUSTOM_HANDLER
                        ;#define	DpaApiRfTxDpaPacketCoordinator()				  DpaApiEntry( param2, param3.low8, DPA_API_COORDINATOR_RFTX_DPAPACKET )
                        ;#endif
                        ;
                        ;#ifdef COORDINATOR_CUSTOM_HANDLER
                        ;#undef	DpaEvent_Interrupt
                        ;#undef	DpaEvent_BeforeSleep
                        ;#undef	DpaEvent_AfterSleep
                        ;#undef	DpaEvent_FrcValue
                        ;#undef	DpaEvent_FrcResponseTime
                        ;#else
                        ;#undef  DpaEvent_ReceiveDpaResponse
                        ;#undef  DpaEvent_IFaceReceive
                        ;#endif
                        ;
                        ;// To detect overlapping code in case someone would put some code before this header by mistake
                        ;#pragma origin __APPLICATION_ADDRESS
                        ;#pragma updateBank 0
                        ;
                        ;//############################################################################################
                        ;// Main IQRF entry point jumps to the main DPA entry point
                        ;void APPLICATION()
                        ;//############################################################################################
                        ;{
                        ;  #asm
                        ;    DW  __MOVLP( MAIN_DPA_ADDRESS >> 8 );
                        ;  DW  __GOTO( MAIN_DPA_ADDRESS );
                        ;  #endasm
                        ;
                        ;#ifndef NO_CUSTOM_DPA_HANDLER
                        ;    // Fake call to force CustomDpaHandler() compilation
                        ;    CustomDpaHandler();
                        ;#endif
                        ;
                        ;  // Fake call to force DpaApiEntry() compilation
                        ;  DpaApiEntry( param2, param3.low8, W );
                        ;}
                        ;
                        ;//############################################################################################
                        ;// Entry stub to the real DPA API entry
                        ;#pragma origin DPA_API_ADDRESS_ENTRY
                        ;uns8  DpaApiEntry( uns8 par1 @ param2, uns8 par2 @ param3.low8, uns8 apiIndex @ W )
                        ;//############################################################################################
                        ;{
                        ;  #asm
                        ;    DW  __MOVLP( DPA_API_ADDRESS >> 8 );
                        ;  DW  __CALL( DPA_API_ADDRESS );
                        ;  DW  __MOVLP( DPA_API_ADDRESS_ENTRY >> 8 );
                        ;  #endasm
                        ;
                        ;    return W;
                        ;}
                        ;
                        ;//############################################################################################
                        ;#pragma origin DPA_API_ADDRESS_ENTRY + 0x08
                        ;
                        ;//############################################################################################
                        ;
                        ;#ifndef NO_CUSTOM_DPA_HANDLER
                        ;// Next comes Custom DPA handler routine
                        ;#pragma origin CUSTOM_HANDLER_ADDRESS
                        ;#endif
                        ;
                        ;#pragma updateBank 1
                        ;
                        ;//############################################################################################
                        ;#else // _CUSTOM_DPA_HANDLER_
                        ;//############################################################################################
                        ;// 2nd include
                        ;
                        ;#ifndef NO_CUSTOM_DPA_HANDLER
                        ;// Code bumper to detect too long code of Custom DPA handler + other routines
                        ;#pragma origin CUSTOM_HANDLER_ADDRESS_END
        ORG 0x3D80
                        ;// To avoid adding some code after handler by mistake
                        ;#pragma origin __MAX_FLASH_ADDRESS
        ORG 0x3FFF

        END


; *** KEY INFO ***

; 0x0100 P0    5 word(s)  0 % : main

; 0x3A00 P7    4 word(s)  0 % : APPLICATION
; 0x3810 P7    2 word(s)  0 % : dummy
; 0x3816 P7    2 word(s)  0 % : iqrfSleep
; 0x3819 P7    2 word(s)  0 % : _debug
; 0x381C P7    2 word(s)  0 % : moduleInfo
; 0x3822 P7    2 word(s)  0 % : pulsingLEDR
; 0x3825 P7    2 word(s)  0 % : pulseLEDR
; 0x3828 P7    2 word(s)  0 % : stopLEDR
; 0x382B P7    2 word(s)  0 % : pulsingLEDG
; 0x382E P7    2 word(s)  0 % : pulseLEDG
; 0x3831 P7    2 word(s)  0 % : stopLEDG
; 0x3834 P7    2 word(s)  0 % : setOnPulsingLED
; 0x3837 P7    2 word(s)  0 % : setOffPulsingLED
; 0x383A P7    2 word(s)  0 % : eeReadByte
; 0x383D P7    2 word(s)  0 % : eeReadData
; 0x3840 P7    2 word(s)  0 % : eeWriteByte
; 0x3843 P7    2 word(s)  0 % : eeWriteData
; 0x3846 P7    2 word(s)  0 % : readFromRAM
; 0x384C P7    2 word(s)  0 % : clearBufferINFO
; 0x384F P7    2 word(s)  0 % : swapBufferINFO
; 0x3852 P7    2 word(s)  0 % : compareBufferINFO2RF
; 0x3855 P7    2 word(s)  0 % : copyBufferINFO2COM
; 0x3858 P7    2 word(s)  0 % : copyBufferINFO2RF
; 0x385B P7    2 word(s)  0 % : copyBufferRF2COM
; 0x385E P7    2 word(s)  0 % : copyBufferRF2INFO
; 0x3861 P7    2 word(s)  0 % : copyBufferCOM2RF
; 0x3864 P7    2 word(s)  0 % : copyBufferCOM2INFO
; 0x3867 P7    2 word(s)  0 % : copyMemoryBlock
; 0x386A P7    2 word(s)  0 % : startDelay
; 0x386D P7    2 word(s)  0 % : startLongDelay
; 0x3870 P7    2 word(s)  0 % : isDelay
; 0x3873 P7    2 word(s)  0 % : waitDelay
; 0x3876 P7    2 word(s)  0 % : waitMS
; 0x3879 P7    2 word(s)  0 % : startCapture
; 0x387C P7    2 word(s)  0 % : captureTicks
; 0x3882 P7    2 word(s)  0 % : waitNewTick
; 0x3885 P7    2 word(s)  0 % : enableSPI
; 0x3888 P7    2 word(s)  0 % : disableSPI
; 0x388B P7    2 word(s)  0 % : startSPI
; 0x388E P7    2 word(s)  0 % : stopSPI
; 0x3891 P7    2 word(s)  0 % : restartSPI
; 0x3894 P7    2 word(s)  0 % : getStatusSPI
; 0x3897 P7    2 word(s)  0 % : setRFpower
; 0x389A P7    2 word(s)  0 % : setLEDG
; 0x389D P7    2 word(s)  0 % : setRFchannel
; 0x38A0 P7    2 word(s)  0 % : setRFmode
; 0x38A3 P7    2 word(s)  0 % : setRFspeed
; 0x38A6 P7    2 word(s)  0 % : setRFsleep
; 0x38A9 P7    2 word(s)  0 % : setRFready
; 0x38AC P7    2 word(s)  0 % : RFTXpacket
; 0x38AF P7    2 word(s)  0 % : RFRXpacket
; 0x38B2 P7    2 word(s)  0 % : checkRF
; 0x38B8 P7    2 word(s)  0 % : amIBonded
; 0x38BB P7    2 word(s)  0 % : removeBond
; 0x38BE P7    2 word(s)  0 % : bondNewNode
; 0x38C1 P7    2 word(s)  0 % : isBondedNode
; 0x38C4 P7    2 word(s)  0 % : removeBondedNode
; 0x38C7 P7    2 word(s)  0 % : rebondNode
; 0x38CA P7    2 word(s)  0 % : clearAllBonds
; 0x38CD P7    2 word(s)  0 % : setNonetMode
; 0x38D0 P7    2 word(s)  0 % : setCoordinatorMode
; 0x38D3 P7    2 word(s)  0 % : setNodeMode
; 0x38D6 P7    2 word(s)  0 % : setNetworkFilteringOn
; 0x38D9 P7    2 word(s)  0 % : setNetworkFilteringOff
; 0x38DC P7    2 word(s)  0 % : getNetworkParams
; 0x38DF P7    2 word(s)  0 % : setRoutingOn
; 0x38E2 P7    2 word(s)  0 % : setRoutingOff
; 0x38E8 P7    2 word(s)  0 % : answerSystemPacket
; 0x38EB P7    2 word(s)  0 % : discovery
; 0x38EE P7    2 word(s)  0 % : wasRouted
; 0x38F1 P7    2 word(s)  0 % : optimizeHops
; 0x38F4 P7    2 word(s)  0 % : getSupplyVoltage
; 0x38F7 P7    2 word(s)  0 % : getTemperature
; 0x38FA P7    2 word(s)  0 % : clearBufferRF
; 0x3910 P7    2 word(s)  0 % : isDiscoveredNode
; 0x3913 P7    2 word(s)  0 % : enableRFPGM
; 0x3916 P7    2 word(s)  0 % : disableRFPGM
; 0x3919 P7    2 word(s)  0 % : setupRFPGM
; 0x391C P7    2 word(s)  0 % : runRFPGM
; 0x391F P7    2 word(s)  0 % : iqrfDeepSleep
; 0x3922 P7    2 word(s)  0 % : wasRFICrestarted
; 0x3925 P7    2 word(s)  0 % : eeeWriteData
; 0x3928 P7    2 word(s)  0 % : eeeReadData
; 0x3931 P7    2 word(s)  0 % : setINDF0
; 0x3934 P7    2 word(s)  0 % : setINDF1
; 0x3937 P7    2 word(s)  0 % : getRSSI
; 0x393A P7    2 word(s)  0 % : removeBondAddress
; 0x393D P7    2 word(s)  0 % : sendFRC
; 0x3940 P7    2 word(s)  0 % : responseFRC
; 0x3943 P7    2 word(s)  0 % : bondRequestAdvanced
; 0x3946 P7    2 word(s)  0 % : prebondNodeAtNode
; 0x3949 P7    2 word(s)  0 % : nodeAuthorization
; 0x394C P7    2 word(s)  0 % : dummy01
; 0x3958 P7    2 word(s)  0 % : setAccessPassword
; 0x395B P7    2 word(s)  0 % : setUserKey
; 0x3961 P7    2 word(s)  0 % : amIRecipientOfFRC
; 0x3964 P7    2 word(s)  0 % : setLEDR
; 0x3967 P7    2 word(s)  0 % : encryptBufferRF
; 0x396A P7    2 word(s)  0 % : decryptBufferRF
; 0x396D P7    2 word(s)  0 % : prebondNodeAtCoordinator
; 0x3970 P7    2 word(s)  0 % : setFSRs
; 0x3973 P7    2 word(s)  0 % : updateCRC16
; 0x3976 P7    2 word(s)  0 % : smartConnect
; 0x3979 P7    2 word(s)  0 % : addressBitmap
; 0x397C P7    3 word(s)  0 % : setServiceChannel
; 0x3A08 P7    4 word(s)  0 % : DpaApiEntry
; 0x3A20 P7  341 word(s) 16 % : CustomDpaHandler
; 0x3BCF P7   15 word(s)  0 % : Get0_Temperature
; 0x3C0B P7    4 word(s)  0 % : Get1_BinaryData_Light
; 0x3C0F P7    4 word(s)  0 % : Get2_BinaryData_Potentiometer
; 0x3B7B P7   15 word(s)  0 % : StoreValue
; 0x3C6C P7   18 word(s)  0 % : Ds18B20WriteConfig
; 0x3C51 P7   20 word(s)  0 % : OneWireReset
; 0x3C38 P7   25 word(s)  1 % : OneWireReadByte
; 0x3C1F P7   25 word(s)  1 % : OneWireWriteByte
; 0x3B75 P7    2 word(s)  0 % : returnTRUE
; 0x3B77 P7    2 word(s)  0 % : returnFALSE
; 0x3B79 P7    2 word(s)  0 % : setPlusPlusINDF1
; 0x3B8A P7    5 word(s)  0 % : setFRCerror
; 0x3B8F P7   64 word(s)  3 % : AdjustFrcTemperature
; 0x3BDE P7   45 word(s)  2 % : Get_BinaryData_Final
; 0x3C13 P7   12 word(s)  0 % : Delay5us
; 0x3C65 P7    7 word(s)  0 % : OneWireCmd

; RAM usage: 991 bytes (5 local), 33 bytes free
; Maximum call level: 6
;  Codepage 0 has    6 word(s) :   0 %
;  Codepage 1 has    0 word(s) :   0 %
;  Codepage 2 has    0 word(s) :   0 %
;  Codepage 3 has    0 word(s) :   0 %
;  Codepage 4 has    0 word(s) :   0 %
;  Codepage 5 has    0 word(s) :   0 %
;  Codepage 6 has    0 word(s) :   0 %
;  Codepage 7 has  823 word(s) :  40 %
; Total of 829 code words (5 %)
