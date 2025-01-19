#ifndef _LVGLCONFIG_H_
#define _LVGLCONFIG_H_
#include <arduino.h>

typedef enum {
    NO_DIALOGUE,
    WAKEUP,
    RECORDING,
    RECORDED,
    ANSWERING,
    SPEAKING
} SpeakState_t;

void initLVGLConfig(void);
void startLVGLTask(void);
void lv_closeWifi();
void lv_setMQTTState(const char *text);
void lv_setMQTTSwitchState(bool state);
bool lv_getMQTTSwitchState();
void lv_setWIFIState(const char *text);
void lv_setWIFISwitchState(bool state);
void lv_setBLESwitchState(bool state);
void lv_setWeatherinfo(const char *text);
void lv_setCityinfo(const char *text);
void lv_setWeatherImage(int number);
void lv_setTipinfo(const char *text);
void lv_gohome(void);
void lv_setPlayState(bool state);
void lv_setDropdown(int index);
void lv_setMusicinfo(const char *text);
void lv_setDropdowninfo(const char *options);
void lv_setDropdownaddinfo(const char *option, int pos);
void lv_setSliderVolume(int value);
void lv_setCameraImage(const void *path);
void lv_setSpeechinfo(const char *text);
void lv_setIPinfo(const char *text);
void lv_setstatusbarLabel(int status);
void msgboxTip(const char *text);
void lv_setPriState(bool state);
void lv_setPriAlarmState(bool state);
void lv_setVoiceState(bool state);
void lv_setRainState(bool state);
void lv_setUser(const String &user);
void lv_setLampButton1(bool state);
void lv_setLampButton2(bool state);
void lv_setButtonFan(bool state);
void lv_setButtonWindow(bool state);
void lv_setButtonCurtain(bool state);
void lv_setButtonDoorContact(bool state);
void lv_setButtonDoorContactOpenSound(bool state);
void lv_setButtonDoorContactTimeout(bool state);
void lv_setDropdownDoorContactTimeoutTime(int time);
void lv_setCurtainRunTime(int runTime);
void lv_setPriButtonState(bool state);
void lv_setVoiceButtonState(bool state);
void lv_speakState(const SpeakState_t &state);
void lv_ai_control(const String &handl, bool state);
void lv_ai_control_offline(const String &handl, int state);
void lv_updataOATbar(int value);
void msgboxBarTip();

void ui_calender_update(void);
void startCameraTask();
bool wifiConnect();

void perinit();

#endif