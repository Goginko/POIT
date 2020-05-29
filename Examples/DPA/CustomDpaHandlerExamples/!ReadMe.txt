##########################################################################################
## Copyright (c) IQRF Tech s.r.o.
##########################################################################################

##########################################################################################
# DPA V4.13 header files:

DPA.h                   General public DPA header file
DPAcustomHandler.h      Custom DPA Handler header
IQRFstandard.h          IQRF Alliance standards
IQRF_HWPID.h            IQRF Alliance allocated HWPIDs

##########################################################################################
# Custom DPA Handler examples:
                                                      RF mode support*      Uses (E)EEPROM** 
CustomDpaHandler-AsyncRequest.c                            STD,LP                 no 
CustomDpaHandler-Autobond.c                                STD,LP                 no 
CustomDpaHandler-Bonding.c                                 STD,LP                 no
CustomDpaHandler-BondingButton.c                           STD,LP                 no
CustomDpaHandler-BondingNoSleep.c                          STD,LP                 no
CustomDpaHandler-Bridge-SPI.c                              STD,LP                 no 
CustomDpaHandler-Bridge-UART.c                             STD,LP                 no 
CustomDpaHandler-Buttons.c                                 STD                    no 
CustomDpaHandler-Coordinator-AutoNetworkV2-Embedded.c      STD,LP                 YES
CustomDpaHandler-Coordinator-FRCandSleep.c                 STD,LP                 no 
CustomDpaHandler-Coordinator-PollNodes.c                   STD,LP                 no 
CustomDpaHandler-Coordinator-PulseLEDs.c                   STD,LP                 no 
CustomDpaHandler-Coordinator-ReflexGame.c                  STD                    no 
CustomDpaHandler-CustomIndicate                            STD,LP                 no
CustomDpaHandler-DDC-RE01.c                                STD                    no 
CustomDpaHandler-DDC-SE01.c                                STD                    no 
CustomDpaHandler-DDC-SE01_RE01.c                           STD                    no 
CustomDpaHandler-FRC-Minimalistic.c                        STD,LP                 no 
CustomDpaHandler-FRC.c                                     STD,LP                 no 
CustomDpaHandler-HookDpa.c                                 STD                    no 
CustomDpaHandler-LED-Green-On.c                            STD,LP                 no 
CustomDpaHandler-LED-MemoryMapping.c                       STD,LP                 no 
CustomDpaHandler-LED-Red-On.c                              STD,LP                 no 
CustomDpaHandler-LED-UserPeripheral.c                      STD,LP                 no 
CustomDpaHandler-MultiResponse.c                           STD                    no
CustomDpaHandler-Peer-to-Peer.c                            STD,LP                 no 
CustomDpaHandler-PeripheralMemoryMapping.c                 STD,LP                 no 
CustomDpaHandler-PIRlighting.c                             STD                    YES
CustomDpaHandler-ScanRSSI.c                                STD                    no 
CustomDpaHandler-SelfLoadCode.c                            STD,LP                 YES
CustomDpaHandler-SPI.c                                     STD,LP                 no 
CustomDpaHandler-Template-OptimizedSwitch.c                STD,LP                 no 
CustomDpaHandler-Template.c                                STD,LP                 no 
CustomDpaHandler-Timer.c                                   STD                    no 
CustomDpaHandler-TimerCalibrated.c                         STD                    no 
CustomDpaHandler-UART.c                                    STD,LP                 YES
CustomDpaHandler-UartHwRxSwTx.c                            STD                    no 
CustomDpaHandler-UARTrepeater.c                            STD                    no 
CustomDpaHandler-UserEncryption.c                          STD,LP                 no 
CustomDpaHandler-UserPeripheral-18B20-Idle.c               STD                    no 
CustomDpaHandler-UserPeripheral-18B20-Multiple.c           STD,LP                 no 
CustomDpaHandler-UserPeripheral-18B20.c                    STD,LP                 no 
CustomDpaHandler-UserPeripheral-ADC.c                      STD,LP                 no 
CustomDpaHandler-UserPeripheral-HW-UART.c                  STD                    no
CustomDpaHandler-UserPeripheral-I2C.c                      STD,LP                 no 
CustomDpaHandler-UserPeripheral-I2Cmaster.c                STD,LP                 no 
CustomDpaHandler-UserPeripheral-McuTempIndicator.c         STD,LP                 no 
CustomDpaHandler-UserPeripheral-PWM.c                      STD                    no 
CustomDpaHandler-UserPeripheral-PWMandTimer.c              STD                    no 
CustomDpaHandler-UserPeripheral-SPImaster.c                STD,LP                 no
CustomDpaHandler-UserPeripheral.c                          STD,LP                 no 
CustomDpaHandler-XLPstandBy.c                              STD,LP                 no
DP2Papp.c                                                  STD,LP                 no
DpaAutoexec.c                                              STD,LP                 YES
DpaIoSetup.c                                               STD,LP                 YES
Peer-to-Peer-Transmitter.c                                 STD,LP                 no 

0002_DDC-SE01.c                                            STD,LP                 no
0402_DDC-SE+RE.c                                           STD                    no
0802_TrThermometer.c                                       STD,LP                 no
0C02_BinaryOutput-Template.c                               STD                    no
1002_Light-Template.c                                      STD                    no
1402_Sensor-Template.c                                     STD,LP                 no
4402_DDC-SE+RE_LP.c                                        LP                     no
4802_DDC-RE_LP.c                                           LP                     no
4C02_DDC-SE_LP.c                                           LP                     no
5802_DALItemplate.c                                        STD,LP                 no

##########################################################################################
# Notes:
 *  When both STD and LP RF modes are supported by the code, then the actual RF mode (STD or LP) supported is controlled by HWPID in case of the certified product as the product is tagged STD or LP in the IQRF Repository.
 ** When EEPROM or external EEPROM is used (sometimes depends on conditional compilation), then handler cannot be fully loaded by CMD_OS_LOAD_CODE. See DPA documentation for more information.

##########################################################################################
