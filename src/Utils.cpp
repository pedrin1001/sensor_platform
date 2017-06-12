#include <Arduino.h>
#include "PinMap.h"

int readMQ(int pin) {
    int value = analogRead(pin);
    if (isnan(value)) {
        return -1;
    } else {
        return value;
    }
}

uint16_t rgbToVoltage(const uint8_t &value) {
    return map(value, 0, 255, 0, 1023);
}

void setLEDColor(const uint8_t &r, const uint8_t &g, const uint8_t &b) {
    analogWrite(LED_RED, rgbToVoltage(r));
    analogWrite(LED_GREEN, rgbToVoltage(g));
    analogWrite(LED_BLUE, rgbToVoltage(b));
}

void error(char const* msg) {
    setLEDColor(242, 14, 48);
    #ifdef DEBUG
    Serial.println(msg);
    #endif
    // watchdog will reset arduino
    while(1);
}
