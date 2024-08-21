#include "module_camera.h"
#include "Arduino.h"
#include "esp_camera.h"
#include "module_devices.h"
#include "module_mqtt.h"
#include <Arduino.h>

int g_mqtt_time_count = 0;
extern struct tm g_time;
void initCamera()
{
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 10000000;
    config.pixel_format = PIXFORMAT_JPEG;
    // init with high specs to pre-allocate larger buffers
    if (psramFound()) {
        config.frame_size = FRAMESIZE_QVGA; // 320x240
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        ESP.restart();
        return;
    }

    // sensor_t *s = esp_camera_sensor_get();
    // if (s->id.PID == OV3660_PID) {
    //     s->set_vflip(s, 1);
    //     s->set_brightness(s, 1);
    //     s->set_saturation(s, -2);
    // }
    // if (config.pixel_format == PIXFORMAT_JPEG) {
    //     s->set_framesize(s, FRAMESIZE_QVGA);
    // }
    // s->set_vflip(s, 1);
    // s->set_hmirror(s, 1);
}

void uploadImage(void)
{
    Serial.println("uploadImage via MQTT");
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        return;
    }

    if (fb->format != PIXFORMAT_JPEG) {
        Serial.println("Non-JPEG data not implemented");
        esp_camera_fb_return(fb);
        return;
    }
    Serial.println(fb->len);
    if (!publishMQTT((const char *)fb->buf, fb->len)) {
        Serial.println("[Failure] Uploading Image via MQTT");
    }

    esp_camera_fb_return(fb);
}

void sendImgPieces(void)
{
    camera_fb_t *fb = esp_camera_fb_get();
    if (fb) {
        String s_message; // 待发送消息

        // 添加元信息(时间+长度)
        Serial.printf("IMG:width: %d, height: %d, buf: 0x%x, len: %d\n", fb->width, fb->height, fb->buf, fb->len);
        char header_msg[28];
        // ","也要计入
        sprintf(header_msg, "%03d,%03d,%08x,%9d,", fb->width, fb->height, fb->buf, fb->len);
        Serial.print("header_msg:");
        for (int i = 0; i < sizeof(header_msg); i++) {
            s_message += header_msg[i];
            Serial.printf("0x%02X ", header_msg[i]);
        }
        Serial.println();

        char time_msg[15]; // 用于存储格式化后的时间字符串
        // sprintf(time_msg,"%04X%04X%04X%04X%04X%04X",'a','b','c','d','e','f');
        Serial.println(&g_time, "%Y-%m-%d %H:%M:%S");
        strftime(time_msg, sizeof(time_msg), "%Y%m%d%H%M%S", &g_time);
        // sprintf(time_msg, "%04X%04X%04X%04X%04X%04X", g_time.tm_year, g_time.tm_mon, g_time.tm_mday, g_time.tm_hour, g_time.tm_min, g_time.tm_sec);
        Serial.print("time_msg:");
        for (int i = 0; i < sizeof(time_msg); i++) {
            s_message += time_msg[i];
            Serial.printf("0x%02X ", time_msg[i]);
        }
        Serial.println();

        // 分帧传送图像数据
        char data[1];

        for (int i = 0; i < fb->len; i++) {
            sprintf(data, "%02X", *((fb->buf + i)));
            s_message += data;
        }

        if (s_message.length() > 0) {
            mqtt_beginPublish(s_message.length(), 0);
            mqtt_print(s_message);
            mqtt_endPublish();
            s_message = "";
        }
        g_mqtt_time_count = 0;
        esp_camera_fb_return(fb);

        Serial.println("IMG sent!");
    }
}