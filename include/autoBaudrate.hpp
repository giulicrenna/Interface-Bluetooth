#include <Arduino.h>
//#include <HardwareSerial.h>
#include <esp32-hal-uart.h>
#include "rgbLeds.hpp"

myLeds leds(PIN_RED, PIN_GREEN, PIN_BLUE);

int detRate(int RXD, int TXD, bool isRS232 = true);

class autoBAudrate
{
public: 
  
    int rxd_pin, txd_pin;
    bool isRS232_;
    autoBAudrate(int RXD, int TXD, bool isRS232)
    {
        rxd_pin = RXD;
        txd_pin = TXD;
        isRS232_ = isRS232;
    }

private:
};

bool areAnyKnownCharacter(std::string str)
{
    int numbers[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
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
        else
        {
            return false;
        }
    }
    return false;
}

// Auto Baudrate
int detRate(int RXD, int TXD, bool isRS232)
{
    int count = 0;
    int bauds[14] = {9600, 110, 300, 600, 1200, 115200, 57600, 2400, 4800, 14400, 19200, 38400, 128000, 256000};
    for (int baud : bauds)
    {
        // ITERATE OVER RS232
        if (isRS232)
        {
            leds.blink(count, true);
            count++;
            //Serial.println("Testing: " + String(baud) + " bauds.");
            BLE_notify("Testing: " + String(baud) + " bauds.\n");
            Serial.end();
            Serial1.begin(baud, SERIAL_8N1, RXD, TXD);
            Serial1.setTimeout(1500);
            String incoming = Serial1.readString();
            if (areAnyKnownCharacter(incoming.c_str()))
            {
                BLE_notify("Correct config found at RS232\n");
                Serial1.end();
                return baud;
            }
            if (count == 5)
            {
                count = 0;
            }
        }
        else
        {
            // ITERATE OVER RS485
            leds.blink(count, false);
            count++;
            //Serial.println("Testing: " + String(baud) + " bauds.");
            BLE_notify("Testing: " + String(baud) + " bauds.\n");
            Serial.end();
            Serial1.begin(baud, SERIAL_8N1, RXD, TXD);
            Serial1.setTimeout(1500);
            digitalWrite(TX, LOW);
            String incoming = Serial1.readString();
            if (areAnyKnownCharacter(incoming.c_str()))
            {
                BLE_notify("Correct confog found at RS485\n");
                Serial1.end();
                return baud;
            }
            if (count == 5)
            {
                count = 0;
            }
        }
    }
    BLE_notify("Failed to detect any baud at the serial ports (RS232/RS485)\n");
    return 1;
}
