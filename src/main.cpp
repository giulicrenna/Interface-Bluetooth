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

#include "mStandars.h"
#include "global.hpp"
#include "functions.hpp"
#include "BLE_configs.hpp"
#include "traditionalBlue.hpp"
#include "autoBaudrate.hpp"
// #include "rgbLeds.hpp"

Preferences config;

int previousMillis = 0;
int ledState = LOW;
States currentState = BLUE_PAIRING;

void initUART();
void setupPreferences();
void blink(int timelapse = 500);
void askKey();
void sendTest();
void sendFail();
void sendData();
void lexator();

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
        case READ_DATA:
        {
            /**
             * @brief In this section the ESP32 intercept BT commands
             *
             */
            lexator();
            digitalWrite(PIN_RED, LOW);
            break;
        }
        }
        vTaskDelay(10);
    }
}

/**
 * @brief
 *
 * @param parameters
 */
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
            askKey();
            break;
        }

        case MANAGE_UART:
        {
            if (UARTparam.isAuto)
            {
                currentState = DETERMINATE_BAUD_232_NI;
                break;
            }

            if (UARTparam.isRS232)
            {
                UARTparam.rxd = RXD_232;
                UARTparam.txd = TXD_232;
            }
            else
            {
                UARTparam.rxd = RXD_485;
                UARTparam.txd = TXD_485;
            }
            currentState = INIT_UART;
            break;
        }

        case DETERMINATE_BAUD_232_NI:
        {
            UARTparam.baud = optimalBaudrateDetection(false, RXD_232, TXD_232);
            if (UARTparam.baud == 0)
            {
#ifdef DEBUG
                Blue_send(debugging.err_0);
#endif
                currentState = DETERMINATE_BAUD_485_NI;
                break;
            }
            else
            {
                SerialBT.println(String(debugging.sta_5) + String(UARTparam.baud));
                UARTparam.inverted = false;
                UARTparam.rxd = RXD_232;
                UARTparam.txd = TXD_232;
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
                Blue_send(debugging.err_1);
#endif
                currentState = SEND_FAIL;
                break;
            }
            else
            {
                digitalWrite(RE, LOW);
                SerialBT.println(String(debugging.sta_5) + String(UARTparam.baud));
                UARTparam.inverted = false;
                UARTparam.rxd = RXD_485;
                UARTparam.txd = TXD_485;
                currentState = INIT_UART;
                break;
            }
        }

        case INIT_UART:
        {
            initUART();
            currentState = READ_DATA;
            break;
        }

        case READ_DATA:
        {

            if (isAnyone())
            {
                sendData();
                break;
            }
            else
            {
                ESP.restart();
            }
            break;
        }

        case SEND_FAIL:
        {
            sendFail();
            break;
        }

        case SEND_TEST:
        {
            sendTest();
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
    Blue_setup(deviceName, pinc.c_str());
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
    setupPreferences();
    pinMode(GPIO_NUM_12, OUTPUT);
    pinMode(RE, OUTPUT);
    digitalWrite(GPIO_NUM_12, HIGH);
}

void loop()
{
}

void initUART()
{
    Serial.end();
    Serial.begin(UARTparam.baud, SERIAL_8N1, UARTparam.rxd, UARTparam.txd, UARTparam.inverted);
}

void sendData()
{
    if (millis() - currentTimeSendMessage >= sendTime && sendToDevice)
    {
        char msg[INCOME_BUFFER];
        if (Serial.available() > 0)
        {
            Serial.readBytes(msg, INCOME_BUFFER);
        }

        String temp(std::string(msg).substr(0, INCOME_BUFFER - 4).c_str());
        Blue_send(temp);
        currentTimeSendMessage = millis();
    }
}

void askKey()
{
    if (askForKey(pinc.c_str()))
    {
        SerialBT.println(debugging.sta_0);
        // SerialBT.println("key: " + String(keyring));
        currentState = MANAGE_UART; // CHANGE THIS TO SEND_TEST IF WANT TO TEST RANDOM NUMERS
        return;
    }
    SerialBT.println(debugging.err_2);
}

void setupPreferences()
{
    config.begin("config", false);
    INCOME_BUFFER = config.getInt("buffer", 32);
    sendTime = config.getInt("time", 1000);
    sendToDevice = config.getBool("send", true);
    pinc = config.getString("pinc", "12345");
}

void sendFail()
{
    Blue_send(debugging.err_3);
    delay(1000);
    ESP.restart();
}

void sendTest()
{
    if (millis() - currentTimeBluetoothTestMessage >= sendTime && sendToDevice)
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
        }
        currentTimeBluetoothTestMessage = millis();
    }
}

void lexator()
{
    if (SerialBT.available() > 0)
    {
        String command = SerialBT.readString();
        command.trim();
        if (command == "PAUSE")
        {
            sendToDevice = false;
            config.putBool("send", false);
            SerialBT.println(debugging.sta_1);
            // break;
        }
        else if (command == "CONTINUE")
        {
            sendToDevice = true;
            config.putBool("send", true);
            SerialBT.println(debugging.sta_2);
        }
        else
        {
            String cmd[5];
            std::vector<std::string> cmd_v = mstd::strip(command.c_str(), ':');
            for (int k = 0; k < cmd_v.size(); k++)
            {
                cmd[k] = String(cmd_v.at(k).c_str());
            }
            if (cmd[0] == "TIME")
            {
                try
                {
                    int temp_time = std::stoi(cmd[1].c_str());
                    sendTime = temp_time;
                    config.putInt("time", sendTime);
                    SerialBT.println(debugging.sta_3);
                }
                catch (const std::exception &e)
                {
                    SerialBT.println(debugging.err_4);
                    // break;
                }
            }
            else if (cmd[0] == "BUFFER")
            {
                try
                {
                    int temp_buffer = std::stoi(cmd[1].c_str());
                    INCOME_BUFFER = temp_buffer;
                    config.putInt("buffer", INCOME_BUFFER);
                    SerialBT.println(debugging.sta_4);
                }
                catch (const std::exception &e)
                {
                    SerialBT.println(debugging.err_4);
                    // break;
                }
            }
            else if (cmd[0] == "PASSW")
            {
                try
                {
                    if (cmd[1] == pinc)
                    {
                        pinc = cmd[2].c_str();
                        config.putString("pinc", cmd[2]);
                        SerialBT.println(debugging.sta_6);
                        // break;
                    }
                    else
                    {
                        SerialBT.println(debugging.err_5);
                        // break;
                    }
                }
                catch (const std::exception &e)
                {
                    SerialBT.println(debugging.err_6);
                }
            }
            else
            {
                SerialBT.println(debugging.err_7);
            }
            // break;
        }
    }
}

/**
 * @brief
 *
 * @param timelapse
 */
void blink(int timelapse)
{
    if (millis() - previousMillis >= timelapse)
    {
        ledState = (ledState == LOW) ? HIGH : LOW;
        digitalWrite(PIN_RED, ledState);
        previousMillis = millis();
    }
}