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

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

int baudrate_ = 9600;

#define RS232

void setup()
{
  Serial.begin(9600);
  SerialBT.begin("Darkflow-Device"); // Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
#ifdef RS232
  SerialPort.begin(9600, SERIAL_8N1, RXD_232, TXD_232);
#endif
#ifdef RS485
  Serial1.begin(baudRate_, SERIAL_8N1, RXD_485, TXD_485);
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