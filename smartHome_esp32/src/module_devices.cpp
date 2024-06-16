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

int pos = 0;

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
    if (audio.isRunning()) {
        audio.stopSong();
    }
    if (audio.connecttohost(stations[cur_station].c_str())) {
        Serial.println("Connect to host");
    } else {
        Serial.println("Connect to host failed");
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
        vTaskDelay(5);
    }
    vTaskDelete(NULL);
}

void startAudioTack()
{
    xTaskCreatePinnedToCore(audioTask, "audio_task", 1024 * 5, NULL, 2, NULL, 1);
}
#endif

/********************************************************************
                         sg90
********************************************************************/

void sg90_init()
{
    pinMode(SG90_PIN, OUTPUT);
    ledcSetup(1, 50, 8);
    ledcAttachPin(SG90_PIN, 1);
}
void sg90_setAngle(int angle)
{
    ledcWrite(1, angle);
}

/********************************************************************
                         RAIN
********************************************************************/

void rainTask(void *pt)
{
    int rain;
    while (1) {
        // analogWrite(RAIN_PIN, (map(analogRead(A0), 0, 1023, 235, 0)));
        rain = map(analogRead(A0), 0, 1023, 235, 0);
        Serial.print("rain = ");
        Serial.println(rain); // 串口输出雨量
        int soundState = digitalRead(SOUND_PIN);
        Serial.print("soundState = ");
        Serial.println(soundState);
        int pirState = digitalRead(PIR_PIN);
        Serial.print("PIR_PIN = ");
        Serial.println(pirState);
        if (rain > 100) {
            if (pos <= 0) {
                for (pos = 0; pos <= 180; pos += 1) {
                    sg90_setAngle(pos);
                    vTaskDelay(15);
                }
            }
        } else {
            if (pos >= 180) {
                for (pos = 180; pos >= 0; pos -= 1) {
                    sg90_setAngle(pos);
                    vTaskDelay(15);
                }
            }
        }
        vTaskDelay(500);
    }
}

void rain_init()
{
    pinMode(RAINADCPIN, INPUT); // A0口接收模拟输入信号，即接收是否有雨水的信号
    pinMode(RAIN_PIN, OUTPUT);
}

void startRainTask()
{
    xTaskCreatePinnedToCore(rainTask, "rain_task", 1024 * 5, NULL, 2, NULL, 1);
}

/********************************************************************
                         initDevices
********************************************************************/
void initDevices()
{
    led_init();
    rgbled_init();
    pinMode(SOUND_PIN, INPUT);
    pinMode(PIR_PIN, INPUT);
    sg90_init();
    rain_init();
#if USE_AUDIO
    audio_init();
#endif
}