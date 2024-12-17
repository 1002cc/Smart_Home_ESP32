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
        if (lv_getMQTTSwitchState()) {
            enable_mqtt = true;
        }
        Serial.println("已连接到接入点");
        lv_setstatusbarLabel(1);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        lv_setWIFIState("未连接");
        enable_mqtt = false;
        Serial.println("与WiFi接入点断开连接");
        lv_setstatusbarLabel(0);
        break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
        Serial.println("接入点的身份验证模式已更改");
        break;
    case SYSTEM_EVENT_STA_GOT_IP: {
        Serial.print("Obtained IP address: ");
        String str = String("IP:" + WiFi.localIP().toString());
        lv_setIPinfo(str.c_str());
        Serial.println(WiFi.localIP());
        break;
    }
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
    default:
        break;
    }
}

bool wifiConnect()
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    Serial.println("preferences read wifi info");

    String storedSSID = ReadData("ssid");
    String storedPassword = ReadData("password");

    Serial.println("Connecting to ");
    lv_setTipinfo("正在连接WiFi...");
    if (storedSSID != "null" && storedPassword != "null") {
        Serial.printf("use flash wifi info : \nssid:%s \nPassword:%s\n", storedSSID, storedPassword);
        ssid = storedSSID.c_str();
        password = storedPassword.c_str();
    }

    Serial.printf("wifi connect:\nssid:%s \nPassword:%s\n", ssid, password);
    WiFi.onEvent(WiFiEvent);
    WiFi.setAutoReconnect(false);
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
    Serial.println("Connection to WiFi failed");
    WSLED_OFF();
    return false;
}

bool getwifistate()
{
    return WiFi.status() == WL_CONNECTED;
}

String getwifissid()
{
    return WiFi.SSID();
}

void wifiDisconnect(void)
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

/********************************************************************
                         WIFI_UI_CONNECT
********************************************************************/
void wifiConnector()
{
    if (ntConnectTaskHandler == NULL) {
        xTaskCreatePinnedToCore(beginWIFITask, "beginWIFITask", 3096, NULL, 10, &ntConnectTaskHandler, 0);
    }
}

void beginWIFITask(void *pvParameters)
{
    Serial.println("wifiConnector");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);
    vTaskDelay(100);
    Serial.println(wifi_buf.ssid);
    Serial.println(wifi_buf.pass);

    WiFi.begin(wifi_buf.ssid.c_str(), wifi_buf.pass.c_str());
    unsigned long startingTime = millis();
    int ledstatus = 0;
    while (WiFi.status() != WL_CONNECTED && (millis() - startingTime) < NETWORK_TIMEOUT) {
        ledstatus ? WSLED_Red() : WSLED_OFF();
        ledstatus = !ledstatus;
        Serial.print(".");
        vTaskDelay(250);
    }

    if (WiFi.status() == WL_CONNECTED) {
        xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
        networkStatus = NETWORK_CONNECTED;
        xSemaphoreGive(xnetworkStatusSemaphore);
        StoreData("ssid", wifi_buf.ssid.c_str());
        StoreData("password", wifi_buf.pass.c_str());
        Serial.printf("\r\n-- wifi connect success! --\r\n");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        lv_setWIFIState("已连接");
        hasNetwork = isNetworkAvailable();
        ntpTimerCallback(NULL);
    } else {
        xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
        networkStatus = NETWORK_CONNECT_FAILED;
        xSemaphoreGive(xnetworkStatusSemaphore);
        Serial.println("Connection to WiFi failed");
        lv_setWIFIState("连接失败");
    }

    WSLED_OFF();
    Serial.println("delete wifiConnecttask");
    ntConnectTaskHandler = NULL;
    vTaskDelete(NULL);
}

void networkScanner()
{
    if (ntScanTaskHandler == NULL) {
        xTaskCreatePinnedToCore(scanWIFITask, "ScanWIFITask", 4096, NULL, 10, &ntScanTaskHandler, 0);
    }
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