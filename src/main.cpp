#include <Arduino.h>
#include <avr/wdt.h>
#include <TinyGPS++.h>
#include <idDHT11.h>
#include <SDCard.h>
#include <Utils.h>
#include <PinMap.h>

#ifdef DEBUG
#include <MemoryFree.h>
#include <SoftwareSerial.h>
SoftwareSerial gpsSerial(8, 9);
#endif

SDCard sd(SD_PIN);
TinyGPSPlus gps;
Utils utils;

bool firstEntry = true;
char fileName [11];
int ledState = LOW;

/**
 * initialize dht as on library documentation
 * @see https://github.com/niesteszeck/idDHT11/blob/master/examples/idDHT11_Lib_example/idDHT11_Lib_example.ino
 * @see https://www.arduino.cc/en/Reference/AttachInterrupt
 */
void dht11_wrapper();
idDHT11 dht(DHT_PIN, 1, dht11_wrapper);
void dht11_wrapper() {
    dht.isrCallback();
}

/**
 * wrap all data together to be appended to csv log file
 */
void serialize(char* entry) {
    while(dht.acquiring());
    int result = dht.getStatus();
    if (result != IDDHTLIB_OK) {
        #ifdef DEBUG
        dht.printError(result);
        #endif
    } else {
        // save new values
        dht.saveLastValidData();
    }
    sprintf(
        entry,
        "%i,%i,%i,%i,%0.6f,%0.6f,%0.2f,%lu",
        utils.readMQ(MQ2_PIN), utils.readMQ(MQ135_PIN),
        (int)dht.getLastValidCelsius(), (int)dht.getLastValidHumidity(),
        gps.location.lat(), gps.location.lng(), gps.altitude.meters(),
        gps.time.value()
    );
}

void createFileName(char *str) {
    // limited to 8.3 file format https://en.wikipedia.org/wiki/8.3_filename
    // so .csv was removed
    sprintf(str, "%d-%d-%d", gps.date.day(), gps.time.hour(), gps.time.minute());
}

/**
 * delay while keep reading gps data
 */
void smartDelay(const unsigned long &delay) {
    unsigned long initialTime = millis();
    unsigned long currentTime;
    do {
        #ifdef DEBUG
        if (gpsSerial.available() > 0) {
            gps.encode(gpsSerial.read());
        #else
        if (Serial.available() > 0) {
            gps.encode(Serial.read());
        #endif
        }
        currentTime = millis();
    } while (currentTime - initialTime < delay);
}

void setup() {

    Serial.begin(9600);
    #ifdef DEBUG
    Serial.println(F("setup: initializing sensor platform..."));
    gpsSerial.begin(9600);
    #endif

    // setup control led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // enable watchdog with 2 seconds timer
    wdt_enable(WDTO_2S);

    if(!sd.begin()) {
        utils.error("setup: sd could not begin!");
    }

    // start acquiring first value, result is interrupt driven
    // the dht sensor is having issues since the soldering on the board,
    // this is a tmeporary work around to get valid values
    if (dht.acquireAndWait() != IDDHTLIB_OK) {
        utils.error("setup: dht could not acquire proper data!");
    } else {
        dht.saveLastValidData();
    }

    #ifdef DEBUG
    Serial.println(F("setup: initialization went okay!"));
    #endif

}

void loop() {
    smartDelay(255);
    if (firstEntry && gps.date.isValid() && gps.location.isValid()) {
        // if valid state is entered, led is on
        digitalWrite(LED_PIN, HIGH);
        createFileName(fileName);
        if (sd.exists(fileName)) {
            #ifdef DEBUG
            Serial.print(F("appending to file "));
            Serial.println(fileName);
            #endif
            firstEntry = false;
        } else {
            #ifdef DEBUG
            Serial.print(F("new log file "));
            Serial.println(fileName);
            #endif
            firstEntry = false;
        }
    }
    if (!firstEntry && gps.location.isValid()) {
        // if valid state is entered, led is on
        digitalWrite(LED_PIN, HIGH);
        // get new dht value
        dht.acquire();
        char entry [40];
        serialize(entry);
        #ifdef DEBUG
        Serial.println(entry);
        #endif
        if (!sd.writeToFile(fileName, entry)) {
            utils.error("could not write data to file");
        }
    } else {
        // blink for invalid gps data
        if (ledState == LOW) {
            ledState = HIGH;
        } else {
            ledState = LOW;
        }
        digitalWrite(LED_PIN, ledState);
    }
    // reset watchdog timer
    wdt_reset();
}
