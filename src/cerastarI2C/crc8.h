#ifndef CRC8_H
#define CRC8_H

#include <Arduino.h>

void initCrc8();
uint8_t crc8(uint8_t crc_in, uint8_t new_value);

#endif
