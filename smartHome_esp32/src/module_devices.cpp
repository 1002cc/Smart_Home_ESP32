#include "module_devices.h"

#if USE_AUDIO
#include "Audio.h"
#include <Preferences.h>
#endif

#if USE_INMP411
#include <driver/i2s.h>

#endif

#include "DHT.h"
#include "ui.h"
#include <MQUnifiedsensor.h>

#if USE_AUDIO
String stations[] = {
    "0n-80s.radionetz.de:8000/0n-70s.mp3",
    "https://music.163.com/song/media/outer/url?id=1932354158",
    "www.surfmusic.de/m3u/100-5-das-hitradio,4529.m3u",
    "stream.1a-webradio.de/deutsch/mp3-128/vtuner-1a",
    "mp3.ffh.de/radioffh/hqlivestream.aac", //  128k aac
    "www.antenne.de/webradio/antenne.m3u",
    "listen.rusongs.ru/ru-mp3-128",
    "edge.audio.3qsdn.com/senderkw-mp3",
    "macslons-irish-pub-radio.com/media.asx",
};
Audio audio;
uint8_t max_stations = 0;
uint8_t cur_station = 0;
uint8_t cur_volume = 0;
extern Preferences preferences;
#endif

#if USE_INMP411

#define I2S_NMP411_PORT I2S_NUM_1
#define bufferLen 64
int16_t sBuffer[bufferLen];

#endif

DHT dht(DHTPIN, DHTTYPE);

#define Board ("ESP-32")
#define Pin (39)
#define Type ("MQ-2")
#define Voltage_Resolution (3.3)
#define ADC_Bit_Resolution (12)
#define RatioMQ2CleanAir (9.83)
MQUnifiedsensor MQ2(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

/********************************************************************
                         led
********************************************************************/
void led_init()
{
    pinMode(ONBOARDLAMPPIN, OUTPUT);
}

void led_on()
{
    digitalWrite(ONBOARDLAMPPIN, 1);
}
void led_off()
{
    digitalWrite(ONBOARDLAMPPIN, 0);
}

void rgbled_init()
{
    pinMode(RPIN, OUTPUT);
    pinMode(GPIN, OUTPUT);
    pinMode(BPIN, OUTPUT);
}

void setColor(int r, int g, int b)
{
    analogWrite(RPIN, r);
    analogWrite(GPIN, g);
    analogWrite(BPIN, b);
}

void rgbled_red()
{
    setColor(255, 0, 0);
}

void rgbled_green()
{
    setColor(0, 255, 0);
}

void rgbled_blue()
{
    setColor(0, 0, 255);
}

void rgbled_off()
{
    setColor(0, 0, 0);
}

void rgbled_setColor(int r, int g, int b)
{
    setColor(r, g, b);
}

/********************************************************************
                         audio
********************************************************************/
#if USE_AUDIO
String musicSubstring(String str)
{
    int lastSlashIndex = str.lastIndexOf('/');
    if (lastSlashIndex != -1) {
        return str.substring(lastSlashIndex + 1);
    }
    return "";
}

String optionsGet()
{
    String options;
    for (int i = 0; i < sizeof(stations) / sizeof(stations[0]); i++) {
        String url = stations[i];
        options += musicSubstring(url);
        options += "\n";
    }
    options.trim();
    return options;
}
void audio_init()
{
    max_stations = sizeof(stations) / sizeof(stations[0]);
    Serial.println(max_stations);
    audio.setPinout(PIN_I2S_MAX98357_BCLK, PIN_I2S_MAX98357_LRC, PIN_I2S_MAX98357_DOUT);
    audio.setVolume(cur_volume);
    // audio.connecttohost(stations[cur_station].c_str());
}

void audioVolume(int volume)
{
    cur_volume = volume;
    preferences.putInt("volume", cur_volume);
    audio.setVolume(volume);
}

void audioStation(int station)
{
    cur_station = station;
    preferences.putInt("station", cur_station);
    audioPlay();
}
void audioPrevious()
{
    if (cur_station > 0) {
        cur_station--;
        audioStation(cur_station);
    }
}

void audioNext()
{
    if (cur_station < max_stations - 1) {
        cur_station++;
        audioStation(cur_station);
    }
}

void audioPlay()
{
    Serial.println("Play");
    lv_dropdown_set_selected(ui_musicDropdown, cur_station);
    lv_label_set_text(ui_Label25, musicSubstring(stations[cur_station]).c_str());
    if (audio.isRunning()) {
        audio.stopSong();
    }
    if (audio.connecttohost(stations[cur_station].c_str())) {
        Serial.println("Connect to host");
        lv_label_set_text(ui_playLabel, LV_SYMBOL_PAUSE);
    } else {
        Serial.println("Connect to host failed");
        lv_label_set_text(ui_playLabel, LV_SYMBOL_PLAY);
    }
    Serial.printf("cur station %s\n", stations[cur_station].c_str());
}

void audioPause()
{
    audio.stopSong();
    Serial.println(audio.isRunning());
}

void audioTask(void *pt)
{
    while (1) {
        audio.loop();
        vTaskDelay(2);
    }
    vTaskDelete(NULL);
}

void startAudioTack()
{
    xTaskCreatePinnedToCore(audioTask, "audio_task", 1024 * 5, NULL, 2, NULL, 1);
}
#endif

/********************************************************************
                          inmp441
********************************************************************/
#if USE_INMP411
bool initinmp441()
{
    const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 44100,
        .bits_per_sample = i2s_bits_per_sample_t(16),
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = bufferLen,
        .use_apll = false};
    esp_err_t err;
    err = i2s_driver_install(I2S_NMP411_PORT, &i2s_config, 0, NULL);
    f(err != ESP_OK)
    {
        Serial.printf("Failed installing driver: %d\n", err);
        return false;
    }
    const i2s_pin_config_t pin_config = {
        .bck_io_num = PIN_I2S_INMP411_SCK,
        .ws_io_num = PIN_I2S_INMP411_WS,
        .data_out_num = -1,
        .data_in_num = PIN_I2S_INMP411_SD};

    err = i2s_set_pin(I2S_NMP411_PORT, &pin_config);
    err = i2s_driver_install(I2S_NMP411_PORT, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("Failed installing driver: %d\n", err);
        return false;
    }
    return true;
}

int I2Sread(int16_t *samples, int count) // read from i2s
{
    size_t bytes_read = 0;
    if (count > 128) {
        count = 128; // 最少读取128
    }
    i2s_read(REC_I2S_PORT, (char *)samples_32bit, sizeof(int32_t) * count, &bytes_read, portMAX_DELAY);
    int samples_read = bytes_read / sizeof(int32_t);
    for (int i = 0; i < samples_read; i++) {
        int32_t temp = samples_32bit[i] >> 11;
        samples[i] = (temp > INT16_MAX) ? INT16_MAX : (temp < -INT16_MAX) ? -INT16_MAX
                                                                          : (int16_t)temp;
    }
    return samples_read;
}

void micinmp441Task(void *parameter)
{

    initinmp441();
    i2s_start(I2S_NMP411_PORT);

    size_t bytesIn = 0;
    while (1) {
        esp_err_t result = i2s_read(I2S_NMP411_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
        if (result == ESP_OK && isWebSocketConnected) {
            // client.sendBinary((const char *)sBuffer, bytesIn);
        }
    }
}

void startinmp441()
{
    i2s_start(I2S_NMP411_PORT);
    xTaskCreatePinnedToCore(micinmp441Task, "inmp441_task", 1024 * 5, NULL, 2, NULL, 1);
}

#endif

/********************************************************************
                         mq2
********************************************************************/

void initmq2()
{
    MQ2.setRegressionMethod(1);
    MQ2.setA(987.99);
    MQ2.setB(-2.162);
    MQ2.init();
    Serial.print("Calibrating please wait.");
    float calcR0 = 0;
    for (int i = 1; i <= 10; i++) {
        MQ2.update(); // Update data, the arduino will read the voltage from the analog pin
        calcR0 += MQ2.calibrate(RatioMQ2CleanAir);
        Serial.print(".");
    }
    MQ2.setR0(calcR0 / 10);
    Serial.println("  done!.");

    if (isinf(calcR0)) {
        Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
        while (1)
            ;
    }
    if (calcR0 == 0) {
        Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
        while (1)
            ;
    }
}
float readmq2()
{
    MQ2.update();
    return MQ2.readSensor();
}

/********************************************************************
                         DHT11
********************************************************************/

void initDHT()
{
    dht.begin();
}
float dhtReadTemperature()
{
    return dht.readTemperature();
}

float dhtReadHumidity()
{
    return dht.readHumidity();
}

/********************************************************************
                         initDevices
********************************************************************/
void initDevices()
{
    led_init();
    rgbled_init();
#if USE_AUDIO
    audio_init();
#endif
}