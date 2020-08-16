#ifndef _configure
#define _configure

#include "functions.h"
#define ENCODER_DO_NOT_USE_INTERRUPTS

#define DATA  11  ///< SPI Data pin number
#define CLK   13  ///< SPI Clock pin number
#define FSYNC 10

const int rs = 5, en = 6, d4 = 7, d5 = 8, d6 = 9, d7 = 12;
const int encCLK = 2, encDT = 3, entBTN = 4;
const int button = 4;

#endif
