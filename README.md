# BadUSB-AT90USB162
Simple BadUSB Keyboard HID attack implementation with small support for rubber ducky scripting language based on popular AVR AT90USB162.

## USBHIDTest
Main project written in ATMEL STUDIO, uses LUFA lib for USB.

## USBPayloadGenerator
Project used for generating the Payload.h file from rubber ducky script used in the project above. The file contains list of instructions translated from rubber ducky format to C code.

Currently the project supports only STRING, REM, GUI, ENTER, DELAY commands.

## How to install and use
1. Write a simple rubber ducky script.
2. Run  USBPayloadGenerator <FILENAME>. It will generate the file "Payload.h" in current working directory.
3. Replace the original "Payload.h" file from the USBHIDTest project with the new one.
4. Set your device in bootloader mode and compile and write the USBHIDTest to the AVR.
5. Profit :) (unplug your device and plug it again, the script will execute)

## Hardware
Any AT90USB162 device with DFU bootloader. 
I used: http://goo.gl/QMpuR7

## License
MIT license

## Changelog

**2016-Mar**
First version original functionality.

**2016-July**
Second version improved the rubber ducky code.

##Author
Nikola Vitanovic
Contact me via GitHub.