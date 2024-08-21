#include "lvglconfig.h"
#include "confighelpr.h"
#include "module_audio.h"
#include "module_devices.h"
#include "module_mqtt.h"
#include "module_service.h"
#include "module_speak.h"
#include "ui.h"
#include "wificonfig.h"
#include <ArduinoWebsockets.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <lvgl.h>
#include <vector>

#include "lv_fs_if.h"

static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

static int foundNetworks = 0;
SemaphoreHandle_t lvglSemaphore = NULL;
SemaphoreHandle_t xnetworkStatusSemaphore = NULL;

// wifi
extern wifi_buf_t wifi_buf;
extern Network_Status_t networkStatus;
extern TaskHandle_t ntScanTaskHandler, ntConnectTaskHandler;
std::vector<String> foundWifiList;

// web camera
using namespace websockets;
String websockets_server_host = "172.20.10.4";
uint16_t websockets_server_port = 3000;
WebsocketsClient cameraClient;
uint16_t *image_buffer;
static lv_img_dsc_t img_dsc;

extern bool enable_mqtt;
bool isStartCamera = false;

static lv_style_t main_black_style;
static lv_timer_t *chartTimer = NULL;

static const int chartSize = 200;
lv_coord_t pos_array[5][5] = {
    {10, 10, 10, 10, 10},
    {10, 20, 30, 20, 10},
    {50, 40, 10, 40, 50},
    {20, 40, 50, 40, 20},
    {50, 40, 30, 40, 50}};

extern SpeakState_t speakState;
extern String ai_speak;
extern int useAIMode;

extern String ntpServer2;
extern int updatetimentp;

extern TaskHandle_t speakTaskHandle;
/********************************************************************
                         BUILD UI
********************************************************************/

static lv_obj_t *wfList;
// static lv_obj_t *wfListtitle;
static lv_obj_t *mboxConnect;
static lv_obj_t *mboxTitle;
static lv_obj_t *mboxPassword;
static lv_obj_t *mboxConnectBtn;
static lv_obj_t *mboxCloseBtn;
static lv_obj_t *popupBox;
static lv_obj_t *popupBoxCloseBtn;
static lv_timer_t *wifiListtimer;
static lv_obj_t *subTextarea;
static lv_obj_t *pubTextarea;
static lv_obj_t *weatherTextarea;
static lv_obj_t *ntpTextarea;
static lv_obj_t *ntpTimeTextarea;
static lv_obj_t *inputKeyboard;
static lv_obj_t *videoImage;
static lv_obj_t *ipTextarea;
static lv_obj_t *portTextarea;

static TaskHandle_t cameraTaskHandle = NULL;

lampButtonData mqttSwitchState = {false, false, false, false};
extern String username;
extern String upassword;
extern bool loginState;

void ui_timer_init(void);
void ui_clock_update(lv_timer_t *timer);
void my_ui_init(void);

static void timerForNetwork(lv_timer_t *timer);
static void showingFoundWiFiList();
static void list_event_handler(lv_event_t *e);
static void buildPWMsgBox();
static void settext_input_event_cb(lv_event_t *e);
bool startCameraServer();
void closeCameraServer();
static void drawing_screen(void);

/********************************************************************
                         TFT INIT
********************************************************************/

void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();
    lv_disp_flush_ready(disp_drv);
}

void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;
    bool touched = tft.getTouch(&touchX, &touchY, 600);
    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX * 1.33;
        data->point.y = touchY * 0.75;
        // Serial.printf("Data_x: %d , Data_y: %d", touchX, touchY);
    }
}

/********************************************************************
                         INIT LVGL
********************************************************************/

void initLVGLConfig(void)
{
    lv_init();

    // lv_fs_if_init();
    // lv_png_init();

    tft.begin();
    tft.setRotation(1);
    // tft.setRotation(3);
    tft.setTextColor(0xFFFF, 0x0000);
    tft.setSwapBytes(true);
    uint16_t calData[5] = {436, 3332, 277, 3365, 7};
    // uint16_t calData[5] = {490, 3259, 422, 3210, 1};
    tft.setTouch(calData);

    lv_color_t *draw_buf1 = (lv_color_t *)ps_malloc(screenWidth * screenHeight * 2);
    lv_color_t *draw_buf2 = (lv_color_t *)ps_malloc(screenWidth * screenHeight * 2);

    lv_disp_draw_buf_init(&draw_buf, draw_buf1, draw_buf2, screenWidth * screenHeight);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = 1;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    ui_init();
    ui_timer_init();
    my_ui_init();

    // lv_img_set_src(ui_Image4, "F:/lvglimage/t2.png");
    //   LV_FONT_DECLARE(sFont_20)
    //   lv_obj_set_style_text_font(ui_cityLabel, &sFont_20, 0);

    lv_obj_del(ui_weathericonImage2);
    lv_obj_del(ui_weathericonImage3);
    lv_obj_del(ui_weathericonImage4);

    lvglSemaphore = xSemaphoreCreateMutex();
    xnetworkStatusSemaphore = xSemaphoreCreateMutex();

    startLVGLTask();
    Serial.println("lvgl init successfully");
}

void lvgl_task(void *pvParameter)
{
    // TickType_t xLastWakeTime;
    // const TickType_t xPeriod = pdMS_TO_TICKS(5);
    // xLastWakeTime = xTaskGetTickCount();
    while (1) {
        // vTaskDelayUntil(&xLastWakeTime, xPeriod);
        //  xSemaphoreTake(lvglSemaphore, portMAX_DELAY);
        lv_timer_handler();
        // xSemaphoreGive(lvglSemaphore);
        vTaskDelay(5);
    }
    vTaskDelete(NULL);
}

void startLVGLTask(void)
{
    xTaskCreatePinnedToCore(lvgl_task, "lvgl_task", 5 * 1024, NULL, 5, NULL, 1);
}

/********************************************************************
                         LVGL_DEVICES_CONTROL
********************************************************************/
void lampButtonCB(lv_event_t *e)
{
    lv_obj_t *sw = lv_event_get_target(e);
    bool is_on = lv_obj_has_state(sw, LV_STATE_CHECKED);
    // Serial.println(is_on);
    if (sw == ui_lampButton1) {
        mqttSwitchState.lampButton1 = is_on;
        // if (is_on) {
        //     redled_on();
        // } else {
        //     redled_off();
        // }
    } else if (sw == ui_lampButton2) {
        mqttSwitchState.lampButton2 = is_on;
        // if (is_on) {
        //     greenled_on();
        // } else {
        //     greenled_off();
        // }

    } else if (sw == ui_lampButton3) {
        mqttSwitchState.lampButton3 = is_on;
        // if (is_on) {
        //     blueled_on();
        // } else {
        //     blueled_off();
        // }
    } else if (sw == ui_lampButton4) {
        mqttSwitchState.lampButton4 = is_on;
        // if (is_on) {
        //     blueled_on();
        // } else {
        //     blueled_off();
        // }
    }
    pulishSwitchDatas(mqttSwitchState);
}

void updateSwitchState(lampButtonData uSwitchState)
{
    if (uSwitchState.lampButton1) {
        lv_obj_add_state(ui_lampButton1, LV_EVENT_CLICKED);
        redled_on();
    } else {
        lv_obj_clear_flag(ui_lampButton1, LV_EVENT_CLICKED);
        redled_off();
    }

    if (uSwitchState.lampButton2) {
        lv_obj_add_state(ui_lampButton2, LV_EVENT_CLICKED);
        greenled_on();
    } else {
        lv_obj_clear_flag(ui_lampButton2, LV_EVENT_CLICKED);
        greenled_off();
    }

    if (uSwitchState.lampButton3) {
        lv_obj_add_state(ui_lampButton3, LV_EVENT_CLICKED);
        blueled_on();
    } else {
        lv_obj_clear_flag(ui_lampButton3, LV_EVENT_CLICKED);
        blueled_off();
    }

    if (uSwitchState.lampButton4) {
        lv_obj_add_state(ui_lampButton4, LV_EVENT_CLICKED);
        blueled_on();
    } else {
        lv_obj_clear_flag(ui_lampButton4, LV_EVENT_CLICKED);
        blueled_off();
    }
}

/********************************************************************
                         LVGL_TIME UPDATE
********************************************************************/
void ui_timer_init(void)
{
    lv_timer_t *ui_timer = lv_timer_create(ui_clock_update, 2000, NULL);
    ui_clock_update(ui_timer);
}

void ui_clock_update(lv_timer_t *timer)
{
    time_t now;
    struct tm *timeinfo;
    char time_str[9];

    time(&now);                 // 获取当前时间
    timeinfo = localtime(&now); // 将时间转化为本地时间

    sprintf(time_str, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min); // 格式化时间
    lv_label_set_text(ui_timeLabel, time_str);
}

void ui_calender_update(void)
{
    time_t now;
    struct tm *timeinfo;

    time(&now);                 // 获取当前时间
    timeinfo = localtime(&now); // 将时间转化为本地时间

    lv_calendar_set_showed_date(ui_Calendar2, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1);
    lv_calendar_set_today_date(ui_Calendar2, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
    Serial.println("LVGL_Calender_Update Successfully");
}

/********************************************************************
                         LVGL_SET AND GET FUNCTION
********************************************************************/
void lv_setMQTTState(const char *text)
{
    lv_label_set_text(ui_mqttStateLabel, text);
}

void lv_setMQTTSwitchState(bool state)
{
    if (state) {
        lv_label_set_text(ui_mqttConnectStateLabel, "解绑");
        lv_obj_add_state(ui_mqttuseButton, LV_STATE_CHECKED);
    } else {
        lv_label_set_text(ui_mqttConnectStateLabel, "绑定");
        lv_obj_add_state(ui_mqttuseButton, LV_STATE_DEFAULT);
    }
}

bool lv_getMQTTSwitchState()
{
    return lv_obj_has_state(ui_mqttuseButton, LV_STATE_CHECKED);
}

void lv_setWIFIState(const char *text)
{
    lv_label_set_text(ui_wifiStateLabel, text);
}
void lv_setWIFISwitchState(bool state)
{
    if (state) {
        lv_obj_add_state(ui_wifiSwitch, LV_STATE_CHECKED);
    } else {
        lv_obj_add_state(ui_wifiSwitch, LV_STATE_DEFAULT);
    }
}

void lv_setWeatherinfo(const char *text)
{
    lv_label_set_text(ui_weatherLabel, text);
}

void lv_setCityinfo(const char *text)
{
    lv_label_set_text(ui_cityLabel, text);
}

void lv_setWeatherImage(int number)
{
    if (number == 4 || number == 9) {
        lv_img_set_src(ui_weathericonImage, &ui_img_520433372);
    } else if (number == 0) {
        lv_img_set_src(ui_weathericonImage, &ui_img_1282432712);
    } else if (number == 5 || number == 8) {
        lv_img_set_src(ui_weathericonImage, &ui_img_31831977);
    } else if (number >= 10 && number <= 19) {
        lv_img_set_src(ui_weathericonImage, &ui_img_1809401540);
    } else {
        lv_img_set_src(ui_weathericonImage, &ui_img_520433372);
    }
}

void lv_setTipinfo(const char *text)
{
    if (ui_tipLabel == NULL) {
        return;
    }
    lv_label_set_text(ui_tipLabel, text);
}

void lv_gohome(void)
{
    Serial.println("loading main screen ...");
    lv_scr_load_anim(ui_MainScreen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, true);
}

void lv_setPlayState(bool state)
{
    if (state) {
        lv_label_set_text(ui_playLabel, LV_SYMBOL_PAUSE);
    } else {
        lv_label_set_text(ui_playLabel, LV_SYMBOL_PLAY);
    }
}

void lv_setDropdown(int index)
{
    lv_dropdown_set_selected(ui_musicDropdown, index);
}
void lv_setMusicinfo(const char *text)
{
    lv_label_set_text(ui_Label25, text);
}

void lv_setDropdowninfo(const char *options)
{
    lv_dropdown_set_options(ui_musicDropdown, options);
}

void lv_setDropdownaddinfo(const char *option, int pos)
{
    lv_dropdown_add_option(ui_musicDropdown, option, pos);
}

void lv_setSliderVolume(int value)
{
    lv_slider_set_value(ui_volumeSlider, value, LV_ANIM_OFF);
}

void lv_setCameraImage(const void *path)
{
    lv_img_set_src(videoImage, path);
}

void lv_setSpeechinfo(const char *text)
{
    lv_label_set_text(ui_speechStateLabel, text);
}

void lv_setIPinfo(const char *text)
{
    lv_label_set_text(ui_ipLabel, text);
}

void lv_setstatusbarLabel(int status)
{
    if (status == 0) {
        lv_label_set_text(ui_statusbarLabel, "");
    } else if (status == 1) {
        lv_label_set_text(ui_statusbarLabel, LV_SYMBOL_WIFI);
    } else if (status == 2) {
        lv_label_set_text(ui_statusbarLabel, LV_SYMBOL_DRIVE);
    } else if (status == 3) {
        lv_label_set_text(ui_statusbarLabel, LV_SYMBOL_WIFI LV_SYMBOL_DRIVE);
    } else if (status == 4) {
        lv_label_set_text(ui_statusbarLabel, LV_SYMBOL_WARNING);
    }
}

/********************************************************************
                         LVGL_SET_UI
********************************************************************/
static void kbClear_cb(lv_event_t *event)
{
    lv_keyboard_t *kb = (lv_keyboard_t *)event->target;
    lv_textarea_set_text(kb->ta, "");
}
static void kbHide_cb(lv_event_t *event)
{
    lv_obj_add_flag(inputKeyboard, LV_OBJ_FLAG_HIDDEN);
}

static void list_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        wifi_buf.ssid = String(lv_list_get_btn_text(wfList, obj));
        Serial.println(wifi_buf.ssid);
        lv_label_set_text_fmt(mboxTitle, "Selected WiFi SSID: %s", wifi_buf.ssid);
        lv_obj_move_foreground(mboxConnect);
        lv_obj_clear_flag(mboxConnect, LV_OBJ_FLAG_HIDDEN);
    }
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

void setChooseScreenCD(lv_event_t *e)
{
    lv_obj_t *setbt = lv_event_get_target(e);
    lv_scr_load_anim(ui_set1Screen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, false);
    if (setbt == ui_wifiSetButton) {
        lv_tabview_set_act(ui_chooseTabView, 0, LV_ANIM_OFF);
        if (lv_obj_has_state(ui_wifiSwitch, LV_STATE_CHECKED)) {
            if (ntScanTaskHandler == NULL) {
                xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
                networkStatus = NETWORK_SEARCHING;
                xSemaphoreGive(xnetworkStatusSemaphore);
                networkScanner();
                if (wifiListtimer == NULL) {
                    wifiListtimer = lv_timer_create(timerForNetwork, 1000, wfList);
                }
                lv_list_add_text(wfList, "WiFi:Looking for Networks");
            }
        }
    } else if (setbt == ui_networkSetButton) {
        lv_tabview_set_act(ui_chooseTabView, 1, LV_ANIM_OFF);
    } else if (setbt == ui_timeSetButton) {
        lv_tabview_set_act(ui_chooseTabView, 2, LV_ANIM_OFF);
    } else if (setbt == ui_speechSetButton) {
        lv_tabview_set_act(ui_chooseTabView, 3, LV_ANIM_OFF);
    } else if (setbt == ui_monitorSetButton) {
        lv_tabview_set_act(ui_chooseTabView, 4, LV_ANIM_OFF);
    }
}

void chooseBtEventCD(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {
        if (btn == mboxConnectBtn) {
            // 连接wifi按钮,开始连接wifi
            wifi_buf.pass = String(lv_textarea_get_text(mboxPassword));
            lv_textarea_set_text(mboxPassword, "");
            // 开一个任务连接wifi
            wifiConnector();
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
            if (getwifistate()) {
                if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                    String substr = lv_textarea_get_text(subTextarea);
                    String pubstr = lv_textarea_get_text(pubTextarea);
                    if (!substr.isEmpty()) {
                        lv_textarea_set_placeholder_text(subTextarea, substr.c_str());
                        lv_textarea_set_text(subTextarea, "");
                    }
                    if (!pubstr.isEmpty()) {
                        // lv_textarea_set_placeholder_text(pubTextarea, pubstr.c_str());
                        lv_textarea_set_text(pubTextarea, "");
                    }
                    // 请求验证账号
                    if (postlogin(substr, pubstr)) {
                        lv_setMQTTSwitchState(true);
                        StoreintData("isLogin", 1);
                        StoreData("username", substr.c_str());
                        mqtMontage(substr);
                        loginState = true;
                        enable_mqtt = true;
                        lv_setMQTTState("正在连接");
                        Serial.println("打开mqtt开关,开始连接");
                    } else {
                        lv_setMQTTSwitchState(false);
                        StoreintData("isLogin", 0);
                        loginState = false;
                        enable_mqtt = false;
                        Serial.println("no network dont check mqttSwitch");
                        lv_obj_add_state(btn, LV_STATE_DEFAULT);
                    }
                } else {
                    lv_setMQTTSwitchState(false);
                    StoreintData("isLogin", 0);
                    loginState = false;
                    enable_mqtt = false;
                    Serial.println("关闭mqtt开关,断开连接");
                    if (getMqttStart()) {
                        lv_setMQTTState("未连接");
                        mqtt_disconnect();
                        if (getwifistate()) {
                            lv_setstatusbarLabel(1);
                        } else {
                            lv_setstatusbarLabel(0);
                        }
                    }
                    Serial.println("ui_mqttSwitch NO LV_STATE_CHECKED");
                }
            } else {
                lv_setMQTTSwitchState(false);
                loginState = false;
                enable_mqtt = false;
                Serial.println("no network dont check mqttSwitch");
                lv_obj_add_state(btn, LV_STATE_DEFAULT);
            }
        } else if (btn == ui_timeuseButton) {
            Serial.println("ui_timeuseButton LV_EVENT_CLICKED");
            String ntpstr = lv_textarea_get_text(ntpTextarea);
            String updatetimestr = lv_textarea_get_text(ntpTimeTextarea);
            if (!ntpstr.isEmpty()) {
                StoreData("timeurl", ntpstr.c_str());
                ntpServer2 = ntpstr;
            }
            if (!updatetimestr.isEmpty()) {
                StoreData("updatetime", updatetimestr.c_str());
                updatetimentp = updatetimestr.toInt();
                updateTimer();
            }

            String citystr = lv_textarea_get_text(weatherTextarea);
            if (!citystr.isEmpty()) {
                lv_textarea_set_placeholder_text(weatherTextarea, citystr.c_str());
                lv_textarea_set_text(weatherTextarea, "");
                StoreData("cityid", citystr.c_str());
                Serial.println(citystr);
                updateCityID(citystr);
                weatherQuery();
            }
        } else if (btn == ui_cameraButton) {
            Serial.println("ui_cameraButton LV_EVENT_CLICKED");
            String ipstr = lv_textarea_get_text(ipTextarea);
            String portstr = lv_textarea_get_text(portTextarea);
            if (!ipstr.isEmpty()) {
                StoreData("cameraip", ipstr.c_str());
                websockets_server_host = ipstr;
            }
            if (portstr.isEmpty()) {
                portstr = "100001";
            }
            StoreData("cameraport", portstr.c_str());
            websockets_server_port = portstr.toInt();
            lv_scr_load_anim(ui_monitorScreen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, false);
        }
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        if (btn == ui_wifiSwitch) {
            if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                if (ntScanTaskHandler == NULL) {
                    xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
                    networkStatus = NETWORK_SEARCHING;
                    xSemaphoreGive(xnetworkStatusSemaphore);
                    networkScanner();
                    wifi_buf.ssid = ReadData("ssid");
                    wifi_buf.pass = ReadData("password");
                    wifiConnector();
                    if (wifiListtimer == NULL) {
                        wifiListtimer = lv_timer_create(timerForNetwork, 1000, wfList);
                        lv_list_add_text(wfList, "WiFi:Looking for Networks");
                    }
                }
            } else {
                lv_list_add_text(wfList, "");
                lv_obj_clean(wfList);

                if (ntScanTaskHandler != NULL) {
                    xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
                    networkStatus = NONE;
                    xSemaphoreGive(xnetworkStatusSemaphore);
                    vTaskDelete(ntScanTaskHandler);
                    ntScanTaskHandler = NULL;
                }

                if (ntConnectTaskHandler != NULL) {
                    vTaskDelete(ntConnectTaskHandler);
                }

                if (wifiListtimer != NULL) {
                    lv_timer_del(wifiListtimer);
                    wifiListtimer = NULL;
                }

                if (getwifistate()) {
                    wifiDisconnect();
                    lv_label_set_text(ui_wifiStateLabel, "未连接");
                }
            }
        }
    }
}

void chooseScreenFCD(lv_event_t *e)
{
    lv_list_add_text(wfList, "");

    if (ntScanTaskHandler != NULL) {
        vTaskDelete(ntScanTaskHandler);
        ntScanTaskHandler = NULL;
        if (wifiListtimer != NULL) {
            lv_timer_del(wifiListtimer);
            wifiListtimer = NULL;
        }
    }
    lv_scr_load_anim(ui_setchooseScreen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, false);
}

void SpeechSetDCD(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    char tmp_buf[32];
    lv_dropdown_get_selected_str(btn, tmp_buf, sizeof(tmp_buf));
    Serial.println(tmp_buf);
    if (btn == ui_speechDropdown) {
        Serial.printf("speech dropdown number : %d\n", lv_dropdown_get_selected(btn));
        StoreintData("speech_id", lv_dropdown_get_selected(btn));
        ai_speak = tmp_buf;
    } else if (btn == ui_aimodeDropdown) {
        Serial.printf("ai mdoe dropdown number : %d\n", lv_dropdown_get_selected(btn));
        useAIMode = lv_dropdown_get_selected(btn);
        StoreintData("speech_ai_mode", lv_dropdown_get_selected(btn));
    }
}

void initSetConfigUI()
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
    // lv_textarea_set_placeholder_text(subTextarea, "/smartHome/esp32_sub");
    lv_obj_set_style_text_color(subTextarea, lv_color_hex(0x808080), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(subTextarea, 255, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_textarea_set_one_line(subTextarea, true);

    pubTextarea = lv_textarea_create(ui_w2);
    lv_obj_set_size(pubTextarea, 200, 35);
    lv_obj_align_to(pubTextarea, ui_Label17, LV_ALIGN_LEFT_MID, 80, 0);
    // lv_textarea_set_placeholder_text(pubTextarea, "/smartHome/esp32_pub");
    lv_obj_set_style_text_color(pubTextarea, lv_color_hex(0x808080), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(pubTextarea, 255, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_textarea_set_one_line(pubTextarea, true);

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
    lv_textarea_set_placeholder_text(ntpTimeTextarea, "360(s)");
    lv_obj_set_style_text_color(ntpTimeTextarea, lv_color_hex(0x808080), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ntpTimeTextarea, 255, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_textarea_set_one_line(ntpTimeTextarea, true);

    weatherTextarea = lv_textarea_create(ui_w3);
    lv_obj_set_size(weatherTextarea, 200, 35);
    lv_obj_align_to(weatherTextarea, ui_Label10, LV_ALIGN_LEFT_MID, 80, 0);
    lv_textarea_set_placeholder_text(weatherTextarea, "guangzhou");
    lv_obj_set_style_text_color(weatherTextarea, lv_color_hex(0x808080), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(weatherTextarea, 255, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_textarea_set_one_line(weatherTextarea, true);

    ipTextarea = lv_textarea_create(ui_W5);
    lv_obj_set_size(ipTextarea, 200, 35);
    lv_obj_align_to(ipTextarea, ui_Label66, LV_ALIGN_LEFT_MID, 40, 0);
    lv_textarea_set_placeholder_text(ipTextarea, websockets_server_host.c_str());
    lv_obj_set_style_text_color(ipTextarea, lv_color_hex(0x808080), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ipTextarea, 255, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_textarea_set_one_line(ipTextarea, true);

    portTextarea = lv_textarea_create(ui_W5);
    lv_obj_set_size(portTextarea, 200, 35);
    lv_obj_align_to(portTextarea, ui_Label67, LV_ALIGN_LEFT_MID, 60, 0);
    lv_textarea_set_placeholder_text(portTextarea, "3000");
    lv_obj_set_style_text_color(portTextarea, lv_color_hex(0x808080), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(portTextarea, 255, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_textarea_set_one_line(portTextarea, true);

    lv_obj_add_event_cb(subTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(pubTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(weatherTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(ntpTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(ntpTimeTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(ipTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(portTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);

    lv_obj_add_event_cb(ui_wifiSwitch, chooseBtEventCD, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_speechDropdown, SpeechSetDCD, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_aimodeDropdown, SpeechSetDCD, LV_EVENT_VALUE_CHANGED, NULL);

    lv_style_init(&main_black_style);
    lv_style_set_border_width(&main_black_style, 0);
    lv_style_set_radius(&main_black_style, 20);
    lv_style_set_bg_opa(&main_black_style, 0);
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
        if (getwifistate()) {
            // Serial.println(getwifissid().c_str());
            if (*item == getwifissid()) {
                lv_obj_set_style_bg_color(wifibtnc, lv_color_hex(0x2095f6), LV_PART_MAIN);
            }
        }

        delay(1);
        num++;
        if (num >= 15) {
            break;
        }
    }
    foundNetworks = foundWifiList.size();
}

static void timerForNetwork(lv_timer_t *timer1)
{
    LV_UNUSED(timer1);
    if (lv_scr_act() != ui_set1Screen) {
        Serial.println("不是当前屏幕删除删除计时器");
        xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
        networkStatus = NONE;
        xSemaphoreGive(xnetworkStatusSemaphore);
        if (ntScanTaskHandler != NULL) {
            vTaskDelete(ntScanTaskHandler);
            ntScanTaskHandler = NULL;
        }
        if (wifiListtimer != NULL) {
            lv_timer_del(wifiListtimer);
            wifiListtimer = NULL;
        }
    }

    switch (networkStatus) {
    case NETWORK_SEARCHING:
        if (ntScanTaskHandler == NULL) {
            networkScanner();
        }
        break;
    case NETWORK_SEARCHED: {
        xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
        networkStatus = NETWORK_SEARCHING;
        xSemaphoreGive(xnetworkStatusSemaphore);
        showingFoundWiFiList();
        break;
    }
    case NETWORK_CONNECTED: {
        lv_setWIFIState("已连接");
        xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
        networkStatus = NETWORK_SEARCHING;
        xSemaphoreGive(xnetworkStatusSemaphore);
        showingFoundWiFiList();
        break;
    }
    case NETWORK_CONNECT_FAILED: {
        xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
        networkStatus = NETWORK_SEARCHING;
        xSemaphoreGive(xnetworkStatusSemaphore);
        if (ntConnectTaskHandler != NULL) {
            vTaskDelete(ntConnectTaskHandler);
        }
        lv_setWIFIState("连接失败");
        popupMsgBox("Oops!", "连接失败,请检查密码");
        break;
    }
    default:
        break;
    }
}

/********************************************************************
                         AUDIO_UI
********************************************************************/
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
    Serial.println((int)lv_slider_get_value(slider));
    audioVolume((int)lv_slider_get_value(slider));
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

    lv_obj_add_event_cb(ui_musicDropdown, dropdown_event_cd, LV_EVENT_VALUE_CHANGED, NULL);
}

#endif

/********************************************************************
                         CAMMER_UI
********************************************************************/
void cameraScreenCD(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    if (btn == ui_connectCameraButton) {
        if (code == LV_EVENT_CLICKED) {
            if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                lv_label_set_text(ui_cameraLabel, "关闭");
                Serial.println("connect cameraserver");
                isStartCamera = true;
                startCameraTask();
            } else {
                lv_label_set_text(ui_cameraLabel, "连接");
                Serial.println("disconnect cameraserver");
                closeCameraServer();
            }
        }
    }
}

void monitorScreenOCD(lv_event_t *e)
{
    vTaskSuspend(speakTaskHandle);
    lv_scr_load_anim(ui_monitorScreen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, false);
}

void monitorScreenFCD(lv_event_t *e)
{
    Serial.println("return cameraserver");
    lv_obj_clear_state(ui_connectCameraButton, LV_STATE_CHECKED);
    lv_label_set_text(ui_cameraLabel, "连接");
    lv_label_set_text(ui_cameraStateLabel, "未连接");
    closeCameraServer();
    if (eTaskGetState(speakTaskHandle) == eTaskState::eSuspended) {
        Serial.println("vTaskResume speakTaskHandle");
        vTaskResume(speakTaskHandle);
    }
    lv_scr_load_anim(ui_MainScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, false);
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    // if (y >= tft.height())
    //     return 0;
    // if (x >= tft.width())
    //     return 0;

    for (uint16_t j = 0; j < h; j++) {
        for (uint16_t i = 0; i < w; i++) {
            image_buffer[(y + j) * screenWidth + (x + i)] = bitmap[j * w + i];
        }
    }

    // for (uint16_t j = 0; j < h; j++) {
    //     for (uint16_t i = 0; i < w; i++) {
    //         uint16_t pixel = bitmap[j * w + i];
    //         uint16_t swapped_pixel = (pixel >> 8) | (pixel << 8);
    //         image_buffer[(y + j) * screenWidth + (x + i)] = swapped_pixel;
    //     }
    // }

    return 1;
}
bool startCameraServer()
{
    lv_label_set_text(ui_cameraStateLabel, "正在连接");
    if ((websockets_server_port == 100001) && (cameraClient.connect(websockets_server_host))) {
    } else if ((cameraClient.connect(websockets_server_host, websockets_server_port, "/"))) {
    } else {
        Serial.println("WebSocket connect failed.");
        lv_label_set_text(ui_cameraStateLabel, "连接失败");
        return false;
    }
    Serial.println("Connected to WebSocket server");
    lv_label_set_text(ui_cameraStateLabel, "已连接");
    return true;
}

void closeCameraServer()
{
    lv_label_set_text(ui_cameraStateLabel, "未连接");

    if (isStartCamera) {
        publishStartVideo(false);
        isStartCamera = false;
    }

    cameraClient.close();
    if (cameraTaskHandle != NULL) {
        vTaskDelete(cameraTaskHandle);
        cameraTaskHandle = NULL;
    }
}

void initCameraUI()
{
    videoImage = lv_img_create(ui_monitorScreen);
    lv_obj_set_pos(videoImage, 0, 0);
    lv_obj_set_size(videoImage, 320, 240);
    lv_obj_move_foreground(ui_connectCameraButton);
    lv_obj_move_foreground(ui_cameraStateLabel);

    TJpgDec.setJpgScale(1);
    TJpgDec.setSwapBytes(false);
    // TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(tft_output);
    image_buffer = (uint16_t *)ps_malloc(320 * 240 * sizeof(uint16_t));
    img_dsc.header.always_zero = 0;
    img_dsc.header.w = 320;
    img_dsc.header.h = 240;
    img_dsc.data_size = img_dsc.header.w * img_dsc.header.h * sizeof(uint16_t);
    img_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
}

void videocameratask(void *pvParameter)
{
    Serial.println("start camera task");

    while (1) {
        if (cameraClient.poll()) {
            WebsocketsMessage msg = cameraClient.readBlocking();
            TJpgDec.drawJpg(0, 0, (const uint8_t *)msg.c_str(), msg.length());

            img_dsc.data = (const uint8_t *)image_buffer;
            lv_img_set_src(videoImage, &img_dsc);
            lv_obj_invalidate(videoImage);
        }
        vTaskDelay(100);
    }
    vTaskDelete(NULL);
}

void startCameraTask()
{
    if (isStartCamera && cameraTaskHandle == NULL) {
        if (!startCameraServer()) {
            lv_obj_clear_state(ui_connectCameraButton, LV_STATE_CHECKED);
            lv_label_set_text(ui_cameraLabel, "连接");
            lv_label_set_text(ui_cameraStateLabel, "连接失败");
            isStartCamera = false;
            return;
        }
        publishStartVideo(true);
        xTaskCreatePinnedToCore(&videocameratask, "videocamera_task", 5 * 1024, NULL, 3, &cameraTaskHandle, 1);
    } else {
        Serial.println("cameraTaskHandle is not NULL");
    }
}

/********************************************************************
                         LVGL_SPEAK_UI
********************************************************************/

void speakScreenCD(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        if (btn == ui_Button4) {
            lv_scr_load_anim(ui_speechScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, false);
            drawing_screen();
        } else if (btn == ui_speakButton) {
            if (lv_obj_has_state(btn, LV_STATE_FOCUSED)) {
                lv_label_set_text(ui_speakLabel, "录制");
                speakState = RECORDING;
                Serial.println("录制中");
            } else {
                lv_label_set_text(ui_speakLabel, "录音");
                Serial.println("录音");
            }
            Serial.println("speakState = " + String(speakState));
        }
    }
}

void speakScreenFCD(lv_event_t *e)
{
    if (chartTimer != NULL) {
        lv_timer_del(chartTimer);
        chartTimer = NULL;
    }

    lv_scr_load_anim(ui_MainScreen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, false);
}

static void update_particle_effect(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    // Serial.println("updata particle");
    lv_chart_set_ext_y_array(ui_expressionChart, lv_chart_get_series_next(ui_expressionChart, NULL), pos_array[speakState]);
    lv_chart_refresh(ui_expressionChart);
}

static void drawing_screen(void)
{
    if (chartTimer == NULL) {
        chartTimer = lv_timer_create(update_particle_effect, 1000, NULL);
        update_particle_effect(chartTimer);
    }
}

/********************************************************************
                         LVGL_DATA_UPDATE
********************************************************************/
void initDataUI()
{
    String cityIDStr = ReadData("cityid");
    if (cityIDStr == "null") {
        lv_textarea_set_placeholder_text(weatherTextarea, "guangzhou");
        updateCityID("guangzhou");
        StoreData("cityid", "guangzhou");
    } else {
        Serial.printf("device city: %s\n", cityIDStr);
        lv_textarea_set_placeholder_text(weatherTextarea, cityIDStr.c_str());
        updateCityID(cityIDStr);
    }

    String timeurl = ReadData("timeurl");
    if (timeurl != "null") {
        lv_textarea_set_placeholder_text(ntpTextarea, timeurl.c_str());
        ntpServer2 = timeurl;
    }

    String updatetime = ReadData("updatetime");
    if (updatetime != "null") {
        lv_textarea_set_placeholder_text(ntpTimeTextarea, updatetime.c_str());
        updatetimentp = updatetime.toInt();
    }

    int speech_id = ReadintData("speech_id");
    if (speech_id != 1000) {
        lv_dropdown_set_selected(ui_speechDropdown, speech_id);
        char tmp_buf[32];
        lv_dropdown_get_selected_str(ui_speechDropdown, tmp_buf, sizeof(tmp_buf));
        ai_speak = tmp_buf;
    }
    int speech_ai_mode = ReadintData("speech_ai_mode");
    if (speech_ai_mode != 1000) {
        lv_dropdown_set_selected(ui_aimodeDropdown, speech_ai_mode);
        useAIMode = speech_ai_mode;
    }

    String camera_ip = ReadData("cameraip");
    if (camera_ip != "null") {
        lv_textarea_set_placeholder_text(ipTextarea, camera_ip.c_str());
        websockets_server_host = camera_ip;
    } else {
        lv_textarea_set_placeholder_text(ipTextarea, websockets_server_host.c_str());
        StoreData("cameraip", websockets_server_host.c_str());
    }
    String camera_port = ReadData("cameraport");
    if (camera_port != "null") {
        lv_textarea_set_placeholder_text(portTextarea, camera_port.c_str());
        websockets_server_port = camera_port.toInt();

    } else {
        lv_textarea_set_placeholder_text(portTextarea, String(websockets_server_port).c_str());
        StoreData("cameraport", String(websockets_server_port).c_str());
    }
}

/********************************************************************
                         LVGL_MY_UI_INIT
********************************************************************/
void my_ui_init(void)
{
    initSetConfigUI();
    buildPWMsgBox();
    initCameraUI();
    initDataUI();

#if USE_AUDIO
    initUIspeech();
#endif
}