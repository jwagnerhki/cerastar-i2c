#include "i2c-ramblock.h"


////////////////////////////////////////////////////////////////////////////////////////////////
//// THERMAL CONTROL INFO ACCESSORS
////////////////////////////////////////////////////////////////////////////////////////////////

void furnaceStateStr(const FurnaceState_t& s, String& out)
{
  out = "Therme: ";
  out += "vmax=";  out += s.vlMax_x2 / 2;     out += "C "; // Vorlauf max limit, 0.5 degC steps
  out += "vcurr="; out += s.vlTemp_x2 / 2;   out += "C "; // Vorlauf actual
  out += "dmax=";  out += s.dlMax_x2 / 2;     out += "C "; // ???lauf
  out += "dcurr="; out += s.dlTemp_x2 / 2;    out += "C ";
  out += "wmax=";  out += s.wwMax_x2 / 2;     out += "C "; // Boiler max temp
  out += "wcurr="; out += s.wwTemp_x2 / 2;    out += "C "; // Boiler actual
  out += "aux=error/flame/pump/flags=";
  out += s.error; out += "/";
  out += s.flame; out += "/";
  out += s.pump; out += "/";
  out += s.flags;
}

void busmoduleStateStr(const BusmoduleState_t& s, String& out)
{
  out = "BM1: "; 
  out += "power="; out += s.power;
  out += "vtarget="; out += s.vlSet_x2 / 2; out += "C "; // Vorlauf setpoint, 0.5 degC steps
  out += "wtarget="; out += s.wwSet_x2 / 2; out += "C "; // Boiler setpoint, 0.5 degC steps
  out += "aux=stopPump/error=";
  out += s.stopPump; out += "/";
  out += s.error;
}


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

void ramDatablockStr(const I2C_RAM_t* memblk, String& out)
{
  int addr = 0;
  unsigned int n = 0;
  out = "";
  while (n < sizeof(I2C_RAM_t)) {
    int i=0;
    out += "addr ";
    out += addr;
    out += " : ";
    while (i<16 && n < sizeof(I2C_RAM_t)) {
      out += (int)(memblk->raw_data[addr]);
      out += " ";
      i++;
      n++;
      addr++;
    }
    out += "\r\n";
  }
}
