#include "i2c-ramblock.h"

String byte_to_hex(unsigned char b);

////////////////////////////////////////////////////////////////////////////////////////////////
//// EMULATED I2C EEPROM/RAM
////////////////////////////////////////////////////////////////////////////////////////////////

void initRamDatablock(I2C_RAM_t* memblk)
{
    const byte initSequence[] = { 0x49, 0x46, 0x20, 0x30, 0x34, 0x2e, 0x30, 0x30 };
    memset(memblk->raw_data, 0x00, sizeof(I2C_RAM_t));
    memcpy(memblk->fields.init_seq, initSequence, sizeof(memblk->fields.init_seq));
    memblk->fields.dev_magic = 0xFC;
    memblk->fields.dev_version = 0x03;
    memblk->fields.furnace_data_avail = 0;
    memblk->fields.bm1_data_avail = 0;
    memblk->fields.bm1_state.power = 0x80; // 0xFF is 100% so 0x80 could be ~50%
}

String ramDatablockStr(const I2C_RAM_t* memblk)
{
  String s;
  unsigned int n = 0;
  unsigned char addr = 0;
  while (n < sizeof(I2C_RAM_t)) {
    s += String("addr 0x") + byte_to_hex(addr) + String(" : ");
    for (int i = 0; i < 16 && n < sizeof(I2C_RAM_t); i++, n++, addr++) {
      s += byte_to_hex(memblk->raw_data[addr]) + String(" ");
    }
    s += "\r\n";
  }
  return s;
}

String byte_to_hex(unsigned char b)
{
  String s;
  unsigned char v = b >> 4;
  if (v > 9) {
    s += 'A' + (v-10);
  } else {
    s += '0' + v;
  }
  v = b & 0x0F;
  if (v > 9) {
    s += 'A' + (v-10);
  } else {
    s += '0' + v;
  }
  return s;
}

