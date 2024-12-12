#include "module_devices.h"
#include "module_mqtt.h"
#include "module_server.h"
#include "module_wifi.h"

lampButtonData mqttSwitchState = {false, false};

extern bool enable_pri;
extern bool enable_VoiceControl;
extern bool activeEnableAutoLamp;

int rainState = 0, soundState = 1, pirState = 0;

// 门磁开启语音
bool enableOpenSound = false;
// 用于存储门磁传感器当前读取到的状态（HIGH表示门开，LOW表示门关）
int isDoorContact = HIGH;
// 记录门的当前状态，0表示关闭，1表示打开
int doorContactState = 0;
// 是否开启门磁超时未关闭
bool enableDoorContactTimeout = true;
// 记录上一次门的状态，用于对比判断状态是否改变
int lastDoorContactState = 0;
// 用于防抖处理，记录连续读取到相同状态的次数
int debounceCount = 0;
// 防抖阈值，连续多少次读取到相同状态才认为状态稳定，可根据实际情况调整
const int debounceThreshold = 5;
// 记录门打开的时间戳（以毫秒为单位）
unsigned long doorOpenTime = 0;
// 记录门关闭的时间戳（以毫秒为单位）
unsigned long doorCloseTime = 0;
// 门打开超时时间阈值（以秒为单位），超过此时间门没关闭视为超时未关闭，可按需调整
int doorOpenTimeoutThreshold = 30;

bool mqttVoice = false;
bool mqttPri = false;

// 风扇开关
bool enable_fan = false;

// 窗帘开关
bool enable_curtain = false;
// 控制窗帘的方向
bool curtain_direction = 1;
// 电机运行时长
int motorRunTime = 10;
// 电机参数
int stepSequence[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}};
// 记录当前的步数索引
int currentStep = 0;
// 定义电机转动的速度（单位：毫秒，即每步之间的延迟时间，数值越大速度越慢）
const int stepDelay = 1;
// 定义电机转动的总步数
const int totalSteps = 200;
unsigned long startTime = 0;
TaskHandle_t curtainForwardReverseTaskHandle;

/********************************************************************
                         LED
********************************************************************/

void rgbled_init()
{
    pinMode(AUTOLED_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(ONBOARDLAMPPIN, OUTPUT);
    led_off();
    autoled_off();
}

void led_on()
{
    digitalWrite(ONBOARDLAMPPIN, 1);
}
void led_off()
{
    digitalWrite(ONBOARDLAMPPIN, 0);
}

void blinkLED(int n, int t)
{
    for (int i = 0; i < 2 * n; i++) {
        digitalWrite(ONBOARDLAMPPIN, !digitalRead(ONBOARDLAMPPIN));
        delay(t);
    }
}

void autoled_on()
{
    digitalWrite(AUTOLED_PIN, 1);
}
void autoled_off()
{
    digitalWrite(AUTOLED_PIN, 0);
}

int autoled_state()
{
    return digitalRead(AUTOLED_PIN);
}

void led3w_on()
{
    digitalWrite(LED_PIN, 1);
}

void led3w_off()
{
    digitalWrite(LED_PIN, 0);
}

int led3w_state()
{
    return digitalRead(LED_PIN);
}

/********************************************************************
                         sg90
********************************************************************/

// void sg90_init()
// {
//     pinMode(SG90_PIN, OUTPUT);
//     ledcSetup(1, 50, 8);
//     ledcAttachPin(SG90_PIN, 1);
// }
// void sg90_setAngle(int angle)
// {
//     ledcWrite(1, angle);
// }

/********************************************************************
                         RAIN
********************************************************************/

void sensorTask(void *pt)
{
    unsigned long lastPrintTime = 0;
    unsigned long autoLampTime = 0, rainTime = 0, priLongTime = 0, priTime = 0, voiceTime = 0;
    bool rainFlag = 0, isRain = 0;

    while (1) {

        // 雨滴检测
        rainState = map(analogRead(RAIN_CHANNL), 0, 4095, 235, 0);
        if (rainState > 0) {
            if (!rainTime && !isRain) {
                rainTime = millis();
            }
        } else {
            rainTime = 0;
            isRain = false;
            if (rainFlag) {
                rainFlag = 0;
                pulishState("rainState", isRain);
            }
        }
        if (rainState && rainTime && millis() - rainTime >= RAINTIME) {
            isRain = true;
            rainTime = 0;
            rainFlag = 1;
            Serial.println("has rain");
            playAudio(AUDIO_NAME::RAIN);
            pulishState("rainState", isRain);
        }

        // 人体感应
        if (enable_pri) {
            pirState = analogRead(PIR_CHANNL);
            if (pirState > 0) {
                priTime = millis();
                if (millis() - priLongTime >= DETECTIONLONGTIME) {
                    playAudio(AUDIO_NAME::LT);
                }

                if (!mqttPri) {
                    mqttPri = true;
                    priLongTime = millis();
                    pulishState("priState", true);
                }
                Serial.println("has people");
            } else {
                if (mqttPri && millis() - priTime >= DETECTIONTIME) {
                    mqttPri = false;
                    pulishState("priState", false);
                }
                priLongTime = millis();
            }

        } else {
            if (mqttPri) {
                mqttPri = false;
                pulishState("priState", false);
            }
            pirState = 0;
        }

        // 声音检测
        if (enable_VoiceControl) {
            soundState = digitalRead(SOUND_PIN);
            if (!soundState) {
                if (!mqttVoice) {
                    mqttVoice = true;
                    voiceTime = millis();
                    pulishState("voiceState", true);
                }
                Serial.println("has sound");
            } else {
                if (mqttVoice && millis() - voiceTime >= DETECTIONTIME) {
                    mqttVoice = false;
                    pulishState("voiceState", false);
                }
            }
        } else {
            if (mqttVoice) {
                mqttVoice = false;
                pulishState("voiceState", false);
            }
            soundState = 1;
        }

        // 自动灯延迟打开和关闭
        if (pirState > 0 || !soundState || activeEnableAutoLamp) {
            autoLampTime = millis();
            if (!autoled_state()) {
                autoled_on();
            }
        } else {
            if (autoled_state() && millis() - autoLampTime >= AUTOLAMPTIME) {
                autoled_off();
            }
        }

        // 打印传感器数据
        if (millis() - lastPrintTime > 2000) {
            Serial.printf("rainState = %d  soundState = %d  pirState = %d  activeEnableAutoLamp = %d\n", rainState, soundState, pirState, activeEnableAutoLamp);
            lastPrintTime = millis();
        }

        // 门磁检测
        // 读取门磁传感器的当前状态
        isDoorContact = digitalRead(DOOR_CONTACT_PIN);
        // 防抖处理逻辑
        if (isDoorContact == lastDoorContactState) {
            debounceCount++;
            if (debounceCount >= debounceThreshold) {
                // 状态稳定，进行后续判断处理
                if (isDoorContact == HIGH) {
                    Serial.println("The door is open");
                    if (doorContactState == 0) {
                        doorContactState = 1;
                        doorOpenTime = millis(); // 记录门打开的时间
                        if (enableOpenSound) {
                            playAudio(AUDIO_NAME::DC1);
                        }
                        pulishState("doorcontact", true, "switches");
                    }
                } else {
                    Serial.println("The door is closed");
                    if (doorContactState == 1) {
                        doorContactState = 0;
                        pulishState("doorcontact", false, "switches");
                    }
                    doorOpenTime = millis();
                }
                // 判断门是否超时未关闭
                if (enableDoorContactTimeout) {
                    if (millis() - doorOpenTime >= doorOpenTimeoutThreshold * 1000 * 60 && doorContactState == 0) {
                        Serial.println("The door has been open for too long and is overdue!");
                        playAudio(AUDIO_NAME::DC2);
                    }
                }
                lastDoorContactState = isDoorContact;
                debounceCount = 0; // 重置防抖计数
            }
        } else {
            debounceCount = 0;
            lastDoorContactState = isDoorContact;
        }

        // 开关1
        if (digitalRead(BUTTON_PIN1)) {
            delay(30);
            if (digitalRead(BUTTON_PIN1)) {
                Serial.println("BUTTON_PIN1");
                mqttSwitchState.lampButton1 = !led3w_state();
                mqttSwitchState.lampButton1 ? led3w_on() : led3w_off();
                pulishState("lampButton1", mqttSwitchState.lampButton1, "switches");
            }
        }

        // 开关2
        if (digitalRead(BUTTON_PIN2)) {
            delay(30);
            if (digitalRead(BUTTON_PIN2)) {
                Serial.println("BUTTON_PIN2");
                mqttSwitchState.lampButton2 = !autoled_state();
                mqttSwitchState.lampButton2 ? autoled_on() : autoled_off();
                pulishState("lampButton2", mqttSwitchState.lampButton2, "switches");
            }
        }
        if (enable_curtain) {
            if (curtainForwardReverseTaskHandle == NULL) {
                xTaskCreate(curtainForwardReverseTasks, "curtainForwardTask", 4096, NULL, 5, &curtainForwardReverseTaskHandle);
            }
        }

        vTaskDelay(100);
    }
    vTaskDelete(NULL);
}

// 窗帘正转任务函数
void curtainForwardReverseTasks(void *pvParameters)
{
    unsigned long currentRunTime = 0, elapsedTime = 0, newCurrentRunTime = 0;
    bool currentState = curtain_direction, hasInterrupt = false;
    while (1) {
        if (currentState) {
            currentStep++;
            if (currentStep >= totalSteps) {
                currentStep = 0;
            }
            setStep(stepSequence[currentStep % 8][0],
                    stepSequence[currentStep % 8][1],
                    stepSequence[currentStep % 8][2],
                    stepSequence[currentStep % 8][3]);
        } else {
            currentStep--;
            if (currentStep < 0) {
                currentStep = totalSteps - 1;
            }
            setStep(stepSequence[currentStep % 8][0],
                    stepSequence[currentStep % 8][1],
                    stepSequence[currentStep % 8][2],
                    stepSequence[currentStep % 8][3]);
        }

        if (currentRunTime == 0) {
            currentRunTime = millis();
        }
        if (millis() - currentRunTime >= motorRunTime * 1000) {
            enable_curtain = 0;
            // vTaskDelete(curtainForwardReverseTaskHandle);
            break;
        }

        // 处理方向的变化
        if (curtain_direction != currentState) {
            currentState = curtain_direction;
            elapsedTime = millis() - currentRunTime;
            newCurrentRunTime = millis();
            hasInterrupt = true;
        }

        if (hasInterrupt) {
            if (millis() - newCurrentRunTime >= elapsedTime) {
                enable_curtain = 0;
                // vTaskDelete(curtainForwardReverseTaskHandle);
                break;
            }
        }
        vTaskDelay(3);
    }
    curtainForwardReverseTaskHandle = NULL;
    vTaskDelete(NULL);
}

void sensor_init()
{
    pinMode(SOUND_PIN, INPUT);

    pinMode(BUTTON_PIN1, INPUT);
    pinMode(BUTTON_PIN2, INPUT);
}

void startSensorTask()
{
    xTaskCreatePinnedToCore(sensorTask, "sensor_task", 1024 * 5, NULL, 2, NULL, 0);
}

/********************************************************************
                         electrical machinery devices
********************************************************************/

void electrical_machinery_init()
{
    // 风扇
    pinMode(FAN_PINA, OUTPUT);
    fan_off();
    // 门磁
    pinMode(DOOR_CONTACT_PIN, INPUT_PULLUP);
    // 窗帘电机
    pinMode(CURTAIN_PIN1, OUTPUT);
    pinMode(CURTAIN_PIN2, OUTPUT);
    pinMode(CURTAIN_PIN3, OUTPUT);
    pinMode(CURTAIN_PIN4, OUTPUT);
}

void fan_on()
{
    digitalWrite(FAN_PINA, HIGH);
}
void fan_off()
{
    digitalWrite(FAN_PINA, LOW);
}

void setStep(int pin1, int pin2, int pin3, int pin4)
{
    digitalWrite(CURTAIN_PIN1, pin1);
    digitalWrite(CURTAIN_PIN2, pin2);
    digitalWrite(CURTAIN_PIN3, pin3);
    digitalWrite(CURTAIN_PIN4, pin4);
}

/********************************************************************
                         initDevices
********************************************************************/
void initDevices()
{
    rgbled_init();
    sensor_init();
    audio_init();
    electrical_machinery_init();
}

void initDevicesDatas()
{
    int priData = ReadintData("pri");
    if (priData != 1000) {
        enable_pri = priData;
    }

    int voiceControlData = ReadintData("voiceControl");
    if (voiceControlData != 1000) {
        enable_VoiceControl = voiceControlData;
    }

    int enableOpenSoundData = ReadintData("dcos");
    if (enableOpenSoundData != 1000) {
        enableOpenSound = enableOpenSoundData;
    }

    int enableDoorContactTimeoutData = ReadintData("dco");
    if (enableDoorContactTimeoutData != 1000) {
        enableDoorContactTimeout = enableDoorContactTimeoutData;
    }
}