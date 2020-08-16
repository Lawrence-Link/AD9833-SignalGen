#include "functions.h"

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//MD_AD9833  AD(FSYNC);

MD_AD9833 AD(DATA, CLK, FSYNC);

int j1threshold = 0;
int j2threshold = 0;

int menuState, waveform, freqCursor, menuCursor;

long buttonTimer = 0;
long longPressTime = 500;

bool buttonActive = false;
bool longPressActive = false;
bool pressType = false;
bool getPressType = false;
bool IsFreqChanging = true;

bool statusSelectingReg = false;
bool statusSelectingPWR = true;
int statusSelectingSig = 0;
bool statusSelectingRegF = false;

bool powerStatus = true;
bool freqRegister = false;
bool menuLocked = false;

unsigned long current_frequency = 10;

extern bool DataReadyEnc;
extern int positionData;

byte figure_pointer[8] = {
  0b00000,
  0b11000,
  0b11100,
  0b11110,
  0b11110,
  0b11100,
  0b11000,
  0b00000
};

byte arrow[8] = {
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b11100,
  0b00000,
  0b00000,
  0b00000
};

void lcdBegin(short col, short row) {
  lcd.begin(col, row);
}
void lcdColumn(uint8_t _line) {
  lcd.setCursor(NULL, _line);
}

void lcdRow(uint8_t _line) {
  lcd.setCursor(_line, NULL);
}

void lcdClear() {
  lcd.clear();
}

void lcdPrint(String _content) {
  lcd.print(_content);
}
void lcdPrintChar(char _a[20]) {
  lcd.print(_a);
}

void lcdBlink(bool _status) {
  if (_status == true) lcd.blink();
  if (_status == false) lcd.noBlink();
}

void printIntroAndVersion() {
  lcdColumn(0);
  lcdPrint("Function Gen");
  lcdColumn(1);
  lcdPrint("Firmware ");
  lcdPrint(___firmware_version___);
  delay(1000);
}

void setup() {
  pinMode(button, INPUT_PULLUP);
  lcdBegin(16, 2);
  AD.begin();
  lcd.createChar(0, figure_pointer);
  lcd.createChar(1, arrow);
  timer1_initialize();
  printIntroAndVersion();
  delay(1000);
  menuState = 0;
  waveform = 0;
  freqCursor = 0;
  menuCursor = 0;
  buttonFresh();
  EncFresh();
  lcdClear();
  printFrequency(false); printFreqRegister(false); printWaveform(); printPower();
  
}

void loop() {
  if (DataReadyEnc == true || getPressType == true) { //触发后刷新屏幕
began:
    lcd.clear();
    uint32_t  ul;
    MD_AD9833::channel_t chan;
    MD_AD9833::mode_t mode;

    /* else { //按键
      /*  if (pressType == true) { //短按(频率模式切换位置)

        } else { //长按
          if (menuLocked == false) { //没有进到下属菜单
            menuLocked == true;
            //进入下属菜单
          } else { //进到了下属菜单,根据条件执行命令

          }
        }
        getPressType = false;
    */

    switch (menuState) {
      case 0: {
          if (DataReadyEnc == true) { //编码器软中断
            if (positionData == 1) menuCursor++;
            if (positionData == 2) menuCursor--;

            if (menuCursor == 5) {
              menuCursor = 0;
            } else if (menuCursor == -1) {
              menuCursor = 4;
            }
            dataEncHandler(false);
          }
          if (menuCursor == 0) {
            
            printFrequency(false);
            printFreqRegister(false);
            printWaveform();
            printPower();

          } else if (menuCursor == 1) {
            if (getPressType == true && pressType == true) {
              menuState = 1;
              //    getPressType = false;
            }
            lcd.write(byte(0));
            printFrequency(true);
            printFreqRegister(false);
            printWaveform();
            printPower();
          } else if (menuCursor == 2) {
            if (getPressType == true && pressType == true) {
              menuState = 2;
              //    getPressType = false;
            }
            printFrequency(false);
            lcd.setCursor(12, 0);
            lcd.write(byte(0));
            printFreqRegister(false);
            printWaveform();
            printPower();
          } else if (menuCursor == 3) {
            if (getPressType == true && pressType == true) {
              menuState = 3;
              //    getPressType = false;
            }
            printFrequency(false);
            printFreqRegister(true);
            printWaveform();
            lcd.setCursor(0, 1);
            lcd.write(byte(0));
            printPower();
          } else if (menuCursor == 4) {
            if (getPressType == true && pressType == true) {
              menuState = 4;
              //    getPressType = false;
            }
            printFrequency(false);
            printFreqRegister(false);
            printWaveform();
            if (powerStatus == true) {
              lcd.setCursor(13, 1);
            } else {
              lcd.setCursor(12, 1);
            }
            lcd.write(byte(0));
            printPower();
          }
          break;
        }

      case 1: { //频率设置
          if (getPressType == true && pressType == true) {
            getPressType = false;
            IsFreqChanging = !IsFreqChanging;
          } else if (getPressType == true && pressType == false) {
            getPressType = false;
            menuCursor = 0;
            menuState = 5;
            goto began;
          }
          if (DataReadyEnc == true) { //编码器软中断
            if (IsFreqChanging == false) {
              if (positionData == 1) freqCursor++;
              if (positionData == 2) freqCursor--;

              if (freqCursor == 8) {
                freqCursor = 0;
              } else if (freqCursor == -1) {
                freqCursor = 7;
              }
            } else if (IsFreqChanging == true) {
              if (positionData == 1) {
                switch (freqCursor) {
                  case 0: current_frequency += 10000000; break;
                  case 1: current_frequency += 1000000; break;
                  case 2: current_frequency += 100000; break;
                  case 3: current_frequency += 10000; break;
                  case 4: current_frequency += 1000; break;
                  case 5: current_frequency += 100; break;
                  case 6: current_frequency += 10; break;
                  case 7: current_frequency += 1; break;
                }
                if (current_frequency < 0 || current_frequency > 10000000) {
                  current_frequency = 0;
                }
              }

              if (positionData == 2) {
                switch (freqCursor) {
                  case 0: current_frequency -= 10000000; break;
                  case 1: current_frequency -= 1000000; break;
                  case 2: current_frequency -= 100000; break;
                  case 3: current_frequency -= 10000; break;
                  case 4: current_frequency -= 1000; break;
                  case 5: current_frequency -= 100; break;
                  case 6: current_frequency -= 10; break;
                  case 7: current_frequency -= 1; break;
                }
                if (current_frequency < 0 || current_frequency > 10000000) {
                  current_frequency = 0;
                }
              }
            }
          }
          if (freqCursor == 8) {
            freqCursor = 0;
          } else if (freqCursor == -1) {
            freqCursor = 7;
          }
          printFrequencyChanging();
          break;
        }

      case 2: { //波形合成
          if (getPressType == true && pressType == true) {
            getPressType = false;
          } else if (getPressType == true && pressType == false) {
            waveform = statusSelectingSig;
            if (powerStatus == true){
            if (waveform == 0){
              AD.setMode(MD_AD9833::MODE_SINE);
            }else if (waveform == 1){
              AD.setMode(MD_AD9833::MODE_TRIANGLE);
            }else if (waveform == 2){
              AD.setMode(MD_AD9833::MODE_SQUARE1);
            }
            }else{
              AD.setMode(MD_AD9833::MODE_OFF);
            }
            getPressType = false;
            menuCursor = 0;
            menuState = 0;
            goto began;
          }

          if (DataReadyEnc == true) { //编码器软中断
            if (positionData == 1) statusSelectingSig++;
            if (positionData == 2) statusSelectingSig--;

            if (statusSelectingSig == 3) {
              statusSelectingSig = 0;
            } else if (statusSelectingSig == -1) {
              statusSelectingSig = 2;
            }
            dataEncHandler(false);
          }

          printWaveformChanging();
          break;
        }

      case 3: {  //寄存器
          if (getPressType == true && pressType == true) {
            getPressType = false;
          } else if (getPressType == true && pressType == false) {
            freqRegister = statusSelectingReg;
            if (freqRegister == false) {
              AD.setActiveFrequency(MD_AD9833::CHAN_0);
            } else {
              AD.setActiveFrequency(MD_AD9833::CHAN_1);
            }
            getPressType = false;
            menuCursor = 0;
            menuState = 0;
            goto began;
          }

          if (DataReadyEnc == true) { //编码器软中断
            if (positionData == 1) statusSelectingReg = true;
            if (positionData == 2) statusSelectingReg = false;
            dataEncHandler(false);
          }

          printFreqRegisterChanging();
          break;
        }
      case 4: {  //输出
          if (getPressType == true && pressType == true) {
            getPressType = false;
          } else if (getPressType == true && pressType == false) {
            powerStatus = statusSelectingPWR;
            if (powerStatus == true){
            if (waveform == 0){
              AD.setMode(MD_AD9833::MODE_SINE);
            }else if (waveform == 1){
              AD.setMode(MD_AD9833::MODE_TRIANGLE);
            }else if (waveform == 2){
              AD.setMode(MD_AD9833::MODE_SQUARE1);
            }
            }else{
              AD.setMode(MD_AD9833::MODE_OFF);
            }
            getPressType = false;
            menuCursor = 0;
            menuState = 0;
            goto began;
          }

          if (DataReadyEnc == true) { //编码器软中断
            if (positionData == 1) statusSelectingPWR = true;
            if (positionData == 2) statusSelectingPWR = false;
            dataEncHandler(false);
          }
          printPWRChanging();
          break;
        }
      case 5: { //频率设置应用-CHANNEL
          if (getPressType == true && pressType == true) {
            getPressType = false;
          } else if (getPressType == true && pressType == false) {
            if (statusSelectingRegF == false) {
              AD.setFrequency(MD_AD9833::CHAN_0, current_frequency);
            } else if (statusSelectingRegF == true){
              AD.setFrequency(MD_AD9833::CHAN_1, current_frequency);
            }
            getPressType = false;
            menuCursor = 0;
            menuState = 0;
            goto began;
          }

          if (DataReadyEnc == true) { //编码器软中断
            if (positionData == 1) statusSelectingRegF = true;
            if (positionData == 2) statusSelectingRegF = false;
            dataEncHandler(false);
          }
          freqCH_Sel();
          break;
        }
    }
    getPressType = false;
    dataEncHandler(false);
  }
}



/*
  void printFrequency(bool sel) {
  if (sel == true) {
    lcd.setCursor(1, 0);
  }
  else {
    lcd.setCursor(0, 0);
  }
  char _buff[20];
  if (current_frequency < 1000) {
    lcd.print(current_frequency);
  } else if (current_frequency >= 1000 && current_frequency < 1000000) {
    lcd.print(double(current_frequency / 1000));
    lcd.print("K");
  } else {
    lcd.print(double(current_frequency / 1000000));
    lcd.print("M");
  }
  lcdPrint("Hz");
  }
*/

void printFrequency(bool sel) {
  if (sel == true) {
    lcd.setCursor(1, 0);
  }
  else {
    lcd.setCursor(0, 0);
  }
  lcd.print(current_frequency);
  lcdPrint("Hz");
}

void printFrequencyChanging() {
  char _buff[20];
  sprintf(_buff, "%s%08lu", "f=", current_frequency);
  lcdPrintChar(_buff);
  lcdPrint("Hz");
  lcdColumn(1);
  //lcd.write(byte(0));
  lcd.setCursor(freqCursor + 2, 1);
  lcd.write(byte(1));
}

void printFreqRegister(bool sel) {
  if (sel == true) {
    lcd.setCursor(1, 1);
  }
  else {
    lcd.setCursor(0, 1);
  }
  if (freqRegister == false) {
    lcd.print("FREQ0");
  } else {
    lcd.print("FREQ1");
  }
}

void printFreqRegisterChanging() {
  lcd.setCursor(1, 0);
  lcd.print("FREQ_REG0");
  lcd.setCursor(1, 1);
  lcd.print("FREQ_REG1");
  if (statusSelectingReg == false) {
    lcd.setCursor(0, 0);
  } else {
    lcd.setCursor(0, 1);
  }
  lcd.write(byte(0));
}

void printWaveform() {
  lcd.setCursor(13, 0);
  if (waveform == 0) {
    lcdPrint("SIN");
  } else if (waveform == 1) {
    lcdPrint("TRI");
  } else {
    lcdPrint("SQR");
  }
}

void printWaveformChanging() {
  lcd.setCursor(1, 0);
  lcd.print("SIN");
  lcd.setCursor(6, 0);
  lcd.print("TRI");
  lcd.setCursor(12, 0);
  lcd.print("SQR");
  lcd.setCursor(0, 1);
  lcd.print("Hold to select.");
  switch (statusSelectingSig) {
    case 0: lcd.setCursor(0, 0); break;
    case 1: lcd.setCursor(5, 0); break;
    case 2: lcd.setCursor(11, 0); break;
  }
  lcd.write(byte(0));
}
//statusSelectingPWR
void printPower() {
  if (powerStatus == true) {
    lcd.setCursor(14, 1);
    lcd.print("ON");
  } else {
    lcd.setCursor(13, 1);
    lcd.print("OFF");
  }
}

void printPWRChanging() {
  lcd.setCursor(1, 0);
  lcd.print("ON");
  lcd.setCursor(1, 1);
  lcd.print("OFF");
  if (statusSelectingPWR == true) {
    lcd.setCursor(0, 0);
  } else {
    lcd.setCursor(0, 1);
  }
  lcd.write(byte(0));
}

void freqCH_Sel() {
  lcd.setCursor(1, 0);
  lcd.print("FREQ_REG0");
  lcd.setCursor(1, 1);
  lcd.print("FREQ_REG1");
  if (statusSelectingRegF == false) {
    lcd.setCursor(0, 0);
  } else {
    lcd.setCursor(0, 1);
  }
  lcd.write(byte(0));
}

void buttonFresh() {
  if (digitalRead(button) == LOW) {
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      longPressActive = true;
      pressType = false;
      getPressType = true;
      ///////////long press
    }
  } else {
    if (buttonActive == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {
        pressType = true;
        getPressType = true;
        ////////// short press
      }
      buttonActive = false;
    }
  }
}

void timer1_initialize() {
  cli();
  TCCR1A = 0;
  TCCR1B = (1 << WGM12);
  TCCR1B |= (1 << CS10) | (1 << CS11);
  OCR1A = 24;
  TCNT1 = 0;
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

ISR(TIMER1_COMPA_vect)
{
  static unsigned int j1counter = 0;
  static unsigned int j2counter = 0;

  ++j1counter; j2counter++;

  EncFresh();
  buttonFresh();

  if (j1counter >= j1threshold) {
    j1counter = 0;
  }
  if (j2counter >= j2threshold) {
    j2counter = 0;
  }
}
