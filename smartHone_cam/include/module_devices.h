#ifndef _MODULE_DEVICES_H_
#define _MODULE_DEVICES_H_

#define LED_PIN 33
#define PIR_PIN 13
#define LED_R_PIN 2
#define LED_G_PIN 14
#define LED_B_PIN 15
#define SOUND_PIN 12
#define BUZZER_PIN 16

void led_on();
void led_off();
void rgbled_red();
void rgbled_green();
void rgbled_blue();
void rgbled_off();
void rgbled_setColor(int r, int g, int b);

void buzzer_off();
void buzzer_on();

void device_init();

#endif