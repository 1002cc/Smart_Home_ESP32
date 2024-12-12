#include "module_blue.h"
#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

// BLE服务器端

BLEServer *pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

#define SERVICE_UUID "3e1a2f0e-dc52-4762-acdd-472652ba31bb"
#define CHARACTERISTIC_UUID_RX "c3510192-908b-482a-9710-ff2e0c4cee60"
#define CHARACTERISTIC_UUID_TX "21d4853d-cff3-4c3c-9d8a-8b8b1a30a3f6"

extern String username;
String bleServerName = "";
String resStr = "";

bool enableBLE = false;

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

class BLERXCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0) {
            Serial.println("*********");
            Serial.print("Received Value: ");
            for (int i = 0; i < rxValue.length(); i++) {
                Serial.print(rxValue[i]);
                resStr += rxValue[i];
            }
            Serial.println();
            Serial.println("*********");
            if (resStr == "getid") {
                pTxCharacteristic->setValue("nihao");
                pTxCharacteristic->notify();
            } else if (resStr == "light1on") {
                Serial.println("light1on");
            } else if (resStr == "light1off") {
                Serial.println("light1off");
            } else if (resStr == "light2on") {
                Serial.println("light2on");
            } else if (resStr == "light2off") {
                Serial.println("light2off");
            }
            resStr = "";
        }
    }
};

void initBLE()
{
    // 初始化蓝牙设备，设置蓝牙名称
    bleServerName = "smartHome_esp32_" + username;
    BLEDevice::init(bleServerName);
    // 创建蓝牙服务器
    pServer = BLEDevice::createServer();
    // 设置服务器回调函数
    pServer->setCallbacks(new MyServerCallbacks());

    // 创建服务
    pService = pServer->createService(SERVICE_UUID);

    // 创建特征RX和TX
    pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY);
    pTxCharacteristic->addDescriptor(new BLE2902());

    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE);
    pRxCharacteristic->setCallbacks(new BLERXCallbacks());
}

void startBLE()
{
    // 开启服务
    pService->start();
    // 开启服务器广播
    pServer->getAdvertising()->start();
    Serial.println("Waiting a client connection to notify...");
}

void stopBLE()
{
    pService->stop();
    pServer->getAdvertising()->stop();
}

void BLELoop()
{
    if (enableBLE) {
        // if (deviceConnected) {

        // }
        if (!deviceConnected && oldDeviceConnected) {
            delay(500);
            pServer->startAdvertising();
            Serial.println("start advertising");
            oldDeviceConnected = deviceConnected;
        }

        if (deviceConnected && !oldDeviceConnected) {
            Serial.println("has device connected");
            oldDeviceConnected = deviceConnected;
        }
    }
}
