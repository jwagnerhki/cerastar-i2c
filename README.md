# cerastar-i2c - Junkers Cerastar I2C bus interfacing

This repository is a work-in-progress implementation of microcontroller software (esp. Arduino Nano, 5V) to interact with a Junkers Cerastar ZWR/ZSR series gas furnace. These gas furnaces are antiquated, but some houses still have them installed. They lack any sort of sensible home automation interface.

## Background

The Cerastar mainboard exposes 5V I2C interface(s) on at least two card-edge PCB connectors. One labeled "ST9 TA" accepts a Lumberg 2351 RAST-2.5 6-pin clip-on connector. For custom I2C access purposes one can assemble a RAST-2.5 6-pin connector and cable assembly oneself. Alternatively, one can get a ready cable as a spare part (Bosch 87144041510) and cut it in half. The official purpose of "ST9 TA" is to attach an  obsolete Junkers-Bosch BM1 "bus module" (Junkers 8748300289, Junkers 8748300370). The BM1 bus module bridges the I2C interface of the mainboard to an external CAN bus. Various Junkers thermostat units can be attached to that CAN bus (FR 100, TA 250, ...).

For home automation and remote control of the Cerastar it would be nice to attach a microcontroller directly to the mainboard I2C, avoiding the pricey and now hard to get BM1 bus module and complications of the CAN bus. Luckily, people on a mikrocontroller.net forum who own a Cerastar ZWR/ZSR and a BM1 bus module have been able to sniff the I2C traffic between the Cerastar and BM1. A lot has been reverse-engineered, see https://www.mikrocontroller.net/topic/81265

## Components

1. 1 x Lumberg 2351 RAST-2.5 6-pin connector (Digikey) plus pins, some cabling - or directly a Bosch 87144041510 cable from somewhere
2. 5V microcontroller, Arduino Nano in case of this project, and som
3. (future plan: 5V<=>3.3V I2C level shifter board, plus an Arduino Nano 33 IoT or maybe a Raspberry Pi Pico W wifi)

## I2C Bus Traffic Details

As described at https://www.mikrocontroller.net/topic/81265, ...
