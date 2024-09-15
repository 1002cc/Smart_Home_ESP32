#include "module_mqtt.h"
#include "module_devices.h"
#include "module_server.h"
#include "module_wifi.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <cJSON.h>

const char *mqtt_url = "47.120.7.163";
String mqtt_wroom_sub = "";
String mqtt_app_pub = "";
String mqtt_qt_pub = "";
String mqtt_pub = "";
const uint16_t mqtt_broker_port = 1883;
const uint16_t mqtt_client_buff_size = 1024;
const char *mqtt_username = "chen";
const char *mqtt_password = "1002";
String mqtt_client_id = "SmartHome_esp32";
const int mqtt_keepalive = 60;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

bool enable_mqtt = true;
bool enable_pri = true;
bool enable_VoiceControl = true;
bool activeEnableAutoLamp = false;
extern bool mqttPri;
extern bool mqttVoice;
extern lampButtonData mqttSwitchState;

static void
mqtt_callback(char *topic, byte *payload, unsigned int length);

extern void sendImgPieces(void);

bool initMQTTConfig(void)
{
    mqttClient.setServer(mqtt_url, mqtt_broker_port);
    mqttClient.setBufferSize(mqtt_client_buff_size);
    mqttClient.setCallback(mqtt_callback);
    mqttClient.setKeepAlive(mqtt_keepalive);
    mqtt_client_id += String(WiFi.macAddress());
    String userlogin = ReadData("username");
    if (userlogin != "null") {
        Serial.printf("userlogin : %s\n", userlogin);
        mqttMontage(userlogin);
    } else {
        Serial.println("no username, start wifi config");
        wifiConfig();
    }
    return mqttconnect();
}

void mqttMontage(const String &user)
{
    mqtt_wroom_sub = "/smartHome/" + user + "/esp32_wroom_pub";
    mqtt_app_pub = "/smartHome/" + user + "/esp32_app_pub";
    mqtt_qt_pub = "/smartHome/" + user + "/esp32_qt_pub";
    mqtt_pub = "/smartHome/" + user + "/esp32_sub";

    Serial.printf("mqtt_sub:%s\n", mqtt_wroom_sub.c_str());
    Serial.printf("mqtt_pub:%s\n", mqtt_pub.c_str());
    Serial.printf("mqtt_app_pub:%s\n", mqtt_app_pub.c_str());
    Serial.printf("mqtt_qt_pub:%s\n", mqtt_qt_pub.c_str());

    if (mqttClient.connected()) {
        mqttClient.subscribe(mqtt_wroom_sub.c_str());
        // 上报信息
        pulishAllSwitchDatas();
    }
}

bool mqttconnect(void)
{
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect(mqtt_client_id.c_str(), mqtt_username, mqtt_password)) {
        Serial.println("MQTT connected!");
        mqttClient.publish(mqtt_pub.c_str(), "ESP32 wroom connect mqtt!");
        mqttClient.subscribe(mqtt_wroom_sub.c_str());
        // 上报信息
        pulishAllSwitchDatas();
        return true;
    } else {
        Serial.print("failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 5 seconds");
        return false;
    }
}

void mqttLoop(void)
{
    if (!mqttClient.connected() && WiFi.status() == WL_CONNECTED) {
        Serial.println("MQTT disconnected, reconnecting...");
        mqttconnect();
    }
    mqttClient.loop();
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
        cJSON *switches = cJSON_GetObjectItem(root, "switches");
        if (switches != NULL) {
            cJSON *startPri = cJSON_GetObjectItem(switches, "pri");
            if (startPri != NULL) {
                enable_pri = startPri->valueint;
                StoreintData("pri", enable_pri);
                Serial.printf("startPri: %d  enable_pri:%d\n", startPri->valueint, enable_pri);
            }
            cJSON *startVoiceControl = cJSON_GetObjectItem(switches, "voiceControl");
            if (startVoiceControl != NULL) {
                enable_VoiceControl = startVoiceControl->valueint;
                StoreintData("voiceControl", enable_VoiceControl);
                Serial.printf("startVoiceControl: %d  enable_VoiceControl:%d\n", startVoiceControl->valueint, enable_VoiceControl);
            }
            cJSON *lampButton1json = cJSON_GetObjectItem(switches, "lampButton1");
            if (lampButton1json != NULL) {
                mqttSwitchState.lampButton1 = lampButton1json->valueint;
                mqttSwitchState.lampButton1 ? led3w_on() : led3w_off();
                Serial.printf("lampButton1json: %d  lampButton1:%d\n", lampButton1json->valueint, mqttSwitchState.lampButton1);
            }
            cJSON *lampButton2json = cJSON_GetObjectItem(switches, "lampButton2");
            if (lampButton2json != NULL) {
                // if (!mqttPri || !mqttVoice) {
                activeEnableAutoLamp = lampButton2json->valueint;
                Serial.printf("lampButton2json: %d  lampButton2:%d  activeEnableAutoLamp:%d\n", lampButton2json->valueint, mqttSwitchState.lampButton2, activeEnableAutoLamp);
                //}
            }
        }
        cJSON *getdatas = cJSON_GetObjectItem(root, "getdatas");
        if (getdatas != NULL) {
            Serial.printf("getdatas:%d", getdatas->valueint);
            if (getdatas->valueint) {
                pulishAllSwitchDatas();
            }
        }
        cJSON *repw = cJSON_GetObjectItem(root, "repw");
        if (repw != NULL) {
            Serial.printf("repw:%d", repw->valueint);
            if (repw->valueint) {
                wifiConfig();
            }
        }
    }
    cJSON_Delete(root);
    Serial.println("\n----------------END----------------");
}

bool publishMQTT(const char payload[])
{
    return mqttClient.publish(mqtt_pub.c_str(), payload);
}

bool publishMQTT(const char payload[], unsigned int plength)
{
    return mqttClient.publish(mqtt_pub.c_str(), payload, plength);
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

void mqtt_beginPublish(unsigned int plength, boolean retained)
{
    mqttClient.beginPublish(mqtt_pub.c_str(), plength, retained);
}

void mqtt_print(const String &s)
{
    mqttClient.print(s);
}

void mqtt_endPublish()
{
    mqttClient.endPublish();
}

void pulishState(const String &object, const bool &state, const String &item)
{
    if (mqttClient.state() == MQTT_CONNECTED) {
        cJSON *root = cJSON_CreateObject();
        cJSON *datas = cJSON_CreateObject();
        cJSON_AddItemToObject(root, item.c_str(), datas);
        cJSON_AddBoolToObject(datas, object.c_str(), state);
        char *jsonStr = cJSON_PrintUnformatted(root);
        Serial.println(jsonStr);
        mqttClient.publish(mqtt_pub.c_str(), jsonStr);
        mqttClient.publish(mqtt_qt_pub.c_str(), jsonStr);
        mqttClient.publish(mqtt_app_pub.c_str(), jsonStr);
        cJSON_Delete(root);
    } else {
        Serial.println("mqtt disconnect");
    }
}

void pulishSwitchDatas(const lampButtonData &data)
{
    cJSON *root = cJSON_CreateObject();
    cJSON *datas = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "switches", datas);
    cJSON_AddBoolToObject(datas, "lampButton1", data.lampButton1);
    cJSON_AddBoolToObject(datas, "lampButton2", data.lampButton2);
    char *jsonStr = cJSON_PrintUnformatted(root);

    mqttClient.publish(mqtt_pub.c_str(), jsonStr);
    mqttClient.publish(mqtt_qt_pub.c_str(), jsonStr);
    mqttClient.publish(mqtt_app_pub.c_str(), jsonStr);

    cJSON_Delete(root);
}
void pulishAllSwitchDatas()
{
    Serial.println("updata datas");
    cJSON *root = cJSON_CreateObject();
    cJSON *switches = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "switches", switches);
    cJSON_AddBoolToObject(switches, "lampButton1", mqttSwitchState.lampButton1);
    cJSON_AddBoolToObject(switches, "lampButton2", mqttSwitchState.lampButton2);
    cJSON_AddBoolToObject(switches, "pri", enable_pri);
    cJSON_AddBoolToObject(switches, "voiceControl", enable_VoiceControl);
    char *jsonStr = cJSON_PrintUnformatted(root);
    Serial.println(jsonStr);
    mqttClient.publish(mqtt_pub.c_str(), jsonStr);
    mqttClient.publish(mqtt_qt_pub.c_str(), jsonStr);
    mqttClient.publish(mqtt_app_pub.c_str(), jsonStr);
    cJSON_Delete(root);
}
