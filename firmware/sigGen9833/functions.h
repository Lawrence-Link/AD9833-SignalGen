#ifndef _functions
#define _functions

#include "configure.h"
#include <LiquidCrystal.h>
#include <Encoder.h>
#include <SPI.h>
#include <MD_AD9833.h>

#define ___firmware_version___ "8.16"

void lcdBegin(short col, short row);
void printIntroAndVersion();
void lcdColumn(uint8_t _line);
void lcdRow(uint8_t _line);
void Blink();
void dataEncHandler(bool _a);
void EncFresh();

#endif
