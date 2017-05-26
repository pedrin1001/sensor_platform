#include "Arduino.h"
#include <avr/wdt.h>
#include <DHT.h>
#include <TimerOne.h>
#include "lib/SDCard.h"
#include "PinMap.h"

DHT dht(DHTPIN, DHTTYPE);
SDCard sd(SD_PIN);

float hum = 0;
float tmp = 0;
float hid = 0;

int mq7 = 0;
int mq2 = 0;
int mq135 = 0;

int readMQ(int pin) {
    int value = analogRead(pin);
    if (isnan(value)) {
        return -1;
    } else {
        return value;
    }
}

void serialize(char* fmt) {
    sprintf(fmt, "mq7:%i,mq2:%i,mq135:%i,hum:%i,tmp:%i,hid:%i;", mq7, mq2, mq135, (int)hum, (int)tmp, (int)(hid+0.5));
}

void callback() {
    mq7 = readMQ(MQ7PIN);
    mq2 = readMQ(MQ2PIN);
    mq135 = readMQ(MQ135PIN);
    hid = dht.computeHeatIndex(tmp, hum, false);

    char fmt [64];
    serialize(fmt);
    if (Serial.available()) {
        Serial.println(fmt);
    }
}

long rgbToVoltage(long value) {
    return map(value, 0, 255, 0, 1023);
}

void setup() {
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);

    analogWrite(LED_RED, rgbToVoltage(242));
    analogWrite(LED_GREEN, rgbToVoltage(14));
    analogWrite(LED_BLUE, rgbToVoltage(48));
    // test();
    Serial.begin(9600);
    // dht.begin();
    // Timer1.initialize(1000000);
    // Timer1.attachInterrupt(callback);
    // if(!sd.begin()) {Serial.println("why");}
    delay(1000);
    // setup went ok, blue color means good to go
    // red light is more intense than the others, so the voltage is cut down by three
    analogWrite(LED_RED, rgbToVoltage(39)/3);
    analogWrite(LED_GREEN, rgbToVoltage(18));
    analogWrite(LED_BLUE, rgbToVoltage(229));

    // enable watchdog with 2 seconds timer
    wdt_enable(WDTO_2S);
}

void loop() {
    // sd.readFromFile("log.txt");
    // hum = dht.readHumidity();
    // tmp = dht.readTemperature();
    // if (isnan(tmp) || isnan(hum)) {
        /* Serial.println("Failed to read from DHT"); */
    // }

    // reset watchdog timer
    wdt_reset();
}
