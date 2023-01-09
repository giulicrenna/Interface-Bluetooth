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
#include <Preferences.h>

#include "global.hpp"
#include "functions.hpp"
#include "BLE_configs.hpp"
#include "autoBaudrate.hpp"
#include "rgbLeds.hpp"

States currentState = DETERMINATE_BAUD_232_NI;
myLeds leds{GPIO_NUM_27,
            GPIO_NUM_26,
            GPIO_NUM_25};

void task1(void *param)
{
    for (;;)
    {
        BLE_notify(msg.c_str());
#ifdef DEBUG
// Serial.println();
#endif
    }
    vTaskDelay(10);
}

void task2(void *parameters)
{
    for (;;)
    {
        switch (currentState)
        {
        case DETERMINATE_BAUD_232_NI:
        {
            if (temporalData.getInt("baud", 0) == 0)
            {
                UARTparam.baud = optimalBaudrateDetection(false, RXD_232, TXD_232);
                if (UARTparam.baud != 0)
                {
                    UARTparam.inverted = false;
                    UARTparam.rxd = RXD_232;
                    UARTparam.txd = TXD_232;
                    currentState = READ_DATA;
                    loadData(UARTparam.inverted, UARTparam.rxd, UARTparam.txd, UARTparam.baud);
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
            else
            {
                currentState = INIT_UART;
                break;
            }
        }
        case DETERMINATE_BAUD_232_I:
        {
            if (temporalData.getInt("baud", 0) == 0)
            {
                UARTparam.baud = optimalBaudrateDetection(true, RXD_232, TXD_232);
                if (UARTparam.baud != 0)
                {
                    UARTparam.inverted = true;
                    UARTparam.rxd = RXD_232;
                    UARTparam.txd = TXD_232;
                    currentState = READ_DATA;
                    loadData(UARTparam.inverted, UARTparam.rxd, UARTparam.txd, UARTparam.baud);
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
            else
            {
                currentState = INIT_UART;
                break;
            }
        }
        case DETERMINATE_BAUD_485_NI:
        {
            if (temporalData.getInt("baud", 0) == 0)
            {
                digitalWrite(RE, LOW);
                UARTparam.baud = optimalBaudrateDetection(false, RXD_485, TXD_485);
                if (UARTparam.baud != 0)
                {
                    UARTparam.inverted = false;
                    UARTparam.rxd = RXD_485;
                    UARTparam.txd = TXD_485;
                    currentState = READ_DATA;
                    loadData(UARTparam.inverted, UARTparam.rxd, UARTparam.txd, UARTparam.baud);
                    break;
                }
                else
                {
#ifdef DEBUG
                    BLE_notify("Could not detect at RS485 not inverted");
#endif
                    currentState = DETERMINATE_BAUD_485_I;
                    break;
                }
            }
            else
            {
                currentState = INIT_UART;
                break;
            }
        }
        case DETERMINATE_BAUD_485_I:
        {
            if (temporalData.getInt("baud", 0) == 0)
            {
                digitalWrite(RE, LOW);
                UARTparam.baud = optimalBaudrateDetection(true, RXD_485, TXD_485);
                if (UARTparam.baud != 0)
                {
                    UARTparam.inverted = true;
                    UARTparam.rxd = RXD_485;
                    UARTparam.txd = TXD_485;
                    currentState = READ_DATA;
                    loadData(UARTparam.inverted, UARTparam.rxd, UARTparam.txd, UARTparam.baud);
                    break;
                }
                else
                {
#ifdef DEBUG
                    BLE_notify("Could not detect at RS485 inverted");
#endif
                    currentState = SEND_FAIL;
                    break;
                }
            }
            else
            {
                currentState = INIT_UART;
                break;
            }
        }
        case INIT_UART:
        {
            loadData();
            Serial.end();
            Serial.begin(UARTparam.baud, SERIAL_8N1, UARTparam.rxd, UARTparam.txd, UARTparam.inverted);
            currentState = READ_DATA;

            break;
        }
        case READ_DATA:
        {
            // msg = Serial.readString();
            while (Serial.available() > 0)
            {
                BLE_notify(String((char)Serial.read()).c_str());
            }
            leds.changeColour(random(0, 4));
            break;
        }
        case SEND_FAIL:
        {
            msg = "[err 0] Couldn't detect any baudrate\n";
            leds.changeColour(0);
            delay(3000);
            ESP.restart();
            break;
        }

        default:
            break;
        }
        vTaskDelay(10);
    }
}

void setup()
{
    setupFileSystem();
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
        12000,
        NULL,
        1,
        NULL,
        1);
    BLE_setup();
    leds.setupLed();
    pinMode(RE, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
}