#include "module_audio.h"
#include "confighelpr.h"
#include "lvglconfig.h"
#include "module_devices.h"

#if USE_AUDIO
#include "Audio.h"
#include <Preferences.h>
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
};

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
    Serial.printf("volume : %d  station: %d  max_stations:%d\n", volume_c, station_c, max_stations);
    if (stations_list.size() >= 1) {
        lv_setDropdowninfo(optionsGet(stations_list).c_str());
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

void audioLoop()
{
    audio.loop();
}

void startAudioTack()
{
    Serial.println("start Audio Tack");
    audio_init();
    xTaskCreatePinnedToCore(audioTask, "audio_task", 5 * 1024, NULL, 2, NULL, 0);
}
#endif
