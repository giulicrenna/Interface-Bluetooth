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

#include "global.hpp"
#include "BLE_configs.hpp"
#include "autoBaudrate.hpp"
#include <HardwareSerial.h>

typedef enum
{
    DETERMINATE_BAUD_232,
    DETERMINATE_BAUD_485,
    SEND_OK_DATA,
    SEND_FAIL
} States;

States currentState = DETERMINATE_BAUD_232;
HardwareSerial SerialPort(1);

String msg = "";

void task1(void *param)
{
    for (;;)
    {
        if(msg != ""){
            BLE_notify(msg);
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
        case DETERMINATE_BAUD_232:
        {
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
            msg = SerialPort.readString();
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
}

void loop()
{
}