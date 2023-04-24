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
#include <Preferences.h>
#include <map>
#include <list>
#include <HardwareSerial.h>

#include "global.hpp"
#include "mStandars.h"
#include "functions.hpp"
#include "BLE_configs.hpp"
#include "traditionalBlue.hpp"
#include "autoBaudrate.hpp"
#include "lexator.hpp"
// #include "rgbLeds.hpp"

int previousMillis = 0;
int ledState = LOW;

bool detectNonAscii(const char *tempBuffer);
void findEOL();
long getBestTimeout();
long timeFlags(int flag, int pin_ = RXD_232);
void serial_flush(void);
void initUART();
void setupPreferences();
void blink(int timelapse = 500);
void askKey();
void sendTest();
void sendFail();
void sendData();
void sendByPetition();
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
        case SEND_TEST:
        {
            digitalWrite(PIN_RED, LOW);
            break;
        }
        case SEND_BY_TIME:
        {
            /**
             * @brief In this section the ESP32 intercept BT commands
             *
             */
            lexator();
            digitalWrite(PIN_RED, LOW);
            break;
        }
        case SEND_BY_PETITION:
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
            askKey(); // TO MANAGE_DEVICE_CONFIGS
            break;
        }

        case MANAGE_DEVICE_CONFIGS:
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

        case CHANGE_UART_CONFIG:
        {
            Blue_send(debugging.sta_13);
            UARTparam.parity = UART_CONFIGS[lastUartConfigIndex];
            determinateParity(UART_CONFIGS[lastUartConfigIndex]);
            lastUartConfigIndex++;
            currentState = INIT_UART;
            break;
        }

        case DETECT_NON_ASCII:
        {
            Serial.end();
            Serial.begin(UARTparam.baud, UARTparam.parity, UARTparam.rxd, UARTparam.txd);

            char tempBuffer[32];

            while (!Serial.available())
                ;

            if (Serial.available())
            {
                Serial.readBytes(tempBuffer, 32);

                if (detectNonAscii(tempBuffer))
                {
                    currentState = CHANGE_UART_CONFIG;
                    break;
                }

                currentState = INIT_UART;
                break;
            }
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
                SerialBT.println(String(debugging.sta_5) + String(UARTparam.baud) + " RS232");
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
                currentState = DETERMINATE_BAUD_232_I;
                break;
            }
            else
            {
                digitalWrite(RE, LOW);
                SerialBT.println(String(debugging.sta_5) + String(UARTparam.baud) + " RS485");
                UARTparam.inverted = false;
                UARTparam.rxd = RXD_485;
                UARTparam.txd = TXD_485;
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
                Blue_send(debugging.err_0);
#endif
                currentState = DETERMINATE_BAUD_485_I;
                break;
            }
            else
            {
                digitalWrite(RE, LOW);
                SerialBT.println(String(debugging.sta_5) + String(UARTparam.baud) + " RS232 INVERTED");
                UARTparam.inverted = false;
                UARTparam.rxd = RXD_485;
                UARTparam.txd = TXD_485;
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
                Blue_send(debugging.err_1);
#endif
                currentState = SEND_FAIL;
                break;
            }
            else
            {
                digitalWrite(RE, LOW);
                SerialBT.println(String(debugging.sta_5) + String(UARTparam.baud) + " RS485 INVERTED");
                UARTparam.inverted = false;
                UARTparam.rxd = RXD_485;
                UARTparam.txd = TXD_485;
                currentState = INIT_UART;
                break;
            }
        }

        case INIT_UART:
        {
            Blue_send(debugging.sta_14);
            initUART();
            if (petitionMode)
            {
                currentState = SEND_BY_PETITION;
                break;
            }
            currentState = SEND_BY_TIME;
            break;
        }

        case SEND_BY_PETITION:
        {
            if (isAnyone())
            {
                if (petition)
                {
                    sendByPetition();
                    petition = false;
                }
                break;
            }
            else
            {
                ESP.restart();
            }
            break;
        }

        case SEND_BY_TIME:
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

        delay(2);
    }
}

void setup()
{
    setupPreferences();
    Blue_setup(deviceName);
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

void serial_flush(void)
{
    initUART();
    while (Serial.read() > 0)
        ;
}

void initUART()
{
    Serial.end();
    Serial.begin(UARTparam.baud, UARTparam.parity, UARTparam.rxd, UARTparam.txd, UARTparam.inverted, UARTparam.timeout);
    if (couldDetectUartConfig)
    {
        Serial.onReceiveError(onRxInterrups);
    };
    Serial.setRxBufferSize(INCOME_BUFFER * 2);
    Serial.setTimeout(UARTparam.timeout);
}

long getBestTimeout()
{
    long time_ = 0;
    do
    {
        long new_time_ = timeFlags(LOW);
        Serial.read();
        if (new_time_ > time_)
        {
            time_ = new_time_;
        }
    } while (Serial.available());

    return time_;
}

long timeFlags(int flag, int pin_)
{
    int init_ = micros();

    if (UARTparam.isRS232)
    {
        while (digitalRead(RXD_232) == flag)
            ;
        int final_ = micros();
        return final_ - init_;
    }
    else
    {
        while (digitalRead(RXD_485) == flag)
            ;
        int final_ = micros();
        return final_ - init_;
    }

    if (pin != RXD_232)
    {
        while (digitalRead(pin_) == flag)
            ;
        int final_ = micros();
        return final_ - init_;
    }

    return 0;
}

void findEOL()
{
    char common_EOL[] = {'\r', '\n', '\t', '\v', '\f', ' ', '=', '-'};
    for (char character : common_EOL)
    {
        if (Serial.available() && Serial.peek() == character)
        {
            UARTparam.EOL = character;
            return;
        }
    }
    UARTparam.EOL = '\n';
}

bool detectNonAscii(const char *tempBuffer)
{
    int count = 0;

    for (int index_ = 0; index_ < 128; index_++)
    {
        int ascii_char_ = int(tempBuffer[index_]);
        if (ascii_char_ <= 0 || ascii_char_ > 128)
        {
            count++;
        }
    }

    if (count > 10)
    {
        return true;
    }

    return false;
}

void readFromUART()
{
    char msg_[INCOME_BUFFER + 8];
    serial_flush();

    Serial.readBytes(msg_, INCOME_BUFFER + 8);

    if (millis() - currentCheckAscii > 10000)
    {
        if (couldDetectUartConfig && detectNonAscii(msg_))
        {
            currentState = CHANGE_UART_CONFIG;
            return;
        }

        currentCheckAscii = millis();
    }

    String str_ = String(std::string(msg_).c_str()).substring(4, str_.length() - 4);
    Blue_send(str_);
}

void sendByPetition()
{
    readFromUART();
}

void sendData()
{
    if (millis() - currentTimeSendMessage >= sendTime && sendToDevice)
    {
        readFromUART();
        currentTimeSendMessage = millis();
    }
}

void askKey()
{
    if (askForKey(pinc.c_str()))
    {
        SerialBT.println(debugging.sta_0);
        // SerialBT.println("key: " + String(keyring));
        currentState = MANAGE_DEVICE_CONFIGS; // CHANGE THIS TO SEND_TEST IF WANT TO TEST RANDOM NUMERS
        return;
    }
    SerialBT.println(debugging.err_2);
}

void setupPreferences()
{
    config.begin("config", false);

    INCOME_BUFFER = config.getInt("buffer", 64);
    sendTime = config.getInt("time", 1000);
    sendToDevice = config.getBool("send", true);
    pinc = config.getString("pinc", master);

    UARTparam.baud = config.getInt("baud", 115200);
    UARTparam.isAuto = config.getBool("isAuto", true);
    UARTparam.isRS232 = config.getBool("isRS232", true);
    couldDetectUartConfig = config.getBool("couldDetectUartConfig", false);
    couldDetectUartConfig = config.getBool("couldDetectUartConfig", false);
    if (!UARTparam.isAuto)
    {
        UARTparam.parity = config.getInt("parity", SERIAL_8N1);
    }
    UARTparam.timeout = config.getInt("rx_timeout", 2000);
    petitionMode = config.getBool("petitionMode", true);
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

/*
void test(){
    Serial.onReceive();
    Serial.readString();
    //Serial.onReceiveError();
    Serial.readBytes(msg, INCOME_BUFFER);
}
*/