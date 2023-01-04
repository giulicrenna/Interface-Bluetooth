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
#include "rgbLeds.hpp"

typedef enum
{
    DETERMINATE_BAUD_232_NI,
    DETERMINATE_BAUD_232_I,
    DETERMINATE_BAUD_485_NI,
    DETERMINATE_BAUD_485_I,
    READ_DATA,
    SEND_FAIL
} States;

States currentState = DETERMINATE_BAUD_232_NI;
HardwareSerial SerialPort(1);
myLeds leds{GPIO_NUM_27,
            GPIO_NUM_26,
            GPIO_NUM_25};

String msg = "";
int baud;

void task1(void *param)
{
    for (;;)
    {
        if (msg != "")
        {
            BLE_notify(msg);
#ifdef DEBUG
// Serial.println();
#endif
        }
        vTaskDelay(100);
    }
}

void task2(void *parameters)
{
    for (;;)
    {
        switch (currentState)
        {
        case DETERMINATE_BAUD_232_NI:
        {
            int baud = optimalBaudrateDetection(false, RXD_232, TXD_232);
            if (baud != 0)
            {
                Serial.begin(baud, SERIAL_8N1, RXD_232, TXD_232, false);
                currentState = READ_DATA;
                break;
            }
            else
            {
#ifdef DEBUG
                BLE_notify("Could not detect at RS232 not inverted");
#endif
                currentState = DETERMINATE_BAUD_232_I;
                break;
            }
        }
        case DETERMINATE_BAUD_232_I:
        {
            int baud = optimalBaudrateDetection(true, RXD_232, TXD_232);
            if (baud != 0)
            {
                Serial.begin(baud, SERIAL_8N1, RXD_232, TXD_232, true);
                currentState = READ_DATA;
                break;
            }
            else
            {
#ifdef DEBUG
                BLE_notify("Could not detect at RS232 inverted");
#endif
                currentState = DETERMINATE_BAUD_485_NI;
                break;
            }
        }
        case DETERMINATE_BAUD_485_NI:
        {
            int baud = optimalBaudrateDetection(false, RXD_485, TXD_485);
            if (baud != 0)
            {
                Serial.begin(baud, SERIAL_8N1, RXD_485, TXD_485, false);
                currentState = READ_DATA;
                break;
            }
            else
            {
#ifdef DEBUG
                BLE_notify("Could not detect at RS485 not inverted");
#endif
                currentState = DETERMINATE_BAUD_485_NI;
                break;
            }
        }
        case DETERMINATE_BAUD_485_I:
        {
            int baud = optimalBaudrateDetection(true, RXD_485, TXD_485);
            if (baud != 0)
            {
                Serial.begin(baud, SERIAL_8N1, RXD_485, TXD_485, true);
                currentState = READ_DATA;
                break;
            }
            else
            {
#ifdef DEBUG
                BLE_notify("Could not detect at RS485 inverted");
#endif
                currentState = DETERMINATE_BAUD_232_NI;
                break;
            }
        }
        case READ_DATA:
        {
            leds.changeColour(random(0, 4));
            msg = Serial.readString();
            break;
        }
        case SEND_FAIL:
        {
            msg = "[err 0] Couldn't detect any baudrate\n";
            leds.changeColour(0);
            delay(20000);
            ESP.restart();
            break;
        }

        default:
            break;
        }
    }
}

void setup()
{
    xTaskCreatePinnedToCore(
        task1,
        "Task 1...",
        5000,
        NULL,
        1,
        NULL,
        0);
    xTaskCreatePinnedToCore(
        task2,
        "Task 2...",
        10000,
        NULL,
        1,
        NULL,
        1);
    BLE_setup();
    leds.setupLed();
#ifdef DEBUG
    delay(1000);
#endif
}

void loop()
{
}