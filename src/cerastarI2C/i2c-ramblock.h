#ifndef CERASTAR_I2C_H
#define CERASTAR_I2C_H

#include <Arduino.h>

////////////////////////////////////////////////////////////////////////////////////////////////
//// Gas furnace state info
////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct FurnaceState_tt {
    // 13 byte total, starting at offset 0x20
    uint8_t vlMax_x2;     // byte 0
    uint8_t vlTemp_x2;    // byte 1
    uint8_t dlMax_x2;     // byte 2
    uint8_t dlTemp_x2;    // byte 3
    uint8_t wwMax_x2;     // byte 4
    uint8_t wwTemp_x2;    // byte 5
    uint8_t error;        // byte 6
    uint8_t dummy1;       // byte 7
    uint8_t dummy2;       // byte 8
    uint8_t flame;        // byte 9
    uint8_t pump;         // byte 10
    uint8_t flags;        // byte 11
    uint8_t dummy3;       // byte 12
} FurnaceState_t;


////////////////////////////////////////////////////////////////////////////////////////////////
//// Emulated BM1 Bus Module thermal controller state info
////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct BusmoduleState_tt {
    // 16 byte total, starting at offset 0x90
    // log: BM1 "write to 0x50 ack data: offset:0x90 0:0xFF 1:0xB4 2:0x00 3:0x01 4:0x00 5:0x01 
    //           6:0x03 7:0x00 9:0x00 9:0x00 10:0x00 11:0x00 12:0x00 13:0x00 14:0xFF 15:0x0D"
    uint8_t power;      // byte 0
    uint8_t vlSet_x2;   // byte 1
    uint8_t wwSet_x2;   // byte 2
    uint8_t dummy1;     // byte 3
    uint8_t stopPump;   // byte 4
    uint8_t dummy2;     // byte 5
    uint8_t error;      // byte 6
    uint8_t dummy3[7];  // byte 7..13
    uint8_t dummy4;     // byte 14
    uint8_t checksum;   // byte 15
} BusmoduleState_t;


////////////////////////////////////////////////////////////////////////////////////////////////
//// Emulated 256-Byte I2C-RAM/EEPROM data area
////////////////////////////////////////////////////////////////////////////////////////////////

typedef union I2C_RAM_tt {
    byte raw_data[256];
    struct fields_t {
        // for i in range(256): print("    byte field_%03d; // offset %d 0x%02X" % (i,i,i))
        byte field_000; // offset 0 0x00
        byte field_001; // offset 1 0x01
        byte field_002; // offset 2 0x02
        byte field_003; // offset 3 0x03
        byte field_004; // offset 4 0x04
        byte field_005; // offset 5 0x05
        byte field_006; // offset 6 0x06
        byte field_007; // offset 7 0x07
        byte field_008; // offset 8 0x08
        byte field_009; // offset 9 0x09
        byte field_010; // offset 10 0x0A
        byte field_011; // offset 11 0x0B
        byte field_012; // offset 12 0x0C
        byte field_013; // offset 13 0x0D
        byte field_014; // offset 14 0x0E
        byte field_015; // offset 15 0x0F
        byte furnace_data_avail; // offset 16 0x10 ; 1 = data-write from gas furnace mainboard to "I2C RAM" has completed
        byte bm1_data_avail;     // offset 17 0x11 ; 1 = data-write from Bus Nodule BM1 to "I2C RAM" has completed
        byte field_018; // offset 18 0x12
        byte field_019; // offset 19 0x13
        byte field_020; // offset 20 0x14
        byte field_021; // offset 21 0x15
        byte field_022; // offset 22 0x16
        byte field_023; // offset 23 0x17
        byte field_024; // offset 24 0x18
        byte field_025; // offset 25 0x19
        byte field_026; // offset 26 0x1A
        byte field_027; // offset 27 0x1B
        byte field_028; // offset 28 0x1C
        byte field_029; // offset 29 0x1D
        byte field_030; // offset 30 0x1E
        byte field_031; // offset 31 0x1F
        // State data of the gas furnace, written to by the furnace
        FurnaceState_t furnace_state; // offset 32 0x20 to 44 0x2C inclusive, 13 bytes
        byte field_045; // offset 45 0x2D
        byte field_046; // offset 46 0x2E
        byte field_047; // offset 47 0x2F
        byte field_048; // offset 48 0x30
        byte field_049; // offset 49 0x31
        byte field_050; // offset 50 0x32
        byte field_051; // offset 51 0x33
        byte field_052; // offset 52 0x34
        byte field_053; // offset 53 0x35
        byte field_054; // offset 54 0x36
        byte field_055; // offset 55 0x37
        byte field_056; // offset 56 0x38
        byte field_057; // offset 57 0x39
        byte field_058; // offset 58 0x3A
        byte field_059; // offset 59 0x3B
        byte field_060; // offset 60 0x3C
        byte field_061; // offset 61 0x3D
        byte field_062; // offset 62 0x3E
        byte field_063; // offset 63 0x3F
        byte field_064; // offset 64 0x40
        byte field_065; // offset 65 0x41
        byte field_066; // offset 66 0x42
        byte field_067; // offset 67 0x43
        byte field_068; // offset 68 0x44
        byte field_069; // offset 69 0x45
        byte field_070; // offset 70 0x46
        byte field_071; // offset 71 0x47
        byte field_072; // offset 72 0x48
        byte field_073; // offset 73 0x49
        byte field_074; // offset 74 0x4A
        byte field_075; // offset 75 0x4B
        byte field_076; // offset 76 0x4C
        byte field_077; // offset 77 0x4D
        byte field_078; // offset 78 0x4E
        byte field_079; // offset 79 0x4F
        byte field_080; // offset 80 0x50
        byte field_081; // offset 81 0x51
        byte field_082; // offset 82 0x52
        byte field_083; // offset 83 0x53
        byte field_084; // offset 84 0x54
        byte field_085; // offset 85 0x55
        byte field_086; // offset 86 0x56
        byte field_087; // offset 87 0x57
        byte field_088; // offset 88 0x58
        byte field_089; // offset 89 0x59
        byte field_090; // offset 90 0x5A
        byte field_091; // offset 91 0x5B
        byte field_092; // offset 92 0x5C
        byte field_093; // offset 93 0x5D
        byte field_094; // offset 94 0x5E
        byte field_095; // offset 95 0x5F
        byte field_096; // offset 96 0x60
        byte field_097; // offset 97 0x61
        byte field_098; // offset 98 0x62
        byte field_099; // offset 99 0x63
        byte field_100; // offset 100 0x64
        byte field_101; // offset 101 0x65
        byte field_102; // offset 102 0x66
        byte field_103; // offset 103 0x67
        byte field_104; // offset 104 0x68
        byte field_105; // offset 105 0x69
        byte field_106; // offset 106 0x6A
        byte field_107; // offset 107 0x6B
        byte field_108; // offset 108 0x6C
        byte field_109; // offset 109 0x6D
        byte field_110; // offset 110 0x6E
        byte field_111; // offset 111 0x6F
        byte field_112; // offset 112 0x70
        byte field_113; // offset 113 0x71
        byte field_114; // offset 114 0x72
        byte field_115; // offset 115 0x73
        byte field_116; // offset 116 0x74
        byte field_117; // offset 117 0x75
        byte field_118; // offset 118 0x76
        byte field_119; // offset 119 0x77
        byte field_120; // offset 120 0x78
        byte field_121; // offset 121 0x79
        byte field_122; // offset 122 0x7A
        byte field_123; // offset 123 0x7B
        byte field_124; // offset 124 0x7C
        byte field_125; // offset 125 0x7D
        byte field_126; // offset 126 0x7E
        byte field_127; // offset 127 0x7F
        byte field_128; // offset 128 0x80
        byte field_129; // offset 129 0x81
        byte field_130; // offset 130 0x82
        byte field_131; // offset 131 0x83
        byte field_132; // offset 132 0x84
        byte field_133; // offset 133 0x85
        byte field_134; // offset 134 0x86
        byte field_135; // offset 135 0x87
        byte field_136; // offset 136 0x88
        byte field_137; // offset 137 0x89
        byte field_138; // offset 138 0x8A
        byte field_139; // offset 139 0x8B
        byte field_140; // offset 140 0x8C
        byte field_141; // offset 141 0x8D
        byte field_142; // offset 142 0x8E
        byte field_143; // offset 143 0x8F
        // State data of Bus Module BM1, normally written to by the BM1
        BusmoduleState_t bm1_state; // offset 144 0x90 to 159 0x9F inclusive, 16 bytes
        byte field_160; // offset 160 0xA0
        byte field_161; // offset 161 0xA1
        byte field_162; // offset 162 0xA2
        byte field_163; // offset 163 0xA3
        byte field_164; // offset 164 0xA4
        byte field_165; // offset 165 0xA5
        byte field_166; // offset 166 0xA6
        byte field_167; // offset 167 0xA7
        byte field_168; // offset 168 0xA8
        byte field_169; // offset 169 0xA9
        byte field_170; // offset 170 0xAA
        byte field_171; // offset 171 0xAB
        byte field_172; // offset 172 0xAC
        byte field_173; // offset 173 0xAD
        byte field_174; // offset 174 0xAE
        byte field_175; // offset 175 0xAF
        byte field_176; // offset 176 0xB0
        byte field_177; // offset 177 0xB1
        byte field_178; // offset 178 0xB2
        byte field_179; // offset 179 0xB3
        byte field_180; // offset 180 0xB4
        byte field_181; // offset 181 0xB5
        byte field_182; // offset 182 0xB6
        byte field_183; // offset 183 0xB7
        byte field_184; // offset 184 0xB8
        byte field_185; // offset 185 0xB9
        byte field_186; // offset 186 0xBA
        byte field_187; // offset 187 0xBB
        byte field_188; // offset 188 0xBC
        byte field_189; // offset 189 0xBD
        byte field_190; // offset 190 0xBE
        byte field_191; // offset 191 0xBF
        byte field_192; // offset 192 0xC0
        byte field_193; // offset 193 0xC1
        byte field_194; // offset 194 0xC2
        byte field_195; // offset 195 0xC3
        byte field_196; // offset 196 0xC4
        byte field_197; // offset 197 0xC5
        byte field_198; // offset 198 0xC6
        byte field_199; // offset 199 0xC7
        byte field_200; // offset 200 0xC8
        byte field_201; // offset 201 0xC9
        byte field_202; // offset 202 0xCA
        byte field_203; // offset 203 0xCB
        byte field_204; // offset 204 0xCC
        byte field_205; // offset 205 0xCD
        byte field_206; // offset 206 0xCE
        byte field_207; // offset 207 0xCF
        byte field_208; // offset 208 0xD0
        byte field_209; // offset 209 0xD1
        byte field_210; // offset 210 0xD2
        byte field_211; // offset 211 0xD3
        byte field_212; // offset 212 0xD4
        byte field_213; // offset 213 0xD5
        byte field_214; // offset 214 0xD6
        byte field_215; // offset 215 0xD7
        byte field_216; // offset 216 0xD8
        byte field_217; // offset 217 0xD9
        byte field_218; // offset 218 0xDA
        byte field_219; // offset 219 0xDB
        byte field_220; // offset 220 0xDC
        byte field_221; // offset 221 0xDD
        byte field_222; // offset 222 0xDE
        byte field_223; // offset 223 0xDF
        byte init_seq[8]; // offset 224 0xE0 until 231 0xE7 inclusive
        byte field_232; // offset 232 0xE8
        byte field_233; // offset 233 0xE9
        byte field_234; // offset 234 0xEA
        byte field_235; // offset 235 0xEB
        byte field_236; // offset 236 0xEC
        byte field_237; // offset 237 0xED
        byte field_238; // offset 238 0xEE
        byte field_239; // offset 239 0xEF
        byte field_240; // offset 240 0xF0
        byte field_241; // offset 241 0xF1
        byte field_242; // offset 242 0xF2
        byte field_243; // offset 243 0xF3
        byte field_244; // offset 244 0xF4
        byte field_245; // offset 245 0xF5
        byte field_246; // offset 246 0xF6
        byte field_247; // offset 247 0xF7
        byte field_248; // offset 248 0xF8
        byte field_249; // offset 249 0xF9
        byte field_250; // offset 250 0xFA
        byte field_251; // offset 251 0xFB
        byte field_252; // offset 252 0xFC
        byte field_253; // offset 253 0xFD
        byte dev_magic; // offset 254 0xFE, data alway 0xFC
        byte dev_version; // offset 255 0xFF, data always 0x03
    } fields;
} I2C_RAM_t;


////////////////////////////////////////////////////////////////////////////////////////////////
//// Init and pretty-printing functions
////////////////////////////////////////////////////////////////////////////////////////////////

void initRamDatablock(I2C_RAM_t*);

void ramDatablockToStr(const I2C_RAM_t*, String& out);

String byte_to_hex(unsigned char b);

String furnaceStateToStr(const FurnaceState_t& s);
String busmoduleStateToStr(const BusmoduleState_t& s);


#endif
