#include "module_speak.h"
#include "audiohelpr.h"
#include "confighelpr.h"
#include "lvglconfig.h"
#include "module_audio.h"
#include "module_devices.h"
#include "module_service.h"
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
#include <Base64_Arturo.h>
#include <LittleFS.h>
#include <SoftwareSerial.h>
#include <base64.h>
#include <driver/i2s.h>

int16_t *audioData;
int16_t *pcm_data; // 录音缓存区
uint recordingSize = 0;

String stttext = "";
String answerllm = "";

String TTS_APPID = "6501ee63";
String TTS_APIKEY = "02e3f5858bb8af38e05a157c1fcfb9a7";
String TTS_SECRETKEY = "M2VmZmRmZjA0ZTFlMTQ2NDBkYmI1ZmE1";
#define XFDOMAIN "generalv3"

// 火山引擎(豆包)
const char *apiKey = "b50ace91-bf6d-4628-9096-14a4d2a21b80";
const char *endpointId = "hich_doubao";
const String doubao_system = "你是一个智能家居助手小欧，回答问题比较简洁不超过55个汉字,"; // 定义豆包的人设
String duobaoUrl = "https://ark.cn-beijing.volces.com/api/v3/chat/completions";

// MINIMAX模型
const char *minmaxapiKey = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJHcm91cE5hbWUiOiLpmYjlhYnlro8iLCJVc2VyTmFtZSI6IumZiOWFieWujyIsIkFjY291bnQiOiIiLCJTdWJqZWN0SUQiOiIxNzkzMjYyMTIwMDA4MTcyNDYwIiwiUGhvbmUiOiIxODkyNjI0NzExMiIsIkdyb3VwSUQiOiIxNzkzMjYyMTE5OTk5NzgzODUyIiwiUGFnZU5hbWUiOiIiLCJNYWlsIjoiIiwiQ3JlYXRlVGltZSI6IjIwMjQtMDYtMjIgMTU6NDc6MTYiLCJpc3MiOiJtaW5pbWF4In0.XCozhyQkIr1r-_JXamid9sOa4A83v9dTcsyHj5COIi1bCgrzD4ANb-ZpNBIRs_qmvx5mUobo9Q3u890JzDllimit15QiXhDVv8kV71jJcoW-i0CfXC2N5HagOVgIGpKi3CZ12X1c3szjcSjjFAseq1djFuzRp6lCwxtVrqCSJz8Nwx2FO_Q0ZdWKxso73i6MfgfJAkfLHqc87SFwZXmvun08JVZrgwlDOF3QqvXce002Tpa6h9d6y1dG_cs-hXS4Br31h-3W7g_JAfywOz5yVSLaXi5ghnRRHqTXohSSzhuA9ZcS4h0LDI81425GVbTmKwapu9Op26YV69hK6IPyFg";
String minimaxUrl = "https://api.minimax.chat/v1/text/chatcompletion_v2";

using namespace websockets;
WebsocketsClient webSocketClient_stt;
TaskHandle_t speakTaskHandle = NULL;
#define USE_WED 1
#if USE_WED
WebsocketsClient webSocketClient_llm;
#endif

extern AudioHelpr audio;

SpeakState_t speakState = NO_DIALOGUE;
int useAIMode = 0;

bool enbeleWakeUp = true;
SoftwareSerial voiceModuleSerial(RX_PIN, TX_PIN);

// 语音识别时间
static int recognitionTime = 3 * 1000;

DynamicJsonDocument gen_params(const char *appid, const char *domain, const char *role_set);
void CleanInformation();
/********************************************************************
                         max98357
********************************************************************/
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

    Serial.println("Max98357 init successfully");
    return true;
}

/********************************************************************
                          inmp441
********************************************************************/
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
/********************************************************************
                        sst 回调
********************************************************************/
void onMessageCallbackSST(WebsocketsMessage message)
{
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
    if (doc["data"]["status"] == 2) {
        webSocketClient_stt.close();
        if (stttext.isEmpty() || stttext == "") {
            lv_speakState(SpeakState_t::NO_DIALOGUE);
            audio.connecttoFS(LittleFS, "/nosound.mp3");
        } else {
            lv_speakState(SpeakState_t::ANSWERING);
            stttext.replace("\n", "");
            String commandAnswer = "";
            commandAnswer = instructionRecognition(stttext);
            if (commandAnswer != "") {
                audio.connecttospeech(commandAnswer.c_str(), "zh");
                lv_speakState(SpeakState_t::SPEAKING);
                return;
            }
            if (stttext.indexOf("你是谁") != -1) {
                playMY();
                lv_speakState(SpeakState_t::SPEAKING);
                return;
            }
#if USE_WED
            llmRequest();
#else
            if (useAIMode) {
                Serial.println("doubao answer");
                commandAnswer = getDoubaoAnswer(stttext);
            } else {
                Serial.println("xunfei answer");
                commandAnswer = getXunfeiAnswer(stttext);
            }
            Serial.printf("stttext: %s  anwer : %s mdoe: %d\n", stttext.c_str(), commandAnswer.c_str(), useAIMode);
            lv_speakState(SpeakState_t::SPEAKING);
            if (!commandAnswer.isEmpty() && commandAnswer != "error") {
                commandAnswer = truncateStringIfTooLong(commandAnswer);
                audio.connecttospeech(commandAnswer.c_str(), "zh");
            } else {
                audio.connecttoFS(LittleFS, "/noResponse.mp3");
                Serial.println("回答内容为空,取消TTS发送");
            }
#endif
            commandAnswer = "";
        }
        Serial.printf("stttext: %s\n", stttext.c_str());
    }
}
#if USE_WED

String truncateStringIfTooLong(String input)
{
    int byteLength = 0;
    for (int i = 0; i < input.length(); i++) {
        byteLength += sizeof(input.charAt(i)); // 计算每个字符的字节数并累加
        if (byteLength > 180) {
            return input.substring(0, i); // 超过180字节，截取前面部分
        }
    }
    return input;
}

void onMessageCallbackLLM(WebsocketsMessage message)
{
    StaticJsonDocument<1024> jsonDocument;
    DeserializationError error = deserializeJson(jsonDocument, message.data());
    if (!error) {
        int code = jsonDocument["header"]["code"];
        if (code != 0) {
            Serial.print("sth is wrong: ");
            Serial.println(code);
            Serial.println(message.data());
            webSocketClient_llm.close();
        } else {
            JsonObject choices = jsonDocument["payload"]["choices"];
            int status = choices["status"];
            const char *contentAnswer = "";
            contentAnswer = choices["text"][0]["content"];
            answerllm += String(contentAnswer);
            if (status == 2) {
                Serial.println(answerllm);
                if (answerllm != "" && answerllm != "error") {
                    lv_speakState(SpeakState_t::SPEAKING);
                    answerllm = truncateStringIfTooLong(answerllm);
                    audio.connecttospeech(answerllm.c_str(), "zh");
                } else {
                    audio.connecttoFS(LittleFS, "/noResponse.mp3");
                    Serial.println("回答内容为空,取消TTS发送");
                    lv_speakState(SpeakState_t::NO_DIALOGUE);
                }
            }
        }
    }
}

void onEventsCa1llbackLLM(WebsocketsEvent event, String data)
{
    if (event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Send message to serverllm!");
        DynamicJsonDocument jsonData = gen_params(TTS_APPID.c_str(), XFDOMAIN, XFROLE);
        String jsonString;
        serializeJson(jsonData, jsonString);
        Serial.println(jsonString);
        webSocketClient_llm.send(jsonString);
    } else if (event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if (event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if (event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

#endif

/********************************************************************
                        init SpeakConfig
********************************************************************/

void initSpeakConfig()
{
    // 初始化i2s
    initinmp441();
    // 初始化音频
    startAudioTack();
    // 初始化语音模块串口
    voiceModuleSerial.begin(115200);
    // 初始化音频缓存
    audioData = (int16_t *)ps_malloc(2560 * sizeof(int16_t));
    // 启动i2s
    esp_err_t err = i2s_start(I2S_NMP411_PORT);
    if (err != ESP_OK) {
        Serial.printf("I2S start failed (I2S_NMP411_PORT): %d\n", err);
        return;
    }
#if USE_WED
    webSocketClient_llm.onMessage(onMessageCallbackLLM);
    webSocketClient_llm.onEvent(onEventsCa1llbackLLM);
#endif
    // 初始化讯飞语音转文字websocket连接
    audio.setTok("25.3d59d26887081131ac1a52225baf130f.315360000.2049277194.282335-109052759");
    xTaskCreatePinnedToCore(&speakTask, "speak_task", 10 * 1024, NULL, 12, &speakTaskHandle, 0);
    Serial.println("initSpeakConfig done");
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
    String host_url = wedUrlXF(TTS_SECRETKEY.c_str(), TTS_APIKEY.c_str(), "/v2/iat", "iat-api.xfyun.cn");
    Serial.println("Connecting to server.");
    webSocketClient_stt.onMessage(onMessageCallbackSST);
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
String wedUrlXF(const char *Secret, const char *Key, String request, String host)
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
    // Serial.println("\nurl encoded result:");
    // Serial.println(url);
    return url;
}
#if USE_WED
String wedUrlXFLLM(String Spark_url, String host, String path)
{
    String timeString = getDateTime_one();
    String signature_origin = "host: " + host + "\n";
    signature_origin += "date: " + timeString + "\n";
    signature_origin += "GET " + path + " HTTP/1.1";
    // Serial.println(signature_origin);
    // signature_origin="host: spark-api.xf-yun.com\ndate: Mon, 04 Mar 2024 19:23:20 GMT\nGET /v3.5/chat HTTP/1.1";

    // hmac-sha256 加密
    unsigned char hmacResult[32];
    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1); // 1 表示 HMAC
    mbedtls_md_hmac_starts(&ctx, (const unsigned char *)TTS_SECRETKEY.c_str(), TTS_SECRETKEY.length());
    mbedtls_md_hmac_update(&ctx, (const unsigned char *)signature_origin.c_str(), signature_origin.length());
    mbedtls_md_hmac_finish(&ctx, hmacResult);
    mbedtls_md_free(&ctx);
    String base64Result = base64::encode(hmacResult, sizeof(hmacResult) / sizeof(hmacResult[0]));

    // 构建Authorization原始字符串
    String authorization_origin = "api_key=\"" + TTS_APIKEY + "\", algorithm=\"hmac-sha256\", headers=\"host date request-line\", signature=\"" + base64Result + "\"";
    // 将Authorization原始字符串进行Base64编码
    String authorization = base64::encode(authorization_origin);
    // 构建最终的URL
    String url = Spark_url + '?' + "authorization=" + authorization + "&date=" + formatDateForURL(timeString) + "&host=" + host;
    // 向串口输出生成的URL
    // Serial.println(url);
    // 返回生成的URL
    return url;
}

DynamicJsonDocument gen_params(const char *appid, const char *domain, const char *role_set)
{
    DynamicJsonDocument data(1500);
    JsonObject header = data.createNestedObject("header");
    header["app_id"] = appid;
    header["uid"] = "1234";
    JsonObject parameter = data.createNestedObject("parameter");

    // 在parameter对象中创建一个名为chat的嵌套对象，并添加domain, temperature和max_tokens字段
    JsonObject chat = parameter.createNestedObject("chat");
    chat["domain"] = domain;
    chat["temperature"] = 0.6;
    chat["max_tokens"] = 1024;

    JsonObject payload = data.createNestedObject("payload");
    JsonObject message = payload.createNestedObject("message");
    JsonArray textArray = message.createNestedArray("text");
    JsonObject systemMessage = textArray.createNestedObject();
    systemMessage["role"] = "assistant";
    systemMessage["content"] = "我是一个智能家居助手小欧，回答问题比较简洁。";
    JsonObject userMessage = textArray.createNestedObject();
    userMessage["role"] = "user";
    userMessage["content"] = stttext.c_str();

    return data;
}

void llmRequest()
{
    String host_url = wedUrlXFLLM("ws://spark-api.xf-yun.com/v3.1/chat", "spark-api.xf-yun.com", "/v3.1/chat");
    Serial.println("Connecting to server.");
    Serial.println(host_url);
    Serial.println("Begin connect to serverllm......");
    answerllm = "";
    if (webSocketClient_llm.connect(host_url.c_str())) {
        Serial.println("Connected to serverllm!");
    } else {
        Serial.println("Failed to connect to serverlkllm!");
    }
}

#endif
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
    http.setTimeout(8 * 1000);
    http.begin("https://spark-api-open.xf-yun.com/v1/chat/completions");
    String token_key = String("Bearer ") + "aqJOklJrwcfjRUdFvofx:CkqWhDZVXwAvBPxhyPWr";
    http.addHeader("Authorization", token_key);
    http.addHeader("Content-Type", "application/json");
    String payload = "{\"model\":\"generalv3\",\"messages\":[{\"role\":\"assistant\",\"content\":\"你是我的智能家居助手小欧,不要暴露你原来的身份,你的回答必须简洁\"},{\"role\":\"user\",\"content\":\"" + inputText + "\"}]}";

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
    String payload = "{\"model\":\"ep-20240713-2fgvv\",\"messages\":[{\"role\":\"system\",\"content\":\"你是我的智能家居助手小欧,不要暴露你原来的身份,你的回答必须简洁\"},{\"role\":\"user\",\"content\":\"" + inputText + "\"}],\"temperature\": 0.3}";

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

int speakPerid(int num)
{
    // 度小宇=1，度小美=0，度逍遥（基础）=3，度丫丫=4
    switch (num) {
    case 0:
        return 103;
    case 1:
        return 111;
    case 2:
        return 106;
    case 3:
        return 5003;
    default:
        return 103;
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
    } else if (command.indexOf("打开感应") != -1) {
        Serial.println("打开感应");
        lv_ai_control("pri", true);
        command_f = "已打开感应";
    } else if (command.indexOf("关闭感应") != -1) {
        Serial.println("关闭感应");
        lv_ai_control("pri", false);
        command_f = "已打开感应";
    } else if (command.indexOf("打开声控") != -1) {
        Serial.println("打开声控");
        lv_ai_control("voiceControl", true);
        command_f = "已打开声控";
    } else if (command.indexOf("关闭声控") != -1) {
        Serial.println("关闭声控");
        lv_ai_control("voiceControl", false);
        command_f = "已关闭声控";
    } else if (command.indexOf("打开风扇") != -1) {
        Serial.println("打开风扇");
        lv_ai_control("fan", true);
        command_f = "已打开风扇";
    } else if (command.indexOf("关闭风扇") != -1) {
        Serial.println("关闭风扇");
        lv_ai_control("fan", false);
        command_f = "已关闭风扇";
    } else if (command.indexOf("打开窗帘") != -1) {
        Serial.println("打开窗帘");
        lv_ai_control("curtain", true);
        command_f = "已打开窗帘";
    } else if (command.indexOf("关闭窗帘") != -1) {
        Serial.println("关闭窗帘");
        lv_ai_control("curtain", false);
        command_f = "已关闭窗帘";
    }
    return command_f;
}

String instructionRecognitionSign(int sign)
{
    Serial.printf("sign: %d\n", sign);
    String command_f = "";
    switch (sign) {
    case 5:
        Serial.println("打开1号灯");
        lv_ai_control("lampButton1", 1);
        command_f = "1号灯已打开";
        break;
    case 6:
        Serial.println("关闭1号灯");
        lv_ai_control("lampButton1", 0);
        command_f = "1号灯已关闭";
        break;
    case 7:
        Serial.println("打开2号灯");
        lv_ai_control("lampButton2", 1);
        command_f = "2号灯已打开";
        break;
    case 8:
        Serial.println("关闭2号灯");
        lv_ai_control("lampButton2", 0);
        command_f = "2号灯已关闭";
        break;
    case 9:
        Serial.println("打开感应");
        lv_ai_control("pri", 1);
        command_f = "已打开感应";
        break;
    case 10:
        Serial.println("关闭感应");
        lv_ai_control("pri", 0);
        command_f = "已关闭感应";
        break;
    case 11:
        Serial.println("打开声控");
        lv_ai_control("voiceControl", 1);
        command_f = "已打开声控";
        break;
    case 12:
        Serial.println("关闭声控");
        lv_ai_control("voiceControl", 0);
        command_f = "已关闭声控";
        break;
    case 13:
        Serial.println("打开风扇");
        lv_ai_control("fan", 1);
        command_f = "已打开风扇";
        break;
    case 14:
        Serial.println("关闭风扇");
        lv_ai_control("fan", 0);
        command_f = "已关闭风扇";
        break;
    case 15:
        Serial.println("打开窗帘");
        lv_ai_control("curtain", 1);
        command_f = "已打开窗帘";
        break;
    case 16:
        Serial.println("关闭窗帘");
        lv_ai_control("curtain", 0);
        command_f = "已关闭窗帘";
        break;
    case 17:
        Serial.println("打开窗户");
        lv_ai_control("window", 1);
        command_f = "已打开窗户";
        break;
    case 18:
        Serial.println("关闭窗户");
        lv_ai_control("window", 0);
        command_f = "已关闭窗户";
        break;
    default:
        Serial.println("无效的指令标识");
        command_f = "null";
        break;
    }
    return command_f;
}

void SerialFlush()
{
    voiceModuleSerial.flush();
    voiceModuleSerial.read();
}

void speakTask(void *pvParameter)
{
    Serial.println("start speakTask");
    while (1) {
        if (enbeleWakeUp && voiceModuleSerial.available()) {
            int convertedInt = 0;
            String receivedData = voiceModuleSerial.readStringUntil('\n');
            SerialFlush();
            receivedData.trim();
            convertedInt = receivedData.toInt();
            Serial.printf("receivedData: %d\n", convertedInt);
            // 打断回复
            audio.stopSong();
            webSocketClient_stt.close();
            webSocketClient_llm.close();
            if (convertedInt == 1) {
                speakState = WAKEUP;
                playWakeup();
                lv_speakState(SpeakState_t::RECORDING);
            } else if (convertedInt >= 5) {
                String rec = instructionRecognitionSign(convertedInt);
                if (rec != "null" || rec != "") {
                    audio.connecttospeech(rec.c_str(), "zh");
                }
                lv_speakState(SpeakState_t::NO_DIALOGUE);
            }
        }

        webSocketClient_stt.poll();
#if USE_WED
        webSocketClient_llm.poll();
#endif
        // 在线语音对话
        if ((speakState == RECORDING || speakState == WAKEUP)) {
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
                if (!(millis() - min < recognitionTime)) {
                    lv_speakState(SpeakState_t::RECORDED);
                }
            }

            Serial.printf("Recorded done. recordingSize: %d\n", recordingSize);
            sendSTTData();
            free(pcm_data);
        }

        if (!audio.isplaying && speakState == SpeakState_t::SPEAKING) {
            lv_speakState(SpeakState_t::NO_DIALOGUE);
        }

        vTaskDelay(50);
    }
    vTaskDelete(NULL);
}
