
# Autograder 2.0
A website for automating the grading process of embedded systems assignments with microcontrollers. Implements communication between the website and a microcontroller.

# Table of Contents
- [Hardware](#hardware)
- [PWM Assignment](#pwm-assignment)
  * [Communication/Setting Up](#communication-and-setting-up)
- [Dependencies](#dependencies)
- [How to Run](#how-to-run)

## Hardware
This system uses [mBed's](https://www.mbed.com/en/) [LPC1768](https://os.mbed.com/platforms/mbed-LPC1768/) and [NUCLEO F746ZG](https://os.mbed.com/platforms/ST-Nucleo-F746ZG/) development boards. The student programs the NUCLEO board, while the LPC1768 records data from the NUCLEO and reports data back to the website.

## PWM Assignment
Currently the only supported assignment. The student codes the NUCLEO board to interpret signals from the LPC1768 as binary numbers representing a period and duty cycle for a PWM wave and generates the waves. The LPC1768 records the time between rises and falls and sends the timestamps back to the website, which processes the data.


### Communication and Setting Up
In order to communicate with the website, the LPC1768 must be set up as a USB device. See [here](https://os.mbed.com/handbook/USBDevice) for information on how to do that.
\
Communication between the two boards uses GPIO pins. The pin assignments for the boards can be found in their instruction files, located in the [mbed_Board_Code](https://github.com/nesl/Autograder2.0/tree/master/mBed_Board_Code) directory, but a table has also been provided below. Check out the pin map for the [NUCLEO](https://os.mbed.com/platforms/ST-Nucleo-F746ZG/#board-pinout) and [LPC1768](https://os.mbed.com/users/fraserphillips/notebook/mbed-gpio-pin-table/) for the locations of these pins.

**Period Pins, ordered from most significant to least significant bit**

LPC1768 | NUCLEO
------------ | -------------
p5 | PB_10
p6 | PE_15
p7 | PE_14
p8| PE_12
p9| PE_10
**Duty Cycle Pins, ordered from most significant to least significant bit**

LPC1768 | NUCLEO
------------ | -------------
 p11 | PE_7
 p12 | PE_8
 p13 | PG_9
 p14 | PG_14
 p15 | PF_15
 p16 | PE_13
 p17 | PF_14
 
 **Other Pins**
 
LPC1768 | NUCLEO
------------ | -------------
 p21 | PE_0
 p25 | PC_10
 p27 | PC_8
 
The period is interpreted as (binary number + 1)*10 in ms (range of 10-320), while the duty cycle is the binary number as a percentage. 
## Dependencies
The library for communication between the board and the browser has been provided on the github page as the LPC1768_Library directory. Import it to the mBed online compiler when compiling the instructions for the LPC1768. This library is not needed for the NUCLEO board.
The website uses the [plotly.js](https://plot.ly/javascript/) library to graph the PWM waveforms, as well as Google Chrome's [WebUSB](https://wicg.github.io/webusb/) API to communicate with the LPC1768. Google Chrome is the only supported browser for this reason.
## How to Run
Visit the [website](https://nesl.github.io/Autograder2.0/) and use the  `select device` button to connect the LPC1768. The `send` button allows you to send your own custom period and duty cycle to the LPC1768, while the `Graph all test cases` button uses 5 hard-coded test cases. Currently, the website does not save or store data.