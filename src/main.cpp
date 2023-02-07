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
#include "traditionalBlue.hpp"
#include "autoBaudrate.hpp"
#include "rgbLeds.hpp"

States currentState = BLUE_PAIRING;
myLeds leds{GPIO_NUM_27,
            GPIO_NUM_26,
            GPIO_NUM_25};

void task1(void *param)
{
    for (;;)
    {
        try
        {
            if (msg != "")
            {
                BLE_notify(msg.c_str());
            }
            blink();
        }
        catch (const std::exception &e)
        {
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void task2(void *parameters)
{
    for (;;)
    {
        switch (currentState)
        {
        case BLUE_PAIRING:
        {
            bool couldConn = false;
            int cnt = 0;
            Serial.begin(115200);
            while (cnt < 15000)
            {
                couldConn = confirmAuth();
                if (couldConn)
                {
                    break;
                }
                if (isAnyone())
                {
                    Serial.print("\nClient connected...\n");
                    couldConn = true;
                    break;
                }
                delay(2);
                cnt += 2;
                Serial.print("*");
            }
            if (!couldConn)
            {
                Serial.print("\n[err 3] Could not pair device, restarting board...\n");
                Serial.end();
                ESP.restart();
            }
            Serial.end();
            currentState = BLUE_ASK_KEY;
            break;
        }
        case BLUE_ASK_KEY:
        {
            if (askForKey(pinc))
            {
                SerialBT.println("Succesfully validated");
                currentState = SEND_TEST;
                break;
            }
            SerialBT.println("[err 4] incorrect key");
            break;
        }
        case DETERMINATE_BAUD_232_NI:
        {
            UARTparam.baud = optimalBaudrateDetection(false, RXD_232, TXD_232);
            if (UARTparam.baud == 0)
            {
#ifdef DEBUG
                BLE_notify("Could not detect at RS232 not inverted");
#endif
                currentState = DETERMINATE_BAUD_232_I;
                break;
            }
            else
            {
                UARTparam.inverted = false;
                UARTparam.rxd = RXD_232;
                UARTparam.txd = TXD_232;
                currentState = READ_DATA;
                currentState = INIT_UART;
                break;
            }
        }
        case DETERMINATE_BAUD_232_I:
        {
            UARTparam.baud = optimalBaudrateDetection(true, RXD_232, TXD_232);
            if (UARTparam.baud == 0)
            {
#ifdef DEBUG
                BLE_notify("Could not detect at RS232 inverted");
#endif
                currentState = DETERMINATE_BAUD_485_NI;
                break;
            }
            else
            {
                UARTparam.inverted = true;
                UARTparam.rxd = RXD_232;
                UARTparam.txd = TXD_232;
                currentState = READ_DATA;
                currentState = INIT_UART;
                break;
            }
        }
        case DETERMINATE_BAUD_485_NI:
        {
            UARTparam.baud = optimalBaudrateDetection(false, RXD_485, TXD_485);
            if (UARTparam.baud == 0)
            {
#ifdef DEBUG
                BLE_notify("Could not detect at RS485 not inverted");
#endif
                currentState = DETERMINATE_BAUD_485_I;
                break;
            }
            else
            {
                digitalWrite(RE, LOW);

                UARTparam.inverted = false;
                UARTparam.rxd = RXD_485;
                UARTparam.txd = TXD_485;
                currentState = READ_DATA;
                currentState = INIT_UART;
                break;
            }
        }
        case DETERMINATE_BAUD_485_I:
        {
            UARTparam.baud = optimalBaudrateDetection(true, RXD_485, TXD_485);
            if (UARTparam.baud == 0)
            {
#ifdef DEBUG
                BLE_notify("Could not detect at RS485 inverted");
#endif
                currentState = SEND_FAIL;
                break;
            }
            else
            {
                digitalWrite(RE, LOW);
                UARTparam.inverted = true;
                UARTparam.rxd = RXD_485;
                UARTparam.txd = TXD_485;
                currentState = READ_DATA;
                currentState = INIT_UART;
                break;
            }
        }
        case INIT_UART:
        {
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
                // BLE_notify(String((char)Serial.read()).c_str());
                Blue_send((char)Serial.read());
            }
            break;
        }
        case SEND_FAIL:
        {
            msg = "[err 0] Couldn't detect any baudrate\n";
            delay(1000);
            ESP.restart();
            break;
        }

        case SEND_TEST:
        {
            String val = String(random(98989888)) + String("\r\n"); 
            Blue_send(val);
            // BLE_notify("Hola Chingo\r\n");
        }

        default:
            break;
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void setup()
{
    // BLE_setup();
    Blue_setup(deviceName, pinc);
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
    pinMode(PIN_RED, OUTPUT);
    pinMode(RE, OUTPUT);
}

void loop()
{
}