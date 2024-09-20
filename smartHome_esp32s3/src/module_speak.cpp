#include "module_speak.h"
#include "confighelpr.h"
#include "lvglconfig.h"
#include "module_audio.h"
#include "module_devices.h"
#include "module_service.h"
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
#include <Base64_Arturo.h>
#include <LittleFS.h>
#include <base64.h>
#include <driver/i2s.h>
#if USE_AUDIO
#include "audiohelpr.h"
#endif

// int16_t audioData[2560];
int16_t *audioData;
int16_t *pcm_data; // 录音缓存区
uint recordingSize = 0;

String answer_list[10];
uint8_t answer_list_num = 0;
bool answer_ste = 0;
String stttext = "";

// 火山引擎(豆包)
const char *apiKey = "b50ace91-bf6d-4628-9096-14a4d2a21b80";
const char *endpointId = "hich_doubao";
const String doubao_system = "你是一个智能家居助手小欧，回答问题比较简洁。"; // 定义豆包的人设
String duobaoUrl = "https://ark.cn-beijing.volces.com/api/v3/chat/completions";

// MINIMAX模型
const char *minmaxapiKey = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJHcm91cE5hbWUiOiLpmYjlhYnlro8iLCJVc2VyTmFtZSI6IumZiOWFieWujyIsIkFjY291bnQiOiIiLCJTdWJqZWN0SUQiOiIxNzkzMjYyMTIwMDA4MTcyNDYwIiwiUGhvbmUiOiIxODkyNjI0NzExMiIsIkdyb3VwSUQiOiIxNzkzMjYyMTE5OTk5NzgzODUyIiwiUGFnZU5hbWUiOiIiLCJNYWlsIjoiIiwiQ3JlYXRlVGltZSI6IjIwMjQtMDYtMjIgMTU6NDc6MTYiLCJpc3MiOiJtaW5pbWF4In0.XCozhyQkIr1r-_JXamid9sOa4A83v9dTcsyHj5COIi1bCgrzD4ANb-ZpNBIRs_qmvx5mUobo9Q3u890JzDllimit15QiXhDVv8kV71jJcoW-i0CfXC2N5HagOVgIGpKi3CZ12X1c3szjcSjjFAseq1djFuzRp6lCwxtVrqCSJz8Nwx2FO_Q0ZdWKxso73i6MfgfJAkfLHqc87SFwZXmvun08JVZrgwlDOF3QqvXce002Tpa6h9d6y1dG_cs-hXS4Br31h-3W7g_JAfywOz5yVSLaXi5ghnRRHqTXohSSzhuA9ZcS4h0LDI81425GVbTmKwapu9Op26YV69hK6IPyFg";
String minimaxUrl = "https://api.minimax.chat/v1/text/chatcompletion_v2";

using namespace websockets;
// WebsocketsClient webSocketClient_tts;
WebsocketsClient webSocketClient_stt;

#if USE_AUDIO
extern AudioHelpr audio;
#endif

int noise = 50;

SpeakState_t speakState = NO_DIALOGUE;
int useAIMode = 0;

TaskHandle_t speakTaskHandle = NULL;
SemaphoreHandle_t xspeakSemaphore = NULL;

void sendSTTData();
String XF_wsUrl(const char *Secret, const char *Key, String request, String host);
void speakTask(void *pvParameter);

String postDouBaoAnswer(String *answerlist, int listnum);
String getMiniMaxAnswer(String inputText);
String getXunfeiAnswer(const String &inputText);
String getDoubaoAnswer(const String &inputText);

/********************************************************************
                         max98357
********************************************************************/
#if USE_MAX98357

bool initMax98357()
{
    i2s_config_t i2sOut_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = i2s_bits_per_sample_t(16),
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024};

    esp_err_t err = i2s_driver_install(I2S_MAX_PORT, &i2sOut_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("I2S driver install failed (I2S_PORT_1): %d\n", err);
        return false;
    }

    const i2s_pin_config_t i2sOut_pin_config = {
        .bck_io_num = PIN_I2S_MAX98357_BCLK,
        .ws_io_num = PIN_I2S_MAX98357_LRC,
        .data_out_num = PIN_I2S_MAX98357_DOUT,
        .data_in_num = -1};

    err = i2s_set_pin(I2S_MAX_PORT, &i2sOut_pin_config);
    if (err != ESP_OK) {
        Serial.printf("I2S set pin failed (I2S_PORT_1): %d\n", err);
        return false;
    }
    Serial.println("INMP411 init successfully");
    return true;
}

#endif

/********************************************************************
                          inmp441
********************************************************************/
#if USE_INMP411
bool initinmp441()
{
    const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = i2s_bits_per_sample_t(16),
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
        .intr_alloc_flags = 0, // default interrupt priority
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false};

    esp_err_t err = i2s_driver_install(I2S_NMP411_PORT, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("I2S driver install failed (I2S_NMP411_PORT): %d\n", err);
        return false;
    }
    const i2s_pin_config_t pin_config = {
        .bck_io_num = PIN_I2S_INMP411_SCK,
        .ws_io_num = PIN_I2S_INMP411_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = PIN_I2S_INMP411_SD};

    err = i2s_set_pin(I2S_NMP411_PORT, &pin_config);
    if (err != ESP_OK) {
        Serial.printf("I2S set pin failed (I2S_NMP411_PORT): %d\n", err);
        return false;
    }
    Serial.println("INMP411 init successfully");
    return true;
}

#endif

bool initI2SConfig()
{
    Serial.println("Setup I2S ...");
#if USE_AUDIO
    if (initinmp441())
#else
    if (initMax98357() && initinmp441())
#endif
    {
        Serial.println("I2S init success");
        return true;
    } else {
        Serial.println("I2S init failed");
    }
    return false;
}

void initSpeakConfig()
{
    initI2SConfig();
    xspeakSemaphore = xSemaphoreCreateMutex();
    audioData = (int16_t *)ps_malloc(2560 * sizeof(int16_t));

    esp_err_t err = i2s_start(I2S_NMP411_PORT);
    if (err != ESP_OK) {
        Serial.printf("I2S start failed (I2S_NMP411_PORT): %d\n", err);
        return;
    }

    // webSocketClient_tts.onMessage([&](WebsocketsMessage message) { // 讯飞TTS的 wx连接回调函数
    //     // Serial.print("Got Message: ");
    //     speakState = ANSWERING;
    //     DynamicJsonDocument responseJson(51200);
    //     DeserializationError error = deserializeJson(responseJson, message.data());
    //     const char *response = responseJson["data"]["audio"].as<String>().c_str();
    //     int response_len = responseJson["data"]["audio"].as<String>().length();
    //     // Serial.printf("lan: %d  \n", response_len);

    //     // 分段获取PCM音频数据并输出到I2S上
    //     for (int i = 0; i < response_len; i += CHUNK_SIZE) {
    //         int remaining = min(CHUNK_SIZE, response_len);                                       // 计算剩余数据长度
    //         char chunk[CHUNK_SIZE];                                                              // 创建一个缓冲区来存储读取的数据
    //         int decoded_length = Base64_Arturo.decode(chunk, (char *)(response + i), remaining); // 从response中解码数据到chunk
    //         size_t bytes_written = 0;
    //         i2s_write(I2S_MAX_PORT, chunk, decoded_length, &bytes_written, portMAX_DELAY);
    //     }

    //     if (responseJson["data"]["status"].as<int>() == 2) { // 收到结束标志
    //         Serial.println("Playing complete.");
    //         delay(500);
    //         i2s_zero_dma_buffer(I2S_MAX_PORT); // 清空I2S DMA缓冲区
    //         lv_setSpeechinfo(" ");
    //         speakState = NO_DIALOGUE;
    //     }
    // });

    webSocketClient_stt.onMessage([&](WebsocketsMessage message) { // STT ws连接的回调函数
        Serial.print("Got Message: ");
        Serial.println(message.data());
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, message.data());
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }
        JsonArray ws = doc["data"]["result"]["ws"];
        for (JsonObject word : ws) {
            int bg = word["bg"];
            const char *w = word["cw"][0]["w"];
            stttext += w;
        }
        if (doc["data"]["status"] == 2) { // 收到结束标志

            if (stttext.isEmpty()) {
                lv_speakState(SpeakState_t::NO_DIALOGUE);
                audio.connecttoFS(LittleFS, "/nosound.mp3");
            } else {
                lv_speakState(SpeakState_t::ANSWERING);
            }
            Serial.printf("stttext: %s\n", stttext.c_str());
            xTaskNotify(speakTaskHandle, 1, eSetBits);
        }
    });
    Serial.println("initSpeakConfig done");
    // startSpeakTask();
}

// 向讯飞STT发送音频数据
void sendSTTData()
{
    uint8_t status = 0;
    int dataSize = 1280 * 8;
    int audioDataSize = recordingSize * 2;
    uint lan = (audioDataSize) / dataSize;
    uint lan_end = (audioDataSize) % dataSize;
    if (lan_end > 0) {
        lan++;
    }

    Serial.printf("byteDatasize: %d , lan: %d , lan_end: %d \n", audioDataSize, lan, lan_end);
    String host_url = XF_wsUrl(TTS_SECRETKEY, TTS_APIKEY, "/v2/iat", "iat-api.xfyun.cn");
    Serial.println("Connecting to server.");
    bool connected = webSocketClient_stt.connect(host_url);
    if (connected) {
        Serial.println("Connected!");
    } else {
        Serial.println("Not Connected!");
        lv_speakState(SpeakState_t::NO_DIALOGUE);
        lv_setSpeechinfo("服务器连接失败");

        return;
    }
    // 分段向STT发送PCM音频数据
    for (int i = 0; i < lan; i++) {
        if (i == (lan - 1)) {
            status = 2;
        }
        if (status == 0) {
            String input = "{";
            input += "\"common\":{ \"app_id\":\"6501ee63\" },";
            input += "\"business\":{\"domain\": \"iat\", \"language\": \"zh_cn\", \"accent\": \"mandarin\", \"vinfo\":1,\"vad_eos\":10000},";
            input += "\"data\":{\"status\": 0, \"format\": \"audio/L16;rate=16000\",\"encoding\": \"raw\",\"audio\":\"";
            String base64audioString = base64::encode((uint8_t *)pcm_data, dataSize);
            input += base64audioString;
            input += "\"}}";
            // Serial.printf("input: %d , status: %d \n", i, status);
            //  Serial.println(input);
            webSocketClient_stt.send(input);
            status = 1;
        } else if (status == 1) {
            String input = "{";
            input += "\"data\":{\"status\": 1, \"format\": \"audio/L16;rate=16000\",\"encoding\": \"raw\",\"audio\":\"";
            String base64audioString = base64::encode((uint8_t *)pcm_data + (i * dataSize), dataSize);
            input += base64audioString;
            input += "\"}}";
            // Serial.printf("input: %d , status: %d \n", i, status);
            webSocketClient_stt.send(input);
        } else if (status == 2) {
            if (lan_end == 0) {
                String input = "{";
                input += "\"data\":{\"status\": 2, \"format\": \"audio/L16;rate=16000\",\"encoding\": \"raw\",\"audio\":\"";
                String base64audioString = base64::encode((uint8_t *)pcm_data + (i * dataSize), dataSize);
                input += base64audioString;
                input += "\"}}";
                // Serial.printf("input: %d , status: %d \n", i, status);
                webSocketClient_stt.send(input);
            }
            if (lan_end > 0) {
                String input = "{";
                input += "\"data\":{\"status\": 2, \"format\": \"audio/L16;rate=16000\",\"encoding\": \"raw\",\"audio\":\"";

                String base64audioString = base64::encode((uint8_t *)pcm_data + (i * dataSize), lan_end);

                input += base64audioString;
                input += "\"}}";
                // Serial.printf("input: %d , status: %d \n", i, status);
                webSocketClient_stt.send(input);
            }
        }
        delay(10);
    }
}

String formatDateForURL(String dateString)
{
    dateString.replace(" ", "+");
    dateString.replace(",", "%2C");
    dateString.replace(":", "%3A");
    return dateString;
}

// 构造讯飞ws连接url
String XF_wsUrl(const char *Secret, const char *Key, String request, String host)
{
    String timeString = getDateTime_one();
    String signature_origin = "host: " + host;
    signature_origin += "\n";
    signature_origin += "date: ";
    signature_origin += timeString;
    signature_origin += "\n";
    signature_origin += "GET " + request + " HTTP/1.1";

    unsigned char hmacResult[32];
    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1); // 1 表示 HMAC
    mbedtls_md_hmac_starts(&ctx, (const unsigned char *)Secret, strlen(Secret));
    mbedtls_md_hmac_update(&ctx, (const unsigned char *)signature_origin.c_str(), signature_origin.length());
    mbedtls_md_hmac_finish(&ctx, hmacResult);
    mbedtls_md_free(&ctx);
    String base64Result = base64::encode(hmacResult, 32);

    String authorization_origin = "api_key=\"";
    authorization_origin += Key;
    authorization_origin += "\", algorithm=\"hmac-sha256\", headers=\"host date request-line\", signature=\"";
    authorization_origin += base64Result;
    authorization_origin += "\"";
    String authorization = base64::encode(authorization_origin);

    String url = "ws://" + host + request;
    url += "?authorization=";
    url += authorization;
    url += "&date=";
    url += formatDateForURL(timeString);
    url += "&host=" + host;
    Serial.println("\nurl encoded result:");
    Serial.println(url);
    return url;
}

// 向讯飞TTS发送请求
// void postTTS(String texttts)
// {
//     String TTSurl = XF_wsUrl(TTS_SECRETKEY, TTS_APIKEY, "/v2/tts", TTS_WED_API);
//     bool connected = webSocketClient_tts.connect(TTSurl);
//     if (connected) {
//         Serial.println("Connected!");
//     } else {
//         Serial.println("Not Connected!");
//         lv_setSpeechinfo("服务器连接失败");
//     }

//     String TTStextbase64 = base64::encode(texttts);
//     DynamicJsonDocument requestJson(51200);
//     requestJson["common"]["app_id"] = TTS_APPID;
//     requestJson["business"]["aue"] = "raw";
//     requestJson["business"]["vcn"] = ai_speak.c_str();
//     requestJson["business"]["pitch"] = 50;
//     requestJson["business"]["speed"] = 50;
//     requestJson["business"]["tte"] = "UTF8";
//     requestJson["business"]["auf"] = "audio/L16;rate=16000";
//     requestJson["data"]["status"] = 2;
//     requestJson["data"]["text"] = TTStextbase64;

//     String payload;
//     serializeJson(requestJson, payload);
//     Serial.print("payload: ");
//     Serial.println(payload);
//     webSocketClient_tts.send(payload);
// }

// 向豆包发送请求
String postDouBaoAnswer(String *answerlist, int listnum)
{
    Serial.println("POSTtoDoubao..");
    String answer;

    HTTPClient http;
    http.begin(duobaoUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(apiKey));

    DynamicJsonDocument requestJson(5120);
    requestJson["model"] = endpointId;
    JsonArray list = requestJson.createNestedArray("messages");

    JsonObject item = list.createNestedObject();
    item["role"] = "system";
    item["content"] = doubao_system;

    for (int i = 0; i < listnum; i += 2) {
        item = list.createNestedObject();
        item["role"] = "user";
        item["content"] = answerlist[i];
        Serial.print("answer user: ");
        Serial.println(answerlist[i]);
        if (listnum > 1 and i != listnum - 1) {
            if (answerlist[i + 1] != "") {
                item = list.createNestedObject();
                item["role"] = "assistant";
                item["content"] = answerlist[i + 1];
            }
            Serial.print("answer assistant: ");
            Serial.println(answerlist[i + 1]);
        }
    }

    requestJson["stream"] = false;
    String requestBody;
    serializeJson(requestJson, requestBody);
    Serial.print("payload: ");
    Serial.println(requestBody);

    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("HTTP Response Code: " + String(httpResponseCode));
        Serial.println("Response: " + response);

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, response);
        String content = doc["choices"][0]["message"]["content"];
        Serial.println("Doubao Response:");
        Serial.println(content);
        answer = content;
    } else {
        Serial.println("Error on HTTP request");
        answer = "Error";
    }

    http.end();
    return answer;
}

String getMiniMaxAnswer(String inputText)
{
    HTTPClient http;
    http.setTimeout(10000);
    http.begin(minimaxUrl);
    http.addHeader("Content-Type", "application/json");
    String token_key = String("Bearer ") + minmaxapiKey;
    http.addHeader("Authorization", token_key);
    String payload = "{\"model\":\"abab5.5s-chat\",\"messages\":[{\"role\": \"system\",\"content\": \"你叫小欧,是我的的智能家居助手,要求下面的回答严格控制在256字符以内\"},{\"role\": \"user\",\"content\": \"" + inputText + "\"}]}";
    int httpResponseCode = http.POST(payload);
    if (httpResponseCode == 200) {
        String response = http.getString();
        http.end();
        Serial.println(response);
        DynamicJsonDocument jsonDoc(2048);
        deserializeJson(jsonDoc, response);
        String outputText = jsonDoc["choices"][0]["message"]["content"];
        return outputText;
        Serial.println(outputText);
    } else {
        http.end();
        Serial.printf("Error %i \n", httpResponseCode);
        return "Error";
    }
}

String getXunfeiAnswer(const String &inputText)
{
    HTTPClient http;
    http.setTimeout(20000);
    http.begin("https://spark-api-open.xf-yun.com/v1/chat/completions");
    String token_key = String("Bearer ") + "IXOdYaJFgIitgMpVZYIo:hmfWxQYOWwkqSTpLkHVp";
    http.addHeader("Authorization", token_key);
    http.addHeader("Content-Type", "application/json");
    String payload = "{\"model\":\"general\",\"messages\":[{\"role\":\"system\",\"content\":\"你是我的AI助手小欧,你必须用中文回答且字数不超过60个\"},{\"role\":\"user\",\"content\":\"" + inputText + "\"}]}";

    Serial.print("payload: ");
    Serial.println(payload);

    int httpResponseCode = http.POST(payload);
    if (httpResponseCode == 200) {
        String response = http.getString();
        http.end();
        Serial.println(response);

        DynamicJsonDocument jsonDoc(2048);
        deserializeJson(jsonDoc, response);
        String outputText = jsonDoc["choices"][0]["message"]["content"];
        Serial.println(outputText);
        return outputText;

    } else {
        http.end();
        Serial.printf("Error %i \n", httpResponseCode);
        return "error";
    }
}

String getDoubaoAnswer(const String &inputText)
{
    HTTPClient http;
    http.setTimeout(20000);
    http.begin("https://ark.cn-beijing.volces.com/api/v3/chat/completions");
    String token_key = String("Bearer ") + "b50ace91-bf6d-4628-9096-14a4d2a21b80";
    http.addHeader("Authorization", token_key);
    http.addHeader("Content-Type", "application/json");
    String payload = "{\"model\":\"ep-20240713-2fgvv\",\"messages\":[{\"role\":\"system\",\"content\":\"你是我的AI助手小欧,你必须用中文回答且字数不超过60个\"},{\"role\":\"user\",\"content\":\"" + inputText + "\"}],\"temperature\": 0.3}";

    Serial.print("payload: ");
    Serial.println(payload);

    int httpResponseCode = http.POST(payload);
    if (httpResponseCode == 200) {
        String response = http.getString();
        http.end();
        Serial.println(response);

        DynamicJsonDocument jsonDoc(2048);
        deserializeJson(jsonDoc, response);
        String outputText = jsonDoc["choices"][0]["message"]["content"];
        return outputText;
    } else {
        http.end();
        Serial.printf("Error %i \n", httpResponseCode);
        return "error";
    }
}

String getvAnswer(String ouputText)
{
    HTTPClient http2;
    http2.begin(MINIMAX_TTS);
    http2.addHeader("Content-Type", "application/json");
    http2.addHeader("Authorization", String("Bearer ") + MINIMAX_KEY);
    // 创建一个StaticJsonDocument对象，足够大以存储JSON数据
    StaticJsonDocument<200> doc;
    // 填充数据
    doc["text"] = ouputText;
    doc["model"] = "speech-01";
    doc["audio_sample_rate"] = 32000;
    doc["bitrate"] = 128000;
    doc["voice_id"] = "female-tianmei-jingpin";
    // 创建一个String对象来存储序列化后的JSON字符串
    String jsonString;
    // 序列化JSON到String对象
    serializeJson(doc, jsonString);
    int httpResponseCode = http2.POST(jsonString);
    if (httpResponseCode == 200) {
        DynamicJsonDocument jsonDoc(1024);
        String response = http2.getString();
        Serial.println(response);
        http2.end();
        deserializeJson(jsonDoc, response);
        String aduiourl = jsonDoc["audio_file"];
        return aduiourl;
    } else {
        Serial.printf("tts %i \n", httpResponseCode);
        http2.end();
        return "error";
    }
}

int speakPerid(int num)
{
    // 度小宇=1，度小美=0，度逍遥（基础）=3，度丫丫=4
    switch (num) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 3;
    case 3:
        return 4;
    default:
        return 0;
    }
}

String instructionRecognition(const String &command)
{
    Serial.printf("command: %s\n", command.c_str());
    String command_f = "";

    if (command.indexOf("打开1号灯") != -1) {
        Serial.println("打开1号灯");
        lv_ai_control("lampButton1", true);
        command_f = "1号灯已打开";
    } else if (command.indexOf("关闭1号灯") != -1) {
        Serial.println("关闭1号灯");
        lv_ai_control("lampButton1", false);
        command_f = "1号灯已关闭";
    } else if (command.indexOf("打开2号灯") != -1) {
        Serial.println("打开2号灯");
        lv_ai_control("lampButton2", true);
        command_f = "2号灯已打开";
    } else if (command.indexOf("关闭2号灯") != -1) {
        Serial.println("关闭2号灯");
        lv_ai_control("lampButton2", false);
        command_f = "2号灯已关闭";
    }

    return command_f;
}

void speakTask(void *pvParameter)
{
    Serial.println("start speakTask");
    while (1) {

        if (speakState == RECORDING) {
            stttext = "";
            Serial.println("Recording...");
            size_t bytes_read = 0;
            recordingSize = 0;
            pcm_data = reinterpret_cast<int16_t *>(ps_malloc(BUFFER_SIZE * 2));
            if (!pcm_data) {
                Serial.println("Failed to allocate memory for pcm_data");
            }

            int min = millis();
            while ((speakState != RECORDED)) {
                esp_err_t result = i2s_read(I2S_NMP411_PORT, audioData, sizeof(audioData), &bytes_read, portMAX_DELAY);
                memcpy(pcm_data + recordingSize, audioData, bytes_read);
                recordingSize += bytes_read / 2;
                if (!(millis() - min < 3000)) {
                    lv_speakState(SpeakState_t::RECORDED);
                }
            }

            Serial.printf("Recorded done. recordingSize: %d\n", recordingSize);
            sendSTTData();
            free(pcm_data);
        }

        if (webSocketClient_stt.available()) {
            webSocketClient_stt.poll();
        }

        // if (webSocketClient_tts.available()) {
        //     webSocketClient_tts.poll();
        // }

        if (speakState == ANSWERING) {
            Serial.println(stttext);
            delay(100);

            stttext.replace("\n", "");

            String answer = "";
            answer = instructionRecognition(stttext);
            if (answer != "") {
                audio.connecttospeech(answer.c_str(), "zh");
                lv_speakState(SpeakState_t::NO_DIALOGUE);
                continue;
            }

            answer = "";

            if (useAIMode) {
                Serial.println("doubao answer");
                answer = getDoubaoAnswer(stttext);
            } else {
                Serial.println("xunfei answer");
                answer = getXunfeiAnswer(stttext);
            }
            Serial.printf("stttext: %s  anwer : %s mdoe: %d\n", stttext.c_str(), answer.c_str(), useAIMode);

            lv_speakState(SpeakState_t::SPEAKING);

            if (!answer.isEmpty() && answer != "error") {
                audio.connecttospeech(answer.c_str(), "zh");
            } else {
                audio.connecttoFS(LittleFS, "/noResponse.mp3");
                Serial.println("回答内容为空,取消TTS发送");
                audio.connecttospeech("回答未响应", "zh");
            }

            lv_speakState(SpeakState_t::NO_DIALOGUE);
        }
        vTaskDelay(80);
    }
    vTaskDelete(NULL);
}

void startSpeakTask()
{
    if (speakTaskHandle == NULL) {
        xTaskCreatePinnedToCore(&speakTask, "speak_task", 10 * 1024, NULL, 12, &speakTaskHandle, 1);
    }
}

void stopSpeakTask()
{
    if (speakTaskHandle != NULL) {
        Serial.println("stop speakTask");
        vTaskDelete(speakTaskHandle);
        speakTaskHandle = NULL;
    }
}