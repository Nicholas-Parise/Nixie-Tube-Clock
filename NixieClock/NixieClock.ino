#include <MD_DS1307.h>
#include <Wire.h>

#define A1 4
#define B1 5
#define C1 6
#define D1 7

#define A2 8
#define B2 9
#define C2 10
#define D2 11

#define A3 12
#define B3 13
#define C3 14
#define D3 15

#define A4 30
#define B4 29
#define C4 28
#define D4 26

#define A5 23
#define B5 22
#define C5 21
#define D5 20

#define A6 19
#define B6 18
#define C6 17
#define D6 16

char A[6] = {A1, A2, A3, A4, A5, A6};
char B[6] = {B1, B2, B3, B4, B5, B6};
char C[6] = {C1, C2, C3, C4, C5, C6};
char D[6] = {D1, D2, D3, D4, D5, D6};
int digit_zero;
int digit_one;
int digit_two;
int digit_three;
int digit_four;
int digit_five;

int hour;
int minute;
int second;

void setup() {
  pinMode(A1, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(C1, OUTPUT);
  pinMode(D1, OUTPUT);
  
  pinMode(A2, OUTPUT);
  pinMode(B2, OUTPUT);
  pinMode(C2, OUTPUT);
  pinMode(D2, OUTPUT);
  
  pinMode(A3, OUTPUT);
  pinMode(B3, OUTPUT);
  pinMode(C3, OUTPUT);
  pinMode(D3, OUTPUT);

  pinMode(A4, OUTPUT);
  pinMode(B4, OUTPUT);
  pinMode(C4, OUTPUT);
  pinMode(D4, OUTPUT);

  pinMode(A5, OUTPUT);
  pinMode(B5, OUTPUT);
  pinMode(C5, OUTPUT);
  pinMode(D5, OUTPUT);

  pinMode(A6, OUTPUT);
  pinMode(B6, OUTPUT);
  pinMode(C6, OUTPUT);
  pinMode(D6, OUTPUT);



  for (char i = 0; i < 4; i++) {
    digitalWrite(A[i], HIGH);
    digitalWrite(B[i], HIGH);
    digitalWrite(C[i], HIGH);
    digitalWrite(D[i], HIGH);
  }
  if (!RTC.isRunning())
    RTC.control(DS1307_CLOCK_HALT, DS1307_OFF);
//  Serial.begin(9600);
}

void loop() {
  RTC.readTime();
  hour = RTC.h;
  minute = RTC.m;
  second = RTC.s;

  digit_zero = (hour / 10) % 10;
  digit_one = hour % 10;
  digit_two =  (minute / 10) % 10;
  digit_three = minute % 10;
  digit_four = (second / 10) % 10;
  digit_five = second % 10;

//  Serial.println(zero);
//  Serial.println(one);
//  Serial.println(two);
//  Serial.println(three);
//  Serial.println(four);
//  Serial.println(five);
  writenumber(0, zero);
  writenumber(1, one);
  writenumber(2, two);
  writenumber(3, three);
  writenumber(4, four);
  writenumber(5, five);
  delay(1000);
}

void writenumber(int a, int b) {
  switch (b) {
    case 0:
      digitalWrite(A[a], LOW);
      digitalWrite(B[a], LOW);
      digitalWrite(C[a], LOW);
      digitalWrite(D[a], LOW);
      break;
    case 1:
      digitalWrite(A[a], HIGH);
      digitalWrite(B[a], LOW);
      digitalWrite(C[a], LOW);
      digitalWrite(D[a], LOW);
      break;
    case 2:
      digitalWrite(A[a], LOW);
      digitalWrite(B[a], HIGH);
      digitalWrite(C[a], LOW);
      digitalWrite(D[a], LOW);
      break;
    case 3:
      digitalWrite(A[a], HIGH);
      digitalWrite(B[a], HIGH);
      digitalWrite(C[a], LOW);
      digitalWrite(D[a], LOW);
      break;
    case 4:
      digitalWrite(A[a], LOW);
      digitalWrite(B[a], LOW);
      digitalWrite(C[a], HIGH);
      digitalWrite(D[a], LOW);
      break;
    case 5:
      digitalWrite(A[a], HIGH);
      digitalWrite(B[a], LOW);
      digitalWrite(C[a], HIGH);
      digitalWrite(D[a], LOW);
      break;
    case 6:
      digitalWrite(A[a], LOW);
      digitalWrite(B[a], HIGH);
      digitalWrite(C[a], HIGH);
      digitalWrite(D[a], LOW);
      break;
    case 7:
      digitalWrite(A[a], HIGH);
      digitalWrite(B[a], HIGH);
      digitalWrite(C[a], HIGH);
      digitalWrite(D[a], LOW);
      break;
    case 8:
      digitalWrite(A[a], LOW);
      digitalWrite(B[a], LOW);
      digitalWrite(C[a], LOW);
      digitalWrite(D[a], HIGH);
      break;
    case 9:
      digitalWrite(A[a], HIGH);
      digitalWrite(B[a], LOW);
      digitalWrite(C[a], LOW);
      digitalWrite(D[a], HIGH);
      break;
  }
}

void test(){

  for (int i = 0; i < 10; i++) {
    for (int m = 0; m < 6; m++){
      writenumber(m, i);
    }
    delay(500);
  }
}


void off(int a) {
  digitalWrite(A[a], HIGH);
  digitalWrite(B[a], HIGH);
  digitalWrite(C[a], HIGH);
  digitalWrite(D[a], HIGH);
}
