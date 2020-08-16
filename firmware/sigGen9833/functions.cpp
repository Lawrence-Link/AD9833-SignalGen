#include "functions.h"

Encoder encoder(encCLK, encDT);

bool DataReadyEnc = false;

int positionData = 0;
int countPulses = 0;

void dataEncHandler(bool _a) {
  DataReadyEnc = _a;
}

long oldPosition = -999;

void EncFresh() {
  long newPosition = encoder.read();
  if (newPosition != oldPosition) {
    countPulses++;
    if (countPulses == 4) {
      if (newPosition < oldPosition) {
        positionData = 2;
        dataEncHandler(true);
      } else {
        positionData = 1;
        dataEncHandler(true);
      }
      countPulses = 0;
    }
    oldPosition = newPosition;
  }
}
