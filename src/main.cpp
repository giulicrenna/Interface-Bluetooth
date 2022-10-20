#include <Arduino.h>
#include <BluetoothSerial.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <LittleFS.h>
#include <Arduino.h>
#include <vector>
#include <cstdlib>

<<<<<<< Updated upstream
#include "global.hpp"
=======
#include "global.hpp" // WARNING! global.h must be included before BluethoothSerial.h, 'cause bluethooth pin is set from global.h
#include <HardwareSerial.h>
#include <BluetoothSerial.h>
>>>>>>> Stashed changes

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

<<<<<<< Updated upstream
void setup() {
  Serial.begin(115200);
  SerialBT.begin(deviceID);
  SerialBT.enableSSP();
=======
void setup()
{
  Serial.begin(9600);
  int baudrate = detRate(RXD_232, TXD_232);
  SerialBT.begin("Darkflow-Device"); // Bluetooth device name
  SerialBT.enableSSP();
  SerialBT.setPin(bluethoothPin);
>>>>>>> Stashed changes
  Serial.println("The device started, now you can pair it with bluetooth!");
  
}

<<<<<<< Updated upstream
void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
=======
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

bool areAnyKnownCharacter(std::string str)
{
  int numbers[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  for (int num : numbers)
  {
    if (str.find("\r") != std::string::npos)
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
  int bauds[14] = {9600, 115200, 110, 300, 600, 1200, 2400, 4800, 14400, 19200, 38400, 57600, 128000, 256000};
  for (int baud : bauds)
  {
    Serial.println("Testing: " + String(baud) + " bauds.");
    Serial1.begin(baud, SERIAL_8N1, RXD, TXD);
    String incomming = Serial1.readString();
    if (areAnyKnownCharacter(incomming.c_str()))
    {
      Serial.println("Correct configuration found!");
      Serial1.end();
      return baud;
    }
>>>>>>> Stashed changes
  }
  delay(20);
}