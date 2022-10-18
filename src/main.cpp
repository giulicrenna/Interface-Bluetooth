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

#include <HardwareSerial.h>
#include <BluetoothSerial.h>
#include "global.hpp"

BluetoothSerial SerialBT;
HardwareSerial SerialPort(1);

int detRate(int RXD, int TXD);

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

#define RS232

void setup()
{
  int baudrate = detRate(RXD_232, TXD_232);
  Serial.begin(9600);
  SerialBT.begin("Darkflow-Device"); // Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
#ifdef RS232
  SerialPort.begin(baudrate, SERIAL_8N1, RXD_232, TXD_232);
#endif
#ifdef RS485
  Serial1.begin(baudrate, SERIAL_8N1, RXD_485, TXD_485);
#endif
}

void loop()
{
#ifdef RS232
  String msg = SerialPort.readString();

  SerialBT.println(msg);
  Serial.println(msg);
#endif

#ifdef RS485
  String msg = SerialPort.readString();

  SerialBT.println(msg);
  Serial.println(msg);
#endif
}

bool areAnyNumber(std::string str)
{
  int numbers[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  for (int num : numbers)
  {
    if (str.find("KG") != std::string::npos)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  return false;
}

// Auto Baudrate
int detRate(int RXD, int TXD)
{
  int bauds[14] = {110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 128000, 256000};
  for (int baud : bauds)
  {
    Serial.begin(baud, SERIAL_8N1, RXD, TXD);
    String incomming = Serial.readString();
    if (areAnyNumber(incomming.c_str()))
    {
      Serial.end();
      return baud;
    }
  }
  return 115200;
}