#include "module_devices.h"

#if USE_AUDIO
#include "Audio.h"
#include <Preferences.h>
#endif

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