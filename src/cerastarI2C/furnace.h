#ifndef FURNACE_H
#define FURNACE_H

#include "i2c-ramblock.h"

String temperatureToStr(signed char tempC_x2);
String furnaceStateToStr(const FurnaceState_t& s);
String busmoduleStateToStr(const BusmoduleState_t& s);

void updateBusmoduleTargets(volatile I2C_RAM_t&, unsigned char power, unsigned char vlTempC, unsigned char wwTempC, unsigned char stopPump);

#endif
