#include <SoftwareSerial.h>

const byte ModReadBuffer[] = {0x01, 0x03, 0x00, 0x1e,
                              0x00, 0x01, 0xe4, 0x0c};
byte BufferValueRS485[8];

byte ModbusData();

SoftwareSerial mod(RXD, TXD); // RX=26 , TX =27

int setupRS485(uint8_t baudRate)
{
    mod.begin(baudRate);
    pinMode(RE, OUTPUT);
    pinMode(DE, OUTPUT);

    return 0;
}

byte communicationRS485()
{
    byte i;
    digitalWrite(DE, HIGH);
    digitalWrite(RE, HIGH);
    delay(10);
    if (mod.write(ModReadBuffer, sizeof(ModReadBuffer)) == 8)
    {
        digitalWrite(DE, LOW);
        digitalWrite(RE, LOW);
        for (i = 0; i < 4; i++)
        {
            // Serial.print(mod.read(),HEX);
            BufferValueRS485[i] = mod.read();
        }

        // }
    }
    return BufferValueRS485[8];
}
