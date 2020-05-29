**********************************************************************************
                                   USB CDC class

CDC class provides a simple serial bus via USB interface. A device equipped
with the firmware supporting CDC creates a virtual serial port enabling
to communicate with PC or another equipment (supporting USB) via the USB interface
like through a standard COM port.

CDC is supported by several IQRF devices (e.g. GW-USB-05, CK-USB-04A and GW-USB-03A).
This support is implemented in main MCU of the GW (but not in the MCU in TR module).
Refer to the CDC Implementation in IQRF USB devices User's guide (User_Guide_CDC.pdf)
to check the current list of devices supported.


                          IQRF device         User equipment
                          -----------           ----------
                          |   GW    |           |   PC   |
                          |         |           |        |
                          |  MCU    | ––––––––– |  CDC   |
                          | CDC FW  |    USB    | driver |
                          |         | ––––––––– |        |
                          | ------  |           |        |
                          | | TR |  |           |        |
                          | ------  |           |        |
                          -----------           ----------

CDC implementation is described in the User_Guide_CDC.
The device can be switched to/from the CDC class by the IQRF IDE.
See the IQRF IDE Help.

USB CDC driver iqrfcdc must be installed on the PC first. This is usually performed
as a part of IQRF IDE installation.

To test a communication in CDC mode:
- The device should have the E07-SPI example uploaded in TR module inside.
- A suitable PC terminal (PC SW operating with PC serial ports) should be used.
  Refer to User_guide_CDC to select a proper one.

Files included:
- User_Guide_CDC         PDF   CDC Implementation in IQRF USB devices User's guide
- CDC-Terminal-project   PTP   Ready project for Docklight (recommended CDC terminal)

**********************************************************************************

