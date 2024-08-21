#include "module_mqtt.h"
#include "module_server.h"
#include "module_wifi.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <cJSON.h>

const char *mqtt_url = "o083a17e.ala.cn-hangzhou.emqxsl.cn";

String mqtt_wroom_sub = "";
String mqtt_pub = "";
const uint16_t mqtt_broker_port = 8883;
const uint16_t mqtt_client_buff_size = 1024;
const char *mqtt_username = "chen";
const char *mqtt_password = "1002";
String mqtt_client_id = "SmartHome_esp32";
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
bool enable_mqtt = true;

static void mqtt_callback(char *topic, byte *payload, unsigned int length);

extern void sendImgPieces(void);

bool initMQTTConfig(void)
{
    net.setCACert(ca_cert);

    mqttClient.setClient(net);
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
    mqtt_pub = "/smartHome/" + user + "/esp32_sub";

    Serial.printf("mqtt_sub:%s\n", mqtt_wroom_sub.c_str());
    Serial.printf("mqtt_pub:%s\n", mqtt_pub.c_str());

    if (mqttClient.connected()) {
        mqttClient.subscribe(mqtt_wroom_sub.c_str());
    }
}

bool mqttconnect(void)
{
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect(mqtt_client_id.c_str(), mqtt_username, mqtt_password)) {
        Serial.println("MQTT connected!");
        mqttClient.publish(mqtt_pub.c_str(), "ESP32 wroom connect mqtt!");
        mqttClient.subscribe(mqtt_wroom_sub.c_str());
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

                    // Serial.printf("startVideo: %d  isStartCamera:%d\n", startVideo->valueint, enableVideoSteam);
                }
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
        // mqttClient.publish(mqtt_qt_pub.c_str(), jsonStr);
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
    // mqttClient.publish(mqtt_qt_pub.c_str(), jsonStr);
    cJSON_Delete(root);
}
