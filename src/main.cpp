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

double alt = 0.00;
float tmp = 0;

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

void serialize(char* entry) {
    sprintf(
        entry,
        "%i,%i,%i,%i,%0.2f,%0.2f,%0.2f",
        readMQ(MQ7PIN), readMQ(MQ2PIN), readMQ(MQ135PIN), (int)tmp,
        gps.location.lat(), gps.location.lng(), alt
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
        sprintf(str, "%d-%dh.csv", gps.date.day(), gps.time.hour());
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

// ISR(WDT_vect) {
// }

// delay while keep reading gps data
void smartDelay(const uint8_t &delay) {
    unsigned long initialTime = millis();
    unsigned long currentTime;
    do {
        if (gpsSerial.available() > 0) {
            gps.encode(gpsSerial.read());
        }
        currentTime = millis();
    } while (currentTime - initialTime < delay);
}

void setup() {
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);

    setLEDColor(242, 14, 48);

    // enable watchdog with 2 seconds timer
    wdt_enable(WDTO_2S);

    Serial.begin(9600);
    #ifdef DEBUG
    gpsSerial.begin(9600);
    #endif
    // Timer1.initialize(500000);
    // Timer1.attachInterrupt(callback);
    if(!sd.begin()) {
        error("sd could not begin!");
    }
    // if(!baro.begin()) {
    //     Serial.println("baro could not begin!");
    //     while(1);
    // }

    // setup went ok, blue color means good to go
    // red light is more intense than the others, so the voltage is cut down by three
    setLEDColor(13, 18, 229);
}

void loop() {
    #ifdef DEBUG
    if (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    #else
    if (Serial.available() > 0) {
        gps.encode(Serial.read());
    #endif
        if (firstEntry && gps.date.isValid() && gps.location.isValid()) {
            createFileName(fileName);
            if (sd.writeToFile(fileName, CSV_COLUMNS)) {
                #ifdef DEBUG
                Serial.print("new log file ");
                Serial.println(fileName);
                #endif
                firstEntry = false;
            } else {
                error("could not save new log file");
            }
        }
        if (!firstEntry && gps.location.isValid() && gps.location.isUpdated()) {
            // baro.getAltitude(alt);
            char entry [64];
            serialize(entry);
            #ifdef DEBUG
            Serial.println(entry);
            #endif
            if (sd.writeToFile(fileName, entry)) {
                setLEDColor(13, 18, 229);
            } else {
                error("could not write data to file");
            }
        } else {
            setLEDColor(242, 14, 48);
        }
    }

    smartDelay(255);
    // reset watchdog timer
    wdt_reset();
}
