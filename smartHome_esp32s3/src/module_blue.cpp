#include "module_blue.h"
#if USE_BLE

#include "confighelpr.h"
#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <lvglconfig.h>
#include <map>

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

            // 解析命令
            std::map<String, String> configMap;
            //
            int index = resStr.indexOf(',');
            if (index != -1) {
                String substring = resStr.substring(0, index);
                String substring2 = resStr.substring(index + 1);

                int index1 = substring.indexOf(':');
                if (index1 != -1) {
                    String key = substring.substring(0, index1);
                    String value = substring.substring(index1 + 1);
                    configMap[key] = value;
                }

                int index2 = substring2.indexOf(':');
                if (index2 != -1) {
                    String key = substring2.substring(0, index2);
                    String value = substring2.substring(index2 + 1);
                    configMap[key] = value;
                }
            } else {
                int index1 = resStr.indexOf(':');
                if (index1 != -1) {
                    String key = resStr.substring(0, index1);
                    String value = resStr.substring(index1 + 1);
                    configMap[key] = value;
                }
            }
            for (const auto &element : configMap) {
                Serial.print("key: ");
                Serial.print(element.first.c_str());
                Serial.print(", value: ");
                Serial.println(element.second.c_str());
                lv_ai_control_offline(element.first, element.second.toInt());
            }
            resStr = "";
        }
    }
};

void initBLE()
{
    Serial.println("initBLE");
    // 初始化蓝牙设备，设置蓝牙名称
    bleServerName = "smartHome_esp32_" + username;
    Serial.println(bleServerName);
    BLEDevice::init(bleServerName.c_str());
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
    Serial.println("startBLE");
    initBLE();
    // 开启服务
    pService->start();
    // 开启服务器广播
    pServer->getAdvertising()->start();
    Serial.println("Waiting a client connection to notify...");
}

void stopBLE()
{
    Serial.println("stopBLE");
    pService->stop();
    pServer->getAdvertising()->stop();
}

void BLELoop()
{
    if (enableBLE) {
        if (deviceConnected) {
        }
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

void sendIntDataBLE(int num)
{
    pTxCharacteristic->setValue(num);
    pTxCharacteristic->notify();
}

void sendStrDataBLEStr(const String &str)
{
    pTxCharacteristic->setValue(str.c_str());
    pTxCharacteristic->notify();
}
#endif
