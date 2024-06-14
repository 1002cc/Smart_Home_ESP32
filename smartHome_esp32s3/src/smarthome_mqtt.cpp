#include "smarthome_mqtt.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
// mqtt
// MQTT Broker settings
const char *mqtt_url = "o083a17e.ala.cn-hangzhou.emqxsl.cn";
const char *mqtt_sub = "/smartHome/esp32_sub";
const char *mqtt_pub = "/smartHome/esp32_pub";
const uint16_t mqtt_broker_port = 8883;
const uint16_t mqtt_client_buff_size = 4096;
const char *mqtt_username = "chen";
const char *mqtt_password = "1002";
String mqtt_client_id = "esp32SmartHome";
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

void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
    Serial.printf("Message arrived in topic %s, length %d\n", topic, length);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println("\n----------------END----------------");
}

bool mqttconnect()
{
    Serial.println("Connecting to MQTT...");
    mqtt_client_id += String(WiFi.macAddress());
    if (mqttClient.connect(mqtt_client_id.c_str(), mqtt_username, mqtt_password)) {
        Serial.println("MQTT connected!");
        mqttClient.publish(mqtt_pub, "ESP32 S3 connect mqtt!");
        mqttClient.subscribe(mqtt_sub);
        return true;
    } else {
        Serial.print("failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 5 seconds");
        return false;
    }
}

void mqttTask(void *pvParameters)
{
    mqttClient.loop();
    if (!mqttClient.connected() && WiFi.status() == WL_CONNECTED) {
        Serial.println("MQTT disconnected, reconnecting...");
        mqttconnect();
    }
}

void mqttLoop()
{
    mqttClient.loop();
    if (!mqttClient.connected() && WiFi.status() == WL_CONNECTED) {
        Serial.println("MQTT disconnected, reconnecting...");
        mqttconnect();
    }
    vTaskDelay(3000 / portTICK_PERIOD_MS);
}

void startMqttTask()
{
    xTaskCreate(mqttTask, "mqttTask", 4096, NULL, 1, NULL);
}

bool initMQTT()
{
    // 设置根CA证书
    net.setCACert(ca_cert);

    mqttClient.setClient(net);
    mqttClient.setServer(mqtt_url, mqtt_broker_port);
    mqttClient.setBufferSize(mqtt_client_buff_size);
    mqttClient.setCallback(mqtt_callback);
    mqttClient.setKeepAlive(mqtt_keepalive);

    return mqttconnect();
}

void publishSensorData(const SensorData &data)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "code", "200");

    cJSON *dates = cJSON_CreateObject();
    char tempCharArray[32], humidityCharArray[32], mq2CharArray[32];
    snprintf(tempCharArray, sizeof(tempCharArray), "%.0f", data.temp);
    snprintf(humidityCharArray, sizeof(humidityCharArray), "%.0f", data.humidity);
    snprintf(mq2CharArray, sizeof(mq2CharArray), "%.0f", data.mq);
    cJSON_AddItemToObject(root, "dates", dates);
    cJSON_AddStringToObject(dates, "temp", tempCharArray);
    cJSON_AddStringToObject(dates, "humidity", humidityCharArray);
    cJSON_AddStringToObject(dates, "mq", mq2CharArray);

    char *jsonStr = cJSON_PrintUnformatted(root); // 获取未格式化的JSON字符串

    // 发布消息到指定的MQTT主题
    publishMQTT(jsonStr);
}

bool publishMQTT(const char payload[])
{
    return mqttClient.publish(mqtt_pub, payload);
}

bool subscribeMQTT(const char topic[])
{
    return mqttClient.subscribe(topic);
}