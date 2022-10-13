#ifndef BLUEMAN_OUTLINE
#include <BluetoothSerial.h>
#endif

BluetoothSerial SerialBT;

int setupBluethooth()
{
    if (!SerialBT.begin(deviceID))
    {
        Serial.println("The device could not start the bluethooth daemon");
        return 1;
    }
    SerialBT.enableSSP();
    Serial.println("The device started, now you can pair it with bluetooth!");

    return 0;
}

int dataHermes(byte message)
{
    if (millis() - currentTimeBluethoothMessage > bluethoothMessageTime)
    {
        if (Serial.available())
        {
            SerialBT.write(message);
            Serial.write(SerialBT.read());
        }

        currentTimeBluethoothMessage = millis();
    }

    return 0;
}
