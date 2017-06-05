#include <Arduino.h>
#include <avr/wdt.h>
#include <TimerOne.h>
#include "lib/SDCard.h"
#include "PinMap.h"
// #include "lib/Barometer.h"
#include <TinyGPS++.h>

#ifdef DEBUG
#include <MemoryFree.h>
#include <SoftwareSerial.h>
SoftwareSerial gpsSerial(9, 10);
#endif

#define CSV_COLUMNS "mq7,mq2,mq135,tmp,lat,lng,alt"


SDCard sd(SD_PIN);
// Barometer baro;
TinyGPSPlus gps;

double lat = 0.00;
double lng = 0.00;
double alt = 0.00;
uint32_t date = 0;
uint32_t time = 0;

float tmp = 0;

int mq7 = 0;
int mq2 = 0;
int mq135 = 0;

bool firstEntry = true;
char fileName [32];

int readMQ(int pin) {
    int value = analogRead(pin);
    if (isnan(value)) {
        return -1;
    } else {
        return value;
    }
}

void serialize(char* fmt) {
    sprintf(
        fmt,
        "%i,%i,%i,%i,%0.2f,%0.2f,%0.2f",
        mq7, mq2, mq135, (int)tmp, lat, lng, alt
    );
}

void callback() {
    // mq7 = readMQ(MQ7PIN);
    // mq2 = readMQ(MQ2PIN);
    // mq135 = readMQ(MQ135PIN);
    // hid = dht.computeHeatIndex(tmp, hum, false);
    //
    // char fmt [64];
    // serialize(fmt);
    // if (Serial.available()) {
    //     Serial.println(fmt);
    // }
}

void createFileName(char *str) {
    sprintf(str, "log_%lu_%lu.csv", date, time);
}

uint16_t rgbToVoltage(uint8_t value) {
    return map(value, 0, 255, 0, 1023);
}

#ifdef DEBUG
void error(char const* msg) {
    analogWrite(LED_RED, rgbToVoltage(242));
    analogWrite(LED_GREEN, rgbToVoltage(14));
    analogWrite(LED_BLUE, rgbToVoltage(48));
    Serial.println(msg);
    // watchdog will reset arduino
    while(1);
}
#else
void error() {
    analogWrite(LED_RED, rgbToVoltage(242));
    analogWrite(LED_GREEN, rgbToVoltage(14));
    analogWrite(LED_BLUE, rgbToVoltage(48));
    // watchdog will reset arduino
    while(1);
}
#endif

// ISR(WDT_vect) {
// }

void setup() {
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);

    analogWrite(LED_RED, rgbToVoltage(242));
    analogWrite(LED_GREEN, rgbToVoltage(14));
    analogWrite(LED_BLUE, rgbToVoltage(48));

    Serial.begin(9600);
    #ifdef DEBUG
    gpsSerial.begin(9600);
    #endif
    // Timer1.initialize(500000);
    // Timer1.attachInterrupt(callback);
    if(!sd.begin()) {
        Serial.println("sd could not begin!");
        while(1);
    }
    // if(!baro.begin()) {
    //     Serial.println("baro could not begin!");
    //     while(1);
    // }

    // setup went ok, blue color means good to go
    // red light is more intense than the others, so the voltage is cut down by three
    analogWrite(LED_RED, rgbToVoltage(39)/3);
    analogWrite(LED_GREEN, rgbToVoltage(18));
    analogWrite(LED_BLUE, rgbToVoltage(229));
    // Serial.print("freeMemory(): ");
    // Serial.println(freeMemory());
    // if(!(sd.readFromFile("log_50617_15534200.csv"))) {
    //     Serial.println("could not read file");
    // }
    // enable watchdog with 2 seconds timer
    wdt_enable(WDTO_2S);
}

void loop() {
    // Serial.println(freeMemory());
    // send data only when you receive data:
    #ifdef DEBUG
    if (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
        // Serial.println(freeMemory());
        if (firstEntry && gps.date.isValid() && gps.location.isValid()) {
            date = gps.date.value();
            time = gps.time.value();
            createFileName(fileName);
            if (sd.writeToFile(fileName, CSV_COLUMNS)) {
                Serial.print("new log file ");
                Serial.println(fileName);
                firstEntry = false;
            } else {
                error("could not save new log file");
            }
        }
        if (gps.date.isValid() && gps.date.isUpdated()) {
            date = gps.date.value();
            time = gps.time.value();
        }
        if (gps.location.isValid() && gps.location.isUpdated()) {
            lat = gps.location.lat();
            lng = gps.location.lng();
            // baro.getAltitude(alt);
            char entry [64];
            serialize(entry);
            // Serial.println(entry);
            if (sd.writeToFile(fileName, entry)) {
                analogWrite(LED_RED, rgbToVoltage(39)/3);
                analogWrite(LED_GREEN, rgbToVoltage(18));
                analogWrite(LED_BLUE, rgbToVoltage(229));
            } else {
                error("could not write data to file");
            }
        } else {
            analogWrite(LED_RED, rgbToVoltage(242));
            analogWrite(LED_GREEN, rgbToVoltage(14));
            analogWrite(LED_BLUE, rgbToVoltage(48));
        }
    }

    // Serial.print("freeMemory(): ");
    // Serial.println(freeMemory());
    #else
    if (Serial.available() > 0) {
        gps.encode(Serial.read());
        if (firstEntry && gps.date.isValid()) {
            date = gps.date.value();
            time = gps.time.value();
            createFileName(fileName);
            if (sd.writeToFile(fileName, CSV_COLUMNS)) {
                firstEntry = false;
            } else {
                error();
            }
        }
        if (gps.date.isValid() && gps.date.isUpdated()) {
            date = gps.date.value();
            time = gps.time.value();
        }
        if (gps.location.isValid() && gps.location.isUpdated()) {
            lat = gps.location.lat();
            lng = gps.location.lng();
            // baro.getAltitude(alt);
            char entry [64];
            serialize(entry);
            if (sd.writeToFile(fileName, entry)) {
                analogWrite(LED_RED, rgbToVoltage(39)/3);
                analogWrite(LED_GREEN, rgbToVoltage(18));
                analogWrite(LED_BLUE, rgbToVoltage(229));
            } else {
                error();
            }
        } else {
            analogWrite(LED_RED, rgbToVoltage(242));
            analogWrite(LED_GREEN, rgbToVoltage(14));
            analogWrite(LED_BLUE, rgbToVoltage(48));
        }
    }

    #endif
    // reset watchdog timer
    wdt_reset();
}
