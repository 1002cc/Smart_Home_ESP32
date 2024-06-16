#include "WiFi.h"
#include <TFT_eSPI.h>
#include <esp_wifi.h>
#include <lvgl.h>
// ui
#include "ui.h"

#include "module_config.h"
#include "module_devices.h"
#include "module_service.h"
#include "smarthome_mqtt.h"

TimerHandle_t ntpTimer;
unsigned long lastMillis = 0;

static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

float temperature, humidity, mq2sensorValue;

bool hasNetwork = false;
extern bool palyState;
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

        /*Set the coordinates*/
        data->point.x = touchX * 1.33;
        data->point.y = touchY * 0.75;
        // Serial.print("Data x ");
        // Serial.println(touchX);

        // Serial.print("Data y ");
        // Serial.println(touchY);
    }
}

void initLvgl();
void ui_timer_init();

void ui_clock_update(lv_timer_t *timer);
void ui_calender_update();
void colorwheel_event_cb(lv_event_t *e);

void ntpTimerCallback(TimerHandle_t xTimer);
bool wifiConnect();

void lvgl_task(void *param);
void ntpTask(void *param);
void sensor_task(void *pvParameter);

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

void ui_timer_init()
{
    // 时钟更新
    lv_timer_t *ui_timer = lv_timer_create(ui_clock_update, 2000, NULL);
    ui_clock_update(ui_timer);
}

void colorwheel_event_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_color_t color = lv_colorwheel_get_rgb(obj);

    uint8_t r = LV_COLOR_GET_R(color);
    uint8_t g = LV_COLOR_GET_G(color);
    uint8_t b = LV_COLOR_GET_B(color);
    rgbled_setColor(r, g, b);

    Serial.print("Color changed to R:");
    Serial.print(r);
    Serial.print(" G:");
    Serial.print(g);
    Serial.print(" B:");
    Serial.println(b);
}

void WiFiEvent(WiFiEvent_t event)
{
    switch (event) {
    case SYSTEM_EVENT_STA_DISCONNECTED:
        hasNetwork = false;
        Serial.println("wifi connect no");
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        Serial.println("wifi connect ok");
        hasNetwork = isNetworkAvailable();
        Serial.printf("\r\n-- wifi connect success! --\r\n");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("isNetworkAvailable : ");
        Serial.println(hasNetwork);
        break;
    }
}

void initLvgl()
{
    lv_init();

    tft.begin();
    tft.setRotation(3);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, 1);
    uint16_t calData[5] = {490, 3259, 422, 3210, 1};
    tft.setTouch(calData);
    static lv_color_t draw_buf1[screenWidth * screenHeight / 2];
    // static lv_color_t draw_buf2[screenWidth * screenHeight / 2];

    lv_disp_draw_buf_init(&draw_buf, draw_buf1, NULL, screenWidth * screenHeight / 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    // disp_drv.full_refresh = 1;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    ui_init();
    ui_timer_init();
    initNetWorkUIConfig();

    lv_obj_del(ui_weathericonImage2);
    lv_obj_del(ui_weathericonImage3);
    lv_obj_del(ui_weathericonImage4);

    lv_obj_add_event_cb(ui_Colorwheel1, colorwheel_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    Serial.println("lvgl init successfully");
}

void lvgl_task(void *pt)
{
    while (1) {
        lv_timer_handler();
        vTaskDelay(5);
    }
    vTaskDelete(NULL);
}

void sensor_task(void *pvParameter)
{
    initDHT();

    char temp_char[12];

    while (1) {
        temperature = dhtReadTemperature();
        humidity = dhtReadHumidity();

        if (isnan(temperature) || isnan(humidity)) {
            Serial.println("Failed to read from DHT sensor!");
            vTaskDelay(3000 / portTICK_PERIOD_MS);
        } else {
            Serial.print("Temperature: ");
            Serial.print(temperature);
            Serial.print(" °C, Humidity:");
            Serial.print(humidity);
            Serial.print("% \n");

            lv_arc_set_value(ui_TemperatureArc, (int16_t)temperature);
            lv_arc_set_value(ui_HumidityArc, (int16_t)humidity);
            snprintf(temp_char, sizeof(temp_char), "%.0f°C", temperature);
            lv_label_set_text(ui_TemperatureLabel, temp_char);
            snprintf(temp_char, sizeof(temp_char), "%.0f%%", humidity);
            lv_label_set_text(ui_HumidityLabel, temp_char);
            SensorData sensorData = {
                .temp = temperature,
                .humidity = humidity,
                .mq = mq2sensorValue,
            };
            publishSensorData(sensorData);
            vTaskDelay(30000 / portTICK_PERIOD_MS);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void mq2_task(void *pvParameter)
{
    initmq2();
    char temp_char[12];
    while (1) {
        mq2sensorValue = readmq2();
        Serial.print("mq2: ");
        Serial.println(mq2sensorValue);

        lv_arc_set_value(ui_MQArc, (int16_t)mq2sensorValue);
        snprintf(temp_char, sizeof(temp_char), "%d%%", (int)mq2sensorValue);
        lv_label_set_text(ui_MQLabel, temp_char);
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        SensorData sensorData = {
            .temp = temperature,
            .humidity = humidity,
            .mq = mq2sensorValue,
        };
        publishSensorData(sensorData);
    }
    vTaskDelete(NULL);
}

void setup()
{
    Serial.begin(115200);

    Serial.printf("Deafult free size: %d\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    Serial.printf("PSRAM free size: %d\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    Serial.printf("Flash size: %d bytes\n", ESP.getFlashChipSize());

    initDevices();

    initLvgl();
    xTaskCreate(lvgl_task, "lvgl_task", 1024 * 10, NULL, 2, NULL);

    vTaskDelay(500);

    WiFi.onEvent(WiFiEvent);
    if (!wifiConnect()) {
        Serial.println("Connection to WiFi failed");
        lv_label_set_text(ui_tipLabel, "连接wifi失败,当前为离线模式,请在配置中连接wifi");
    } else {
        lv_label_set_text(ui_tipLabel, "WiFi连接成功");
        vTaskDelay(3000);
        WSLED_OFF();
    }
    vTaskDelay(500);
    hasNetwork = isNetworkAvailable();
    if (hasNetwork) {
        Serial.println("NetworkAvailable is true");
        lv_label_set_text(ui_tipLabel, "正在连接服务器");
        if (!initMQTT()) {
            lv_label_set_text(ui_tipLabel, "服务器连接失败");
            lv_obj_add_state(ui_mqttSwitch, LV_STATE_DEFAULT);
            lv_label_set_text(ui_mqttStateLabel, "未连接");
        } else {
            lv_label_set_text(ui_tipLabel, "服务器连接成功");
            lv_obj_add_state(ui_mqttSwitch, LV_STATE_CHECKED);
            lv_label_set_text(ui_mqttStateLabel, "已连接");
        }
        // startMqttTask();
        ntpTimer = xTimerCreate("NTP and Weather Timer", pdMS_TO_TICKS(3600000), pdTRUE, (void *)1, ntpTimerCallback);
        xTimerStart(ntpTimer, 0);
        ntpTimerCallback(NULL);
    } else {
        Serial.println("NetworkAvailable is false");
        lv_label_set_text(ui_tipLabel, "网络不可用,无法连接服务器");
    }
    vTaskDelay(500);
    lv_scr_load_anim(ui_MainScreen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, true);

    xTaskCreate(sensor_task, "sensor_task", 3096, NULL, 3, NULL);
    xTaskCreate(mq2_task, "mq2_task", 3096, NULL, 3, NULL);

#if USE_AUDIO
    startAudioTack();
#endif
}

void loop()
{
    if (hasNetwork) {
        mqttLoop();
    }
}
