#include "module_wificonfig.h"
#include "module_devices.h"

const byte DNS_PORT = 53; // 设置DNS端口号
const int webPort = 80;   // 设置Web端口号
int networkTimeout = 10 * 1000;

#include <Preferences.h>

Preferences preferences;

const char *AP_SSID = "ESP32-CAM"; // 设置AP热点名称

const char *HOST_NAME = "ESP32-CAM"; // 设置设备名
String scanNetworksID = "";          // 用于储存扫描到的WiFi ID

IPAddress apIP(192, 168, 4, 1); // 设置AP的IP地址

String wifi_ssid = "";
String wifi_pass = "";

DNSServer dnsServer;       // 创建dnsServer实例
WebServer server(webPort); // 开启web服务, 创建TCP SERVER,参数: 端口号,最大连接数

#define ROOT_HTML "<!DOCTYPE html><html><head><title>WIFI</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><style type=\"text/css\">.input{display: block; margin-top: 10px;}.input span{width: 100px; float: left; float: left; height: 36px; line-height: 36px;}.input input{height: 30px;width: 200px;}.btn{width: 120px; height: 35px; background-color: #000000; border:0px; color:#ffffff; margin-top:15px; margin-left:100px;}</style><body><form method=\"POST\" action=\"configwifi\"><label class=\"input\"><span>WiFi SSID</span><input type=\"text\" name=\"ssid\" value=\"\"></label><label class=\"input\"><span>WiFi PASS</span> <input type=\"text\"  name=\"pass\"></label><input class=\"btn\" type=\"submit\" name=\"submit\" value=\"Submie\"> <p><span> Nearby wifi:</P></form>"

void handleRoot()
{
    if (server.hasArg("selectSSID")) {
        server.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");
    } else {
        server.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");
    }
}
void handleConfigWifi() // 返回http状态
{
    if (server.hasArg("ssid")) // 判断是否有账号参数
    {
        Serial.print("got ssid:");
        wifi_ssid = server.arg("ssid"); // 获取html表单输入框name名为"ssid"的内容

        Serial.println(wifi_ssid);
    } else {
        Serial.println("error, not found ssid");
        server.send(200, "text/html", "<meta charset='UTF-8'>error, not found ssid"); // 返回错误页面
        return;
    }
    // 密码与账号同理
    if (server.hasArg("pass")) {
        Serial.print("got password:");
        wifi_pass = server.arg("pass"); // 获取html表单输入框name名为"pwd"的内容
        Serial.println(wifi_pass);
    } else {
        Serial.println("error, not found password");
        server.send(200, "text/html", "<meta charset='UTF-8'>error, not found password");
        return;
    }
    server.send(200, "text/html", "<meta charset='UTF-8'>SSID：" + wifi_ssid + "<br />password:" + wifi_pass + "<br />已取得WiFi信息,正在尝试连接,请手动关闭此页面。"); // 返回保存成功页面
    delay(2000);
    WiFi.softAPdisconnect(true);
    server.close();
    WiFi.softAPdisconnect();
    Serial.println("WiFi Connect SSID:" + wifi_ssid + "  PASS:" + wifi_pass);

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("开始调用连接函数connectToWiFi()..");
        connectToWiFi();
    } else {
        Serial.println("提交的配置信息自动连接成功..");
    }
}

void handleNotFound()
{
    handleRoot(); // 访问不存在目录则返回配置页面
    server.send(404, "text/plain", "404: Not found");
}

/*
 * 进入AP模式
 */
void initSoftAP()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    if (WiFi.softAP(AP_SSID)) {
        Serial.println("ESP-32S SoftAP is right.");
        Serial.print("Soft-AP IP address = ");
        Serial.println(WiFi.softAPIP());
        Serial.println(String("MAC address = ") + WiFi.softAPmacAddress().c_str());
    } else {
        Serial.println("WiFiAP Failed");
        delay(1000);
        Serial.println("restart now...");
        ESP.restart();
    }
}

void initDNS()
{
    if (dnsServer.start(DNS_PORT, "*", apIP)) // 判断将所有地址映射到esp32的ip上是否成功
    {
        Serial.println("start dnsserver success.");
    } else {
        Serial.println("start dnsserver failed.");
    }
}

void initWebServer()
{
    if (MDNS.begin("esp32-cam")) // 给设备设定域名esp32,完整的域名是esp32.local
    {
        Serial.println("MDNS responder started");
    }
    server.on("/", HTTP_GET, handleRoot);
    server.on("/configwifi", HTTP_POST, handleConfigWifi);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("WebServer started!");
}

/*
 * 扫描附近的WiFi，为了显示在配网界面
 */
bool scanWiFi()
{
    Serial.println("scan start");
    Serial.println("--------->");
    // 扫描附近WiFi
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
        scanNetworksID = "no networks found";
        return false;
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
            scanNetworksID += "<P>" + WiFi.SSID(i) + "</P>";
            delay(10);
        }
        return true;
    }
}

/*
 * 连接WiFi
 */
void connectToWiFi()
{
    WiFi.hostname(HOST_NAME); // 设置设备名
    Serial.println("进入connectToWiFi()函数");
    WiFi.mode(WIFI_STA);       // 设置为STA模式并连接WIFI
    WiFi.setAutoConnect(true); // 设置自动连接

    if (wifi_ssid != "") // wifi_ssid不为空，意味着从网页读取到wifi
    {
        Serial.println("用web配置信息连接.");
        WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str()); // c_str(),获取该字符串的指针
        wifi_ssid = "";
        wifi_pass = "";
    } else // 未从网页读取到wifi
    {
        Serial.println("用nvs保存的信息连接.");
        WiFi.begin(); // begin()不传入参数，默认连接上一次连接成功的wifi
    }

    int Connect_time = 0;                 // 用于连接计时，如果长时间连接不成功，复位设备
    while (WiFi.status() != WL_CONNECTED) // 等待WIFI连接成功
    {
        Serial.print("."); // 一共打印30个点点
        Connect_time % 2 ? led_off() : led_on();
        delay(500);
        Connect_time++;

        if (Connect_time > 2 * networkTimeout) // 长时间连接不上，重新进入配网页面
        {
            led_off();
            Serial.println(""); // 主要目的是为了换行符
            Serial.println("WIFI autoconnect fail, start AP for webconfig now...");
            wifiConfig(); // 开始配网功能
            return;       // 跳出 防止无限初始化
        }
    }

    if (WiFi.status() == WL_CONNECTED) // 如果连接成功
    {
        Serial.println("WIFI connect Success");
        Serial.printf("SSID:%s", WiFi.SSID().c_str());
        Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
        Serial.print("LocalIP:");
        Serial.print(WiFi.localIP());
        Serial.print(" ,GateIP:");
        Serial.println(WiFi.gatewayIP());
        Serial.print("WIFI status is:");
        Serial.print(WiFi.status());
        led_on();
        server.stop(); // 停止开发板所建立的网络服务器。
    }
}

void DNS_HTTP_TACK(void *pvParameters)
{
    while (true) {
        checkDNS_HTTP();
        if (WiFi.status() == WL_CONNECTED) {
            vTaskDelete(NULL);
        }
        delay(30);
    }
}

/*
 * 配置配网功能
 */
void wifiConfig()
{
    initSoftAP();
    initDNS();
    initWebServer();
    // scanWiFi();
    xTaskCreate(DNS_HTTP_TACK, "DNS_HTTP Task", 4096, NULL, 1, NULL);
}

void checkDNS_HTTP()
{
    dnsServer.processNextRequest();
    server.handleClient();
}
