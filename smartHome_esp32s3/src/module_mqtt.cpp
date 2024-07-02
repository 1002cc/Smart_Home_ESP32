#include "module_mqtt.h"
#include "confighelpr.h"
#include "lvgl.h"
#include "lvglconfig.h"
#include "module_service.h"
#include <PubSubClient.h>
#include <TJpg_Decoder.h>
#include <WiFiClientSecure.h>

const char *mqtt_url = "o083a17e.ala.cn-hangzhou.emqxsl.cn";
const char *mqtt_sub = "/smartHome/esp32_sub";
const char *mqtt_pub = "/smartHome/esp32_pub";
const char *mqtt_app_pub = "/smartHome/esp32_app_pub";
const char *mqtt_qt_pub = "/smartHome/esp32_qt_pub";
const char *mqtt_cam_pub = "/smartHome/esp32_cam_pub";
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

bool enable_mqtt;

static void mqtt_callback(char *topic, byte *payload, unsigned int length);
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
    return firstConnectMQTT();
}

bool firstConnectMQTT(void)
{
    if (isNetworkAvailable()) {
        lv_setTipinfo("正在连接服务器");
        if (mqttconnect()) {
            lv_setTipinfo("服务器连接成功");
            lv_setMQTTSwitchState(true);
            lv_setMQTTState("已连接");
            enable_mqtt = true;
        } else {
            lv_setTipinfo("服务器连接失败");
            lv_setMQTTSwitchState(false);
            lv_setMQTTState("未连接");
            enable_mqtt = false;
        }
    } else {
        Serial.println("Network unavailable");
        lv_setTipinfo("网络不可用");
        lv_setMQTTSwitchState(false);
        lv_setMQTTState("未连接");
        enable_mqtt = false;
    }
    return enable_mqtt;
}

bool mqttconnect(void)
{
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect(mqtt_client_id.c_str(), mqtt_username, mqtt_password)) {
        Serial.println("MQTT connected!");
        mqttClient.publish(mqtt_pub, "ESP32 S3 connect mqtt!");
        mqttClient.subscribe(mqtt_sub);
        lv_setMQTTState("已连接");
        return true;
    } else {
        Serial.print("failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 5 seconds");
        lv_setMQTTState("连接失败");
        return false;
    }
}

void mqttLoop(void)
{
    if (enable_mqtt) {
        if (!mqttClient.connected() && WiFi.status() == WL_CONNECTED) {
            lv_setMQTTState("未连接");
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

    // char hex_string[length * 2 + 1]; // 存储转换后的十六进制字符串
    // for (size_t i = 0; i < length; i++) {
    //     sprintf(&hex_string[i * 2], "%02X", payload[i]);
    // }
    // hex_string[length * 2] = '\0'; // 添加字符串终止符

    // // 解析宽度、高度、缓冲区地址和长度（此部分需根据实际消息格式调整）
    // // 注意：这里简化处理，实际应用中可能需要更复杂的解析逻辑
    // // 假设hex_string已经包含了所有需要的信息，且格式正确
    // int width, height, len;
    // char buf[10]; // 假设buf足够长以存放地址表示
    // sscanf(hex_string, "%d,%d,0x%s,%d,...", &width, &height, buf, &len);

    // // 解析时间（同样简化处理，实际应根据真实格式调整）
    // int year, month, day, hour, minute, second;
    // sscanf(hex_string + 27, "%04x%02x%02x%02x%02x%02x", &year, &month, &day, &hour, &minute, &second);

    // // 打印信息（调试用）
    // Serial.printf("width: %d, height: %d, buf: 0x%s, len: %d\n", width, height, buf, len);
    // Serial.printf("year: %d, month: %d, day: %d, hour: %d, minute: %d, second: %d\n", year, month, day, hour, minute, second);

    // // 解析图像数据（直接从payload开始，跳过前面的头部信息）
    // byte img[length - 43]; // 假定有效图像数据从payload的第43个字节开始
    // memcpy(img, payload + 43, length - 43);

    // lv_img_dsc_t img_dsc;
    // lv_img_dsc_init(&img_dsc, NULL, NULL, len - 43, LV_IMG_CF_RAW);
    // lv_img_set_src(image, &img_dsc);

    Serial.println("\n----------------END----------------");
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
    cJSON_AddItemToObject(root, "datas", dates);
    cJSON_AddStringToObject(dates, "temp", tempCharArray);
    cJSON_AddStringToObject(dates, "humidity", humidityCharArray);
    cJSON_AddStringToObject(dates, "mq", mq2CharArray);

    char *jsonStr = cJSON_PrintUnformatted(root);

    mqttClient.publish(mqtt_pub, jsonStr);
    mqttClient.publish(mqtt_qt_pub, jsonStr);
    cJSON_Delete(root);
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
    mqttClient.publish(mqtt_cam_pub, jsonStr);
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
    mqttClient.publish(mqtt_cam_pub, jsonStr);
    cJSON_Delete(root);
}

bool publishMQTT(const char payload[])
{
    return mqttClient.publish(mqtt_pub, payload);
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