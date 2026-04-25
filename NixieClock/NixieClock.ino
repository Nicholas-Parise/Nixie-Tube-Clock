#include "MD_DS1307.h"

#define A1 3
#define B1 4
#define C1 5
#define D1 6

#define A2 7
#define B2 8
#define C2 9
#define D2 10

#define A3 11
#define B3 12
#define C3 13
#define D3 14

#define A4 2
#define B4 1
#define C4 0
#define D4 25

#define A5 22
#define B5 21
#define C5 20
#define D5 19

#define A6 18
#define B6 17
#define C6 16
#define D6 15

#define DIG2(i) ((d[i]-'0')*10 + (d[i+1]-'0'))

const uint8_t A[6] = { A1, A2, A3, A4, A5, A6 };
const uint8_t B[6] = { B1, B2, B3, B4, B5, B6 };
const uint8_t C[6] = { C1, C2, C3, C4, C5, C6 };
const uint8_t D[6] = { D1, D2, D3, D4, D5, D6 };

//uint8_t hour;
//uint8_t minute;
//uint8_t second;

const uint8_t patterns[10][4] = {
  {0,0,0,0},
  {1,0,0,0},
  {0,1,0,0},
  {1,1,0,0},
  {0,0,1,0},
  {1,0,1,0},
  {0,1,1,0},
  {1,1,1,0},
  {0,0,0,1},
  {1,0,0,1}
};

void setup() {
  delay(1000); //programming delay

  if (!RTC.isRunning()) {
    RTC.startClock();
    writeCompileTime();
  }

  for (uint8_t i = 0; i < 6; i++) {
    pinMode(A[i], OUTPUT);
    pinMode(B[i], OUTPUT);
    pinMode(C[i], OUTPUT);
    pinMode(D[i], OUTPUT);
  }

  Serial.begin(9600);
}

void loop() {

  handleSerial();

  RTC.readTime();

  if (RTC.m == 0 && RTC.s == 0) {
    runDigitCycleFancy();
  }

  uint8_t h = RTC.h;

  if (h == 0) h = 12;
  else if (h > 12) h -= 12;
  
  writenumber(0, (h / 10) % 10);
  writenumber(1, h % 10);
  writenumber(2, (RTC.m / 10) % 10);
  writenumber(3, RTC.m % 10);
  writenumber(4, (RTC.s / 10) % 10);
  writenumber(5, RTC.s % 10);
  delay(100);
}

void writenumber(uint8_t a, uint8_t b) {
    digitalWrite(A[a], patterns[b][0]);
    digitalWrite(B[a], patterns[b][1]);
    digitalWrite(C[a], patterns[b][2]);
    digitalWrite(D[a], patterns[b][3]);
}

void runDigitCycleFancy() {
  for (uint8_t step = 0; step < 10; step++) {
    for (uint8_t i = 0; i < 6; i++) {
      uint8_t value;
      if (i % 2 == 0) {
        value = step;
      } else {
        value = 9 - step;
      }
      writenumber(i, value);
    }
    delay(125);
  }
}

void handleSerial(){
  char cmd[30];
  uint8_t idx = 0;
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      cmd[idx] = '\0';
      idx = 0;

      //if (strncmp(cmd, "SET ", 4) == 0) {
      if (cmd[0]=='S' && cmd[1]=='E' && cmd[2]=='T' && cmd[3]==' '){
        writeTime(cmd + 4);
      }
    } else if (idx < sizeof(cmd) - 1) {
      cmd[idx++] = c;
    }
  }
}

void writeCompileTime() {
  char buffer[20];

  // manual copy of __DATE__ + space + __TIME__
  for (uint8_t i = 0; i < 11; i++) buffer[i] = __DATE__[i];
  buffer[11] = ' ';
  for (uint8_t i = 0; i < 8; i++) buffer[12 + i] = __TIME__[i];
  buffer[20 - 1] = '\0';

  writeTime(buffer);
}


void writeTime(const char* d) {
  char monthStr[4];
  monthStr[0] = d[0];
  monthStr[1] = d[1];
  monthStr[2] = d[2];
  monthStr[3] = '\0';

  RTC.dd = DIG2(4);
//    (d[4] - '0') * 10 + (d[5] - '0');

  RTC.yyyy =
    (d[7] - '0') * 1000 +
    (d[8] - '0') * 100 +
    (d[9] - '0') * 10 +
    (d[10] - '0');

  RTC.h = DIG2(12);

  RTC.m = DIG2(15);
 //   (d[15] - '0') * 10 + (d[16] - '0');

  RTC.s = DIG2(18);
  //  (d[18] - '0') * 10 + (d[19] - '0');
  
  const char* months = "JanFebMarAprMayJunJulAugSepOctNovDec";
  for (uint8_t i = 0; i < 12; i++) {
    if (months[i*3] == monthStr[0] &&
        months[i*3+1] == monthStr[1] &&
        months[i*3+2] == monthStr[2]) {
      RTC.mm = i + 1;
      break;
    }
  }

  RTC.writeTime();
}
