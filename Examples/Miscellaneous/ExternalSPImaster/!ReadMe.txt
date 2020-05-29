*******************************************************************************************
                                       SPI master
                           for communication with IQRF TR module

Demonstration how to implement serial SPI connection between TR module and a master MCU
embedded in a user device.

                            ----------           ----------
                            |   TR   |           |  User  |
                            |        |           | device |
                            |        | ––––––––– |        |
                            | MCU OS | ––––––––– |  MCU   |
                            |        | ––––––––– |        |
                            |        | ––––––––– |        |
                            |        |    SPI    |        |
                            ----------           ----------
                            SPI slave            SPI master


SPI slave is implemented in IQRF operating system in TR module (and runs in OS background).

MCU PIC24F series by Microchip is used device in the demo. Source codes in C language
are provided, including individual SPI routines for communication with TR module.

The demo is intended for guidance only. It should be adapted for given application.

Refer to the SPI Implementation in IQRF TR modules Technical guide (Tech_Guide_IQRF-SPI_TR-7xD_xxxxxx.PDF
at the Documentation\SW folder).

Files included:
- .\Demo-example                 Demo example folder
  - Main_Demo                 C  Main program
  - IQRF_SPI                  C  SPI routines
  - IQRF_SPI                  H  Header file

===========================================================================================
                                     Release notes
 2 Nov 2015  Rev. 1.7  Timing modified
 6 Aug 2015  Rev. 1.6  Modified for TR-7xD modules with faster SPI
 4 Nov 2014  Rev. 1.5  Comments slightly changed
14 May 2014  Rev. 1.4  Recommended delay between SPI bytes modified
12 Jul 2012  Rev. 1.3  TR-5xD supported
22 Jul 2010  Rev. 1.2  IQRF_SPI.c modified
10 Mar 2010  Rev. 1.1  First release

*******************************************************************************************

