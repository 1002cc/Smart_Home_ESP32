#include "module_devices.h"

#include "Arduino.h"

void led_on()
{
    digitalWrite(LED_PIN, 1);
}
void led_off()
{
    digitalWrite(LED_PIN, 0);
}

void rgbled_init()
{
    pinMode(LED_PIN, OUTPUT);
    pinMode(LED_R_PIN, OUTPUT);
    pinMode(LED_G_PIN, OUTPUT);
    pinMode(LED_B_PIN, OUTPUT);
}

void setColor(int r, int g, int b)
{
    analogWrite(LED_R_PIN, r);
    analogWrite(LED_G_PIN, g);
    analogWrite(LED_B_PIN, b);
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

void pir_init()
{
    pinMode(PIR_PIN, INPUT);
}

void buzzer_off()
{
    digitalWrite(BUZZER_PIN, LOW);
}

void buzzer_on()
{
    digitalWrite(BUZZER_PIN, HIGH);
}

void device_init()
{
    rgbled_init();
    pir_init();
    pinMode(SOUND_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    rgbled_off();
    buzzer_off();
    led_off();
}