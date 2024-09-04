#include "module_mqtt.h"
#include "confighelpr.h"
#include "lvgl.h"
#include "lvglconfig.h"
#include "module_service.h"
#include <PubSubClient.h>
#include <TJpg_Decoder.h>
#include <WiFiClientSecure.h>

const char *mqtt_url = "o083a17e.ala.cn-hangzhou.emqxsl.cn";
String mqtt_sub = "";
String mqtt_pub = "";
String mqtt_app_pub = "";
String mqtt_qt_pub = "";
String mqtt_cam_pub = "";
String mqtt_wroom_pub = "";
const uint16_t mqtt_broker_port = 8883;
const uint16_t mqtt_client_buff_size = 5 * 1024;
const char *mqtt_username = "esp32";
const char *mqtt_password = "1002";
String mqtt_client_id = "esp32s3SmartHome";
const int mqtt_keepalive = 60;
const char *ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)EOF";

WiFiClientSecure net;
PubSubClient mqttClient;
extern lampButtonData mqttSwitchState;
extern detectionDate detectiondatas;

bool enable_mqtt;
extern bool loginState;

static void
mqtt_callback(char *topic, byte *payload, unsigned int length);
bool firstConnectMQTT(void);

bool initMQTTConfig(void)
{
    Serial.println("Init MQTT Config");
    net.setCACert(ca_cert);
    mqttClient.setClient(net);
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
    enable_mqtt = false;
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
    vTaskDelay(500);
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

void mqttLoop(void)
{
    if (enable_mqtt && loginState) {
        if (!mqttClient.connected() && WiFi.status() == WL_CONNECTED) {
            lv_setMQTTState("未连接");
            Serial.println("MQTT disconnected, reconnecting...");
            mqttconnect();
            vTaskDelay(1000);
        }
        mqttClient.loop();
    }
}

static void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
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
        if (datas) {
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
        if (switches) {
            cJSON *lampButton1_j = cJSON_GetObjectItem(switches, "lampButton1");
            if (lampButton1_j != NULL) {
                mqttSwitchState.lampButton1 = lampButton1_j->valueint;
            }
            cJSON *lampButton2_j = cJSON_GetObjectItem(switches, "lampButton2");
            if (lampButton2_j != NULL) {
                mqttSwitchState.lampButton2 = lampButton2_j->valueint;
            }
            cJSON *pri_j = cJSON_GetObjectItem(switches, "pri");
            if (pri_j != NULL) {
                mqttSwitchState.priButton = pri_j->valueint;
            }
            cJSON *voice_j = cJSON_GetObjectItem(switches, "voiceControl");
            if (voice_j != NULL) {
                mqttSwitchState.voiceButton = voice_j->valueint;
            }
            lv_updateSiwtech();
        }
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

    mqttClient.publish(mqtt_app_pub.c_str(), jsonStr);
    mqttClient.publish(mqtt_qt_pub.c_str(), jsonStr);
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
        mqttClient.publish(mqtt_app_pub.c_str(), jsonStr);
        mqttClient.publish(mqtt_qt_pub.c_str(), jsonStr);
        mqttClient.publish(mqtt_wroom_pub.c_str(), jsonStr);
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
        // mqttClient.publish(mqtt_pub.c_str(), jsonStr);
        // mqttClient.publish(mqtt_qt_pub.c_str(), jsonStr);
        mqttClient.publish(mqtt_wroom_pub.c_str(), jsonStr);
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
        cJSON_AddBoolToObject(root, "getdatas", true);
        char *jsonStr = cJSON_PrintUnformatted(root);

        Serial.println(jsonStr);
        mqttClient.publish(mqtt_wroom_pub.c_str(), jsonStr);
        cJSON_Delete(root);
        return true;
    } else {
        Serial.println("mqtt disconnect");
        return false;
    }
}

void publishGetImage()
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "code", "200");

    cJSON *dates = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "datas", dates);
    cJSON_AddStringToObject(dates, "getImage", "true");
    char *jsonStr = cJSON_PrintUnformatted(root);

    // publishMQTT(jsonStr);
    mqttClient.publish(mqtt_cam_pub.c_str(), jsonStr);
    cJSON_Delete(root);
}

void publishStartVideo(bool isStartVideo)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "code", "200");

    cJSON *dates = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "datas", dates);
    cJSON_AddBoolToObject(dates, "startVideo", isStartVideo);
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
    mqtt_sub = "/smartHome/" + user + "/esp32_sub";
    mqtt_pub = "/smartHome/" + user + "/esp32_pub";
    mqtt_app_pub = "/smartHome/" + user + "/esp32_app_pub";
    mqtt_qt_pub = "/smartHome/" + user + "/esp32_qt_pub";
    mqtt_cam_pub = "/smartHome/" + user + "/esp32_cam_pub";
    mqtt_wroom_pub = "/smartHome/" + user + "/esp32_wroom_pub";
    Serial.printf("mqtt_sub:%s\n", mqtt_sub.c_str());
    Serial.printf("mqtt_pub:%s\n", mqtt_pub.c_str());
    Serial.printf("mqtt_app_pub:%s\n", mqtt_app_pub.c_str());
    Serial.printf("mqtt_qt_pub:%s\n", mqtt_qt_pub.c_str());
    Serial.printf("mqtt_cam_pub:%s\n", mqtt_cam_pub.c_str());
    Serial.printf("mqtt_wroom_pub:%s\n", mqtt_wroom_pub.c_str());
}