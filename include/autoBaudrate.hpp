#include <Arduino.h>
//#include <HardwareSerial.h>
#include <esp32-hal-uart.h>
#include "rgbLeds.hpp"

myLeds leds(PIN_RED, PIN_GREEN, PIN_BLUE);

int detRate(int RXD, int TXD, bool isRS232 = true);

int numbers[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
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
        else if (str.length() >= 8)
        {
            return true;
        }
=======
>>>>>>> parent of f56d8df (Multi core update)
        else
        {
            return false;
        }
    }
    return false;
}

int testRS232(int baud, int rx_, int tx_, bool isInverted = false)
{
    // Serial.println("Testing: " + String(baud) + " bauds.");
    BLE_notify("Testing: " + String(baud) + " bauds.\n");
    Serial.end();
    Serial1.begin(baud, SERIAL_8N1, rx_, tx_, isInverted);
    Serial1.setTimeout(2000);
    String incoming = Serial1.readString();
    BLE_notify("> " + incoming + "\n");
    if (areAnyKnownCharacter(incoming.c_str()))
    {
        BLE_notify("Correct config found at RS232\n");
        Serial1.end();
        return baud;
    }
    return 0;
}

int testRS485(int baud, int rx_, int tx_, bool isinverted = true)
{
    // ITERATE OVER RS485
    // Serial.println("Testing: " + String(baud) + " bauds.");
    BLE_notify("Testing: " + String(baud) + " bauds.\n");
    Serial1.begin(baud, SERIAL_8N1, rx_, tx_, isinverted);
    Serial1.setTimeout(2000);
    digitalWrite(TX, LOW);
    String incoming = Serial1.readString();
    BLE_notify("> " + incoming + "\n");
    if (areAnyKnownCharacter(incoming.c_str()))
    {
        BLE_notify("Correct confog found at RS485\n");
        Serial1.end();
        return baud;
    }

    return 0;
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
            count++;
<<<<<<< HEAD
            leds.blink(count, false);
            int baud0 = testRS232(baud, RXD, TXD);
            int baud1 = testRS232(baud, RXD, TXD, true);
            if(baud0 != 0){return baud0;}
            if(baud1 != 0){return baud1;}
=======
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
>>>>>>> parent of f56d8df (Multi core update)
            if (count == 5)
            {
                count = 0;
            }
        }
        else
        {
            count++;
<<<<<<< HEAD
            leds.blink(count, false);
            int baud0 = testRS485(baud, RXD, TXD);
            int baud1 = testRS485(baud, RXD, TXD, true);
            if(baud0 != 0){return baud0;}
            if(baud1 != 0){return baud1;}
=======
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
>>>>>>> parent of f56d8df (Multi core update)
            if (count == 5)
            {
                count = 0;
            }
        }
    }
    BLE_notify("Failed to detect any baud at the serial ports (RS232/RS485)\n");
    return 1;
}
