**********************************************************************************
                                USB Custom device

PC SW demo illustrating how to use USB in user IQRF applications communicating 
with PC or another computer supporting USB, like the CK-USB-04(A) kit with IQRF IDE.

Custom device class is supported by all IQRF USB devices (e.g. CK-USB-04A
and GW-USB-06). This support is implemented in main MCU of the device
(but not in the MCU in TR module).

                    IQRF device                User equipment
                -------------------           -----------------
                |     CK/GW       |           |      PC       |
                |                 |           |               |
                |      MCU        | ––––––––– |  Custom class |
                | Custom class FW |    USB    |     driver    |
                |                 | ––––––––– |               |
                |     ------      |           |               |
                |     | TR |      |           |               |
                |     ------      |           |               |
                -------------------           -----------------

This Demo is a complete project for WinUSB driver, Visual C++ and Microsoft Visual Studio.

Configuration: 
- PC with WinUSB USB driver by Microsoft installed. (This is usually performed 
  as a part of IQRF IDE installation. See IQRF Quick Start guide or 
  IQRF USB Drivers installation guide.)
- CK-USB-04(A) with firmware supporting WinUSB, connected to PC via USB.
- TR transceiver with the E07-SPI SPI communication example (one of basic IQRF 
  examples, included in IQRF Startup package) uploaded, plugged in the CK-USB-04(A) kit.

Devices supporting more USB classes can be switched
to/from the Custom device by the IQRF IDE. See the IQRF IDE Help.

Files included:
- SampleWinUSBDemo       ZIP   USB Custom device example.

==================================================================================
                                  Release notes

- 12 Aug 2015 – First release for WinUSB and Visual Studio.

**********************************************************************************

