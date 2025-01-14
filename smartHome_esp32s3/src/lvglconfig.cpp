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

static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

static int foundNetworks = 0;
SemaphoreHandle_t xnetworkStatusSemaphore = NULL;

// wifi
extern wifi_buf_t wifi_buf;
extern Network_Status_t networkStatus;
extern TaskHandle_t ntScanTaskHandler, ntConnectTaskHandler;
std::vector<String> foundWifiList;

// web camera
using namespace websockets;
String websockets_server_host = "47.115.139.166";
uint16_t websockets_server_port = 3000;
WebsocketsClient cameraClient;
uint16_t *image_buffer;
static lv_img_dsc_t img_dsc;

extern bool enable_mqtt;
bool isStartCamera = false;

static lv_style_t main_black_style;

static const int chartSize = 200;
lv_coord_t pos_array[6][5] = {
    {10, 10, 10, 10, 10},
    {30, 10, 20, 10, 30},
    {50, 40, 10, 40, 50},
    {20, 40, 50, 40, 20},
    {10, 20, 40, 20, 10},
    {50, 40, 30, 40, 50}};

extern SpeakState_t speakState;
extern int useAIMode;

extern int updatetimentp;
extern String FirmwareVersion;
extern String latestFirmware;

extern bool enbeleWakeUp;

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
static lv_obj_t *ntpTimeTextarea;
static lv_obj_t *inputKeyboard;
static lv_obj_t *videoImage;
static lv_obj_t *videoLeft;
static lv_obj_t *videoRight;
static lv_obj_t *videoLeftlabel;
static lv_obj_t *videoRightlabel;
static lv_obj_t *ipTextarea;
static lv_obj_t *portTextarea;
static lv_obj_t *deviceRePwButton;
static lv_obj_t *deviceRePwLabel;
static lv_obj_t *otaLabel;
static lv_obj_t *otaButton;
static lv_obj_t *otabuttonLabel;
/// 设备控制界面
static lv_obj_t *ButtonFan;
static lv_obj_t *stateLabelFan;
static lv_obj_t *stateImageFan;
static lv_obj_t *nameLabelFan;
static lv_obj_t *ButtonCurtain;
static lv_obj_t *stateLabelCurtain;
static lv_obj_t *stateImageCurtain;
static lv_obj_t *nameLabelCurtain;
static lv_obj_t *ui_ButtonDoorContact;
static lv_obj_t *ui_stateLabelDoorContact;
static lv_obj_t *ui_stateImageDoorContact;
static lv_obj_t *ui_nameLabelDoorContact;
static lv_obj_t *ui_doorContactLabeltitle;
static lv_obj_t *ui_doorContactLabelsound;
static lv_obj_t *ui_doorContactPanel;
static lv_obj_t *ui_openSoundSwitch;
static lv_obj_t *ui_Dropdowntime;
static lv_obj_t *ui_doorContactLabeltimeout;
static lv_obj_t *ui_timeoutSwitch;
static lv_obj_t *ui_doorContactButtonsure;
static lv_obj_t *ui_doorContactLabelsure;
static lv_obj_t *ui_doorContactLabelTime;
static lv_obj_t *ui_curtainPanel;
static lv_obj_t *ui_curtainLabeltitle;
static lv_obj_t *ui_curtainLabelsound;
static lv_obj_t *ui_curtainButtonsure;
static lv_obj_t *ui_curtainLabelsure;
static lv_obj_t *ui_curtainLabelTime;
static lv_obj_t *ui_curtainButtonq;
static lv_obj_t *ui_curtainLabelsure2;
// static lv_obj_t *ui_curtainDropdown;
static lv_obj_t *ui_curtainTextarea;
static lv_obj_t *inputKeyboardNumber;
static lv_obj_t *ui_LabelBLE;
static lv_obj_t *ui_BLESwitch;
static lv_obj_t *ui_Panel4;
static lv_obj_t *ui_otaBar;
static lv_obj_t *ui_otaNumLabel;
static lv_obj_t *ui_otaLabel;

static lv_obj_t *ui_wakeUpLabel;
static lv_obj_t *ui_wakeUpSwitch;

static TaskHandle_t cameraTaskHandle = NULL;

lampButtonData mqttSwitchState = {false, false, false, false, false, false, false};
detectionDate detectiondatas = {false, false, false};
extern String username;
extern String upassword;
extern bool loginState;
extern bool enableBLE;
int enble_startAudio = 1;
String curtainTime = "0";

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
static void lv_deviceRepwCD(lv_event_t *e);
static void lv_deviceOTA(lv_event_t *e);
void monitorAboutCD(lv_event_t *e);
void msgboxBarTip();
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
    // 屏幕旋转方向和触摸点校正
    tft.setRotation(1);
    // tft.setRotation(3);
    // uint16_t calData[5] = {490, 3259, 422, 3210, 1};
    tft.setTextColor(0xFFFF, 0x0000);
    tft.setSwapBytes(true);
    uint16_t calData[5] = {436, 3332, 277, 3365, 7};
    tft.setTouch(calData);

    // 初始化屏幕背光
    pinMode(TFT_BL, OUTPUT);

    lv_color_t *draw_buf1 = (lv_color_t *)heap_caps_malloc(screenWidth * screenHeight * 2, MALLOC_CAP_SPIRAM);
    lv_color_t *draw_buf2 = (lv_color_t *)heap_caps_malloc(screenWidth * screenHeight * 2, MALLOC_CAP_SPIRAM);

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

    lv_obj_clear_flag(lv_tabview_get_content(ui_TabView4), LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(lv_tabview_get_content(ui_chooseTabView), LV_OBJ_FLAG_SCROLLABLE);

    xnetworkStatusSemaphore = xSemaphoreCreateMutex();

    startLVGLTask();
    Serial.println("lvgl init successfully");
}

void lvgl_task(void *pvParameter)
{
    Serial.println("lvgl task start");
    while (1) {
        lv_timer_handler();
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
    lv_obj_t *btn = lv_event_get_target(e);
    bool is_on = lv_obj_has_state(btn, LV_STATE_CHECKED);
    if (btn == ui_lampButton3) {
        lv_ai_control("lampButton1", is_on);
    } else if (btn == ui_lampButton2) {
        lv_ai_control("lampButton2", is_on);
    } else if (btn == ButtonFan) {
        lv_ai_control("fan", is_on);
    } else if (btn == ButtonCurtain) {
        lv_ai_control("curtain", is_on);
    }
}

void doorContactCB(lv_event_t *e)
{
    lv_obj_clear_flag(ui_doorContactPanel, LV_OBJ_FLAG_HIDDEN);
}

void doorContactCB1(lv_event_t *e)
{
    lv_obj_add_flag(ui_doorContactPanel, LV_OBJ_FLAG_HIDDEN);
}

void curtainCB(lv_event_t *e)
{

    lv_obj_clear_flag(inputKeyboardNumber, LV_OBJ_FLAG_HIDDEN);

    lv_obj_clear_flag(ui_curtainPanel, LV_OBJ_FLAG_HIDDEN);
}

void curtainCB1(lv_event_t *e)
{
    lv_obj_add_flag(inputKeyboardNumber, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_curtainPanel, LV_OBJ_FLAG_HIDDEN);
}

void curtainCB2(lv_event_t *e)
{
    String runtimestr = lv_textarea_get_text(ui_curtainTextarea);
    if (!runtimestr.isEmpty()) {
        StoreData("runtime", runtimestr.c_str());
        curtainTime = runtimestr;
        Serial.printf("curtainTime:%s\n", curtainTime.c_str());
        pulishState_int("motorRunTime", curtainTime.toInt(), "switches");
    }
    lv_obj_add_flag(inputKeyboardNumber, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_curtainPanel, LV_OBJ_FLAG_HIDDEN);
}

void openSoundSwitchCB(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    bool is_on = lv_obj_has_state(btn, LV_STATE_CHECKED);
    if (btn == ui_openSoundSwitch) {
        pulishState("doorContactOpenSound", is_on, "switches");
    } else if (btn == ui_timeoutSwitch) {
        pulishState("enableDoorContactTimeout", is_on, "switches");
        pulishState_int("timeoutTime", mqttSwitchState.timeoutTime, "switches");
    }
}

void doorContactDropdownCB(lv_event_t *e)
{
    char tmp_buf[32];
    lv_dropdown_get_selected_str(ui_Dropdowntime, tmp_buf, sizeof(tmp_buf));
    mqttSwitchState.timeoutTime = atoi(tmp_buf);
    Serial.printf("timeoutTime:%d\n", mqttSwitchState.timeoutTime);
}

void wakeUpSwitchCB(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    bool is_on = lv_obj_has_state(btn, LV_STATE_CHECKED);
    SerialFlush();
    if (btn == ui_wakeUpSwitch) {
        enbeleWakeUp = is_on;
    }
}

void detectionCB(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    bool is_on = lv_obj_has_state(btn, LV_STATE_CHECKED);
    if (btn == ui_Switch1) {
        Serial.printf("pri:%d\n", is_on);
        if (!pulishState("pri", is_on, "switches")) {
            if (is_on) {
                lv_obj_add_state(ui_Switch1, LV_STATE_CHECKED);
            } else {
                lv_obj_clear_state(ui_Switch1, LV_STATE_CHECKED);
            }
            msgboxTip("请登录账号!!!");
        }
    } else if (btn == ui_Switch3) {
        Serial.printf("voiceControl:%d\n", is_on);
        if (!pulishState("voiceControl", is_on, "switches")) {
            if (is_on) {
                lv_obj_add_state(ui_Switch3, LV_STATE_CHECKED);
            } else {
                lv_obj_clear_state(ui_Switch3, LV_STATE_CHECKED);
            }
            msgboxTip("请登录账号!!!");
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
        lv_obj_add_state(ui_mqttuseButton, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(ui_mqttuseButton, LV_STATE_CHECKED);
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
        lv_obj_clear_state(ui_wifiSwitch, LV_STATE_CHECKED);
    }
}

void lv_setBLESwitchState(bool state)
{
    if (state) {
        lv_obj_add_state(ui_BLESwitch, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(ui_BLESwitch, LV_STATE_CHECKED);
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
    if (enble_startAudio) {
        playStartAudio();
    }
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
        lv_label_set_text(ui_statusbarLabel, LV_SYMBOL_SHUFFLE);
    } else if (status == 3) {
        lv_label_set_text(ui_statusbarLabel, LV_SYMBOL_WIFI LV_SYMBOL_SHUFFLE);
    } else if (status == 4) {
        lv_label_set_text(ui_statusbarLabel, LV_SYMBOL_WARNING);
    }
}

static void mboxevent_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_current_target(e);
    LV_LOG_USER("Button %s clicked", lv_msgbox_get_active_btn_text(obj));
    lv_msgbox_close(obj);
}

void msgboxTip(const char *text)
{
    static const char *btn_str[] = {"确定", ""};
    lv_obj_t *mbox2 = lv_msgbox_create(lv_scr_act(), "Tip", text, btn_str, false);
    lv_obj_set_style_text_font(mbox2, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(mbox2, mboxevent_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_align(mbox2, LV_ALIGN_CENTER, 0, 0);
}

static void mboxevent_cb1(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_current_target(e);
    int btn_index = lv_msgbox_get_active_btn(obj);
    lv_msgbox_close(obj);
    if (btn_index == 0) {
        msgboxBarTip();
        startOTATask();
    }
}

void lv_updataOATbar(int value)
{
    lv_bar_set_value(ui_otaBar, value, LV_ANIM_OFF);
    String text;
    if (value >= 100) {
        lv_label_set_text(ui_otaLabel, "OTA升级完成,重启中...");
    }
    text = String(value) + "%";
    lv_label_set_text(ui_otaNumLabel, text.c_str());
}

void msgboxBarTip()
{
    ui_Panel4 = lv_obj_create(lv_scr_act());
    lv_obj_set_width(ui_Panel4, 219);
    lv_obj_set_height(ui_Panel4, 70);
    lv_obj_set_x(ui_Panel4, 0);
    lv_obj_set_y(ui_Panel4, 0);
    lv_obj_set_align(ui_Panel4, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Panel4, LV_OBJ_FLAG_SCROLLABLE); /// Flags

    ui_otaBar = lv_bar_create(ui_Panel4);
    lv_obj_set_width(ui_otaBar, 150);
    lv_obj_set_height(ui_otaBar, 16);
    lv_obj_set_x(ui_otaBar, -21);
    lv_obj_set_y(ui_otaBar, -11);
    lv_obj_set_align(ui_otaBar, LV_ALIGN_CENTER);

    ui_otaNumLabel = lv_label_create(ui_Panel4);
    lv_obj_set_width(ui_otaNumLabel, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_otaNumLabel, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_otaNumLabel, 81);
    lv_obj_set_y(ui_otaNumLabel, -11);
    lv_obj_set_align(ui_otaNumLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_otaNumLabel, "0%");
    lv_obj_set_style_text_font(ui_otaNumLabel, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_otaLabel = lv_label_create(ui_Panel4);
    lv_obj_set_width(ui_otaLabel, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_otaLabel, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_otaLabel, 8);
    lv_obj_set_y(ui_otaLabel, 14);
    lv_obj_set_align(ui_otaLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_otaLabel, "正在升级中...");
    lv_obj_set_style_text_font(ui_otaLabel, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void msgboxTip1(const char *text)
{
    static const char *btn_str[] = {"确定", "取消", ""};
    lv_obj_t *mbox2 = lv_msgbox_create(lv_scr_act(), "Tip", text, btn_str, false);
    lv_obj_set_style_text_font(mbox2, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(mbox2, 200);
    lv_obj_add_event_cb(mbox2, mboxevent_cb1, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_align(mbox2, LV_ALIGN_CENTER, 0, 0);
}

void lv_speakState(const SpeakState_t &state)
{
    speakState = state;

    switch (state) {
    case SpeakState_t::NO_DIALOGUE:
        lv_obj_clear_state(ui_speakButton, LV_STATE_DISABLED);
        lv_label_set_text(ui_speakLabel, "录音");
        lv_setSpeechinfo("");
        Serial.println("等待");
        break;
    case SpeakState_t::RECORDING:
        lv_label_set_text(ui_speakLabel, "录制中");
        lv_setSpeechinfo("正在录音");
        Serial.println("录制中");
        break;
    case SpeakState_t::RECORDED:
        lv_obj_clear_state(ui_speakButton, LV_STATE_CHECKED);
        lv_obj_add_state(ui_speakButton, LV_STATE_DISABLED);
        lv_label_set_text(ui_speakLabel, "识别");
        lv_setSpeechinfo("正在识别");
        Serial.println("识别中");
        break;
    case SpeakState_t::ANSWERING:
        lv_label_set_text(ui_speakLabel, "思考中");
        lv_setSpeechinfo("正在思考");
        Serial.println("思考中");
        break;
    case SpeakState_t::SPEAKING:
        lv_label_set_text(ui_speakLabel, "回答中");
        lv_setSpeechinfo("正在回答");
        Serial.println("回答中");
        break;
    case SpeakState_t::WAKEUP:
        break;
    default:
        break;
    }
    lv_chart_set_ext_y_array(ui_expressionChart, lv_chart_get_series_next(ui_expressionChart, NULL), pos_array[speakState]);
    lv_chart_refresh(ui_expressionChart);
    Serial.printf("speakState: %d\n", speakState);
}

void lv_setPriState(bool state)
{
    if (state) {
        lv_img_set_src(ui_priImage, &ui_img_h1_png);
    } else {
        lv_img_set_src(ui_priImage, &ui_img_h2_png);
    }
}
void lv_setVoiceState(bool state)
{
    if (state) {
        lv_img_set_src(ui_voiceImage, &ui_img_hh1_png);
    } else {
        lv_img_set_src(ui_voiceImage, &ui_img_hh2_png);
    }
}

void lv_setRainState(bool state)
{
    if (state) {
        lv_img_set_src(ui_rainImage, &ui_img_dd1_png);
    } else {
        lv_img_set_src(ui_rainImage, &ui_img_dd2_png);
    }
}

void lv_setUser(const String &user)
{
    lv_textarea_set_placeholder_text(subTextarea, user.c_str());
    lv_textarea_set_text(subTextarea, "");
    lv_textarea_set_placeholder_text(pubTextarea, "******");
    lv_textarea_set_text(pubTextarea, "");
}

void lv_setLampButton1(bool state)
{
    if (state) {
        lv_obj_add_state(ui_lampButton3, LV_STATE_CHECKED);
        lv_label_set_text(ui_stateLabel1, "ON");
        lv_img_set_src(ui_stateImage1, &ui_img_l2_png);
    } else {
        lv_obj_clear_state(ui_lampButton3, LV_STATE_CHECKED);
        lv_label_set_text(ui_stateLabel1, "OFF");
        lv_img_set_src(ui_stateImage1, &ui_img_l1_png);
    }
}
void lv_setLampButton2(bool state)
{
    if (state) {
        lv_obj_add_state(ui_lampButton2, LV_STATE_CHECKED);
        lv_label_set_text(ui_stateLabel3, "ON");
        lv_img_set_src(ui_stateImage3, &ui_img_l2_png);
    } else {
        lv_obj_clear_state(ui_lampButton2, LV_STATE_CHECKED);
        lv_label_set_text(ui_stateLabel3, "OFF");
        lv_img_set_src(ui_stateImage3, &ui_img_l1_png);
    }
}

void lv_setButtonFan(bool state)
{
    if (state) {
        lv_obj_add_state(ButtonFan, LV_STATE_CHECKED);
        lv_label_set_text(stateLabelFan, "ON");
        lv_img_set_src(stateImageFan, &ui_img_fan_png);
    } else {
        lv_obj_clear_state(ButtonFan, LV_STATE_CHECKED);
        lv_label_set_text(stateLabelFan, "OFF");
        lv_img_set_src(stateImageFan, &ui_img_fan1_png);
    }
}
void lv_setButtonCurtain(bool state)
{
    if (state) {
        lv_obj_add_state(ButtonCurtain, LV_STATE_CHECKED);
        lv_label_set_text(stateLabelCurtain, "ON");
        lv_img_set_src(stateImageCurtain, &ui_img_curtain_png);
    } else {
        lv_obj_clear_state(ButtonCurtain, LV_STATE_CHECKED);
        lv_label_set_text(stateLabelCurtain, "OFF");
        lv_img_set_src(stateImageCurtain, &ui_img_curtain1_png);
    }
}

void lv_setButtonDoorContact(bool state)
{
    if (state) {
        lv_label_set_text(ui_nameLabelDoorContact, "ON");
    } else {
        lv_label_set_text(ui_nameLabelDoorContact, "OFF");
    }
}

void lv_setButtonDoorContactOpenSound(bool state)
{
    if (state) {
        lv_obj_add_state(ui_openSoundSwitch, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(ui_openSoundSwitch, LV_STATE_CHECKED);
    }
}

void lv_setButtonDoorContactTimeout(bool state)
{
    if (state) {
        lv_obj_add_state(ui_timeoutSwitch, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(ui_timeoutSwitch, LV_STATE_CHECKED);
    }
}

void lv_setDropdownDoorContactTimeoutTime(int time)
{
    if (time == 10) {
        time = 6;
    } else if (time == 20) {
        time = 7;
    } else if (time == 30) {
        time = 8;
    }
    lv_dropdown_set_selected(ui_Dropdowntime, time - 1);
}

void lv_setCurtainRunTime(int runTime)
{
    curtainTime = String(runTime);
    lv_textarea_set_placeholder_text(ui_curtainTextarea, String(runTime).c_str());
}

void lv_setPriButtonState(bool state)
{
    if (state) {
        lv_obj_add_state(ui_Switch1, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(ui_Switch1, LV_STATE_CHECKED);
    }
}
void lv_setVoiceButtonState(bool state)
{
    if (state) {
        lv_obj_add_state(ui_Switch3, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(ui_Switch3, LV_STATE_CHECKED);
    }
}

void lv_ai_control(const String &handl, bool state)
{
    Serial.printf("handl:%s  state:%d\n", handl.c_str(), state);
    bool isSuccess = pulishState(handl, state, "switches");

    if (handl == "lampButton1") {
        if (isSuccess) {
            mqttSwitchState.lampButton1 = state;
        } else {
            msgboxTip("请登录账号!!!");
        }
        lv_setLampButton1(mqttSwitchState.lampButton1);
    } else if (handl == "lampButton2") {
        if (isSuccess) {
            mqttSwitchState.lampButton2 = state;
        } else {
            msgboxTip("请登录账号!!!");
        }
        lv_setLampButton2(mqttSwitchState.lampButton2);
    } else if (handl == "fan") {
        if (isSuccess) {
            mqttSwitchState.fan = state;
        } else {
            msgboxTip("请登录账号!!!");
        }
        lv_setButtonFan(mqttSwitchState.fan);
    } else if (handl == "curtain") {
        if (isSuccess) {
            mqttSwitchState.curtain = state;
        } else {
            msgboxTip("请登录账号!!!");
        }
        lv_setButtonCurtain(mqttSwitchState.curtain);
    } else if (handl == "pri") {
        if (isSuccess) {
            mqttSwitchState.priButton = state;
        } else {
            msgboxTip("请登录账号!!!");
        }
        lv_setPriButtonState(mqttSwitchState.priButton);
    } else if (handl == "voiceControl") {
        if (isSuccess) {
            mqttSwitchState.voiceButton = state;
        } else {
            msgboxTip("请登录账号!!!");
        }
        lv_setVoiceButtonState(mqttSwitchState.voiceButton);
    }
}

void lv_ai_control_offline(const String &handl, int state)
{
    Serial.printf("handl:%s  state:%d\n", handl.c_str(), state);
    if (handl == "lampButton1") {
        mqttSwitchState.lampButton1 = state;
        lv_setLampButton1(mqttSwitchState.lampButton1);
        // state ? sendIntDataBLE(5) : sendIntDataBLE(6);
    } else if (handl == "lampButton2") {
        mqttSwitchState.lampButton2 = state;
        lv_setLampButton2(mqttSwitchState.lampButton2);
        // state ? sendIntDataBLE(7) : sendIntDataBLE(8);
    } else if (handl == "fan") {
        mqttSwitchState.fan = state;
        lv_setButtonFan(mqttSwitchState.fan);
        // state ? sendIntDataBLE(13) : sendIntDataBLE(14);
    } else if (handl == "curtain") {
        mqttSwitchState.curtain = state;
        lv_setButtonCurtain(mqttSwitchState.curtain);
        // state ? sendIntDataBLE(15) : sendIntDataBLE(16);
    } else if (handl == "pri") {
        mqttSwitchState.priButton = state;
        lv_setPriButtonState(mqttSwitchState.priButton);
        // state ? sendIntDataBLE(9) : sendIntDataBLE(10);
    } else if (handl == "voiceControl") {
        mqttSwitchState.voiceButton = state;
        lv_setVoiceButtonState(mqttSwitchState.voiceButton);
        // state ? sendIntDataBLE(11) : sendIntDataBLE(12);
    } else if (handl == "doorContactOpenSound") {
        // sendStrDataBLEStr("doorContactOpenSound:" + state);
    } else if (handl == "enableDoorContactTimeout") {
        // sendStrDataBLEStr("enableDoorContactTimeout:" + state);
    } else if (handl == "timeoutTime") {
        // sendStrDataBLEStr("timeoutTime:" + state);
    } else if (handl == "motorRunTime") {
        // sendStrDataBLEStr("motorRunTime:" + state);
    } else if (handl == "message") {
        // sendStrDataBLEStr("message:" + state);
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

static void kbClear_cb1(lv_event_t *event)
{
    lv_keyboard_t *kb = (lv_keyboard_t *)event->target;
    lv_textarea_set_text(kb->ta, "");
}
static void kbHide_cb1(lv_event_t *event)
{
    lv_obj_add_flag(inputKeyboardNumber, LV_OBJ_FLAG_HIDDEN);
}

static void settext_input_number_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *ta = lv_event_get_target(e);

    if (code == LV_EVENT_FOCUSED) {
        lv_obj_move_foreground(inputKeyboardNumber);
        lv_keyboard_set_textarea(inputKeyboardNumber, ta);
        lv_obj_clear_flag(inputKeyboardNumber, LV_OBJ_FLAG_HIDDEN);
    }

    if (code == LV_EVENT_CLICKED) {
        lv_obj_move_foreground(inputKeyboardNumber);
        lv_obj_clear_flag(inputKeyboardNumber, LV_OBJ_FLAG_HIDDEN);
    }

    if (code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(inputKeyboardNumber, NULL);
        lv_obj_add_flag(inputKeyboardNumber, LV_OBJ_FLAG_HIDDEN);
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

void lv_closeWifi()
{
    lv_setWIFISwitchState(false);
    // 清除wifi列表
    lv_list_add_text(wfList, "");
    lv_obj_clean(wfList);
    // 停止扫描wifi任务
    if (ntScanTaskHandler != NULL) {
        xSemaphoreTake(xnetworkStatusSemaphore, portMAX_DELAY);
        networkStatus = NONE;
        xSemaphoreGive(xnetworkStatusSemaphore);
        vTaskDelete(ntScanTaskHandler);
        ntScanTaskHandler = NULL;
    }
    // 停止连接wifi任务
    if (ntConnectTaskHandler != NULL) {
        vTaskDelete(ntConnectTaskHandler);
    }
    // 停止wifi列表定时器
    if (wifiListtimer != NULL) {
        lv_timer_del(wifiListtimer);
        wifiListtimer = NULL;
    }
    // 关闭WiFi
    if (getwifistate()) {
        wifiDisconnect();
        lv_label_set_text(ui_wifiStateLabel, "未连接");
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
                if (lv_obj_has_state(ui_mqttuseButton, LV_STATE_CHECKED)) {
                    String substr = lv_textarea_get_text(subTextarea);
                    String pubstr = lv_textarea_get_text(pubTextarea);
                    if (substr.isEmpty()) {
                        lv_setMQTTSwitchState(false);
                        msgboxTip("账号不能为空!");
                        return;
                    }
                    if (pubstr.isEmpty()) {
                        lv_setMQTTSwitchState(false);
                        msgboxTip("密码不能为空");
                        return;
                    }
                    // 请求验证账号
                    Serial.println("正在验证账号");
                    if (postlogin(substr, pubstr)) {
                        lv_setMQTTSwitchState(true);
                        StoreintData("isLogin", 1);
                        StoreData("username", substr.c_str());
                        mqtMontage(substr);
                        loginState = true;
                        enable_mqtt = true;
                        lv_textarea_set_placeholder_text(subTextarea, substr.c_str());
                        lv_textarea_set_text(subTextarea, "");
                        lv_textarea_set_placeholder_text(pubTextarea, "******");
                        lv_textarea_set_text(pubTextarea, "");
                        lv_setMQTTState("正在连接");
                        Serial.println("打开mqtt开关,开始连接");
                    } else {
                        lv_setMQTTSwitchState(false);
                        StoreintData("isLogin", 0);
                        loginState = false;
                        enable_mqtt = false;
                        lv_textarea_set_placeholder_text(subTextarea, "");
                        lv_textarea_set_text(subTextarea, "");
                        lv_textarea_set_placeholder_text(pubTextarea, "");
                        lv_textarea_set_text(pubTextarea, "");
                        Serial.println("登录失败");
                    }
                } else {
                    lv_setMQTTSwitchState(false);
                    StoreintData("isLogin", 0);
                    loginState = false;
                    enable_mqtt = false;
                    Serial.println("关闭mqtt开关,断开连接");
                    lv_textarea_set_placeholder_text(subTextarea, "");
                    lv_textarea_set_text(subTextarea, "");
                    lv_textarea_set_placeholder_text(pubTextarea, "");
                    lv_textarea_set_text(pubTextarea, "");
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
                msgboxTip("网络未连接");
                lv_setMQTTSwitchState(false);
                loginState = false;
                enable_mqtt = false;
                Serial.println("no network dont check mqttSwitch");
                lv_obj_add_state(btn, LV_STATE_DEFAULT);
            }
        } else if (btn == ui_timeuseButton) {
            Serial.println("ui_timeuseButton LV_EVENT_CLICKED");
            String updatetimestr = lv_textarea_get_text(ntpTimeTextarea);
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
                lv_closeWifi();
            }
        }
        // else if (btn == ui_BLESwitch) {
        //     if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
        //         Serial1.println("enable ble");
        //         lv_closeWifi();
        //         enableBLE = true;
        //         lv_setBLESwitchState(true);
        //         startBLE();
        //     } else {
        //         enableBLE = false;
        //         stopBLE();
        //         lv_setBLESwitchState(false);
        //     }
        // }
    }
}

void chooseScreenFCD(lv_event_t *e)
{
    lv_list_add_text(wfList, "");
    lv_obj_clean(wfList);
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
        int index = lv_dropdown_get_selected(btn);
        index = speakPerid(index);
        Serial.printf("speech dropdown number : %d name: %s\n", index, tmp_buf);
        StoreintData("speech_id", index);
        audioSetPer(index);
    } else if (btn == ui_aimodeDropdown) {
        Serial.printf("ai mdoe dropdown number : %d\n", lv_dropdown_get_selected(btn));
        useAIMode = lv_dropdown_get_selected(btn);
        StoreintData("speech_ai_mode", lv_dropdown_get_selected(btn));
    }
}

void controlBrightnessCD(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    Serial.println((int)lv_slider_get_value(slider));
    analogWrite(TFT_BL, (int)lv_slider_get_value(slider));
    StoreintData("brightness", (int)lv_slider_get_value(slider));
}

void switchThemesCD(lv_event_t *e)
{
    if (lv_obj_has_state(ui_themeSwitch, LV_STATE_CHECKED)) {
        _ui_switch_theme(UI_THEME_DARKTHEME);
        StoreintData("theme_id", 0);
    } else {
        _ui_switch_theme(UI_THEME_LIGHTTHEME);
        StoreintData("theme_id", 1);
    }
}

void switchStartAudioCD(lv_event_t *e)
{
    if (lv_obj_has_state(ui_startAudioSwitch, LV_STATE_CHECKED)) {
        enble_startAudio = 1;
    } else {
        enble_startAudio = false;
    }
    StoreintData("startaudio", enble_startAudio);
}

void lv_deviceRepwCD(lv_event_t *e)
{
    Serial.println("lv_deviceRepwCD: send device repw");
    if (sendRePW()) {
        msgboxTip("发送成功");
    } else {
        msgboxTip("发送失败");
    }
}

void lv_deviceOTA(lv_event_t *e)
{
    Serial.println("lv_deviceOTA");
    if (getwifistate()) {
        if (getOTAVersion()) {
            String verstr = "新版本:V" + latestFirmware + ",是否升级";
            msgboxTip1(verstr.c_str());
        } else {
            msgboxTip("当前是最新版本");
        }
    } else {
        msgboxTip("请连接WIFI");
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
    if (cameraTaskHandle != nullptr && eTaskGetState(cameraTaskHandle) != eTaskState::eSuspended) {
        vTaskSuspend(cameraTaskHandle);
    }
    lv_scr_load_anim(ui_monitorScreen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, false);
}

void monitorScreenFCD(lv_event_t *e)
{
    Serial.println("return cameraserver");
    lv_obj_clear_state(ui_connectCameraButton, LV_STATE_CHECKED);
    lv_label_set_text(ui_cameraLabel, "连接");
    lv_label_set_text(ui_cameraStateLabel, "未连接");
    closeCameraServer();
    if (cameraTaskHandle != nullptr && eTaskGetState(cameraTaskHandle) == eTaskState::eSuspended) {
        Serial.println("vTaskResume cameraTaskHandle");
        vTaskResume(cameraTaskHandle);
    }
    lv_scr_load_anim(ui_MainScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, false);
}

void monitorAboutCD(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    Serial.println("monitorAboutCD");
    if (btn == videoLeft) {
        publishVideoAbout(1);
    } else if (btn == videoRight) {
        publishVideoAbout(2);
    }
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
        isStartCamera = false;
    }

    cameraClient.close();
    if (cameraTaskHandle != NULL) {
        vTaskDelete(cameraTaskHandle);
        cameraTaskHandle = NULL;
    }
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
        xTaskCreatePinnedToCore(&videocameratask, "videocamera_task", 5 * 1024, NULL, 3, &cameraTaskHandle, 1);
    } else {
        Serial.println("cameraTaskHandle is not NULL");
    }
}

void stopCameraTask()
{
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
            lv_tabview_set_act(ui_TabView4, 0, LV_ANIM_OFF);
            lv_scr_load_anim(ui_speechScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, false);
        } else if (btn == ui_Button7) {
            lv_tabview_set_act(ui_TabView4, 1, LV_ANIM_OFF);
            lv_scr_load_anim(ui_speechScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, false);
        } else if (btn == ui_speakButton) {
            if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                lv_speakState(SpeakState_t::RECORDING);
            } else {
                lv_speakState(SpeakState_t::RECORDED);
            }
            Serial.println("speakState = " + String(speakState));
        }
    }
}

void speakScreenFCD(lv_event_t *e)
{
    // lv_speakState(SpeakState_t::NO_DIALOGUE);
    lv_scr_load_anim(ui_MainScreen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, false);
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

    String runtime1 = ReadData("runtime");
    if (runtime1 != "null") {
        lv_textarea_set_placeholder_text(ui_curtainTextarea, runtime1.c_str());
        curtainTime = runtime1;
    }

    String updatetime = ReadData("updatetime");
    if (updatetime != "null") {
        lv_textarea_set_placeholder_text(ntpTimeTextarea, updatetime.c_str());
        updatetimentp = updatetime.toInt();
    }

    String Version = ReadData("Version");
    if (Version != "null") {
        FirmwareVersion = Version;
        Version = "当前版本:V" + FirmwareVersion;
        lv_label_set_text(otaLabel, Version.c_str());
    }

    int speech_ai_mode = ReadintData("speech_ai_mode");
    if (speech_ai_mode != 1000) {
        lv_dropdown_set_selected(ui_aimodeDropdown, speech_ai_mode);
        useAIMode = speech_ai_mode;
        Serial.printf("speech_ai_mode: %d\n", useAIMode);
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

    int theme_id = ReadintData("theme_id");
    if (theme_id != 1000) {
        if (theme_id == 1) {
            _ui_switch_theme(UI_THEME_LIGHTTHEME);
        } else {
            _ui_switch_theme(UI_THEME_DARKTHEME);
        }
        Serial.printf("theme_id: %d\n", theme_id);
    }

    int startA = ReadintData("startaudio");
    if (startA != 1000) {
        enble_startAudio = startA;
        if (enble_startAudio) {
            lv_obj_add_state(ui_startAudioSwitch, LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(ui_startAudioSwitch, LV_STATE_CHECKED);
        }
        Serial.printf("startAudio: %d\n", enble_startAudio);
    }

    int brightness = ReadintData("brightness");
    if (brightness != 1000) {
        analogWrite(TFT_BL, brightness);
        lv_slider_set_value(ui_Slider1, brightness, LV_ANIM_OFF);
        Serial.printf("brightness: %d\n", brightness);
    }
}

void perinit()
{
    int speech_id = ReadintData("speech_id");
    if (speech_id != 1000) {
        lv_dropdown_set_selected(ui_speechDropdown, speech_id);
        char tmp_buf[32];
        lv_dropdown_get_selected_str(ui_speechDropdown, tmp_buf, sizeof(tmp_buf));
        audioSetPer(speech_id);
    }
}

/********************************************************************
                         LVGL_MY_UI_INIT
********************************************************************/

void initDeviceUI(void)
{
    ButtonFan = lv_btn_create(ui_TabPage1);
    lv_obj_set_width(ButtonFan, 80);
    lv_obj_set_height(ButtonFan, 80);
    lv_obj_set_x(ButtonFan, -70);
    lv_obj_set_y(ButtonFan, 65);
    lv_obj_set_align(ButtonFan, LV_ALIGN_CENTER);
    lv_obj_add_flag(ButtonFan, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags
    lv_obj_clear_flag(ButtonFan, LV_OBJ_FLAG_SCROLLABLE);                            /// Flags
    ui_object_set_themeable_style_property(ButtonFan, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_BG_COLOR,
                                           _ui_theme_color_btn);
    ui_object_set_themeable_style_property(ButtonFan, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_BG_OPA,
                                           _ui_theme_alpha_btn);
    lv_obj_set_style_bg_color(ButtonFan, lv_color_hex(0xAB79F7), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ButtonFan, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(ButtonFan, lv_color_hex(0x728BFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_opa(ButtonFan, 150, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ButtonFan, 10, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_spread(ButtonFan, 5, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ButtonFan, lv_color_hex(0x728BFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ButtonFan, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ButtonFan, lv_color_hex(0xA94949), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ButtonFan, 255, LV_PART_MAIN | LV_STATE_FOCUSED);

    stateLabelFan = lv_label_create(ButtonFan);
    lv_obj_set_width(stateLabelFan, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(stateLabelFan, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(stateLabelFan, 17);
    lv_obj_set_y(stateLabelFan, 21);
    lv_obj_set_align(stateLabelFan, LV_ALIGN_CENTER);
    lv_label_set_text(stateLabelFan, "OFF");
    lv_obj_clear_flag(stateLabelFan, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE); /// Flags
    ui_object_set_themeable_style_property(stateLabelFan, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR,
                                           _ui_theme_color_font);
    ui_object_set_themeable_style_property(stateLabelFan, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_OPA,
                                           _ui_theme_alpha_font);
    lv_obj_set_style_text_font(stateLabelFan, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    stateImageFan = lv_img_create(ButtonFan);
    lv_img_set_src(stateImageFan, &ui_img_fan1_png);
    lv_obj_set_width(stateImageFan, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(stateImageFan, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(stateImageFan, -11);
    lv_obj_set_y(stateImageFan, -12);
    lv_obj_set_align(stateImageFan, LV_ALIGN_CENTER);
    lv_obj_add_flag(stateImageFan, LV_OBJ_FLAG_ADV_HITTEST);  /// Flags
    lv_obj_clear_flag(stateImageFan, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_img_set_zoom(stateImageFan, 120);

    nameLabelFan = lv_label_create(ButtonFan);
    lv_obj_set_width(nameLabelFan, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(nameLabelFan, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(nameLabelFan, -19);
    lv_obj_set_y(nameLabelFan, 22);
    lv_obj_set_align(nameLabelFan, LV_ALIGN_CENTER);
    lv_label_set_text(nameLabelFan, "风扇");
    ui_object_set_themeable_style_property(nameLabelFan, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR,
                                           _ui_theme_color_font);
    ui_object_set_themeable_style_property(nameLabelFan, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_OPA,
                                           _ui_theme_alpha_font);
    lv_obj_set_style_text_font(nameLabelFan, &ui_font_unit, LV_PART_MAIN | LV_STATE_DEFAULT);

    ButtonCurtain = lv_btn_create(ui_TabPage1);
    lv_obj_set_width(ButtonCurtain, 80);
    lv_obj_set_height(ButtonCurtain, 80);
    lv_obj_set_x(ButtonCurtain, 60);
    lv_obj_set_y(ButtonCurtain, 65);
    lv_obj_set_align(ButtonCurtain, LV_ALIGN_CENTER);
    lv_obj_add_flag(ButtonCurtain, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags
    lv_obj_clear_flag(ButtonCurtain, LV_OBJ_FLAG_SCROLLABLE);                            /// Flags
    ui_object_set_themeable_style_property(ButtonCurtain, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_BG_COLOR,
                                           _ui_theme_color_btn);
    ui_object_set_themeable_style_property(ButtonCurtain, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_BG_OPA,
                                           _ui_theme_alpha_btn);
    lv_obj_set_style_bg_color(ButtonCurtain, lv_color_hex(0xAB79F7), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ButtonCurtain, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(ButtonCurtain, lv_color_hex(0x728BFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_opa(ButtonCurtain, 150, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ButtonCurtain, 10, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_spread(ButtonCurtain, 5, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ButtonCurtain, lv_color_hex(0x728BFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ButtonCurtain, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ButtonCurtain, lv_color_hex(0xA94949), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ButtonCurtain, 255, LV_PART_MAIN | LV_STATE_FOCUSED);

    stateLabelCurtain = lv_label_create(ButtonCurtain);
    lv_obj_set_width(stateLabelCurtain, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(stateLabelCurtain, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(stateLabelCurtain, 17);
    lv_obj_set_y(stateLabelCurtain, 21);
    lv_obj_set_align(stateLabelCurtain, LV_ALIGN_CENTER);
    lv_label_set_text(stateLabelCurtain, "OFF");
    lv_obj_clear_flag(stateLabelCurtain, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE); /// Flags
    ui_object_set_themeable_style_property(stateLabelCurtain, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR,
                                           _ui_theme_color_font);
    ui_object_set_themeable_style_property(stateLabelCurtain, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_OPA,
                                           _ui_theme_alpha_font);
    lv_obj_set_style_text_font(stateLabelCurtain, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    stateImageCurtain = lv_img_create(ButtonCurtain);
    lv_img_set_src(stateImageCurtain, &ui_img_curtain1_png);
    lv_obj_set_width(stateImageCurtain, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(stateImageCurtain, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(stateImageCurtain, -11);
    lv_obj_set_y(stateImageCurtain, -12);
    lv_obj_set_align(stateImageCurtain, LV_ALIGN_CENTER);
    lv_obj_add_flag(stateImageCurtain, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST); /// Flags
    lv_obj_clear_flag(stateImageCurtain, LV_OBJ_FLAG_SCROLLABLE);                        /// Flags
    lv_img_set_zoom(stateImageCurtain, 80);

    nameLabelCurtain = lv_label_create(ButtonCurtain);
    lv_obj_set_width(nameLabelCurtain, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(nameLabelCurtain, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(nameLabelCurtain, -19);
    lv_obj_set_y(nameLabelCurtain, 22);
    lv_obj_set_align(nameLabelCurtain, LV_ALIGN_CENTER);
    lv_label_set_text(nameLabelCurtain, "窗帘");
    ui_object_set_themeable_style_property(nameLabelCurtain, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR,
                                           _ui_theme_color_font);
    ui_object_set_themeable_style_property(nameLabelCurtain, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_OPA,
                                           _ui_theme_alpha_font);
    lv_obj_set_style_text_font(nameLabelCurtain, &ui_font_unit, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ButtonDoorContact = lv_btn_create(ui_TabPage2);
    lv_obj_set_width(ui_ButtonDoorContact, 115);
    lv_obj_set_height(ui_ButtonDoorContact, 80);
    lv_obj_set_x(ui_ButtonDoorContact, -76);
    lv_obj_set_y(ui_ButtonDoorContact, 58);
    lv_obj_set_align(ui_ButtonDoorContact, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_ButtonDoorContact, LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags
    lv_obj_clear_flag(ui_ButtonDoorContact, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
    ui_object_set_themeable_style_property(ui_ButtonDoorContact, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_BG_COLOR,
                                           _ui_theme_color_btn);
    ui_object_set_themeable_style_property(ui_ButtonDoorContact, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_BG_OPA,
                                           _ui_theme_alpha_btn);
    lv_obj_set_style_bg_color(ui_ButtonDoorContact, lv_color_hex(0xAB79F7), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_ButtonDoorContact, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(ui_ButtonDoorContact, lv_color_hex(0x728BFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_opa(ui_ButtonDoorContact, 150, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui_ButtonDoorContact, 10, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_spread(ui_ButtonDoorContact, 5, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui_ButtonDoorContact, lv_color_hex(0x728BFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui_ButtonDoorContact, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui_ButtonDoorContact, lv_color_hex(0xA94949), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui_ButtonDoorContact, 255, LV_PART_MAIN | LV_STATE_FOCUSED);

    ui_stateLabelDoorContact = lv_label_create(ui_ButtonDoorContact);
    lv_obj_set_width(ui_stateLabelDoorContact, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_stateLabelDoorContact, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_stateLabelDoorContact, 30);
    lv_obj_set_y(ui_stateLabelDoorContact, -20);
    lv_obj_set_align(ui_stateLabelDoorContact, LV_ALIGN_CENTER);
    lv_label_set_text(ui_stateLabelDoorContact, "OFF");
    lv_obj_clear_flag(ui_stateLabelDoorContact, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE); /// Flags
    ui_object_set_themeable_style_property(ui_stateLabelDoorContact, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR,
                                           _ui_theme_color_font);
    ui_object_set_themeable_style_property(ui_stateLabelDoorContact, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_OPA,
                                           _ui_theme_alpha_font);
    lv_obj_set_style_text_font(ui_stateLabelDoorContact, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_stateImageDoorContact = lv_img_create(ui_ButtonDoorContact);
    lv_img_set_src(ui_stateImageDoorContact, &ui_img_doorcontact_png);
    lv_obj_set_width(ui_stateImageDoorContact, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_stateImageDoorContact, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_stateImageDoorContact, -27);
    lv_obj_set_y(ui_stateImageDoorContact, -16);
    lv_obj_set_align(ui_stateImageDoorContact, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_stateImageDoorContact, LV_OBJ_FLAG_ADV_HITTEST);  /// Flags
    lv_obj_clear_flag(ui_stateImageDoorContact, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_img_set_zoom(ui_stateImageDoorContact, 80);

    ui_nameLabelDoorContact = lv_label_create(ui_ButtonDoorContact);
    lv_obj_set_width(ui_nameLabelDoorContact, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_nameLabelDoorContact, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_nameLabelDoorContact, 0);
    lv_obj_set_y(ui_nameLabelDoorContact, 22);
    lv_obj_set_align(ui_nameLabelDoorContact, LV_ALIGN_CENTER);
    lv_label_set_text(ui_nameLabelDoorContact, "门磁感应");
    ui_object_set_themeable_style_property(ui_nameLabelDoorContact, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR,
                                           _ui_theme_color_font);
    ui_object_set_themeable_style_property(ui_nameLabelDoorContact, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_OPA,
                                           _ui_theme_alpha_font);
    lv_obj_set_style_text_font(ui_nameLabelDoorContact, &ui_font_unit, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_doorContactPanel = lv_obj_create(ui_TabPage2);
    lv_obj_set_width(ui_doorContactPanel, 255);
    lv_obj_set_height(ui_doorContactPanel, 155);
    lv_obj_set_align(ui_doorContactPanel, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_doorContactPanel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_doorContactPanel, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_doorContactPanel, lv_color_hex(0xD1D5D9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_doorContactPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_doorContactPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_doorContactLabeltitle = lv_label_create(ui_doorContactPanel);
    lv_obj_set_width(ui_doorContactLabeltitle, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_doorContactLabeltitle, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_doorContactLabeltitle, 3);
    lv_obj_set_y(ui_doorContactLabeltitle, -54);
    lv_obj_set_align(ui_doorContactLabeltitle, LV_ALIGN_CENTER);
    lv_label_set_text(ui_doorContactLabeltitle, "门磁配置");
    lv_obj_set_style_text_font(ui_doorContactLabeltitle, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_doorContactLabelsound = lv_label_create(ui_doorContactPanel);
    lv_obj_set_width(ui_doorContactLabelsound, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_doorContactLabelsound, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_doorContactLabelsound, -67);
    lv_obj_set_y(ui_doorContactLabelsound, -24);
    lv_obj_set_align(ui_doorContactLabelsound, LV_ALIGN_CENTER);
    lv_label_set_text(ui_doorContactLabelsound, "开机语音设置:");
    lv_obj_set_style_text_font(ui_doorContactLabelsound, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_openSoundSwitch = lv_switch_create(ui_doorContactPanel);
    lv_obj_set_width(ui_openSoundSwitch, 50);
    lv_obj_set_height(ui_openSoundSwitch, 25);
    lv_obj_set_x(ui_openSoundSwitch, 21);
    lv_obj_set_y(ui_openSoundSwitch, -24);
    lv_obj_set_align(ui_openSoundSwitch, LV_ALIGN_CENTER);

    ui_Dropdowntime = lv_dropdown_create(ui_doorContactPanel);
    lv_dropdown_set_options(ui_Dropdowntime, "1\n2\n3\n4\n5\n10\n20\n30");
    lv_obj_set_width(ui_Dropdowntime, 43);
    lv_obj_set_height(ui_Dropdowntime, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_Dropdowntime, 74);
    lv_obj_set_y(ui_Dropdowntime, 8);
    lv_obj_set_align(ui_Dropdowntime, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Dropdowntime, LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags

    ui_doorContactLabeltimeout = lv_label_create(ui_doorContactPanel);
    lv_obj_set_width(ui_doorContactLabeltimeout, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_doorContactLabeltimeout, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_doorContactLabeltimeout, -65);
    lv_obj_set_y(ui_doorContactLabeltimeout, 8);
    lv_obj_set_align(ui_doorContactLabeltimeout, LV_ALIGN_CENTER);
    lv_label_set_text(ui_doorContactLabeltimeout, "超时未关设置:");
    lv_obj_set_style_text_font(ui_doorContactLabeltimeout, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_timeoutSwitch = lv_switch_create(ui_doorContactPanel);
    lv_obj_set_width(ui_timeoutSwitch, 50);
    lv_obj_set_height(ui_timeoutSwitch, 25);
    lv_obj_set_x(ui_timeoutSwitch, 19);
    lv_obj_set_y(ui_timeoutSwitch, 8);
    lv_obj_set_align(ui_timeoutSwitch, LV_ALIGN_CENTER);

    ui_doorContactButtonsure = lv_btn_create(ui_doorContactPanel);
    lv_obj_set_width(ui_doorContactButtonsure, 74);
    lv_obj_set_height(ui_doorContactButtonsure, 30);
    lv_obj_set_x(ui_doorContactButtonsure, 0);
    lv_obj_set_y(ui_doorContactButtonsure, 53);
    lv_obj_set_align(ui_doorContactButtonsure, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_doorContactButtonsure, LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags
    lv_obj_clear_flag(ui_doorContactButtonsure, LV_OBJ_FLAG_SCROLLABLE);    /// Flags

    ui_doorContactLabelsure = lv_label_create(ui_doorContactButtonsure);
    lv_obj_set_width(ui_doorContactLabelsure, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_doorContactLabelsure, LV_SIZE_CONTENT); /// 1
    lv_obj_set_align(ui_doorContactLabelsure, LV_ALIGN_CENTER);
    lv_label_set_text(ui_doorContactLabelsure, "确定");
    lv_obj_set_style_text_font(ui_doorContactLabelsure, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_doorContactLabelTime = lv_label_create(ui_doorContactPanel);
    lv_obj_set_width(ui_doorContactLabelTime, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_doorContactLabelTime, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_doorContactLabelTime, 110);
    lv_obj_set_y(ui_doorContactLabelTime, 8);
    lv_obj_set_align(ui_doorContactLabelTime, LV_ALIGN_CENTER);
    lv_label_set_text(ui_doorContactLabelTime, "分");
    lv_obj_set_style_text_font(ui_doorContactLabelTime, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_curtainPanel = lv_obj_create(ui_TabPage1);
    lv_obj_set_width(ui_curtainPanel, 238);
    lv_obj_set_height(ui_curtainPanel, 139);
    lv_obj_set_x(ui_curtainPanel, 6);
    lv_obj_set_y(ui_curtainPanel, -2);
    lv_obj_set_align(ui_curtainPanel, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_curtainPanel, LV_OBJ_FLAG_HIDDEN);       /// Flags
    lv_obj_clear_flag(ui_curtainPanel, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_curtainPanel, lv_color_hex(0xD1D5D9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_curtainPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_curtainPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_curtainLabeltitle = lv_label_create(ui_curtainPanel);
    lv_obj_set_width(ui_curtainLabeltitle, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_curtainLabeltitle, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_curtainLabeltitle, 3);
    lv_obj_set_y(ui_curtainLabeltitle, -54);
    lv_obj_set_align(ui_curtainLabeltitle, LV_ALIGN_CENTER);
    lv_label_set_text(ui_curtainLabeltitle, "窗帘开关配置");
    lv_obj_set_style_text_font(ui_curtainLabeltitle, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_curtainLabelsound = lv_label_create(ui_curtainPanel);
    lv_obj_set_width(ui_curtainLabelsound, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_curtainLabelsound, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_curtainLabelsound, -58);
    lv_obj_set_y(ui_curtainLabelsound, -17);
    lv_obj_set_align(ui_curtainLabelsound, LV_ALIGN_CENTER);
    lv_label_set_text(ui_curtainLabelsound, "窗帘开关时长:");
    lv_obj_set_style_text_font(ui_curtainLabelsound, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_curtainButtonsure = lv_btn_create(ui_curtainPanel);
    lv_obj_set_width(ui_curtainButtonsure, 74);
    lv_obj_set_height(ui_curtainButtonsure, 30);
    lv_obj_set_x(ui_curtainButtonsure, -46);
    lv_obj_set_y(ui_curtainButtonsure, 30);
    lv_obj_set_align(ui_curtainButtonsure, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_curtainButtonsure, LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags
    lv_obj_clear_flag(ui_curtainButtonsure, LV_OBJ_FLAG_SCROLLABLE);    /// Flags

    ui_curtainLabelsure = lv_label_create(ui_curtainButtonsure);
    lv_obj_set_width(ui_curtainLabelsure, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_curtainLabelsure, LV_SIZE_CONTENT); /// 1
    lv_obj_set_align(ui_curtainLabelsure, LV_ALIGN_CENTER);
    lv_label_set_text(ui_curtainLabelsure, "确定");
    lv_obj_set_style_text_font(ui_curtainLabelsure, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_curtainLabelTime = lv_label_create(ui_curtainPanel);
    lv_obj_set_width(ui_curtainLabelTime, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_curtainLabelTime, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_curtainLabelTime, 78);
    lv_obj_set_y(ui_curtainLabelTime, -18);
    lv_obj_set_align(ui_curtainLabelTime, LV_ALIGN_CENTER);
    lv_label_set_text(ui_curtainLabelTime, "秒");
    lv_obj_set_style_text_font(ui_curtainLabelTime, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_curtainButtonq = lv_btn_create(ui_curtainPanel);
    lv_obj_set_width(ui_curtainButtonq, 74);
    lv_obj_set_height(ui_curtainButtonq, 30);
    lv_obj_set_x(ui_curtainButtonq, 68);
    lv_obj_set_y(ui_curtainButtonq, 32);
    lv_obj_set_align(ui_curtainButtonq, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_curtainButtonq, LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags
    lv_obj_clear_flag(ui_curtainButtonq, LV_OBJ_FLAG_SCROLLABLE);    /// Flags

    ui_curtainLabelsure2 = lv_label_create(ui_curtainButtonq);
    lv_obj_set_width(ui_curtainLabelsure2, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_curtainLabelsure2, LV_SIZE_CONTENT); /// 1
    lv_obj_set_align(ui_curtainLabelsure2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_curtainLabelsure2, "取消");
    lv_obj_set_style_text_font(ui_curtainLabelsure2, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_curtainTextarea = lv_textarea_create(ui_curtainPanel);
    lv_obj_set_width(ui_curtainTextarea, 53);
    lv_obj_set_height(ui_curtainTextarea, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_curtainTextarea, 36);
    lv_obj_set_y(ui_curtainTextarea, -17);
    lv_textarea_set_one_line(ui_curtainTextarea, true);
    lv_obj_set_align(ui_curtainTextarea, LV_ALIGN_CENTER);

    inputKeyboardNumber = lv_keyboard_create(ui_lampScreen);
    lv_obj_add_event_cb(inputKeyboardNumber, kbClear_cb1, LV_EVENT_CANCEL, NULL);
    lv_obj_add_event_cb(inputKeyboardNumber, kbHide_cb1, LV_EVENT_READY, NULL);
    lv_obj_add_flag(inputKeyboardNumber, LV_OBJ_FLAG_HIDDEN);
    lv_keyboard_set_mode(inputKeyboardNumber, LV_KEYBOARD_MODE_NUMBER);

    lv_obj_add_event_cb(ui_curtainTextarea, settext_input_number_event_cb, LV_EVENT_ALL, inputKeyboardNumber);

#if USE_BLE
    ui_LabelBLE = lv_label_create(ui_w1);
    lv_obj_set_width(ui_LabelBLE, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_LabelBLE, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_LabelBLE, -108);
    lv_obj_set_y(ui_LabelBLE, -19);
    lv_obj_set_align(ui_LabelBLE, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LabelBLE, "蓝牙模式");
    ui_object_set_themeable_style_property(ui_LabelBLE, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR,
                                           _ui_theme_color_font);
    ui_object_set_themeable_style_property(ui_LabelBLE, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_OPA,
                                           _ui_theme_alpha_font);
    lv_obj_set_style_text_font(ui_LabelBLE, &ui_font_unit, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_BLESwitch = lv_switch_create(ui_w1);
    lv_obj_set_width(ui_BLESwitch, 73);
    lv_obj_set_height(ui_BLESwitch, 33);
    lv_obj_set_x(ui_BLESwitch, -108);
    lv_obj_set_y(ui_BLESwitch, 18);
    lv_obj_set_align(ui_BLESwitch, LV_ALIGN_CENTER);
    lv_obj_add_state(ui_BLESwitch, LV_STATE_DEFAULT); /// States

    lv_obj_add_event_cb(ui_BLESwitch, chooseBtEventCD, LV_EVENT_ALL, NULL);

#endif

    lv_obj_add_event_cb(ButtonFan, lampButtonCB, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ButtonCurtain, lampButtonCB, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(stateImageCurtain, curtainCB, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_curtainButtonsure, curtainCB2, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_curtainButtonq, curtainCB1, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_ButtonDoorContact, doorContactCB, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_doorContactButtonsure, doorContactCB1, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_openSoundSwitch, openSoundSwitchCB, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_timeoutSwitch, openSoundSwitchCB, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_Dropdowntime, doorContactDropdownCB, LV_EVENT_VALUE_CHANGED, NULL);
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

    deviceRePwButton = lv_btn_create(ui_w3);
    lv_obj_set_width(deviceRePwButton, 46);
    lv_obj_set_height(deviceRePwButton, 28);
    lv_obj_align_to(deviceRePwButton, ui_pwLabel, LV_ALIGN_LEFT_MID, 160, 0);

    deviceRePwLabel = lv_label_create(deviceRePwButton);
    lv_obj_set_width(deviceRePwLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(deviceRePwLabel, LV_SIZE_CONTENT);
    lv_obj_set_align(deviceRePwLabel, LV_ALIGN_CENTER);
    lv_label_set_text(deviceRePwLabel, "重置");
    lv_obj_set_style_text_color(deviceRePwLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(deviceRePwLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(deviceRePwLabel, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    otaLabel = lv_label_create(ui_w3);
    lv_obj_set_width(otaLabel, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(otaLabel, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(otaLabel, -96);
    lv_obj_set_y(otaLabel, 98);
    lv_obj_set_align(otaLabel, LV_ALIGN_CENTER);
    lv_label_set_text(otaLabel, "当前版本: V1.0");
    ui_object_set_themeable_style_property(otaLabel, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR,
                                           _ui_theme_color_font);
    ui_object_set_themeable_style_property(otaLabel, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_OPA,
                                           _ui_theme_alpha_font);
    lv_obj_set_style_text_font(otaLabel, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    otaButton = lv_btn_create(ui_w3);
    lv_obj_set_width(otaButton, 46);
    lv_obj_set_height(otaButton, 28);
    lv_obj_align_to(otaButton, otaLabel, LV_ALIGN_LEFT_MID, 160, 0);

    otabuttonLabel = lv_label_create(otaButton);
    lv_obj_set_width(otabuttonLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(otabuttonLabel, LV_SIZE_CONTENT);
    lv_obj_set_align(otabuttonLabel, LV_ALIGN_CENTER);
    lv_label_set_text(otabuttonLabel, "检查");
    lv_obj_set_style_text_color(otabuttonLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(otabuttonLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(otabuttonLabel, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);

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

    ui_wakeUpLabel = lv_label_create(ui_W4);
    lv_obj_set_width(ui_wakeUpLabel, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_wakeUpLabel, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_wakeUpLabel, -110);
    lv_obj_set_y(ui_wakeUpLabel, -54);
    lv_obj_set_align(ui_wakeUpLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_wakeUpLabel, "语音唤醒");
    ui_object_set_themeable_style_property(ui_wakeUpLabel, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR,
                                           _ui_theme_color_font);
    ui_object_set_themeable_style_property(ui_wakeUpLabel, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_TEXT_OPA,
                                           _ui_theme_alpha_font);
    lv_obj_set_style_text_font(ui_wakeUpLabel, &ui_font_tipFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_wakeUpSwitch = lv_switch_create(ui_W4);
    lv_obj_set_width(ui_wakeUpSwitch, 50);
    lv_obj_set_height(ui_wakeUpSwitch, 25);
    lv_obj_set_x(ui_wakeUpSwitch, -15);
    lv_obj_set_y(ui_wakeUpSwitch, -54);
    lv_obj_set_align(ui_wakeUpSwitch, LV_ALIGN_CENTER);
    lv_obj_add_state(ui_wakeUpSwitch, enbeleWakeUp ? LV_STATE_CHECKED : LV_STATE_DEFAULT);

    lv_obj_add_event_cb(subTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(pubTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(weatherTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(ntpTimeTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(ipTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);
    lv_obj_add_event_cb(portTextarea, settext_input_event_cb, LV_EVENT_ALL, inputKeyboard);

    lv_obj_add_event_cb(ui_wifiSwitch, chooseBtEventCD, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_speechDropdown, SpeechSetDCD, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_aimodeDropdown, SpeechSetDCD, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_add_event_cb(deviceRePwButton, lv_deviceRepwCD, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(otaButton, lv_deviceOTA, LV_EVENT_CLICKED, NULL);

    lv_obj_add_event_cb(ui_wakeUpSwitch, wakeUpSwitchCB, LV_EVENT_CLICKED, NULL);

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

void initCameraUI()
{
    videoImage = lv_img_create(ui_monitorScreen);
    lv_obj_set_pos(videoImage, 0, 0);
    lv_obj_set_size(videoImage, 320, 240);
    lv_obj_move_foreground(ui_connectCameraButton);
    lv_obj_move_foreground(ui_cameraStateLabel);

    videoLeft = lv_btn_create(ui_monitorScreen);
    lv_obj_set_width(videoLeft, 35);
    lv_obj_set_height(videoLeft, 35);
    lv_obj_align(videoLeft, LV_ALIGN_LEFT_MID, 10, 0);

    videoLeftlabel = lv_label_create(videoLeft);
    lv_obj_set_align(videoLeftlabel, LV_ALIGN_CENTER);
    lv_label_set_text(videoLeftlabel, LV_SYMBOL_LEFT);

    videoRight = lv_btn_create(ui_monitorScreen);
    lv_obj_set_width(videoRight, 35);
    lv_obj_set_height(videoRight, 35);
    lv_obj_align(videoRight, LV_ALIGN_RIGHT_MID, -10, 0);

    videoRightlabel = lv_label_create(videoRight);
    lv_obj_set_align(videoRightlabel, LV_ALIGN_CENTER);
    lv_label_set_text(videoRightlabel, LV_SYMBOL_RIGHT);

    lv_obj_add_event_cb(videoLeft, monitorAboutCD, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(videoRight, monitorAboutCD, LV_EVENT_CLICKED, NULL);

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

void my_ui_init(void)
{
    // 设备控制界面UI
    initDeviceUI();
    // 设置界面UI
    initSetConfigUI();
    // 消息弹出UI
    buildPWMsgBox();
    // 视频监控界面
    initCameraUI();
    // 设计界面数据初始化
    initDataUI();
    // 音乐界面UI
    initUIspeech();
}