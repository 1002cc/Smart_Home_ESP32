#include "module_blue.h"
#include <Arduino.h>
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>

#define SERVICE_UUID "3e1a2f0e-dc52-4762-acdd-472652ba31bb"
#define CHARACTERISTIC_UUID_RX "21d4853d-cff3-4c3c-9d8a-8b8b1a30a3f6"
#define CHARACTERISTIC_UUID_TX "c3510192-908b-482a-9710-ff2e0c4cee60"

static BLEAddress *pServerAddress;
extern String username;
static String bleServerName;

static boolean doConnect = false;
static boolean connected = false;

static BLERemoteCharacteristic *pRemoteCharacteristic_RX;
static BLERemoteCharacteristic *pRemoteCharacteristic_TX;

const uint8_t notification[] = {0x1, 0x0};

BLEScan *pBLEScan;

static void notifyRXCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.write(pData, length);
    Serial.println();
}

class BLEClientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient *pclient)
    {
        Serial.println("Connected to server");
    }

    void onDisconnect(BLEClient *pclient)
    {
        Serial.println("Disconnected from server");
        connected = false;
        doConnect = true;
    }
};

bool connectToServer(BLEAddress pAddress)
{
    Serial.print("Forming a connection to ");
    // 创建蓝牙客户端
    BLEClient *pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new BLEClientCallback());

    pClient->connect(pAddress);
    Serial.println("Connected to server");

    BLERemoteService *pRemoteService = pClient->getService(SERVICE_UUID);
    if (pRemoteService == nullptr) {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(SERVICE_UUID);
        return false;
    }
    Serial.println("Found our service");

    pRemoteCharacteristic_RX = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID_RX);
    if (pRemoteCharacteristic_RX == nullptr) {
        Serial.print("Failed to find our RX characteristic UUID: ");
        Serial.println(CHARACTERISTIC_UUID_RX);
        return false;
    }
    Serial.println(" Found our RX characteristic");

    pRemoteCharacteristic_TX = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID_TX);
    if (pRemoteCharacteristic_TX == nullptr) {
        Serial.print("Failed to find our TX characteristic UUID: ");
        Serial.println(CHARACTERISTIC_UUID_TX);
        return false;
    }
    Serial.println(" - Found our TX characteristic");

    std::string value = pRemoteCharacteristic_RX->readValue();
    Serial.print("The characteristic Value was: ");
    Serial.println(value.c_str());

    pRemoteCharacteristic_RX->registerForNotify(notifyRXCallback);
    connected = true;
    return true;
}

// 广播扫描蓝牙信号
class bleAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        if (advertisedDevice.getName() == bleServerName) {                  // Check if the name of the advertiser matches
            advertisedDevice.getScan()->stop();                             // Scan can be stopped, we found what we are looking for
            pServerAddress = new BLEAddress(advertisedDevice.getAddress()); // Address of advertiser is the one we need
            doConnect = true;                                               // Set indicator, stating that we are ready to connect
            Serial.println("Device found. Connecting!");
        }
    }
};

void initBLE()
{
    BLEDevice::init("");
}

void startBLE()
{
    bleServerName = "smartHome_esp32_" + username;
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new bleAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(30);
}

void stopBLE()
{
    pBLEScan->stop();
}

void BLELoop()
{
    if (doConnect == true) {
        if (connectToServer(*pServerAddress)) {
            Serial.println("We are now connected to the BLE Server.");
            pRemoteCharacteristic_RX->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t *)notification, 2, true);
            connected = true;
        } else {
            Serial.println("We have failed to connect to the server; there is nothing more we will do.");
        }
        doConnect = false;
    }
    if (connected) {
        // String message = "getid";
        // pRemoteCharacteristic_TX->writeValue(message.c_str(), message.length());
    }
}