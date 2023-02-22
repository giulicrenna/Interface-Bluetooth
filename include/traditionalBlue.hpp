#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

uint32_t keyring = 0;
int currentTimeForPairing = 0;
String MACadd = "AA:BB:CC:11:22:33";
uint8_t address[6] = {0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33};
boolean confirmRequestPending = true;

void BTConfirmRequestCallback(uint32_t numVal)
{
    confirmRequestPending = true;
    keyring = numVal;
}

void BTAuthCompleteCallback(boolean success)
{
    confirmRequestPending = false;
    Serial.print("\nDevice Connected... Closing serial\n");
}

void Blue_setup(const char *deviceName, const char *pin_)
{
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;

    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;

    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t)); // Missing on library when SSP is not active!

    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_FIXED;
    esp_bt_pin_code_t pin_code;

    pin_code[0] = '1';
    pin_code[1] = '2';
    pin_code[2] = '3';
    pin_code[3] = '4';
    esp_bt_gap_set_pin(pin_type, 4, pin_code);

    SerialBT.enableSSP();
    SerialBT.onConfirmRequest(BTConfirmRequestCallback);
    SerialBT.onAuthComplete(BTAuthCompleteCallback);
    SerialBT.begin(deviceName);
}

void Blue_send(char character)
{
    SerialBT.write(character);
}

void Blue_send(const char *msg)
{
    SerialBT.print(msg);
}

void Blue_send(String msg)
{
    SerialBT.print(msg);
}

bool confirmAuth()
{
    SerialBT.confirmReply(true);
    if (confirmRequestPending)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool isAnyone()
{
    if (SerialBT.hasClient() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool askForKey(const char *_pin_)
{
    int cnt = 0;
    while (true)
    {
        if (millis() - currentTimeForPairing >= 2000)
        {
            SerialBT.println("**Send user key**");
            currentTimeForPairing = millis();
        }

        if (SerialBT.available() > 0)
        {
            String key = SerialBT.readString();
            key.trim();

            SerialBT.println("\nValidating key " + key + "...");
            delay(1000);

            return (key == _pin_ ? true : false);
        }
        cnt += 1;
    }
    return false;
}