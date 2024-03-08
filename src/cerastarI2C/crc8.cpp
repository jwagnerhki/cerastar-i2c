////////////////////////////////////////////////////////////////////////////////////////////////
//// CRC-8 CHECKSUMMING
////////////////////////////////////////////////////////////////////////////////////////////////

#include "crc8.h"

static uint8_t crc8_table[256];

void initCrc8()
{
  int i,j;
  uint8_t crc;
  for (i=0; i<256; i++) {
    crc = i;
    for (j=0; j<8; j++) {
      crc = (crc << 1) ^ ((crc & 0x80) ? 0x39 : 0);
    }
    crc8_table[i] = crc & 0xFF;
  }
}

uint8_t crc8(uint8_t crc_in, uint8_t new_value)
{
  return crc8_table[crc_in ^ new_value];
}

