#include <DHT.h>
#include <TimerOne.h>

#define MQ7PIN A0
#define MQ2PIN A2
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float hum = 0;
float tmp = 0;
float hid = 0;

int mq7 = 0;
int mq2 = 0;

char fmt [64];

int readMQ7() {
    int value;
    value = analogRead(MQ7PIN);
    if (isnan(value)) {
        return -1;
    } else {
        return value;
    }
}

int readMQ2() {
    int value;
    value = analogRead(MQ2PIN);
    if (isnan(value)) {
        return -1;
    } else {
        return value;
    }
}

void serialize(char* fmt) {
    char str_hum [6]; char str_tmp [6]; char str_hid [6];
    dtostrf(hum, 4, 2, str_hum);
    dtostrf(tmp, 4, 2, str_tmp);
    dtostrf(hid, 4, 2, str_hid);
    sprintf(fmt, "mq7: %i, mq2: %i, hum: %s, tmp: %s, hid: %s;", mq7, mq2, str_hum, str_tmp, str_hid);
}

void callback() {
    mq7 = readMQ7();
    mq2 = readMQ2();
    hid = dht.computeHeatIndex(tmp, hum, false);

    serialize(fmt);
    Serial.println(fmt);
}

void setup() {
    Serial.begin(9600);
    dht.begin();
    Timer1.initialize(1000000);
    Timer1.attachInterrupt(callback);
}

void loop() {
    hum = dht.readHumidity();
    tmp = dht.readTemperature();
    if (isnan(tmp) || isnan(hum)) {
        /* Serial.println("Failed to read from DHT"); */
    }
}
