#include "lvglconfig.h"
#include "confighelpr.h"
#include "module_audio.h"
#include "module_devices.h"
#include "module_mqtt.h"
#include "module_service.h"
#include "ui.h"
#include "wificonfig.h"
#include <ArduinoWebsockets.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <lvgl.h>
#include <vector>

static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

static int foundNetworks = 0;

// wifi
String ssidName, ssidPW;
std::vector<String> foundWifiList;

// web camera
websockets::WebsocketsClient client;

/********************************************************************
                         BUILD UI
********************************************************************/

static lv_obj_t *wfList;
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

void ui_timer_init(void);
void ui_clock_update(lv_timer_t *timer);
void my_ui_init(void);

static void timerForNetwork(lv_timer_t *timer);
static void showingFoundWiFiList();
static void list_event_handler(lv_event_t *e);
static void buildPWMsgBox();
static void settext_input_event_cb(lv_event_t *e);

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

    tft.begin();
    tft.setRotation(3);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, 1);
    uint16_t calData[5] = {490, 3259, 422, 3210, 1};
    tft.setTouch(calData);
    static lv_color_t draw_buf1[screenWidth * screenHeight / 2];
    static lv_color_t draw_buf2[screenWidth * screenHeight / 2];

    lv_disp_draw_buf_init(&draw_buf, draw_buf1, draw_buf2, screenWidth * screenHeight / 10);

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
    // lv_obj_set_style_text_font(ui_cityLabel, &sFont_20, 0);

    lv_obj_del(ui_weathericonImage2);
    lv_obj_del(ui_weathericonImage3);
    lv_obj_del(ui_weathericonImage4);

    startLVGLTask();
    Serial.println("lvgl init successfully");
}

void lvgl_task(void *pvParameter)
{
    while (1) {
        lv_timer_handler();
        vTaskDelay(5);
    }
    vTaskDelete(NULL);
}

void startLVGLTask(void)
{
    xTaskCreatePinnedToCore(lvgl_task, "lvgl_task", 5 * 1024, NULL, 2, NULL, 1);
}

/********************************************************************
                         LVGL_DEVICES_CONTROL
********************************************************************/
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
        lv_obj_add_state(ui_mqttSwitch, LV_STATE_CHECKED);
    } else {
        lv_obj_add_state(ui_mqttSwitch, LV_STATE_DEFAULT);
    }
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
        // Serial.println(ssidName);
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
            wifiConnector(wifi_buf);
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
                StoreData("cityid", citystr.c_str());
                Serial.println(citystr);
                updateCityID(citystr);
                weatherQuery();
            }

        } else if (btn == ui_timeuseButton) {
            Serial.println("ui_timeuseButton LV_EVENT_CLICKED");
        } else if (btn == ui_connectButton) {
            Serial.println("change screen monitor");
            startCameraTask();
        }
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        if (btn == ui_wifiSwitch) {
            if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                if (ntScanTaskHandler == NULL) {
                    xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
                    networkStatus = NETWORK_SEARCHING;
                    xSemaphoreGive(xnetworkStatusSemaphore);
                    networkScanner();
                    wifiConnector(wifi_buf);
                    if (wifiListtimer == NULL) {
                        wifiListtimer = lv_timer_create(timerForNetwork, 1000, wfList);
                        lv_list_add_text(wfList, "WiFi:Looking for Networks");
                    }
                }
            } else {
                lv_obj_clean(wfList);
                lv_list_add_text(wfList, "");

                if (ntScanTaskHandler != NULL) {
                    xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
                    networkStatus = NONE;
                    xSemaphoreGive(xnetworkStatusSemaphore);
                    vTaskDelete(ntScanTaskHandler);
                    ntScanTaskHandler = NULL;
                    lv_timer_del(wifiListtimer);
                    wifiListtimer = NULL;
                }
                if (getwifistate()) {
                    wifiDisconnect();
                    lv_label_set_text(ui_wifiStateLabel, "未连接");
                }
            }
        } else if (btn == ui_mqttSwitch) {
            Serial.println(hasNetwork);
            if (getwifistate() && hasNetwork) {
                if (lv_obj_has_state(ui_mqttSwitch, LV_STATE_CHECKED)) {
                    enable_mqtt = true;
                    lv_setMQTTState("正在连接");
                    Serial.println("打开mqtt开关,开始连接");
                } else {
                    enable_mqtt = false;
                    Serial.println("关闭mqtt开关,断开连接");
                    if (getMqttStart()) {
                        lv_setMQTTState("未连接");
                        mqtt_disconnect();
                    }
                    Serial.println("ui_mqttSwitch NO LV_STATE_CHECKED");
                }
            } else {
                Serial.println("no network dont check mqttSwitch");
                lv_obj_add_state(ui_mqttSwitch, LV_STATE_DEFAULT);
            }
        }
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

    lv_obj_add_event_cb(ui_wifiSwitch, chooseBtEventCD, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_mqttSwitch, chooseBtEventCD, LV_EVENT_ALL, NULL);
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
        vTaskDelete(ntScanTaskHandler);
        ntScanTaskHandler = NULL;
        lv_timer_del(wifiListtimer);
        wifiListtimer = NULL;
    }

    switch (networkStatus) {
    case NETWORK_SEARCHING:
        break;
    case NETWORK_SEARCHED:
        xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
        networkStatus = NETWORK_SEARCHING;
        xSemaphoreGive(xnetworkStatusSemaphore);
        showingFoundWiFiList();
        break;
    case NETWORK_CONNECTED:
        lv_setWIFIState("已连接");
        xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
        networkStatus = NETWORK_SEARCHING;
        xSemaphoreGive(xnetworkStatusSemaphore);
        showingFoundWiFiList();
        break;
    case NETWORK_CONNECT_FAILED:
        xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
        networkStatus = NETWORK_SEARCHING;
        xSemaphoreGive(xnetworkStatusSemaphore);
        lv_setWIFIState("连接失败");
        popupMsgBox("Oops!", "连接失败,请检查密码");
        break;
    default:
        break;
    }
}

void initMutex()
{
    xnetworkStatusSemaphore = xSemaphoreCreateMutex();
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

void onMessageCallback(websockets::WebsocketsMessage message)
{

    Serial.println(message.length());
    Serial.println(message.c_str());
}

void initCameraService()
{
    client.onMessage(onMessageCallback);
    client.connect("http://192.168.0.163:80");
}

void initCameraUI()
{
    videoImage = lv_img_create(ui_monitorScreen);
    lv_obj_set_pos(videoImage, 0, 0);
    lv_obj_set_size(videoImage, 320, 240);
    initCameraService();
}

void videocameratask(void *pvParameter)
{

    while (1) {
        Serial.println("start camera ....");
        vTaskDelay(1000);

        if (client.available()) {
            client.poll();
        }

        // if (fb == NULL) {
        //     vTaskDelay(100);
        //     Serial.println("get fb failed");
        // } else {
        //     lv_img_set_src(videoImage, &videoimg_dsc);
        // }
        vTaskDelay(100);
    }
}

void startCameraTask()
{
    initCameraService();
    Serial.println("start camera task");
    xTaskCreatePinnedToCore(&videocameratask, "videocamera_task", 5 * 1024, NULL, 3, NULL, 0);
}

/********************************************************************
                         LVGL_DATA_UPDATE
********************************************************************/

void updateFlashDate()
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
}

/********************************************************************
                         LVGL_MY_UI_INIT
********************************************************************/
void my_ui_init(void)
{
    initSetConfigUI();
    buildPWMsgBox();
    updateFlashDate();
    initMutex();
#if USE_AUDIO
    initUIspeech();
#endif
}