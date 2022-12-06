// https://github.com/LiveSparks/ESP32_BLE_Examples/blob/master/BLE_switch/BLE_Switch.ino
// https://github.com/choichangjun/ESP32_arduino/blob/master/ESP32_Arduino_paring_Key.ino
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
#include "global.hpp"
#include "BLE_configs.hpp"
#include "autoBaudrate.hpp"

typedef enum
{
    DETERMINATE_BAUD_232,
    DETERMINATE_BAUD_485,
    SEND_OK_DATA,
    SEND_FAIL
} States;

States currentState = DETERMINATE_BAUD_232;

HardwareSerial SerialPort(1);

void setup()
{
    BLE_setup();
    leds.setupLed();
}

void loop()
{
    switch (currentState)
    {
    case DETERMINATE_BAUD_232:
    {
        Serial.begin(9600);
        int baudrate = detRate(RXD_232, TXD_232, true);

        if (baudrate != 1)
        {
            digitalWrite(TR, HIGH); // Transmitter: HIGH, Receiver: LOW
            leds.changeColour(0);
            SerialPort.begin(baudrate, SERIAL_8N1, RXD_232, TXD_232);
            currentState = SEND_OK_DATA;
            break;
        }
        else
        {
            currentState = DETERMINATE_BAUD_485;
            break;
        }
        break;
    }

    case DETERMINATE_BAUD_485:
    {
        int baudrate = detRate(RXD_485, TXD_485, false);
        digitalWrite(TR, LOW); // Transmitter: LOW, Receiver: HIGH

        if (baudrate != 1)
        {
            leds.changeColour(1);
            SerialPort.begin(baudrate, SERIAL_8N1, RXD_485, TXD_485);
            currentState = SEND_OK_DATA;
            break;
        }
        else
        {
            currentState = SEND_FAIL;
            break;
        }
        break;
    }
    case SEND_OK_DATA:
    {
        leds.changeColour(2);
        String msg = SerialPort.readString();
        BLE_notify(msg);
        break;
    }

    case SEND_FAIL:
    {
        String msg = "[err 0] Couldn't detect baudrate\n";
        BLE_notify(msg);
        leds.changeColour(0);
        delay(2000);
        break;
    }

    default:
        break;
    }
}
