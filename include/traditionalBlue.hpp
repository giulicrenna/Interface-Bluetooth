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


void determinateParity(String parity);

void BTConfirmRequestCallback(uint32_t numVal)
{
    confirmRequestPending = true;
    keyring = numVal;
}

void BTAuthCompleteCallback(boolean success)
{
    confirmRequestPending = false;
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

    // SerialBT.enableSSP();
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
    SerialBT.println(msg);
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
            SerialBT.println("**Enviar clave o configuracion nueva**");
            currentTimeForPairing = millis();
            // SerialBT.println(_pin_);
        }

        if (SerialBT.available() > 0)
        {
            String command = SerialBT.readString();
            command.trim();
            String cmd[5];
            std::vector<std::string> cmd_v = mstd::strip(command.c_str(), ':');
            for (int k = 0; k < cmd_v.size(); k++)
            {
                cmd[k] = String(cmd_v.at(k).c_str());
            }

            if (cmd[0] == "KEY")
            {
                SerialBT.println("\nValidando clave " + cmd[1] + "...");
                delay(1000);

                return (cmd[1] == _pin_ ? true : false);
            }
            else if (cmd[0] == "PARIDAD")
            {
                determinateParity(cmd[1]);
                SerialBT.println(debugging.sta_7 + cmd[1]);
            }
            else if (cmd[0] == "UART")
            {
                if (cmd[1] == "AUTO")
                {
                    UARTparam.isAuto = true;
                    SerialBT.println(debugging.sta_8);
                }
                else
                {
                    UARTparam.baud = std::stoi(cmd[1].c_str());
                    UARTparam.isAuto = false;
                    try
                    {
                        if (cmd[2] == "RS232")
                        {
                            UARTparam.isRS232 = true;
                        }
                        else if (cmd[2] == "RS232")
                        {
                            UARTparam.isRS232 = false;
                        }
                    }
                    catch (const std::exception &e)
                    {
                        SerialBT.println(debugging.sta_9);
                    }
                    determinateParity(cmd[3]);
                    SerialBT.println(debugging.sta_9 + cmd[1]);
                }
            }
        }
        if (!isAnyone())
        {
            ESP.restart();
        }
        cnt += 1;
    }
    return false;
}

void determinateParity(String parity)
{
    if (parity == "SERIAL_5N1")
    {
        UARTparam.parity = SERIAL_5N1;
    }
    else if (parity == "SERIAL_6N1")
    {
        UARTparam.parity = SERIAL_6N1;
    }
    else if (parity == "SERIAL_7N1")
    {
        UARTparam.parity = SERIAL_7N1;
    }
    else if (parity == "SERIAL_8N1")
    {
        UARTparam.parity = SERIAL_8N1;
    }
    else if (parity == "SERIAL_5N2")
    {
        UARTparam.parity = SERIAL_5N2;
    }
    else if (parity == "SERIAL_6N2")
    {
        UARTparam.parity = SERIAL_6N2;
    }
    else if (parity == "SERIAL_7N2")
    {
        UARTparam.parity = SERIAL_7N2;
    }
    else if (parity == "SERIAL_8N2")
    {
        UARTparam.parity = SERIAL_8N2;
    }
    else if (parity == "SERIAL_5E1")
    {
        UARTparam.parity = SERIAL_5E1;
    }
    else if (parity == "SERIAL_6E1")
    {
        UARTparam.parity = SERIAL_6E1;
    }
    else if (parity == "SERIAL_7E1")
    {
        UARTparam.parity = SERIAL_7E1;
    }
    else if (parity == "SERIAL_8E1")
    {
        UARTparam.parity = SERIAL_8E1;
    }
    else if (parity == "SERIAL_5E2")
    {
        UARTparam.parity = SERIAL_5E2;
    }
    else if (parity == "SERIAL_6E2")
    {
        UARTparam.parity = SERIAL_6E2;
    }
    else if (parity == "SERIAL_7E2")
    {
        UARTparam.parity = SERIAL_7E2;
    }
    else if (parity == "SERIAL_8E2")
    {
        UARTparam.parity = SERIAL_8E2;
    }
    else if (parity == "SERIAL_5O1")
    {
        UARTparam.parity = SERIAL_5O1;
    }
    else if (parity == "SERIAL_6O1")
    {
        UARTparam.parity = SERIAL_6O1;
    }
    else if (parity == "SERIAL_7O1")
    {
        UARTparam.parity = SERIAL_7O1;
    }
    else if (parity == "SERIAL_8O1")
    {
        UARTparam.parity = SERIAL_8O1;
    }
    else if (parity == "SERIAL_5O2")
    {
        UARTparam.parity = SERIAL_5O2;
    }
    else if (parity == "SERIAL_6O2")
    {
        UARTparam.parity = SERIAL_6O2;
    }
    else if (parity == "SERIAL_7O2")
    {
        UARTparam.parity = SERIAL_7O2;
    }
    else if (parity == "SERIAL_8O2")
    {
        UARTparam.parity = SERIAL_8O2;
    }
}