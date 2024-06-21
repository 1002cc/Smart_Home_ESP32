#include "module_audio.h"
#include "confighelpr.h"
#include "lvglconfig.h"
#include "module_devices.h"

#if USE_AUDIO
#include "Audio.h"
#include <Preferences.h>
#endif

#if USE_INMP411
#include <driver/i2s.h>
#endif

#if USE_AUDIO
static std::vector<String> stations_list;
Audio audio;
uint8_t max_stations = 0;
uint8_t cur_station = 0;
uint8_t cur_volume = 0;
extern Preferences preferences;

String stations[] = {
    "0n-80s.radionetz.de:8000/0n-70s.mp3",
    "mediaserv30.live-streams.nl:8000/stream",
    "www.surfmusic.de/m3u/100-5-das-hitradio,4529.m3u",
    "stream.1a-webradio.de/deutsch/mp3-128/vtuner-1a",
    "mp3.ffh.de/radioffh/hqlivestream.aac", //  128k aac
    "www.antenne.de/webradio/antenne.m3u",
    "listen.rusongs.ru/ru-mp3-128",
    "edge.audio.3qsdn.com/senderkw-mp3",
    "https://stream.srg-ssr.ch/rsp/aacp_48.asx", // SWISS POP
};

#endif

#if USE_INMP411

#define I2S_NMP411_PORT I2S_NUM_1
#define bufferLen 64
int16_t sBuffer[bufferLen];

#endif

/********************************************************************
                         audio
********************************************************************/

#if USE_AUDIO

void audio_init()
{
    audio.setPinout(PIN_I2S_MAX98357_BCLK, PIN_I2S_MAX98357_LRC, PIN_I2S_MAX98357_DOUT);

    if (!readfsDirlist(stations_list)) {
        stations_list.clear();
        int numStations = sizeof(stations) / sizeof(stations[0]);
        for (int i = 0; i < numStations; ++i) {
            stations_list.push_back(stations[i]);
        }
    }

    max_stations = stations_list.size();

    int volume_c = 15;
    volume_c = ReadintData("volume");
    int station_c = 0;
    station_c = ReadintData("station");
    if (volume_c == 1000) {
        StoreintData("volume", 10);
        StoreintData("station", 0);
    } else {
        if (station_c < max_stations) {
            audiosetStation(station_c);
        }
        audioVolume(volume_c);
    }
    Serial.printf("volume : %d  station: %d\n", volume_c, station_c);
    Serial.println(max_stations);
    if (stations_list.size() >= 1) {
        // String str = optionsGet(stations_list);
        // Serial.println(str);
        // lv_setDropdowninfo(str.c_str());
        for (int i = 0; i < stations_list.size(); i++) {
            Serial.println(stations_list[i]);
            lv_setDropdownaddinfo(musicSubstring(stations_list[i]).c_str(), i);
        }
        lv_setDropdown(cur_station);
        lv_setMusicinfo(musicSubstring(stations_list[cur_station]).c_str());
        lv_setSliderVolume(cur_volume);
    }
}

void audioVolume(int volume)
{
    cur_volume = volume;
    StoreintData("volume", cur_volume);
    audio.setVolume(volume);
}

void audiosetStation(int station)
{
    cur_station = station;
    StoreintData("station", cur_station);
}
void audioStation(int station)
{
    cur_station = station;
    StoreintData("station", cur_station);
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

bool getaudioPlayStatus()
{
    return audio.isRunning();
}

void audioPlay()
{
    Serial.println("Play");
    lv_setDropdown(cur_station);
    lv_setMusicinfo(musicSubstring(stations_list[cur_station]).c_str());
    if (audio.isRunning()) {
        audio.stopSong();
    }
    if (audio.connecttohost(stations_list[cur_station].c_str())) {
        Serial.println("Connect to host");
        lv_setPlayState(true);
    } else {
        Serial.println("Connect to host failed");
        lv_setPlayState(false);
        audio.stopSong();
    }
    Serial.printf("cur station %s\n", stations_list[cur_station].c_str());
}

void audioPause()
{
    audio.stopSong();
    Serial.println("stopSong");
    lv_setPlayState(false);
}

void audioTask(void *pt)
{
    while (1) {
        audio.loop();
        vTaskDelay(3);
    }
    vTaskDelete(NULL);
}

void startAudioTack()
{
    Serial.println("start Audio Tack");
    audio_init();
    xTaskCreatePinnedToCore(audioTask, "audio_task", 5 * 1024, NULL, 2, NULL, 0);
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