#include <Arduino.h>
//#include <HardwareSerial.h>
#include <esp32-hal-uart.h>
#include "rgbLeds.hpp"

myLeds leds(PIN_RED, PIN_GREEN, PIN_BLUE);

int detRate(int RXD, int TXD, bool isRS232 = true);

class autoBaudrate
{
private:
    int count = 0;
    int bauds[14] = {9600, 110, 300, 600, 1200, 115200, 57600, 2400, 4800, 14400, 19200, 38400, 128000, 256000};
    int rxd_pin, txd_pin;
    int currentBaudTest = 0;
public:
    autoBaudrate(int RXD, int TXD)
    {
        rxd_pin = RXD;
        txd_pin = TXD;
    }

    int test232(bool isInverted = false)
    {
        Serial.end();
        leds.blink(count, true);
        count++;
        // Serial.println("Testing: " + String(baud) + " bauds.");
        BLE_notify("Testing: " + String(currentBaudTest) + " bauds.\n");
        Serial1.begin(currentBaudTest, SERIAL_8N1, rxd_pin, txd_pin, isInverted, 1000);
        // Serial1.setTimeout(2000);
        char buffer0[30];
        int len1 = Serial1.readBytes(buffer0, 50);
        for (int i = 0; i < len1; i++)
        {
            if (areAnyKnownCharacter(String(buffer0[i])))
            {
                BLE_notify("Correct config found at RS232\n");
                Serial1.end();
                return currentBaudTest;
            }
        }
        if (count == 5)
        {
            count = 0;
        }
        return 0;
    }

    int test485(bool isInverted = false)
    {
        leds.blink(count, true);
        count++;
        // Serial.println("Testing: " + String(baud) + " bauds.");
        BLE_notify("Testing: " + String(currentBaudTest) + " bauds.\n");
        Serial.end();
        Serial1.begin(currentBaudTest, SERIAL_8N1, rxd_pin, txd_pin, isInverted, 1000);
        digitalWrite(TX, LOW);
        // Serial1.setTimeout(2000);
        char buffer0[30];
        int len1 = Serial1.readBytes(buffer0, 50);
        for (int i = 0; i < len1; i++)
        {
            if (areAnyKnownCharacter(String(buffer0[i])))
            {
                BLE_notify("Correct config found at RS232\n");
                Serial1.end();
                return currentBaudTest;
            }
        }
        if (count == 5)
        {
            count = 0;
        }
        return 0;
    }

    bool areAnyKnownCharacter(String str_)
    {
        int numbers[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        std::string str = std::string(str_.c_str());
        for (int num : numbers)
        {
            if (str.find("\r") != std::string::npos)
            {
                return true;
            }
            else if (str.find("\n") != std::string::npos)
            {
                return true;
            }
            else if (str.find("=") != std::string::npos)
            {
                return true;
            }
            else if (str.find("-") != std::string::npos)
            {
                return true;
            }
            else if (str.find(String(num).c_str()) != std::string::npos)
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
    int detRate(bool isRS232)
    {
        for (int baud : bauds)
        {
            // ITERATE OVER RS232
            if (isRS232)
            {
                currentBaudTest = baud;
                if (test232() != 0)
                {
                    return currentBaudTest;
                }
                if (test232(true) != 0)
                {
                    return currentBaudTest;
                }
                currentBaudTest = 0;
            }
            else
            {
                currentBaudTest = baud;
                if (test485() != 0)
                {
                    return currentBaudTest;
                }
                if (test485(true) != 0)
                {
                    return currentBaudTest;
                }
                currentBaudTest = 0;
            }
        }
        BLE_notify("Failed to detect any baud at the serial ports (RS232/RS485)\n");
        return 1;
    }
};
