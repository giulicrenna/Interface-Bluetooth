#ifndef OLDBT
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#endif
#ifdef OLDBT
#include <BluetoothSerial.h>
#endif

#ifndef OLDBT
/* define the UUID that our custom service will use */
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6e400002‑b5a3‑f393‑e0a9‑e50e24dcca9e"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
char value[50] = "Default";

#define customService BLEUUID((uint16_t)0x1700)
BLECharacteristic customCharacteristic("6e400003‑b5a3‑f393‑e0a9‑e50e24dcca9e", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *customCharacteristic)
    {
        std::string rcvString = customCharacteristic->getValue();
        if (rcvString.length() > 0)
        {
            Serial.println("Value Received from BLE: ");
            for (int i = 0; i < rcvString.length(); ++i)
            {
                Serial.print(rcvString[i]);
                value[i] = rcvString[i];
            }
            for (int i = rcvString.length(); i < 50; ++i)
            {
                value[i] = NULL;
            }
            customCharacteristic->setValue((char *)&value);
        }
        else
        {
            Serial.println("Empty Value Received!");
        }
    }
};

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0)
        {
            Serial.println("*********");
            Serial.print("Received Value: ");
            for (int i = 0; i < rxValue.length(); i++)
                Serial.print(rxValue[i]);

            Serial.println();
            Serial.println("*********");
        }
    }
};

// Security class
class MySecurity : public BLESecurityCallbacks
{

    uint32_t onPassKeyRequest()
    {

        ESP_LOGI(LOG_TAG, "PassKeyRequest");

        return pin;
    }

    void onPassKeyNotify(uint32_t pass_key)
    {

        ESP_LOGI(LOG_TAG, "The passkey Notify number:%d", pass_key);
    }

    bool onConfirmPIN(uint32_t pass_key)
    {

        ESP_LOGI(LOG_TAG, "The passkey YES/NO number:%d", pass_key);

        vTaskDelay(5000);

        return true;
    }

    bool onSecurityRequest()
    {

        ESP_LOGI(LOG_TAG, "SecurityRequest");

        return true;
    }

    void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl)
    {

        ESP_LOGI(LOG_TAG, "Starting BLE work!");
    }
};

void BLE_setup()
{

    // Create the BLE Device
    BLEDevice::init(deviceName);

    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);

    BLEDevice::setSecurityCallbacks(new MySecurity());

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY);

    pTxCharacteristic->addDescriptor(new BLE2902());

    // Custom characteristic
    customCharacteristic.setCallbacks(new MyCharacteristicCallbacks());
    pServer->getAdvertising()->addServiceUUID(customService);

    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE);

    pRxCharacteristic->setCallbacks(new MyCallbacks());

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->start();
    Serial.println("Waiting a client lpm...");

    customCharacteristic.setValue((char *)&value);

    // Security Stuff
    BLESecurity *pSecurity = new BLESecurity(); // pin

    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;

    uint32_t passkey = pin; // PASS

    uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;

    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));

    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);

    pSecurity->setCapability(ESP_IO_CAP_OUT);

    pSecurity->setKeySize(16);

    esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t));

    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));

    // advertisement config

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void BLE_notify(String message = "")
{
    if (deviceConnected)
    {
        pTxCharacteristic->setValue(message.c_str());
        pTxCharacteristic->notify();
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500);                  // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}

#endif

#ifdef OLDBT

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
boolean confirmRequestPending = true;

void BTConfirmRequestCallback(uint32_t numVal)
{
    confirmRequestPending = true;
}

void BTAuthCompleteCallback(boolean success)
{
    confirmRequestPending = false;
    if (success)
    {
        // Serial.println("Pairing success!!");
    }
    else
    {
        //   Serial.println("Pairing failed, rejected by user!!");
    }
}

void BLE_setup()
{
    //SerialBT.enableSSP();
    //SerialBT.onConfirmRequest(BTConfirmRequestCallback);
    //SerialBT.onAuthComplete(BTAuthCompleteCallback);
  //  SerialBT.setPin("7419");
    SerialBT.begin(deviceName,true);
  //  SerialBT.setPin("7419");
}
void BLE_notify(String message = "")
{
    SerialBT.println(message);
}
#endif