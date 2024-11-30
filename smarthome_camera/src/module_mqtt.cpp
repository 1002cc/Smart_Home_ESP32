#include "module_mqtt.h"
#include "module_devices.h"
#include "module_server.h"
#include "module_wifi.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <cJSON.h>

const char *mqtt_url = "47.115.139.166";
String mqtt_sub = "";
String mqtt_pub = "";
String mqtt_qt_pub = "";
const uint16_t mqtt_broker_port = 1883;
const uint16_t mqtt_client_buff_size = 5 * 1024;
const char *mqtt_username = "chen";
const char *mqtt_password = "1002";
String mqtt_client_id = "esp32_cam_";
const int mqtt_keepalive = 60;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

bool enable_mqtt = true;

extern bool mqttControl;
extern bool videoStreamEnable;

extern int servoPos;

static void mqtt_callback(char *topic, byte *payload, unsigned int length);

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
    mqtt_sub = "/smartHome/" + user + "/esp32_cam_pub";
    mqtt_pub = "/smartHome/" + user + "/esp32_sub";
    mqtt_qt_pub = "/smartHome/" + user + "/esp32_qt_pub";
    Serial.printf("mqtt_sub:%s\n", mqtt_sub.c_str());
    Serial.printf("mqtt_pub:%s\n", mqtt_pub.c_str());
    Serial.printf("mqtt_qt_pub:%s\n", mqtt_qt_pub.c_str());

    if (mqttClient.connected()) {
        mqttClient.subscribe(mqtt_sub.c_str());
    }
}

bool mqttconnect(void)
{
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect(mqtt_client_id.c_str(), mqtt_username, mqtt_password)) {
        Serial.println("MQTT connected!");
        mqttClient.publish(mqtt_pub.c_str(), "ESP32-CAM connect mqtt!");
        mqttClient.subscribe(mqtt_sub.c_str());
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
        cJSON *code = cJSON_GetObjectItem(root, "code");
        if (code != NULL) {
            Serial.printf("Code: %s\n", code->valuestring);
            cJSON *datas = cJSON_GetObjectItem(root, "datas");
            if (datas != NULL) {
                cJSON *startVideo = cJSON_GetObjectItem(datas, "startVideo");
                if (startVideo != NULL) {
                    if (startVideo->valueint == 1) {
                        mqttControl = true;
                    } else {
                        mqttControl = false;
                    }
                    Serial.printf("startVideo: %d  isStartCamera:%d\n", startVideo->valueint, mqttControl);
                }
                cJSON *VideoStream = cJSON_GetObjectItem(datas, "Stream");
                if (VideoStream != NULL) {
                    if (VideoStream->valueint == 1) {
                        videoStreamEnable = true;
                    } else {
                        videoStreamEnable = false;
                    }
                    StoreintData("S", videoStreamEnable);
                    Serial.printf("VideoStream: %d  videoStreamEnable:%d\n", VideoStream->valueint, videoStreamEnable);
                }
                cJSON *about_j = cJSON_GetObjectItem(datas, "about");
                if (about_j != NULL) {
                    if (about_j->valueint == 1) {
                        Serial.println("about:Left");
                        ServoLeft();
                    } else if (about_j->valueint == 2) {
                        Serial.println("about:Right");
                        ServoRight();
                    }
                    Serial.printf("about: %d  servoPos:%d\n", about_j->valueint, servoPos);
                }
                cJSON *getStream = cJSON_GetObjectItem(datas, "getStream");
                if (getStream != NULL) {
                    pulishAllDatas();
                    Serial.printf("getStream: %d  videoStreamEnable:%d\n", getStream->valueint, videoStreamEnable);
                }
            }
        }
    }
    cJSON_Delete(root);
    Serial.println("\n----------------END----------------");
}

void sendCameraState(bool state)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddBoolToObject(root, "camerastate", state);
    char *jsonStr = cJSON_PrintUnformatted(root);

    mqttClient.publish(mqtt_pub.c_str(), jsonStr);
    cJSON_Delete(root);
}

void pulishAllDatas()
{
    Serial.println("updata datas");
    cJSON *root = cJSON_CreateObject();
    cJSON *switches = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "switches", switches);
    cJSON_AddBoolToObject(switches, "Stream", videoStreamEnable);
    char *jsonStr = cJSON_PrintUnformatted(root);
    Serial.println(jsonStr);
    mqttClient.publish(mqtt_qt_pub.c_str(), jsonStr);
    cJSON_Delete(root);
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