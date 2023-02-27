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
// #include "rgbLeds.hpp"

int previousMillis = 0;
int ledState = LOW;
States currentState = BLUE_PAIRING;

void blink(int timelapse = 500);

void task1(void *param)
{
    for (;;)
    {
        switch (currentState)
        {
        case BLUE_ASK_KEY:
        {
            blink(50);
            break;
        }
        case BLUE_PAIRING:
        {
            blink(1000);
            break;
        }
        case SEND_MSG:
        {
            // Estaba en high pero se apaga
            // En low se prende
            digitalWrite(PIN_RED, LOW);
            break;
        }
        case SEND_TEST:
        {
            digitalWrite(PIN_RED, LOW);
            break;
        }
        }
        vTaskDelay(10);
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
            while (true)
            {
                couldConn = confirmAuth();
                if (isAnyone() || couldConn)
                {
                    currentState = BLUE_ASK_KEY;
                    break;
                }
            }
        }

        case BLUE_ASK_KEY:
        {
            if (askForKey(pinc))
            {
                SerialBT.println("Succesfully validated");
                SerialBT.println("key: " + String(keyring));
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

            if (isAnyone())
            {
                // msg = Serial.readString();
                while (Serial.available() > 0)
                {
                    // BLE_notify(String((char)Serial.read()).c_str());
                    Blue_send((char)Serial.read());
                }
                break;
            }
            else
            {
                ESP.restart();
                /*
                int lastTimeToPairAgain = 0;
                while (lastTimeToPairAgain != 10000)
                {
                    if (isAnyone())
                    {
                        break;
                    }
                    else
                    {
                        lastTimeToPairAgain++;
                    }
                }
                if (lastTimeToPairAgain == 10000)
                {
                    currentState = BLUE_PAIRING;
                    break;
                }
                */
            }
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
            if (millis() - currentTimeBluetoothTestMessage >= 1000)
            {
                if (isAnyone())
                {
                    String val = String(random(1000, 9999)) + String("\r\n");
                    Blue_send(val);
                }
                else
                {
                    ESP.restart();
                    currentState = BLUE_PAIRING;
                    break;
                }
                currentTimeBluetoothTestMessage = millis();
            }
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
    pinMode(GPIO_NUM_12, OUTPUT);
    pinMode(RE, OUTPUT);
    digitalWrite(GPIO_NUM_12, HIGH);
}

void loop()
{
}

void blink(int timelapse)
{
    if (millis() - previousMillis >= timelapse)
    {
        ledState = (ledState == LOW) ? HIGH : LOW;
        digitalWrite(PIN_RED, ledState);
        previousMillis = millis();
    }
}