#include "module_wifi.h"
#include "module_devices.h"
#include "module_mqtt.h"
#include "module_server.h"
/**
 * @brief  WiFi配置模块
 * 指示灯:
 *        快闪烁 正在连接
 *        慢闪烁 wed配置
 *        常亮 已连接
 *        不亮 未连接
 */

const byte DNS_PORT = 53;
const int webPort = 80;

const char *AP_SSID = "SmartHome_ESP32CAM";

const char *HOST_NAME = "ESP32CAM";
String scanNetworksID = "";

IPAddress apIP(192, 168, 4, 1);

String wifi_ssid = "";
String wifi_pass = "";
String username = "";

DNSServer dnsServer;       // 创建dnsServer实例
WebServer server(webPort); // 开启web服务, 创建TCP SERVER,参数: 端口号,最大连接数

// 上下两段HTML代码

String ROOT_HTML_1 = "<!DOCTYPE html><html><head>  <meta charset=\"UTF-8\">  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">  <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\" />  <title>WIFI配置</title>  <style>   #content,.login,.login-card a,.login-card h1,.login-help{text-align:center}body,html{margin:0;padding:0;width:100%;height:100%;display:table}#content{font-family:\'Source Sans Pro\',sans-serif;-webkit-background-size:cover;-moz-background-size:cover;-o-background-size:cover;background-size:cover;display:table-cell;vertical-align:middle}.login-card{padding:40px;width:274px;background-color:#F7F7F7;margin:0 auto 10px;border-radius:20px;box-shadow:8px 8px 15px rgba(0,0,0,.3);overflow:hidden}.login-card h1{font-weight:400;font-size:2.3em;color:#1383c6}.login-card h1 span{color:#f26721}.login-card img{width:70%;height:70%}.login-card input[type=submit]{width:100%;display:block;margin-bottom:10px;position:relative}.login-card input[type=text],input[type=password]{height:44px;font-size:16px;width:100%;margin-bottom:10px;-webkit-appearance:none;background:#fff;border:1px solid #d9d9d9;border-top:1px solid silver;padding:0 8px;box-sizing:border-box;-moz-box-sizing:border-box}.login-card input[type=text]:hover,input[type=password]:hover{border:1px solid #b9b9b9;border-top:1px solid #a0a0a0;-moz-box-shadow:inset 0 1px 2px rgba(0,0,0,.1);-webkit-box-shadow:inset 0 1px 2px rgba(0,0,0,.1);box-shadow:inset 0 1px 2px rgba(0,0,0,.1)}.login{font-size:14px;font-family:Arial,sans-serif;font-weight:700;height:36px;padding:0 8px}.login-submit{-webkit-appearance:none;-moz-appearance:none;appearance:none;border:0;color:#fff;text-shadow:0 1px rgba(0,0,0,.1);background-color:#4d90fe}.login-submit:disabled{opacity:.6}.login-submit:hover{border:0;text-shadow:0 1px rgba(0,0,0,.3);background-color:#357ae8}.login-card a{text-decoration:none;color:#666;font-weight:400;display:inline-block;opacity:.6;transition:opacity ease .5s}.login-card a:hover{opacity:1}.login-help{width:100%;font-size:12px}.list{list-style-type:none;padding:0}.list__item{margin:0 0 .7rem;padding:0}label{display:-webkit-box;display:-webkit-flex;display:-ms-flexbox;display:flex;-webkit-box-align:center;-webkit-align-items:center;-ms-flex-align:center;align-items:center;text-align:left;font-size:14px;}input[type=checkbox]{-webkit-box-flex:0;-webkit-flex:none;-ms-flex:none;flex:none;margin-right:10px;float:left}.error{font-size:14px;font-family:Arial,sans-serif;font-weight:700;height:25px;padding:0 8px;padding-top: 10px; -webkit-appearance:none;-moz-appearance:none;appearance:none;border:0;color:#fff;text-shadow:0 1px rgba(0,0,0,.1);background-color:#ff1215}@media screen and (max-width:450px){.login-card{width:70%!important}.login-card img{width:30%;height:30%}}  </style></head><body style=\"background-color: #e5e9f2\"><div id=\"content\"> <form name=\'input\' action=\'/configwifi\' method=\'POST\'>  <div class=\"login-card\">    <h1>WiFi登录</h1>   <form name=\"login_form\" method=\"post\" action=\"$PORTAL_ACTION$\">   <input type=\"text\" name=\"ssid\" placeholder=\"请输入 WiFi 名称\" id=\"auth_user\" list = \"data-list\"; style=\"border-radius: 10px\">    <datalist id = \"data-list\">";
String ROOT_HTML_2 = "<input type=\"password\" name=\"password\" placeholder=\"请输入 WiFi 密码\" id=\"auth_pass\"; style=\"border-radius: 10px\"> <input type=\"text\" name=\"username\" placeholder=\"请输入绑定账号名(选填)\"; style=\"border-radius: 10px\">      <div class=\"login-help\">        <ul class=\"list\">          <li class=\"list__item\">          </li>        </ul>      </div>   <input type=\"submit\" class=\"login login-submit\" value=\"连 接\" id=\"login\"; disabled; style=\"border-radius: 15px\"  >    </form> <!-- <form name=\'input\' action=\'/English\' method=\'POST\'>    <input type=\"submit\" class=\"login login-submit\" value=\"English\" id=\"login\"; disabled; style=\"border-radius: 15px\"  >    </form> --></body></html>";
void handleRoot()
{
    if (server.hasArg("selectSSID")) {
        server.send(200, "text/html", ROOT_HTML_1 + scanNetworksID + ROOT_HTML_2);
    } else {
        server.send(200, "text/html", ROOT_HTML_1 + scanNetworksID + ROOT_HTML_2);
    }
}

void handleConfigWifi()
{
    if (server.hasArg("ssid")) {
        Serial.print("got ssid:");
        wifi_ssid = server.arg("ssid");
        Serial.println(wifi_ssid);
    } else {
        Serial.println("error, not found ssid");
        server.send(200, "text/html", "<meta charset='UTF-8'>error, not found ssid");
        return;
    }

    if (server.hasArg("password")) {
        Serial.print("got password:");
        wifi_pass = server.arg("password");
        Serial.println(wifi_pass);
    } else {
        Serial.println("error, not found password");
        server.send(200, "text/html", "<meta charset='UTF-8'>error, not found password");
        return;
    }

    if (server.hasArg("username")) {
        Serial.print("got username:");
        username = server.arg("username");
        StoreData("username", username.c_str());
        mqttMontage(username);
        Serial.println(username);
    } else {
        Serial.println("error, not found serverip");
        server.send(200, "text/html", "<meta charset='UTF-8'>error, not found serverip");
        return;
    }

    server.send(200, "text/html", "<meta charset='UTF-8'>SSID：" + wifi_ssid + "<br />password:" + wifi_pass + "<br />username:" + username + "<br />已取得WiFi信息,正在尝试连接,请手动关闭此页面。"); // 返回保存成功页面
    delay(2000);
    WiFi.softAPdisconnect(true);
    server.close();
    WiFi.softAPdisconnect();
    Serial.println("WiFi Connect SSID:" + wifi_ssid + "  PASS:" + wifi_pass);

    if (WiFi.status() != WL_CONNECTED) // wifi没有连接成功
    {
        Serial.println("开始调用连接函数connectToWiFi()..");
        connectToWiFi(CONNECTTIMEOUT);
    } else {
        Serial.println("提交的配置信息自动连接成功..");
    }
}

void handleNotFound()
{
    handleRoot();
    server.send(404, "text/plain", "404: Not found");
}

void initSoftAP()
{
    WiFi.mode(WIFI_AP);                                         // 配置为AP模式
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); // 设置AP热点IP和子网掩码
    if (WiFi.softAP(AP_SSID))                                   // 开启AP热点,如需要密码则添加第二个参数
    {
        Serial.println("ESP-32-Cam SoftAP is right.");
        Serial.print("Soft-AP IP address = ");
        Serial.println(WiFi.softAPIP());
        Serial.println(String("MAC address = ") + WiFi.softAPmacAddress().c_str());
    } else {
        Serial.println("WiFiAP Failed");
        delay(1000);
        Serial.println("restart now...");
        ESP.restart(); // 重启复位esp32
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
    if (MDNS.begin("esp32")) // 给设备设定域名esp32,完整的域名是esp32.local
    {
        Serial.println("MDNS responder started");
    }
    // 必须添加第二个参数HTTP_GET，以下面这种格式去写，否则无法强制门户
    server.on("/", HTTP_GET, handleRoot);                  //  当浏览器请求服务器根目录(网站首页)时调用自定义函数handleRoot处理，设置主页回调函数，必须添加第二个参数HTTP_GET，否则无法强制门户
    server.on("/configwifi", HTTP_POST, handleConfigWifi); //  当浏览器请求服务器/configwifi(表单字段)目录时调用自定义函数handleConfigWifi处理

    server.onNotFound(handleNotFound); // 当浏览器请求的网络资源无法在服务器找到时调用自定义函数handleNotFound处理

    server.begin(); // 启动TCP SERVER

    Serial.println("WebServer started!");
}

bool scanWiFi()
{
    Serial.println("scan start");
    Serial.println("--------->");
    // 扫描附近WiFi
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
        scanNetworksID += "<option>no networks found</option>";
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
            scanNetworksID += "<option>" + WiFi.SSID(i) + "</option>";
            delay(10);
        }
        scanNetworksID += "</datalist>";
        return true;
    }
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
        Serial.println("已连接到接入点");
        initWedServer();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:

        Serial.println("与WiFi接入点断开连接");

        break;
    }
}

void connectToWiFi(int timeOut_s)
{
    WiFi.hostname(HOST_NAME);
    Serial.println("connecting WiFi");
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.onEvent(WiFiEvent);

    if (wifi_ssid != "") {
        Serial.println("用web配置信息连接.");
        WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
        wifi_ssid = "";
        wifi_pass = "";
    } else {
        Serial.println("用nvs保存的信息连接.");
        WiFi.begin();
    }

    int Connect_time = 0;
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        digitalWrite(LED_RED_NUM, !digitalRead(LED_RED_NUM));
        delay(300);
        Connect_time++;

        if (Connect_time > 2 * timeOut_s) // 长时间连接不上，重新进入配网页面
        {
            led_off();
            Serial.println("\nWIFI autoconnect fail, start AP for webconfig now...");
            wifiConfig();
            blinkLED(5, 500);
            return;
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
        server.stop();
        // ntp校时
        initNtpTime();
    }
}

void wifiConfig()
{
    initSoftAP();
    initDNS();
    initWebServer();
    scanWiFi();
}

void restoreWiFi()
{
    delay(500);
    esp_wifi_restore(); // 删除保存的wifi信息
    Serial.println("连接信息已清空,准备重启设备..");
    delay(10);
    blinkLED(5, 500);
    led_off();
}

void checkConnect(bool reConnect)
{
    if (WiFi.status() != WL_CONNECTED) {
        led_off();
        if (reConnect == true && WiFi.getMode() != WIFI_AP && WiFi.getMode() != WIFI_AP_STA) {
            Serial.println("WIFI未连接.");
            Serial.println("WiFi Mode:");
            Serial.println(WiFi.getMode());
            Serial.println("正在连接WiFi...");
            connectToWiFi(CONNECTTIMEOUT); // 连接wifi函数
        }
    }
}

void checkDNS_HTTP()
{
    dnsServer.processNextRequest();
    server.handleClient();
}

bool wifitate()
{
    return WiFi.status() == WL_CONNECTED;
}