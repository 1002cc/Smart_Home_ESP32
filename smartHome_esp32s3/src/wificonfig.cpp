#include "wificonfig.h"
#include "WiFi.h"
#include "confighelpr.h"
#include "lvglconfig.h"
#include "module_devices.h"
#include "module_mqtt.h"
#include "module_service.h"

const char *ssid = "317";
const char *password = "317123456";
bool hasNetwork;
extern bool enable_mqtt;

wifi_buf_t wifi_buf;
Network_Status_t networkStatus = NONE;
TaskHandle_t ntScanTaskHandler, ntConnectTaskHandler;
extern SemaphoreHandle_t xnetworkStatusSemaphore;

extern std::vector<String> foundWifiList;

static void scanWIFITask(void *pvParameters);
void beginWIFITask(void *pvParameters);
bool initWIFIConfig(void)
{
    return wifiConnect();
}

void WiFiEvent(WiFiEvent_t event)
{
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch (event) {
    case SYSTEM_EVENT_SCAN_DONE:
        Serial.println("已完成对访问点的扫描");
        break;
    case SYSTEM_EVENT_STA_START:
        Serial.println("WiFi client started");
        break;
    case SYSTEM_EVENT_STA_STOP:
        Serial.println("WiFi clients stopped");
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        lv_setWIFIState("已连接");
        Serial.println("已连接到接入点");
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        lv_setWIFIState("未连接");
        Serial.println("与WiFi接入点断开连接");
        break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
        Serial.println("接入点的身份验证模式已更改");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.print("Obtained IP address: ");
        Serial.println(WiFi.localIP());
        break;
    case SYSTEM_EVENT_AP_START:
        Serial.println("WiFi接入点已启动");
        break;
    case SYSTEM_EVENT_AP_STOP:
        Serial.println("WiFi接入点已停止");
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        Serial.println("客户端已连接");
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        Serial.println("客户端已断开连接");
        break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
        Serial.println("已将IP地址分配给客户端");
        break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
        Serial.println("收到探测请求");
        break;
        break;
    default:
        break;
    }
}

bool wifiConnect()
{
    // WiFi.disconnect(true);
    //  WiFi.mode(WIFI_STA);
    Serial.println("preferences read wifi info");

    String storedSSID = ReadData("ssid");
    String storedPassword = ReadData("password");

    Serial.println("Connecting to ");
    lv_setTipinfo("正在连接WiFi...");
    if (storedSSID != "null" && storedPassword != "null") {
        Serial.printf("use flash wifi info : ssid:%s Password:%s\n", storedSSID, storedPassword);
        ssid = storedSSID.c_str();
        password = storedPassword.c_str();
    }

    Serial.printf("ssid:%s Password:%s\n", ssid, password);
    WiFi.onEvent(WiFiEvent);
    WiFi.begin(ssid, password);

    unsigned long startingTime = millis();
    int ledstatus = 0;
    while (WiFi.status() != WL_CONNECTED && (millis() - startingTime) < NETWORK_TIMEOUT) {
        ledstatus ? WSLED_Red() : WSLED_OFF();
        ledstatus = !ledstatus;
        Serial.print(".");
        vTaskDelay(250);
    }
    vTaskDelay(1000);
    if (WiFi.status() == WL_CONNECTED) {
        WSLED_Green();
        lv_setTipinfo("WiFi连接成功");
        vTaskDelay(1000);
        WSLED_OFF();
        return true;
    } else {
        Serial.println("Connection to WiFi failed");
        lv_setWIFIState("未连接");
        lv_setTipinfo("连接wifi失败,当前为离线模式,请在配置中连接wifi");
    }
    vTaskDelay(1000);
    Serial.println("Connection to WiFi failed");
    WSLED_OFF();
    return false;
}

bool getwifistate()
{
    return WiFi.status() == WL_CONNECTED;
}

void wifiDisconnect(void)
{
    WiFi.disconnect(true);
}

/********************************************************************
                         WIFI_UI_CONNECT
********************************************************************/
void wifiConnector(wifi_buf_t wifi_buf)
{
    Serial.println(wifi_buf.ssid);
    Serial.println(wifi_buf.pass);
    xTaskCreatePinnedToCore(beginWIFITask, "beginWIFITask", 2048, &wifi_buf, 10, &ntConnectTaskHandler, 0);
}

void beginWIFITask(void *pvParameters)
{
    wifi_buf_t *pInfo = (wifi_buf_t *)pvParameters;
    unsigned long startingTime = millis();
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    vTaskDelay(100);

    Serial.println(pInfo->ssid);
    WiFi.begin(pInfo->ssid.c_str(), pInfo->pass.c_str());
    while (WiFi.status() != WL_CONNECTED && (millis() - startingTime) < NETWORK_TIMEOUT) {
        vTaskDelay(250);
    }

    if (WiFi.status() == WL_CONNECTED) {
        xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
        networkStatus = NETWORK_CONNECTED;
        xSemaphoreGive(xnetworkStatusSemaphore);
        StoreData("ssid", pInfo->ssid.c_str());
        StoreData("password", pInfo->pass.c_str());
        Serial.printf("\r\n-- wifi connect success! --\r\n");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        lv_setWIFIState("已连接");
        hasNetwork = isNetworkAvailable();
    } else {
        xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
        networkStatus = NETWORK_CONNECT_FAILED;
        xSemaphoreGive(xnetworkStatusSemaphore); // 释放信号量
        Serial.println("Connection to WiFi failed");
        lv_setWIFIState("未连接");
    }

    vTaskDelete(NULL);
}

void networkScanner()
{
    xTaskCreatePinnedToCore(scanWIFITask, "ScanWIFITask", 4096, NULL, 10, &ntScanTaskHandler, 0);
}

static void scanWIFITask(void *pvParameters)
{
    while (1) {
        if (networkStatus == NETWORK_SEARCHING) {
            foundWifiList.clear();
            int n = WiFi.scanNetworks();
            vTaskDelay(10);
            for (int i = 0; i < n; ++i) {
                // String item = WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ") " + ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
                String item = WiFi.SSID(i);
                foundWifiList.push_back(item);
                vTaskDelay(10);
            }
            xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
            networkStatus = NETWORK_SEARCHED;
            xSemaphoreGive(xnetworkStatusSemaphore);
        }
        vTaskDelay(10000);
    }
}