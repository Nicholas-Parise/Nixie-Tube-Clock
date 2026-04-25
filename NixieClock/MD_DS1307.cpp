/*
  MD_DS1307 - Library for using a DS1307 Real Time Clock.
  
  Created by Marco Colli May 2012
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
 */
#include "MD_DS1307.h"
#include <Wire.h>

#ifndef ARDUINO_ARCH_SAMD
class MD_DS1307 RTC;  // one instance created when library is included
#endif

// Useful definitions
#define	DS1307_ID	0x68  // I2C/TWI device address, coded into the device

#define RAM_BASE_READ   0 // smallest read address
#define RAM_BASE_WRITE  8 // smallest write address

// Addresses for the parts of the date/time in RAM
#define ADDR_SEC  ((uint8_t)0x0)
#define ADDR_MIN  ((uint8_t)0x1)
#define ADDR_HR   ((uint8_t)0x2)
#define ADDR_DAY  ((uint8_t)0x3)
#define ADDR_DATE ((uint8_t)0x4)
#define ADDR_MON  ((uint8_t)0x5)
#define ADDR_YR   ((uint8_t)0x6)

// Address for the special control bytes
#define ADDR_CTL_CH   ((uint8_t)0x0)
#define ADDR_CTL_12H  ((uint8_t)0x2)
//#define ADDR_CTL_OUT  ((uint8_t)0x7)
//#define ADDR_CTL_SQWE ((uint8_t)0x7)
//#define ADDR_CTL_RS   ((uint8_t)0x7)

// Bit masks for the control/testable bits
#define CTL_CH    ((uint8_t)0x80)
#define CTL_12H   ((uint8_t)0x40)
#define CTL_PM    ((uint8_t)0x20)
//#define CTL_OUT   ((uint8_t)0x80)
//#define CTL_SQWE  ((uint8_t)0x10)
//#define CTL_RS    ((uint8_t)0x03)

// Define a global buffer we can use in these functions
#define MAX_BUF   8     // time message is the biggest message we need to handle (7 bytes)
uint8_t	bufRTC[MAX_BUF];

// BCD to binary number packing/unpacking functions
/*  // Second time Definitions
static uint8_t BCD2bin(uint8_t v) { return v - 6 * (v >> 4); }
static uint8_t bin2BCD (uint8_t v) { return v + 6 * (v / 10); }
*/

// Interface functions for the RTC device
uint8_t MD_DS1307::readDevice(uint8_t addr, uint8_t* buf, uint8_t len)
{
  Wire.beginTransmission(DS1307_ID);
  Wire.write(addr);       // set register address                  
  if (Wire.endTransmission() != 0)
    return(0);

  Wire.requestFrom(DS1307_ID, (int)len);
  // while (!Wire.available()) ;  // wait
  for (uint8_t i=0; i<len; i++) // Read x data from given address upwards...
  {
    buf[i] = Wire.read();       // ... and store it in the buffer
  }

  return(len);
}

uint8_t MD_DS1307::writeDevice(uint8_t addr, uint8_t* buf, uint8_t len)
{
  Wire.beginTransmission(DS1307_ID);
  Wire.write(addr);             // set register address                  
  for (uint8_t i=0; i<len; i++) // Send x data from given address upwards...
  {
    Wire.write(buf[i]);         // ... and send it from buffer
  }
  Wire.endTransmission();

  return(len);
}

void MD_DS1307::init()
{
  yyyy = mm = dd = 0;
  h = m = s = 0;
  dow = 0;
}

// Class functions
MD_DS1307::MD_DS1307()
{
  init();
  Wire.begin();
}

#ifdef ESP8266
MD_DS1307::MD_DS1307(int sda, int scl)
{
  init();
  Wire.begin(sda, scl);
}
#endif
 
void MD_DS1307::readTime(void)
// Read the current time from the RTC and unpack it into the object variables
{
  readDevice(RAM_BASE_READ, bufRTC, 7);   // get the data

  // unpack it
  s = BCD2bin(bufRTC[ADDR_SEC] & ~CTL_CH);  // mask off the 'CH' bit
  m = BCD2bin(bufRTC[ADDR_MIN]);
  /*
  if (bufRTC[ADDR_HR] & CTL_12H)      // 12 hour clock
  {
    h = BCD2bin(bufRTC[ADDR_HR] & 0x1f);
    pm = (bufRTC[ADDR_HR] & CTL_PM);
  }
  else
  {
  */
    h = BCD2bin(bufRTC[ADDR_HR] & 0x3f);
    pm = 0;
  //}
  dow = BCD2bin(bufRTC[ADDR_DAY]);
  dd = BCD2bin(bufRTC[ADDR_DATE]);
  mm = BCD2bin(bufRTC[ADDR_MON]);
  yyyy = BCD2bin(bufRTC[ADDR_YR]) + 2000;
}

void MD_DS1307::writeTime(void)
// Pack up and write the time stored in the object variables to the RTC
// Note: Setting the time will also start the clock of it is halted
{
  uint8_t	mode12;

  // check what time mode is current
  //readDevice(ADDR_HR, &mode12, 1);
  //mode12 &= CTL_12H;

  // pack it up in the current space
  bufRTC[ADDR_SEC] = bin2BCD(s);
  bufRTC[ADDR_MIN] = bin2BCD(m);
  /*
  if (mode12)     // 12 hour clock
  {
    pm = (h > 12);
    if (pm) h -= 12;
    bufRTC[ADDR_HR] = bin2BCD(h);
    if (pm) bufRTC[ADDR_HR] |= CTL_PM;
    bufRTC[ADDR_HR] |= CTL_12H;
  }
  else
  */
    bufRTC[ADDR_HR] = bin2BCD(h);

  bufRTC[ADDR_DAY] = bin2BCD(dow);
  bufRTC[ADDR_DATE] = bin2BCD(dd);
  bufRTC[ADDR_MON] = bin2BCD(mm);
  bufRTC[ADDR_YR] = bin2BCD(yyyy - 2000);

  writeDevice(RAM_BASE_READ, bufRTC, 7);
}


void MD_DS1307::startClock(void)
// Perform a control action on item, using the value
{
  uint8_t addr;   // address of the byte to change
  uint8_t mask;   // mask used to clear the bits being set (ANDed)
  uint8_t cmd;    // value used to set new nit value (ORed)
  
  //case DS1307_CLOCK_HALT:
  addr = ADDR_CTL_CH;
  mask = (uint8_t)~CTL_CH;
  cmd = 0; 

  // now read the address from the RTC
  readDevice(addr, bufRTC, 1);
  bufRTC[0] = (bufRTC[0] & mask) | cmd;
  writeDevice(addr, bufRTC, 1);

  // 24 hour mode
  readDevice(ADDR_HR, bufRTC, 1);
  bufRTC[0] &= ~CTL_12H;
  writeDevice(ADDR_HR, bufRTC, 1);
 
  return;
}


  boolean MD_DS1307::isRunning(void){
    readDevice(RAM_BASE_READ, bufRTC, 8);   // read all the data once
    return (bufRTC[ADDR_CTL_CH] & CTL_CH) == 0;
  }
