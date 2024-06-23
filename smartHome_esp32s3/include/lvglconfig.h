#ifndef _LVGLCONFIG_H_
#define _LVGLCONFIG_H_
#include <arduino.h>

void initLVGLConfig(void);
void startLVGLTask(void);

void lv_setMQTTState(const char *text);
void lv_setMQTTSwitchState(bool state);
void lv_setWIFIState(const char *text);
void lv_setWIFISwitchState(bool state);
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

void ui_calender_update(void);
void startCameraTask();
bool wifiConnect();

#endif