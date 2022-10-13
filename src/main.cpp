#include <Arduino.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <LittleFS.h>
#include <Arduino.h>
#include <vector>
#include <cstdlib>

#include "global.hpp"
#include "blueMan.hpp"
#include "rs485Read.hpp"
#include "rs232Read.hpp"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

int baudrate = 9600;
#define RS232

void setup() {
  #ifdef RS232
  setupRS232((uint8_t)baudrate);
  #endif
  #ifdef RS485
  setupRS485((uint8_t)baudrate);
  #endif

  setupBluethooth();    
}

void loop() {
  byte message;

  #ifdef RS232
  message = communicationRS232();
  #endif
  #ifdef RS485
  message = communicationRS485();
  #endif

  dataHermes(message);
}