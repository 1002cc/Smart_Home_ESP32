#include "module_speak.h"
#include "confighelpr.h"
#include "lvglconfig.h"
#include "module_audio.h"
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
using namespace websockets;
bool startPlay = false;
const char *tts_wed_api = TTS_WED_API;
const char *stt_wed_api = STT_WED_API;

WebsocketsClient webSocketClient_tts;
WebsocketsClient webSocketClient_sst;

void initSpeakConfig()
{
}

void speakTask(void *pvParameters)
{
    while (1) {
        webSocketClient_tts.poll();
        webSocketClient_sst.poll();
        audioLoop();
        vTaskDelay(3);
    }
    vTaskDelete(NULL);
}