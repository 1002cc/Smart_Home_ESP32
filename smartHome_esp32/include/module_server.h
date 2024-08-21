#ifndef _MODULE_SERVCE_H_
#define _MODULE_SERVCE_H_
#include <Arduino.h>

// MAX98357
#define PIN_I2S_MAX98357_BCLK 22
#define PIN_I2S_MAX98357_LRC 23
#define PIN_I2S_MAX98357_DOUT 21

#define MINIMAX_KEY "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJHcm91cE5hbWUiOiLpmYjlhYnlro8iLCJVc2VyTmFtZSI6IumZiOWFieWujyIsIkFjY291bnQiOiIiLCJTdWJqZWN0SUQiOiIxNzkzMjYyMTIwMDA4MTcyNDYwIiwiUGhvbmUiOiIxODkyNjI0NzExMiIsIkdyb3VwSUQiOiIxNzkzMjYyMTE5OTk5NzgzODUyIiwiUGFnZU5hbWUiOiIiLCJNYWlsIjoiIiwiQ3JlYXRlVGltZSI6IjIwMjQtMDYtMjIgMTU6NDc6MTYiLCJpc3MiOiJtaW5pbWF4In0.XCozhyQkIr1r-_JXamid9sOa4A83v9dTcsyHj5COIi1bCgrzD4ANb-ZpNBIRs_qmvx5mUobo9Q3u890JzDllimit15QiXhDVv8kV71jJcoW-i0CfXC2N5HagOVgIGpKi3CZ12X1c3szjcSjjFAseq1djFuzRp6lCwxtVrqCSJz8Nwx2FO_Q0ZdWKxso73i6MfgfJAkfLHqc87SFwZXmvun08JVZrgwlDOF3QqvXce002Tpa6h9d6y1dG_cs-hXS4Br31h-3W7g_JAfywOz5yVSLaXi5ghnRRHqTXohSSzhuA9ZcS4h0LDI81425GVbTmKwapu9Op26YV69hK6IPyFg"
#define MINIMAX_TTS "https://api.minimax.chat/v1/t2a_pro?GroupId=1793262119999783852"

void printLocalTime(void);
void timeAvailable(struct timeval *t);
void initNtpTime();

void StoreData(const char *key, const char *val);
String ReadData(const char *val);

void printPSRAM(void);

void littlefs_init();

void audio_init();
void startAudioTack();
void audioPause();
void audioSpeak(const String &text);

#endif