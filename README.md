# LayerOne_2024
Repo for LayerOne 2024


# L1 Pov Spinner

Either use Microchip Studio or avr-gcc to build it.

https://www.microchip.com/en-us/tools-resources/develop/microchip-studio


sudo apt-get install binutils gcc-avr avr-libc uisp avrdude flex byacc bison

port

https://ports.macports.org/port/avr-gcc/

homebrew

https://github.com/osx-cross/homebrew-avr


for avr-gcc's use the makefile


the image folder are made of 12xN high 1 bit png's and the imageConvert tool spits them out as a header file, just include them and use drawImage


imageConvert <in.png> <out.h> <label>

use <label> to pass into drawImage use #include <out.h> (with the right name) in the main.c


interrupt driven from a hall effect sensor, triggers the loops to draw and calcs an RPM then it goes into sleep after the effect is drawn, the hall effect can wake it up, but to save battery pressing the button wakes the cpu up again

the slider switch needs to be be in the furthest away position from the programming pads, match the G to gnd on the programmer..
