
#include "furnace.h"
#include "i2c-ramblock.h"
#include "crc8.h"

////////////////////////////////////////////////////////////////////////////////////////////////
//// Helper funcs
////////////////////////////////////////////////////////////////////////////////////////////////

String temperatureToStr(signed char tempC_x2)
{
  String s = String(tempC_x2 / 2);
  if (tempC_x2 & 1) {
    s += ".5C";
  } else {
    s += ".0C";
  }
  return s;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//// THERMAL INFO PRETTY-PRINTING
////////////////////////////////////////////////////////////////////////////////////////////////

String furnaceStateToStr(const FurnaceState_t& s)
{
  String out = "Furnace:  ";
  out += "vmax=" + temperatureToStr(s.vlMax_x2);    // Vorlauf max limit, 0.5 degC steps
  out += " vcurr=" + temperatureToStr(s.vlTemp_x2); // Vorlauf actual
  out += " dmax=" + temperatureToStr(s.dlMax_x2);   // ???lauf
  out += " dcurr=" + temperatureToStr(s.dlTemp_x2); // ???lauf
  out += " wmax=" + temperatureToStr(s.wwMax_x2);   // Boiler max temp
  out += " wcurr=" + temperatureToStr(s.wwTemp_x2); // Boiler actual
  out += "aux=error/flame/pump/flags=";
  out += s.error; out += "/";
  out += s.flame; out += "/";
  out += s.pump; out += "/";
  out += s.flags;
  return out;
}

String busmoduleStateToStr(const BusmoduleState_t& s)
{
  String out = "Fake BM1: "; 
  out += "power="; out += s.power;
  out += " vtarget=" + temperatureToStr(s.vlSet_x2); // Vorlauf setpoint, 0.5 degC steps
  out += " wtarget=" + temperatureToStr(s.wwSet_x2); // Boiler setpoint, 0.5 degC steps
  out += "aux=stopPump/error=";
  out += s.stopPump; out += "/";
  out += s.error;
  return out;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//// THERMAL CONTROL
////////////////////////////////////////////////////////////////////////////////////////////////

static void s_recomputeBusmoduleDataCRC(volatile I2C_RAM_t& shmem)
{
  shmem.fields.bm1_state.checksum = 0;
  uint8_t* dd = (uint8_t*)&shmem.fields.bm1_state;
  while (dd < (uint8_t*)&shmem.fields.bm1_state.checksum) {
    shmem.fields.bm1_state.checksum = crc8(shmem.fields.bm1_state.checksum, *dd);
    dd++;
  }
}

void updateBusmoduleTargets(volatile I2C_RAM_t& shmem, unsigned char power, unsigned char vlTempC, unsigned char wwTempC, unsigned char stopPump)
{
  shmem.fields.bm1_data_avail = 0;
  
  shmem.fields.bm1_state.power = power;
  shmem.fields.bm1_state.vlSet_x2 = 2*vlTempC;
  shmem.fields.bm1_state.wwSet_x2 = 2*wwTempC;
  shmem.fields.bm1_state.stopPump = stopPump;
  shmem.fields.bm1_state.error = 0x00;

  shmem.fields.bm1_state.dummy1 = 1;
  shmem.fields.bm1_state.dummy2 = 1;
  shmem.fields.bm1_state.dummy4 = 0xFF; // might be Leistungsbegrenzung, but, forum, "Ich habe nur die i2c ram addressen 0x90 und 0x9e versucht [...] Aber damit kann ich die Therme nicht regeln"

  s_recomputeBusmoduleDataCRC(shmem);

  shmem.fields.bm1_data_avail = 1;
}

void furnaceStopPump(volatile I2C_RAM_t& shmem)
{
  shmem.fields.bm1_data_avail = 0;
  shmem.fields.bm1_state.stopPump = 0x01;
  s_recomputeBusmoduleDataCRC(shmem);
  shmem.fields.bm1_data_avail = 1;
}

void furnaceStartPump(volatile I2C_RAM_t& shmem)
{
  shmem.fields.bm1_data_avail = 0;
  shmem.fields.bm1_state.stopPump = 0x00;
  s_recomputeBusmoduleDataCRC(shmem);
  shmem.fields.bm1_data_avail = 1;
}

void furnaceStopHeating(volatile I2C_RAM_t& shmem)
{
  shmem.fields.bm1_data_avail = 0;
  shmem.fields.bm1_state.vlSet_x2 = 2*10;
  shmem.fields.bm1_state.wwSet_x2 = 2*10;
  s_recomputeBusmoduleDataCRC(shmem);
  shmem.fields.bm1_data_avail = 1;
}

void furnaceSetPowerlevel(volatile I2C_RAM_t& shmem, unsigned char powerLev)
{
  shmem.fields.bm1_data_avail = 0;  
  shmem.fields.bm1_state.power = powerLev;
  s_recomputeBusmoduleDataCRC(shmem);
  shmem.fields.bm1_data_avail = 1;
}

void furnaceSetHeatingSetpoint(volatile I2C_RAM_t& shmem, unsigned char tempC)
{
  shmem.fields.bm1_data_avail = 0;
  shmem.fields.bm1_state.vlSet_x2 = 2*tempC;
  s_recomputeBusmoduleDataCRC(shmem);
  shmem.fields.bm1_data_avail = 1;
}

void furnaceSetHotWaterSetpoint(volatile I2C_RAM_t& shmem, unsigned char tempC)
{
  shmem.fields.bm1_data_avail = 0;
  shmem.fields.bm1_state.wwSet_x2 = 2*tempC;
  s_recomputeBusmoduleDataCRC(shmem);
  shmem.fields.bm1_data_avail = 1;
}

