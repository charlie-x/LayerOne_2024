# LayerOne_2024
Repo for LayerOne 2024

# L1 Pov Spinner

## What is the Pov Spinner?

The Pov Spinner is an electronic fidget spinner that displays words and patterns via LED's when spinning. It was part of the LayerOne 2024 Conference badge.

## What is needed to change the code on the Spinner?

* Code from repo
* Computer
* AVR programming device
* Build software
* Flash software
* Device drivers

# Code from repo

Clone the git repo or download the zip of the repo

## git Clone
```
git clone https://github.com/charlie-x/LayerOne_2024.git
```

## Download zip

```
wget https://github.com/charlie-x/LayerOne_2024/archive/refs/heads/main.zip
```

## AVR programing device

**NOTE: Any AVR programmer should work.**

The LayerOne attendees were able to obtain a **Kraken** AVR programmer to use for this purpose which is an avrisp/mkii/avr dragon/usbasp.

Kraken based on this board: 

https://www.fischl.de/usbasp/

### Arduino as an AVR programmer

Alternatively, and Ardunio can be used to program the device:

https://docs.arduino.cc/built-in-examples/arduino-isp/ArduinoISP/

https://www.amazon.com/CANADUINO-USBtinyISP-Programmer-Compatible-Arduino/dp/B0751LJH3Z

## Build Software

Either use Microchip Studio or avr-gcc to build it.

### *nix

Install **avr-gcc** via the **gcc-avr** package

sudo apt-get install binutils gcc-avr avr-libc uisp avrdude flex byacc bison

### Windows

https://www.microchip.com/en-us/tools-resources/develop/microchip-studio

### Mac

port

https://ports.macports.org/port/avr-gcc/

homebrew

https://github.com/osx-cross/homebrew-avr

for avr-gcc's use the makefile

## Flashing the board

**NOTE: The kraken needs avrdude, it is not supported in microchip studio.**

### *nix / Windows / Mac - avrdude

AVRDUDE - is a program for downloading and uploading the on-chip memories of Microchip’s AVR microcontrollers.

Flash the board with the compiled **hex** file to device

https://github.com/avrdudes/avrdude/releases

```avrdude -c usbasp -p t4313 -v -B 3 -U flash:w:..\LayerOne_2024-main\LayerOne_2024-main\L1_POVSpinner\L12024POV\Debug\L12024POV.hex```


### Windows - atprogram

atprogram - is a program that comes with Microchip Studio

```
atprogram -cl 200khz -v -d attiny4313 -t atmelice -i isp chiperase
atprogram -cl 200khz -v -d attiny4313 -t atmelice -i isp program -f L12024POV\Release\L12024POV.elf
```

#### Device Drivers for Windows

Zadig is a Windows application that installs generic USB drivers, such as WinUSB, libusb-win32/libusb0.sys or libusbK, to help you access USB devices that can be found here:
https://zadig.akeo.ie/#

Install the **libusbK** driver

## Notes on modifiying the code

The image folder are made of 12xN high 1 bit png's and the imageConvert tool spits them out as a header file, just include them and use drawImage

imageConvert <in.png> <out.h> <label>

use <label> to pass into drawImage use #include <out.h> (with the right name) in the main.c

interrupt driven from a hall effect sensor, triggers the loops to draw and calcs an RPM then it goes into sleep after the effect is drawn, the hall effect can wake it up, but to save battery pressing the button wakes the cpu up again

the slider switch needs to be be in the furthest away position from the programming pads, match the G to gnd on the programmer..

==============================================================================

calculate x/y

30 is 360/nuber of leds around, which is speed dependant  so used the number of leds vertically

  angle_radians = ( led_number * 30 *pi / 180  )

  x_inner = radius * cos ( angle_radius )
  y_inner = radius * cos ( angle_radius )

distance...

  distance = sqrt( (dx^2) + (dy^2))
