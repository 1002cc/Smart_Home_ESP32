#include "module_mqtt.h"
#include "confighelpr.h"
#include "lvgl.h"
#include "lvglconfig.h"
#include "module_audio.h"
#include "module_service.h"
#include <PubSubClient.h>
#include <TJpg_Decoder.h>
#include <WiFiClientSecure.h>

const char *mqtt_url = "47.115.139.166";
String mqtt_sub = "";
String mqtt_pub = "";
String mqtt_cam_pub = "";
const uint16_t mqtt_broker_port = 1883;
const uint16_t mqtt_client_buff_size = 5 * 1024;
const char *mqtt_username = "esp32";
const char *mqtt_password = "1002";
String mqtt_client_id = "esp32_s3_";
const int mqtt_keepalive = 60;
WiFiClient espClient;
PubSubClient mqttClient(espClient);

extern lampButtonData mqttSwitchState;
extern detectionDate detectiondatas;

bool enable_mqtt = true;
extern bool loginState;
String username = "hich";
extern String FirmwareVersion;

static void
mqtt_callback(char *topic, byte *payload, unsigned int length);
bool firstConnectMQTT(void);

bool initMQTTConfig(void)
{
    Serial.println("Init MQTT Config");
    mqttClient.setServer(mqtt_url, mqtt_broker_port);
    mqttClient.setBufferSize(mqtt_client_buff_size);
    mqttClient.setCallback(mqtt_callback);
    mqttClient.setKeepAlive(mqtt_keepalive);
    mqtt_client_id += String(WiFi.macAddress());
    int isLogin = ReadintData("isLogin");
    if (isLogin != 1000 && isLogin == 1) {
        String userlogin = ReadData("username");
        if (userlogin != "null") {
            Serial.printf("userlogin : %s\n", userlogin);
            mqtMontage(userlogin);
            loginState = true;
            lv_setUser(userlogin);
            lv_setMQTTSwitchState(true);
            return firstConnectMQTT();
        }
    }
    Serial.printf("loginState is false \n");
    loginState = false;
    return false;
}

bool firstConnectMQTT(void)
{
    if (isNetworkAvailable()) {
        lv_setTipinfo("正在连接服务器");
        if (mqttconnect()) {
            lv_setTipinfo("服务器连接成功");
            lv_setMQTTSwitchState(true);
            lv_setMQTTState("已连接");
            lv_setstatusbarLabel(3);
            enable_mqtt = true;
            publishGetDatas();
        } else {
            lv_setTipinfo("服务器连接失败");
            lv_setMQTTSwitchState(false);
            lv_setMQTTState("未连接");
            if (WiFi.status() == WL_CONNECTED) {
                lv_setstatusbarLabel(1);
            } else {
                lv_setstatusbarLabel(0);
            }
            enable_mqtt = false;
        }
    } else {
        Serial.println("Network unavailable");
        lv_setTipinfo("网络不可用");
        lv_setMQTTSwitchState(false);
        lv_setMQTTState("未连接");
        enable_mqtt = false;
    }
    vTaskDelay(1000);
    return enable_mqtt;
}

bool mqttconnect(void)
{
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect(mqtt_client_id.c_str(), mqtt_username, mqtt_password)) {
        Serial.println("MQTT connected!");
        lv_setstatusbarLabel(3);
        mqttClient.publish(mqtt_pub.c_str(), "ESP32 S3 connect mqtt!");
        mqttClient.subscribe(mqtt_sub.c_str());
        lv_setMQTTState("已连接");
        publishGetDatas();
        return true;
    } else {
        Serial.print("failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 5 seconds");
        lv_setMQTTState("连接失败");
        if (WiFi.status() == WL_CONNECTED) {
            lv_setstatusbarLabel(1);
        } else {
            lv_setstatusbarLabel(0);
        }
        return false;
    }
}

void stopMQTT()
{
    enable_mqtt = false;
    mqttClient.disconnect();
}

void mqttLoop(void)
{
    if (enable_mqtt && loginState) {
        if (!mqttClient.connected() && WiFi.status() == WL_CONNECTED) {
            lv_setMQTTState("未连接");
            Serial.println("MQTT disconnected, reconnecting...");
            mqttconnect();
            vTaskDelay(5000);
        }
        mqttClient.loop();
    }
}

static void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
    if (strcmp(topic, mqtt_pub.c_str()) == 0) {
        return;
    }
    Serial.printf("Message arrived in topic %s, length %d\n", topic, length);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }

    String payloadString(payload, length);

    cJSON *root = cJSON_Parse(payloadString.c_str());
    if (root == NULL) {
        Serial.println("Failed to parse JSON!");
    } else {
        cJSON *datas = cJSON_GetObjectItem(root, "datas");
        if (datas != NULL) {
            cJSON *priState_j = cJSON_GetObjectItem(datas, "priState");
            if (priState_j != NULL) {
                detectiondatas.pri = priState_j->valueint;
                lv_setPriState(detectiondatas.pri);
            }
            cJSON *voiceState_j = cJSON_GetObjectItem(datas, "voiceState");
            if (voiceState_j != NULL) {
                detectiondatas.voice = voiceState_j->valueint;
                lv_setVoiceState(detectiondatas.voice);
            }
            cJSON *rainState_j = cJSON_GetObjectItem(datas, "rainState");
            if (rainState_j != NULL) {
                detectiondatas.rain = rainState_j->valueint;
                lv_setRainState(detectiondatas.rain);
            }
        }
        cJSON *switches = cJSON_GetObjectItem(root, "switches");
        if (switches != NULL) {
            cJSON *lampButton1_j = cJSON_GetObjectItem(switches, "lampButton1");
            if (lampButton1_j != NULL) {
                mqttSwitchState.lampButton1 = lampButton1_j->valueint;
                lv_setLampButton1(mqttSwitchState.lampButton1);
            }
            cJSON *lampButton2_j = cJSON_GetObjectItem(switches, "lampButton2");
            if (lampButton2_j != NULL) {
                mqttSwitchState.lampButton2 = lampButton2_j->valueint;
                lv_setLampButton2(mqttSwitchState.lampButton2);
            }
            cJSON *buttonFan_j = cJSON_GetObjectItem(switches, "fan");
            if (buttonFan_j != NULL) {
                mqttSwitchState.fan = buttonFan_j->valueint;
                lv_setButtonFan(mqttSwitchState.fan);
            }
            cJSON *buttonCurtain_j = cJSON_GetObjectItem(switches, "curtain");
            if (buttonCurtain_j != NULL) {
                mqttSwitchState.curtain = buttonCurtain_j->valueint;
                lv_setButtonCurtain(mqttSwitchState.curtain);
            }
            cJSON *buttonDoorcontact_j = cJSON_GetObjectItem(switches, "doorcontact");
            if (buttonDoorcontact_j != NULL) {
                mqttSwitchState.doorcontact = buttonDoorcontact_j->valueint;
                lv_setButtonDoorContact(mqttSwitchState.doorcontact);
            }
            cJSON *doorContactOpenSound_j = cJSON_GetObjectItem(switches, "doorContactOpenSound");
            if (doorContactOpenSound_j != NULL) {
                mqttSwitchState.openSound = doorContactOpenSound_j->valueint;
                lv_setButtonDoorContactOpenSound(mqttSwitchState.openSound);
            }
            cJSON *enableDoorContactTimeout_j = cJSON_GetObjectItem(switches, "enableDoorContactTimeout");
            if (enableDoorContactTimeout_j != NULL) {
                mqttSwitchState.timeout = enableDoorContactTimeout_j->valueint;
                cJSON *enableDoorContactTimeoutTime_j = cJSON_GetObjectItem(switches, "timeoutTime");
                if (enableDoorContactTimeoutTime_j != NULL) {
                    mqttSwitchState.timeoutTime = enableDoorContactTimeoutTime_j->valueint;
                    lv_setDropdownDoorContactTimeoutTime(mqttSwitchState.timeoutTime);
                }
                lv_setButtonDoorContactTimeout(mqttSwitchState.timeout);
            }
            cJSON *pri_j = cJSON_GetObjectItem(switches, "pri");
            if (pri_j != NULL) {
                mqttSwitchState.priButton = pri_j->valueint;
                lv_setPriButtonState(mqttSwitchState.priButton);
            }
            cJSON *voice_j = cJSON_GetObjectItem(switches, "voiceControl");
            if (voice_j != NULL) {
                mqttSwitchState.voiceButton = voice_j->valueint;
                lv_setVoiceButtonState(mqttSwitchState.voiceButton);
            }
        }
        cJSON *playmusic_j = cJSON_GetObjectItem(root, "playmusic");
        if (playmusic_j != NULL) {
            String musicUrl = playmusic_j->valuestring;
            if (!musicUrl.isEmpty()) {
                Serial.printf("play music url:%s\n", musicUrl.c_str());
                playMusicUrl(musicUrl);
            }
        }
        cJSON *heartbeat_j = cJSON_GetObjectItem(root, "heartbeat");
        if (heartbeat_j != NULL) {
            publishDeviceState();
        }
        cJSON *version_j = cJSON_GetObjectItem(root, "getVersion");
        if (version_j != NULL) {
            publishDeviceVersion();
        }
        cJSON *update_j = cJSON_GetObjectItem(root, "update");
        if (update_j != NULL) {
            String updateID = update_j->valuestring;
            if (updateID == "esp32s3") {
                if (getOTAVersion()) {
                    msgboxBarTip();
                    startOTATask();
                }
            }
        }
        cJSON_Delete(root);
    }
    Serial.println("\n----------------END----------------");
}

void publishSensorData(const SensorData &data)
{
    cJSON *root = cJSON_CreateObject();
    cJSON *datas = cJSON_CreateObject();
    char tempCharArray[32], humidityCharArray[32], mq2CharArray[32];
    snprintf(tempCharArray, sizeof(tempCharArray), "%.0f", data.temp);
    snprintf(humidityCharArray, sizeof(humidityCharArray), "%.0f", data.humidity);
    snprintf(mq2CharArray, sizeof(mq2CharArray), "%.0f", data.mq);
    cJSON_AddItemToObject(root, "datas", datas);
    cJSON_AddStringToObject(datas, "temp", tempCharArray);
    cJSON_AddStringToObject(datas, "humidity", humidityCharArray);
    cJSON_AddStringToObject(datas, "mq", mq2CharArray);
    char *jsonStr = cJSON_PrintUnformatted(root);
    mqttClient.publish(mqtt_pub.c_str(), jsonStr);
    cJSON_Delete(root);
}

/**
{
    "datas":
    {
        "getmessage": "hello",
        "temp": "28",
        "humidity": "11",
        "mq": "12"
    },
    "switches" : {
        "lampButton1":0,
        "lampButton2": 0
    }
}
 */

bool pulishSwitchDatas(const lampButtonData &data)
{
    if (mqttClient.state() == MQTT_CONNECTED) {
        cJSON *root = cJSON_CreateObject();
        cJSON *datas = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "switches", datas);
        cJSON_AddBoolToObject(datas, "lampButton1", data.lampButton1);
        cJSON_AddBoolToObject(datas, "lampButton2", data.lampButton2);

        char *jsonStr = cJSON_PrintUnformatted(root);
        Serial.println(jsonStr);
        mqttClient.publish(mqtt_pub.c_str(), jsonStr);
        cJSON_Delete(root);
        return true;
    } else {
        Serial.printf("lampButton1:%d,lampButton2:%d\n", data.lampButton1, data.lampButton2);
        Serial.println("mqtt disconnect");
        return false;
    }
}

bool pulishState(const String &object, const bool &state, const String &item)
{
    if (mqttClient.state() == MQTT_CONNECTED) {
        cJSON *root = cJSON_CreateObject();
        cJSON *datas = cJSON_CreateObject();
        cJSON_AddItemToObject(root, item.c_str(), datas);
        cJSON_AddBoolToObject(datas, object.c_str(), state);
        char *jsonStr = cJSON_PrintUnformatted(root);

        Serial.println(jsonStr);
        mqttClient.publish(mqtt_pub.c_str(), jsonStr);
        cJSON_Delete(root);
        return true;
    } else {
        Serial.println("mqtt disconnect");
        return false;
    }
}

bool pulishState_int(const String &object, const int &num, const String &item)
{
    if (mqttClient.state() == MQTT_CONNECTED) {
        cJSON *root = cJSON_CreateObject();
        cJSON *datas = cJSON_CreateObject();
        cJSON_AddItemToObject(root, item.c_str(), datas);
        cJSON_AddNumberToObject(datas, object.c_str(), num);
        char *jsonStr = cJSON_PrintUnformatted(root);
        Serial.println(jsonStr);
        mqttClient.publish(mqtt_pub.c_str(), jsonStr);
        cJSON_Delete(root);
        return true;
    } else {
        Serial.println("mqtt disconnect");
        return false;
    }
}

bool sendRePW()
{
    if (mqttClient.state() == MQTT_CONNECTED) {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddBoolToObject(root, "repw", true);
        char *jsonStr = cJSON_PrintUnformatted(root);

        Serial.println(jsonStr);
        mqttClient.publish(mqtt_pub.c_str(), jsonStr);
        cJSON_Delete(root);
        return true;
    } else {
        Serial.println("mqtt disconnect");
        return false;
    }
}

void publishGetDatas()
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddBoolToObject(root, "getdatas", true);
    char *jsonStr = cJSON_PrintUnformatted(root);
    mqttClient.publish(mqtt_pub.c_str(), jsonStr);
    cJSON_Delete(root);
}

void publishDeviceState()
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "devices", "esp32s3");
    char *jsonStr = cJSON_PrintUnformatted(root);
    mqttClient.publish(mqtt_pub.c_str(), jsonStr);
    cJSON_Delete(root);
}

void publishDeviceVersion()
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "version", FirmwareVersion.c_str());
    char *jsonStr = cJSON_PrintUnformatted(root);
    mqttClient.publish(mqtt_pub.c_str(), jsonStr);
    cJSON_Delete(root);
}

void publishVideoAbout(int value)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "code", "200");

    cJSON *dates = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "datas", dates);
    cJSON_AddNumberToObject(dates, "about", value);
    char *jsonStr = cJSON_PrintUnformatted(root);
    Serial.println(jsonStr);
    mqttClient.publish(mqtt_cam_pub.c_str(), jsonStr);
    cJSON_Delete(root);
}

bool publishMQTT(const char payload[])
{
    return mqttClient.publish(mqtt_pub.c_str(), payload);
}

bool subscribeMQTT(const char topic[])
{
    return mqttClient.subscribe(topic);
}

void mqtt_disconnect(void)
{
    mqttClient.disconnect();
}
bool getMqttStart()
{
    return mqttClient.connected();
}

void mqtMontage(const String &user)
{
    username = user;
    mqtt_sub = "/smartHome/" + user + "/#";
    mqtt_pub = "/smartHome/" + user + "/esp32s3";
    mqtt_cam_pub = "/smartHome/" + user + "/esp32_cam_pub";
    Serial.printf("mqtt_sub:%s\n", mqtt_sub.c_str());
    Serial.printf("mqtt_pub:%s\n", mqtt_pub.c_str());
    Serial.printf("mqtt_cam_pub:%s\n", mqtt_cam_pub.c_str());
}