//
// Decoy BM1 Bus Module for Junkers Cerastar gas furnaces.
//
// Board:   Arduino Nano with ATmega328
// Serial:  9600,8,n,1
// I2C:     30 kHz, 5V, I2C Target at bus addr 0x50
//          pin 27=A4=SDA1 28=A5=SCL1
// Gas furnace mainboard conn "ST9 TA":
//          pin6(left)=SCL pin5=SDA pin4=5V pin3=GND pin2=n/a pin1=24V
// Arduino<->ST9 wiring:
//          pin 27/A4/SDA1-->ST9:pin5, pin 28/A5/SCL1-->ST9:pin6,
//          directly wired and without added pull-ups
//
// Emulates a 256-byte 30 kHz I2C RAM memory, which
// the Junkers gas furnace writes into regulary. The
// thus intercepted data allow the furnace state
// information to be logged on the serial port.
//
// Does not have CAN functionality, unlike the real BM1.
// The real BM1 board has an ancient 1990'ies NXP P80C592
// 8-bit 80C51 MCU with CAN + UART, 2 x 256-byte RAM,
// and while no internal ROM (P83C592 would have 16 kB)
// it has an external address space of 64 kB.
//
// Details: https://www.mikrocontroller.net/topic/81265
//
// A lot of code comes from Joe L.'s junkers.cpp on the forum.
//
// The I2C bus devices are:
//
//   1. I2C Target device at I2C Address 0x50,
//      with an address space that is 0...255
//   2. I2C Controller - the gas furnace
//   3. I2C Controller - the BM1 module (not present)
//
//   NB: Unclear from the forum, but the BM1 would appear
//   to pointlessly use I2C to write into its own RAM!?
//   Unless, perhaps, there is a NXP PCF8570 -like
//   256 x 8-bit static I2C RAM residing on the gas
//   furnace mainboard. Photos of BM1 do not show one,
//   but the blurry online photos of gas furnace PCB bottom
//	 layer have a 8-DIP chip that might be I2C RAM.
//
// The I2C communications protocol with the I2C RAM:
//
// - Writes into RAM are made with a multi-byte I2C write
//    [base index][byte to write to index][byte to index+1][byte to addr+2][...]
//
// - Read requests from RAM are made in two steps,
//   similar to the way that is common with I2C EEPROM's:
//    - First, I2C write of a single byte
//        [base index]
//    - Next, I2C single-byte reads, some N times
//        [byte from index] [byte from index+1] [...]
//      these data need to be Wire.write()'en
//
// RAM address space:
//
//     0x10 : bit0 = gas furnace state data valid flag (1=valid, 0=invalid)
//     0x11 : bit0 = bus module state data valid flag (1=valid, 0=invalid)
//     0x12 + 2 bytes  : unknown but always written as 0x00 0x00 0x00
//     0x20 + 12 bytes : gas furnace state data array
//     0x90 + 15 bytes : bus module state data array
//     0xE0 + 7 bytes  : magic vector 0x49 0x46 0x20 0x30 0x34 0x2e 0x30 0x30
//     0xFE + 1 bytes  : magic word 0xFC 0x03
//
// The higher-level comms sequence is:
//
//  Init
//  1. Gas furnace starts a write to RAM @ 0xFE of data [0xFC 0x03]
//  2. Gas furnace reads back (1) to see if the [0xFC 0x03] got really stored
//  3. Gas furnace writes a mystery sequence into RAM @ 0xE0, ignored
//  Periodic
//  4. Gas furnace regular writing
//     first writes its 13-byte state into RAM @ 0x20,
//     then writes 2-byte validity into    RAM @ 0x10 of data [0x01 0x00]
//  5. Bus module BM1 regular writing
//     first writes 3-byte mystery   into RAM @ 0x12 of data [0x00 0x00 0x00],
//     then writes its 16-byte state into RAM @ 0x90,
//     then writes 2-byte validity into   RAM @ 0x10 of data [0x00 0x01]
//  6. Gas furnace regular read/poll
//     reads 2-byte validity info from RAM @ 0x10 looking for [0x00 0x01],
//     if matched, reads from RAM @ 0x90 the recent 16-byte BM1 data
//  7. Bus module BM1 regular read/poll
//     reads 2-byte validity info from RAM @ 0x10 looking for [0x01 0x00],
//     if matched, reads rom RAM @ 0x20 the recent 13-byte gas furnace data
//

#include <Wire.h> // modified C:\Users\janwa\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\libraries\Wire

#include "crc8.h"
#include "i2c-ramblock.h"
#include "furnace.h"

// I2C properties of our emulated 256-byte target memory
#define I2C_TARGET_ADDR 0x50
#define I2C_FREQ_HZ    30000
#define SDA_PIN  A4 //=SDA1
#define SCL_PIN  A5 //=SCL1
void i2cReqHandler(void);
void i2cReqMoreHandler(void);
void i2cRecvHandler(int);

// 256-Byte Memory Area
static volatile I2C_RAM_t shmem;
static volatile byte i2c_last_cmd_offset = 0;
static volatile byte i2c_offset = 0;
static volatile byte i2c_rx_count = 0;
static volatile byte i2c_tx_count = 0;

// Copies of key data areas of the 256-Byte memory
FurnaceState_t sampled_furnace_state;
BusmoduleState_t sampled_bm1_state;

#define SAMPLING_INTERVAL_MSEC  1000
#define REPORTING_INTERVAL_MSEC 5000
#define UPDATING_INTERVAL_MSEC  2000

static unsigned long furnace_state_time = 0;
static unsigned long prev_report_time = 0;
static unsigned long prev_update_time = 0;


////////////////////////////////////////////////////////////////////////////////////////////////
//// EMULATED I2C EEPROM/RAM
////////////////////////////////////////////////////////////////////////////////////////////////

// Handler: I2C Controller wants to read from our local 256-byte memory,
// starting from earlier-written memory offset
void i2cReqHandler(void)
{
    const byte base_addr = i2c_offset;
    const byte data = shmem.raw_data[i2c_offset]; 
    Wire.write(data);
    i2c_offset++;
    i2c_tx_count++;
}

// Handler: I2C Controller wants continued read from our local 256-byte memory
void i2cReqMoreHandler(void)
{
    const byte base_addr = i2c_offset;
    const byte data = shmem.raw_data[i2c_offset]; 
    Wire.write(data);
    i2c_offset++;
    i2c_tx_count++;
}


// Handler: I2C Controller wants to write into our local 256-byte memory,
// or prepares a subsequent read from an offset written here.
// Data are <base addr> [<data for base addr> <data for base addr+1> <...>]
void i2cRecvHandler(int nbytes)
{
    int n;
    const byte base_addr = Wire.read();

    i2c_offset = base_addr;
    i2c_last_cmd_offset = base_addr; // for debug print only
    n = 1;
    while (n < nbytes) {
        shmem.raw_data[i2c_offset] = Wire.read();
        i2c_offset++;
        n++;
    }
    i2c_rx_count += nbytes;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//// SETUP
////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  initRamDatablock((I2C_RAM_t*)&shmem);
  initCrc8();

  Serial.begin(9600);
  
  Wire.begin(I2C_TARGET_ADDR);
  Wire.setClock(I2C_FREQ_HZ);

  pinMode(SDA_PIN, INPUT);
  pinMode(SCL_PIN, INPUT);
  //digitalWrite(SDA_PIN, LOW); // is supposed to disable internal pull-ups
  //digitalWrite(SCL_PIN, LOW);
  Wire.onRequest(i2cReqHandler);
  Wire.onRequestMore(i2cReqMoreHandler); // new callback for cont'd sequential data (i.e., when controller is trying to clock in more)
  Wire.onReceive(i2cRecvHandler);

  Serial.println("Junkers BM1 on-board I2C 256-byte RAM emulator");
  Serial.print("Listening on I2C address ");
  Serial.println(I2C_TARGET_ADDR, HEX);
}


////////////////////////////////////////////////////////////////////////////////////////////////
//// LOOP
////////////////////////////////////////////////////////////////////////////////////////////////

void loop()
{
  unsigned long t;

  delay(50);

  t = millis();

  // Check for updated info from the Junkers boiler unit
  if (shmem.fields.furnace_data_avail && (t - furnace_state_time) > SAMPLING_INTERVAL_MSEC) {

    memcpy(&sampled_furnace_state, (void*)&shmem.fields.furnace_state, sizeof(sampled_furnace_state));
    shmem.fields.furnace_data_avail = 0;  // 'ack'

    // For whatever it is worth, do as in https://www.mikrocontroller.net/attachment/590667/junkers.cpp and set these to 0:
    shmem.raw_data[0x12] = 0;
    shmem.raw_data[0x13] = 0;
    shmem.raw_data[0x14] = 0;
    shmem.raw_data[0x18] = 0;

    Serial.println(furnaceStateToStr(sampled_furnace_state));

    furnace_state_time = t;
 }

  // Regularly push out temperature setpoints into memory,
  // for the Junkers boiler to pick up sometime
  if ((t - prev_update_time) > UPDATING_INTERVAL_MSEC) {
    //updateBusmoduleTargets(shmem, /*power:*/ 0xFF, /*vlTempC:*/10, /*wwTempC:*/10, /*stopPump:*/0x00);
    updateBusmoduleTargets(shmem, /*power:*/ 128, /*vlTempC:*/10, /*wwTempC:*/10, /*stopPump:*/0x01);
    //updateBusmoduleTargets(shmem, /*power:*/ 128, /*vlTempC:*/30, /*wwTempC:*/10, /*stopPump:*/0x00);
    //updateBusmoduleTargets(shmem, /*power:*/ 128, /*vlTempC:*/50, /*wwTempC:*/10, /*stopPump:*/0x00);
    //updateBusmoduleTargets(shmem, /*power:*/ 0xFF, /*vlTempC:*/50, /*wwTempC:*/10, /*stopPump:*/0x00);
    //updateBusmoduleTargets(shmem, /*power:*/ 128, /*vlTempC:*/10, /*wwTempC:*/45, /*stopPump:*/0x00);
    memcpy(&sampled_bm1_state, (void*)&shmem.fields.bm1_state, sizeof(sampled_bm1_state));

    Serial.println(busmoduleStateToStr(sampled_bm1_state));

    prev_update_time = t;
  }
  
  // Regularly debug-dump the I2C statistics and last-seen Junkers and own setpoint states
  if ((t - prev_report_time) > REPORTING_INTERVAL_MSEC) {
    //Serial.println( ramDatablockStr((const I2C_RAM_t*)&shmem) );
    if (1) {
      unsigned char addr = 0, k, n;
      for (k=0; k<16; k++) {
        String s;
        s += String("addr 0x") + byte_to_hex(addr) + String(" : ");
        for (n=0; n<16; n++) {
          s += byte_to_hex(shmem.raw_data[addr]) + String(" ");
          addr++;
        }
        Serial.println(s);
      }
    }
    Serial.print("i2c rx:");
    Serial.print(i2c_rx_count, DEC);
    Serial.print(" tx:");
    Serial.print(i2c_tx_count, DEC);
    Serial.print(" addr:0x");
    Serial.println(byte_to_hex(i2c_last_cmd_offset));

    prev_report_time = t;
  }
  
}
