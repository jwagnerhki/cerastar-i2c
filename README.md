# cerastar-i2c - Junkers Cerastar I2C bus interfacing

This repository is a work-in-progress implementation of microcontroller software (esp. Arduino Nano, 5V) to interact with a Junkers Cerastar ZWR/ZSR series gas furnace. These gas furnaces are antiquated, but some houses still have them installed. They lack any sort of sensible home automation interface.

## Background

The Cerastar mainboard exposes 5V I2C interface(s) on an on-board pad array and on two card-edge PCB connectors. One labeled "ST9 TA" accepts a Lumberg 2351 RAST-2.5 6-pin clip-on connector. The other labeled "ST7" has traces that connect directly to test pads above, labeled GND/5V/SDA/SCL. Whether ST9, ST7, and pads are the same I2C bus is unclear/untested.

Card edge connector "ST9 TA" officially accepts an obsolete Junkers-Bosch BM1 "bus module" (Junkers 8748300289, Junkers 8748300370). The BM1 bridges mainboard I2C interface over to an external CAN bus. The BM1 is based on a 1990'ies NXP P80C592 8-bit 80C51 MCU. The P80C592 MCU offers hardware CAN and UART (but no hardware I2C) and has a 2 x 256-byte internal RAM. One 256-byte of the page is apparently used as shared memory for communication between I2C devices (furnace mainboard, the BM1 itself) and official Junkers CAN bus devices (thermostats FR 100, TA 250, and others). This project emulates the BM1 256-byte memory. The CAN bus is (NB: eventually - currently absent) replaced by Wifi.

People on a mikrocontroller.net forum have traced I2C traffic between a Cerastar furnace and a real BM1. They were able to deduce most of the I2C user protocol.

For the whole background, see the thread https://www.mikrocontroller.net/topic/81265

For the source code used as a basis of this project, see the 05.03.2023 post by Joe L. (joe_l626) in that thread and his code, esp. [junkers.cpp](https://www.mikrocontroller.net/attachment/590667/junkers.cpp)

## Components

1. 1 x Lumberg 2351 RAST-2.5 6-pin connector (Digikey) plus pins, some cabling - or directly a Bosch 87144041510 cable from somewhere
2. 5V microcontroller, Arduino Nano in case of this project, and som
3. (future plan: 5V<=>3.3V I2C level shifter board, plus an Arduino Nano 33 IoT or maybe a Raspberry Pi Pico W wifi)

The RAST-2.5 6-pin connector cable assembly (or a Bosch 87144041510 cable) will be attached to "ST9 TA" (5V I2C bus) and to the microcontroller 5V, GND, SDA, SCL pins. No pull-up resistors needed.

...

## I2C Bus Traffic Details

As described at https://www.mikrocontroller.net/topic/81265, ...
