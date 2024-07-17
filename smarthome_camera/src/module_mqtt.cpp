#include "module_mqtt.h"
#include "module_server.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <cJSON.h>

const char *mqtt_url = "o083a17e.ala.cn-hangzhou.emqxsl.cn";
const char *mqtt_sub = "/smartHome/esp32_cam_pub";
const char *mqtt_pub = "/smartHome/esp32_sub";
const uint16_t mqtt_broker_port = 8883;
const uint16_t mqtt_client_buff_size = 5 * 1024;
const char *mqtt_username = "chen";
const char *mqtt_password = "1002";
String mqtt_client_id = "SmartHome_esp32_cam";
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
extern bool isStartCamera;
int devices_num = 0;

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

    return mqttconnect();
}

bool mqttconnect(void)
{
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect(mqtt_client_id.c_str(), mqtt_username, mqtt_password)) {
        Serial.println("MQTT connected!");
        mqttClient.publish(mqtt_pub, "ESP32-CAM connect mqtt!");
        mqttClient.subscribe(mqtt_sub);
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
    if (enable_mqtt) {
        if (!mqttClient.connected() && WiFi.status() == WL_CONNECTED) {
            Serial.println("MQTT disconnected, reconnecting...");
            mqttconnect();
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

    String payloadString(payload, length); // 使用构造函数直接转换

    cJSON *root = cJSON_Parse(payloadString.c_str());
    if (root == NULL) {
        Serial.println("Failed to parse JSON!");
    } else {
        cJSON *code = cJSON_GetObjectItem(root, "code");
        if (code != NULL) {
            Serial.printf("Code: %s\n", code->valuestring);
            cJSON *datas = cJSON_GetObjectItem(root, "datas");
            if (datas != NULL) {
                cJSON *getImage = cJSON_GetObjectItem(datas, "getImage");
                if (getImage != NULL) {
                    Serial.printf("getImage: %s\n", getImage->valuestring);
                    if (strcmp(getImage->valuestring, "true") == 0) {
                        Serial.println("getImage: true");
                        sendImgPieces();
                    }
                }
                cJSON *startVideo = cJSON_GetObjectItem(datas, "startVideo");
                if (startVideo != NULL) {
                    if (startVideo->valueint == 1) {
                        isStartCamera = true;
                        devices_num++;
                    } else {
                        isStartCamera = false;
                        devices_num--;
                    }
                    Serial.printf("startVideo: %d  isStartCamera:%d\n", startVideo->valueint, isStartCamera);
                    startcameraTask();
                }
            }
        }
    }

    cJSON_Delete(root);
    Serial.println("\n----------------END----------------");
}

bool publishMQTT(const char payload[])
{
    return mqttClient.publish(mqtt_pub, payload);
}

bool publishMQTT(const char payload[], unsigned int plength)
{
    return mqttClient.publish(mqtt_pub, payload, plength);
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
    mqttClient.beginPublish(mqtt_pub, plength, retained);
}

void mqtt_print(const String &s)
{
    mqttClient.print(s);
}

void mqtt_endPublish()
{
    mqttClient.endPublish();
}