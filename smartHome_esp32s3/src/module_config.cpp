#include "module_config.h"

#include "WiFi.h"
#include "module_devices.h"
#include "module_service.h"
#include "ui.h"
#include <Preferences.h>
#include <lvgl.h>
#include <vector>

// Wifi配置
const char *ssid = "317";
const char *password = "317123456";

typedef enum {
    NONE,
    NETWORK_SEARCHING,
    NETWORK_SEARCHED,
    NETWORK_CONNECTED_POPUP,
    NETWORK_CONNECTED,
    NETWORK_CONNECT_FAILED
} Network_Status_t;
Network_Status_t networkStatus = NONE;

extern bool hasNetwork;
int palyState = 0;

static lv_obj_t *wfList;
static lv_obj_t *mboxConnect;
static lv_obj_t *mboxTitle;
static lv_obj_t *mboxPassword;
static lv_obj_t *mboxConnectBtn;
static lv_obj_t *mboxCloseBtn;
static lv_obj_t *popupBox;
static lv_obj_t *popupBoxCloseBtn;
static lv_timer_t *timer;
static lv_obj_t *subTextarea;
static lv_obj_t *pubTextarea;
static lv_obj_t *weatherTextarea;
static lv_obj_t *ntpTextarea;
static lv_obj_t *ntpTimeTextarea;
static lv_obj_t *inputKeyboard;

static int foundNetworks = 0;
unsigned long networkTimeout = 10 * 1000;
String ssidName, ssidPW;
std::vector<String> foundWifiList;
TaskHandle_t ntScanTaskHandler, ntConnectTaskHandler;
Preferences preferences;

static void timerForNetwork(lv_timer_t *timer);
static void showingFoundWiFiList();
static void list_event_handler(lv_event_t *e);
static void networkConnector();
static void networkScanner();
static void scanWIFITask(void *pvParameters);
void beginWIFITask(void *pvParameters);
static void buildPWMsgBox();
static void settext_input_event_cb(lv_event_t *e);
static void popupMsgBox(String title, String msg);
void StoreData(const char *key, const char *val);
String ReadData(const char *val);
void StoreintData(const char *key, int val);
int ReadintData(const char *val);

void StoreData(const char *key, const char *val)
{
    preferences.begin("config", false);
    preferences.putString(key, val);
    preferences.end();
}
String ReadData(const char *val)
{
    preferences.begin("config", false);
    String ret = preferences.getString(val, "null");
    preferences.end();
    return ret;
}

void StoreintData(const char *key, int val)
{
    preferences.begin("config", false);
    preferences.putInt(key, val);
    preferences.end();
}
int ReadintData(const char *val)
{
    preferences.begin("config", false);
    int ret = preferences.getInt(val, 1000);
    preferences.end();
    return ret;
}

void setChooseScreenCD(lv_event_t *e)
{
    lv_obj_t *setbt = lv_event_get_target(e);
    lv_scr_load_anim(ui_set1Screen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, false);
    if (setbt == ui_wifiSetButton) {
        lv_tabview_set_act(ui_chooseTabView, 0, LV_ANIM_OFF);
        if (lv_obj_has_state(ui_wifiSwitch, LV_STATE_CHECKED)) {
            if (ntScanTaskHandler == NULL) {
                networkStatus = NETWORK_SEARCHING;
                networkScanner();
                timer = lv_timer_create(timerForNetwork, 1000, wfList);
                lv_list_add_text(wfList, "WiFi:Looking for Networks");
            }
        }
    } else if (setbt == ui_networkSetButton) {
        lv_tabview_set_act(ui_chooseTabView, 1, LV_ANIM_OFF);
    } else if (setbt == ui_timeSetButton) {
        lv_tabview_set_act(ui_chooseTabView, 2, LV_ANIM_OFF);
    }
}

void chooseBtEventCD(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {
        if (btn == mboxConnectBtn) {
            ssidPW = String(lv_textarea_get_text(mboxPassword));
            lv_textarea_set_text(mboxPassword, "");
            networkConnector();
            lv_obj_move_background(mboxConnect);
            lv_obj_add_flag(mboxConnect, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(inputKeyboard, LV_OBJ_FLAG_HIDDEN);
        } else if (btn == mboxCloseBtn) {
            lv_obj_move_background(mboxConnect);
            lv_obj_add_flag(mboxConnect, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(inputKeyboard, LV_OBJ_FLAG_HIDDEN);
        } else if (btn == popupBoxCloseBtn) {
            lv_obj_move_background(popupBox);
            lv_obj_add_flag(popupBox, LV_OBJ_FLAG_HIDDEN);
        } else if (btn == ui_mqttuseButton) {
            if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                String substr = lv_textarea_get_text(subTextarea);
                String pubstr = lv_textarea_get_text(pubTextarea);
                if (!substr.isEmpty()) {
                    lv_textarea_set_placeholder_text(subTextarea, substr.c_str());
                    lv_textarea_set_text(subTextarea, "");
                }
                if (!pubstr.isEmpty()) {
                    lv_textarea_set_placeholder_text(pubTextarea, pubstr.c_str());
                    lv_textarea_set_text(pubTextarea, "");
                }
            }
            String citystr = lv_textarea_get_text(weatherTextarea);
            if (!citystr.isEmpty()) {
                lv_textarea_set_placeholder_text(weatherTextarea, citystr.c_str());
                lv_textarea_set_text(weatherTextarea, "");
                preferences.putString("cityid", citystr.c_str());
                Serial.println(citystr);
                updateCityID(citystr);
                weatherQuery();
            }

        } else if (btn == ui_timeuseButton) {
            Serial.println("ui_timeuseButton LV_EVENT_CLICKED");
        }
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        if (btn == ui_wifiSwitch) {
            if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                if (ntScanTaskHandler == NULL) {
                    networkStatus = NETWORK_SEARCHING;
                    networkScanner();
                    networkConnector();
                    timer = lv_timer_create(timerForNetwork, 1000, wfList);
                    lv_list_add_text(wfList, "WiFi:Looking for Networks");
                }
            } else {
                lv_obj_clean(wfList);
                lv_list_add_text(wfList, "");

                if (ntScanTaskHandler != NULL) {
                    networkStatus = NONE;
                    vTaskDelete(ntScanTaskHandler);
                    ntScanTaskHandler = NULL;
                    lv_timer_del(timer);
                }
                if (WiFi.status() == WL_CONNECTED) {
                    WiFi.disconnect(true);
                    lv_label_set_text(ui_wifiStateLabel, "未连接");
                }
            }
        } else if (btn == ui_mqttSwitch) {
            Serial.println(hasNetwork);
            if (WiFi.status() == WL_CONNECTED && hasNetwork) {
                if (lv_obj_has_state(ui_mqttSwitch, LV_STATE_CHECKED)) {
                    Serial.println("ui_mqttSwitch LV_STATE_CHECKED");
                } else {
                    Serial.println("ui_mqttSwitch NO LV_STATE_CHECKED");
                }
            } else {
                Serial.println("no network dont check mqttSwitch");
                lv_obj_add_state(ui_mqttSwitch, LV_STATE_DEFAULT);
            }
        }
    }
}

void lampButtonCB(lv_event_t *e)
{
    lv_obj_t *sw = lv_event_get_target(e);
    bool is_on = lv_obj_has_state(sw, LV_STATE_CHECKED);
    Serial.println(is_on);
    if (sw == ui_lampButton1) {
        if (is_on) {
            redled_on();
        } else {
            redled_off();
        }
    } else if (sw == ui_lampButton2) {
        if (is_on) {
            greenled_on();
        } else {
            greenled_off();
        }

    } else if (sw == ui_lampButton3) {
        if (is_on) {
            blueled_on();
        } else {
            blueled_off();
        }
    } else if (sw == ui_lampButton4) {
        if (is_on) {
            blueled_on();
        } else {
            blueled_off();
        }
    }
}

static void timerForNetwork(lv_timer_t *timer1)
{
    LV_UNUSED(timer1);
    if (lv_scr_act() != ui_set1Screen) {
        Serial.println("不是当前屏幕删除删除计时器");
        networkStatus = NONE;
        vTaskDelete(ntScanTaskHandler);
        ntScanTaskHandler = NULL;
        lv_timer_del(timer);
    }
    switch (networkStatus) {
    case NETWORK_SEARCHING:
        break;
    case NETWORK_SEARCHED:
        networkStatus = NETWORK_SEARCHING;
        showingFoundWiFiList();
        break;
    case NETWORK_CONNECTED_POPUP:
        lv_label_set_text(ui_wifiStateLabel, "已连接");
        networkStatus = NETWORK_CONNECTED;
        break;
    case NETWORK_CONNECTED:
        // showingFoundWiFiList();
        break;
    case NETWORK_CONNECT_FAILED:
        networkStatus = NETWORK_SEARCHING;
        lv_label_set_text(ui_wifiStateLabel, "连接失败");
        popupMsgBox("Oops!", "连接失败,请检查密码");
        break;
    default:
        break;
    }
}

static void showingFoundWiFiList()
{
    if (foundWifiList.size() == 0 || foundNetworks == foundWifiList.size()) {
        return;
    }

    lv_obj_clean(wfList);
    lv_list_add_text(wfList, foundWifiList.size() > 1 ? "WiFi:Found Networks" : "WiFi:Not Found!");

    int num = 0;
    for (std::vector<String>::iterator item = foundWifiList.begin(); item != foundWifiList.end(); ++item) {
        Serial.println((*item).c_str());
        lv_obj_t *wifibtnc = lv_list_add_btn(wfList, LV_SYMBOL_WIFI, (*item).c_str());
        lv_obj_add_event_cb(wifibtnc, list_event_handler, LV_EVENT_CLICKED, NULL);
        delay(1);
        num++;
        if (num >= 15) {
            break;
        }
    }

    foundNetworks = foundWifiList.size();
}

static void list_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        ssidName = String(lv_list_get_btn_text(wfList, obj));
        // Serial.println(ssidName);
        lv_label_set_text_fmt(mboxTitle, "Selected WiFi SSID: %s", ssidName);
        lv_obj_move_foreground(mboxConnect);
        lv_obj_clear_flag(mboxConnect, LV_OBJ_FLAG_HIDDEN);
    }
}

static void networkScanner()
{
    xTaskCreate(scanWIFITask,
                "ScanWIFITask",
                4096,
                NULL,
                1,
                &ntScanTaskHandler);
}

static void networkConnector()
{
    xTaskCreate(beginWIFITask,
                "beginWIFITask",
                2048,
                NULL,
                1,
                &ntConnectTaskHandler);
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
            networkStatus = NETWORK_SEARCHED;
        }
        vTaskDelay(10000);
    }
}

void beginWIFITask(void *pvParameters)
{
    unsigned long startingTime = millis();
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    vTaskDelay(100);

    Serial.println(ssidName);
    WiFi.begin(ssidName.c_str(), ssidPW.c_str());
    while (WiFi.status() != WL_CONNECTED && (millis() - startingTime) < networkTimeout) {
        vTaskDelay(250);
    }

    if (WiFi.status() == WL_CONNECTED) {
        networkStatus = NETWORK_CONNECTED_POPUP;
        StoreData("ssid", ssidName.c_str());
        StoreData("password", ssidPW.c_str());
        Serial.printf("\r\n-- wifi connect success! --\r\n");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        lv_label_set_text(ui_wifiStateLabel, "已连接");
    } else {
        networkStatus = NETWORK_CONNECT_FAILED;
        Serial.println("Connection to WiFi failed");
        lv_label_set_text(ui_wifiStateLabel, "未连接");
    }

    vTaskDelete(NULL);
}

static void buildPWMsgBox()
{
    mboxConnect = lv_obj_create(ui_set1Screen);
    lv_obj_set_size(mboxConnect, 320 * 2 / 3, 240 / 2);
    lv_obj_center(mboxConnect);

    mboxTitle = lv_label_create(mboxConnect);
    lv_label_set_text(mboxTitle, "Selected WiFi SSID");
    lv_obj_align(mboxTitle, LV_ALIGN_TOP_LEFT, 0, 0);

    mboxPassword = lv_textarea_create(mboxConnect);
    lv_obj_set_size(mboxPassword, 320 / 2, 30);
    lv_obj_align_to(mboxPassword, mboxTitle, LV_ALIGN_TOP_LEFT, 0, 20);
    lv_textarea_set_placeholder_text(mboxPassword, "Password");
    lv_obj_add_event_cb(mboxPassword, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);

    mboxConnectBtn = lv_btn_create(mboxConnect);
    lv_obj_add_event_cb(mboxConnectBtn, chooseBtEventCD, LV_EVENT_ALL, NULL);
    lv_obj_align(mboxConnectBtn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_t *btnLabel = lv_label_create(mboxConnectBtn);
    lv_obj_set_style_text_font(btnLabel, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(btnLabel, "连接");
    lv_obj_center(btnLabel);

    mboxCloseBtn = lv_btn_create(mboxConnect);
    lv_obj_add_event_cb(mboxCloseBtn, chooseBtEventCD, LV_EVENT_ALL, NULL);
    lv_obj_align(mboxCloseBtn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_t *btnLabel2 = lv_label_create(mboxCloseBtn);
    lv_obj_set_style_text_font(btnLabel2, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(btnLabel2, "取消");
    lv_obj_center(btnLabel2);

    lv_obj_add_flag(mboxConnect, LV_OBJ_FLAG_HIDDEN);
}

static void settext_input_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *ta = lv_event_get_target(e);

    if (code == LV_EVENT_FOCUSED) {
        lv_obj_move_foreground(inputKeyboard);
        lv_keyboard_set_textarea(inputKeyboard, ta);
        lv_obj_clear_flag(inputKeyboard, LV_OBJ_FLAG_HIDDEN);
    }

    if (code == LV_EVENT_CLICKED) {
        lv_obj_move_foreground(inputKeyboard);
        lv_obj_clear_flag(inputKeyboard, LV_OBJ_FLAG_HIDDEN);
    }

    if (code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(inputKeyboard, NULL);
        lv_obj_add_flag(inputKeyboard, LV_OBJ_FLAG_HIDDEN);
    }
}

static void popupMsgBox(String title, String msg)
{

    if (popupBox != NULL) {
        lv_obj_del(popupBox);
    }

    popupBox = lv_obj_create(ui_set1Screen);
    lv_obj_set_size(popupBox, 320 * 2 / 3, 240 / 2);
    lv_obj_center(popupBox);

    lv_obj_t *popupTitle = lv_label_create(popupBox);
    lv_obj_set_style_text_font(popupTitle, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(popupTitle, title.c_str());
    lv_obj_set_width(popupTitle, 320 * 2 / 3 - 50);
    lv_obj_align(popupTitle, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *popupMSG = lv_label_create(popupBox);
    lv_obj_set_style_text_font(popupMSG, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(popupMSG, 320 * 2 / 3 - 50);
    lv_label_set_text(popupMSG, msg.c_str());
    lv_obj_align(popupMSG, LV_ALIGN_TOP_LEFT, 0, 40);

    popupBoxCloseBtn = lv_btn_create(popupBox);
    lv_obj_add_event_cb(popupBoxCloseBtn, chooseBtEventCD, LV_EVENT_ALL, NULL);
    lv_obj_align(popupBoxCloseBtn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_t *btnLabel = lv_label_create(popupBoxCloseBtn);
    lv_obj_set_style_text_font(btnLabel, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(btnLabel, "确定");
    lv_obj_center(btnLabel);
}

bool wifiConnect()
{
    WiFi.disconnect(true);
    Serial.println("preferences read wifi info");

    String storedSSID = preferences.getString("ssid", "null");
    String storedPassword = preferences.getString("password", "null");

    Serial.println("Connecting to ");
    lv_label_set_text(ui_tipLabel, "正在连接WiFi...");
    Serial.print(ssid);
    Serial.print(" Password:");
    Serial.println(password);
    if (storedSSID != "null" && storedPassword != "null") {
        Serial.print("use flash wifi info : \nssid :");
        Serial.print(storedSSID);
        Serial.print(" Password:");
        Serial.println(storedPassword);

        ssid = storedSSID.c_str();
        password = storedPassword.c_str();
    }

    Serial.print(ssid);
    Serial.print(" Password:");
    Serial.println(password);
    WiFi.begin(ssid, password);

    unsigned long startingTime = millis();
    int ledstatus = 0;
    while (WiFi.status() != WL_CONNECTED && (millis() - startingTime) < networkTimeout) {
        ledstatus ? WSLED_Red() : WSLED_OFF();
        ledstatus = !ledstatus;
        Serial.print(".");
        vTaskDelay(250);
    }
    vTaskDelay(1000);
    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\r\n-- wifi connect success! --\r\n");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        WSLED_Green();
        lv_label_set_text(ui_wifiStateLabel, "已连接");
        return true;
    }

    Serial.println("Connection to WiFi failed");
    lv_label_set_text(ui_wifiStateLabel, "未连接");
    WSLED_OFF();

    return false;
}

static void kbClear_cb(lv_event_t *event)
{
    lv_keyboard_t *kb = (lv_keyboard_t *)event->target;
    lv_textarea_set_text(kb->ta, "");
}
static void kbHide_cb(lv_event_t *event)
{
    lv_obj_add_flag(inputKeyboard, LV_OBJ_FLAG_HIDDEN);
}

void initchooseSetUI()
{
    inputKeyboard = lv_keyboard_create(ui_set1Screen);
    lv_obj_add_event_cb(inputKeyboard, kbClear_cb, LV_EVENT_CANCEL, NULL);
    lv_obj_add_event_cb(inputKeyboard, kbHide_cb, LV_EVENT_READY, NULL);
    lv_obj_add_flag(inputKeyboard, LV_OBJ_FLAG_HIDDEN);

    wfList = lv_list_create(ui_wifilistPanel);
    lv_obj_set_size(wfList, 200, 210);
    lv_obj_align(wfList, LV_ALIGN_CENTER, 0, 15);

    subTextarea = lv_textarea_create(ui_w2);
    lv_obj_set_size(subTextarea, 200, 30);
    lv_obj_align_to(subTextarea, ui_Label13, LV_ALIGN_LEFT_MID, 80, 0);
    lv_textarea_set_placeholder_text(subTextarea, "/smartHome/esp32_sub");
    lv_obj_set_style_text_color(subTextarea, lv_color_hex(0x808080), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(subTextarea, 255, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_textarea_set_one_line(subTextarea, true);

    pubTextarea = lv_textarea_create(ui_w2);
    lv_obj_set_size(pubTextarea, 200, 35);
    lv_obj_align_to(pubTextarea, ui_Label17, LV_ALIGN_LEFT_MID, 80, 0);
    lv_textarea_set_placeholder_text(pubTextarea, "/smartHome/esp32_pub");
    lv_obj_set_style_text_color(pubTextarea, lv_color_hex(0x808080), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(pubTextarea, 255, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_textarea_set_one_line(pubTextarea, true);

    weatherTextarea = lv_textarea_create(ui_w2);
    lv_obj_set_size(weatherTextarea, 200, 35);
    lv_obj_align_to(weatherTextarea, ui_Label10, LV_ALIGN_LEFT_MID, 80, 0);
    lv_textarea_set_placeholder_text(weatherTextarea, "guangzhou");
    lv_obj_set_style_text_color(weatherTextarea, lv_color_hex(0x808080), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(weatherTextarea, 255, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_textarea_set_one_line(weatherTextarea, true);

    ntpTextarea = lv_textarea_create(ui_w3);
    lv_obj_set_size(ntpTextarea, 200, 30);
    lv_obj_align_to(ntpTextarea, ui_Label22, LV_ALIGN_LEFT_MID, 80, 0);
    lv_textarea_set_placeholder_text(ntpTextarea, "ntp1.aliyun.com");
    lv_obj_set_style_text_color(ntpTextarea, lv_color_hex(0x808080), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ntpTextarea, 255, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_textarea_set_one_line(ntpTextarea, true);

    ntpTimeTextarea = lv_textarea_create(ui_w3);
    lv_obj_set_size(ntpTimeTextarea, 200, 35);
    lv_obj_align_to(ntpTimeTextarea, ui_Label23, LV_ALIGN_LEFT_MID, 80, 0);
    lv_textarea_set_placeholder_text(ntpTimeTextarea, "360000(ms)");
    lv_obj_set_style_text_color(ntpTimeTextarea, lv_color_hex(0x808080), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ntpTimeTextarea, 255, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_textarea_set_one_line(ntpTimeTextarea, true);

    lv_obj_add_event_cb(subTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(pubTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(weatherTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(ntpTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(ntpTimeTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
}

void updateFlashDate()
{
    String cityIDStr = ReadData("cityid");
    if (cityIDStr == "null") {
        lv_textarea_set_placeholder_text(weatherTextarea, "guangzhou");
        updateCityID("guangzhou");
        StoreData("cityid", "guangzhou");
    } else {
        Serial.println(cityIDStr);
        lv_textarea_set_placeholder_text(weatherTextarea, cityIDStr.c_str());
        updateCityID(cityIDStr);
    }
#if USE_AUDIO
    int volume_c = 15;
    volume_c = ReadintData("volume");
    int station_c = 0;
    station_c = ReadintData("station");
    if (volume_c == 1000) {
        StoreintData("volume", 10);
        StoreintData("station", 0);
    } else {
        audiosetStation(station_c);
        audioVolume(volume_c);
    }
#endif
}

#if USE_AUDIO
void musicbtnCD(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    if (btn == ui_prevButton) {
        Serial.println("prev");
        audioPrevious();
    } else if (btn == ui_nextButton) {
        Serial.println("next");
        audioNext();
    } else if (btn == ui_playButton) {
        Serial.println("play or pause");
        if (getaudioPlayStatus()) {
            audioPause();
        } else {
            audioPlay();
        }
    }
}

static void slider_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    uint8_t volume = ((int)lv_slider_get_value(slider) * 21 / 100);
    Serial.println(volume);
    audioVolume(volume);
}

static void dropdown_event_cd(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        audioStation(lv_dropdown_get_selected(obj));
    }
}

void initUIspeech()
{
    lv_label_set_text(ui_prevLabel, LV_SYMBOL_PREV);
    lv_label_set_text(ui_nextLabel, LV_SYMBOL_NEXT);
    lv_label_set_text(ui_playLabel, LV_SYMBOL_PLAY);

    lv_obj_add_event_cb(ui_prevButton, musicbtnCD, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_nextButton, musicbtnCD, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_playButton, musicbtnCD, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_volumeSlider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_dropdown_set_options(ui_musicDropdown, optionsGet().c_str());
    lv_obj_add_event_cb(ui_musicDropdown, dropdown_event_cd, LV_EVENT_VALUE_CHANGED, NULL);
}
#endif

void initNetWorkUIConfig()
{
    initchooseSetUI();
    buildPWMsgBox();
    // 读取文件musiclist文件
    // readdataList();
#if USE_AUDIO
    initUIspeech();
#endif
    updateFlashDate();
    lv_obj_add_event_cb(ui_wifiSwitch, chooseBtEventCD, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_mqttSwitch, chooseBtEventCD, LV_EVENT_ALL, NULL);
}
