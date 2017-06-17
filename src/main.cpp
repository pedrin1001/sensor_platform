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
SoftwareSerial gpsSerial(9, 10);
#endif

#define CSV_COLUMNS "mq7,mq2,mq135,tmp,hum,lat,lng,alt"

SDCard sd(SD_PIN);
TinyGPSPlus gps;
Utils utils;

bool firstEntry = true;
char fileName [11];

/**
 * initialize dht as on library documentation
 * @see https://github.com/niesteszeck/idDHT11/blob/master/examples/idDHT11_Lib_example/idDHT11_Lib_example.ino
 * @see https://www.arduino.cc/en/Reference/AttachInterrupt
 */
void dht11_wrapper();
idDHT11 dht(DHTPIN, 0, dht11_wrapper);
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
        // try again using "synchronous" way
        if (dht.acquireAndWait() != IDDHTLIB_OK) {
            utils.error("dht could not acquire proper data");
        }
    }
    sprintf(
        entry,
        "%i,%i,%i,%i,%i,%0.2f,%0.2f,%0.2f",
        utils.readMQ(MQ7PIN), utils.readMQ(MQ2PIN), utils.readMQ(MQ135PIN), (int)dht.getCelsius(), (int)dht.getHumidity(),
        gps.location.lat(), gps.location.lng(), gps.altitude.meters()
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
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);

    utils.setLEDColor(242, 14, 48);

    // enable watchdog with 2 seconds timer
    wdt_enable(WDTO_2S);

    Serial.begin(9600);
    #ifdef DEBUG
    gpsSerial.begin(9600);
    #endif
    if(!sd.begin()) {
        utils.error("sd could not begin!");
    }
    // start acquiring first value, result is interrupt driven
    dht.acquire();
    // setup went ok, green color means waiting for valid gps data
    utils.setLEDColor(200, 252, 2);
}

void loop() {
    smartDelay(255);
    if (firstEntry && gps.date.isValid() && gps.location.isValid()) {
        createFileName(fileName);
        if (sd.exists(fileName)) {
            #ifdef DEBUG
            Serial.print(F("appending to file "));
            Serial.println(fileName);
            #endif
            firstEntry = false;
            utils.setLEDColor(13, 18, 229);
        } else {
            if (sd.writeToFile(fileName, CSV_COLUMNS)) {
                #ifdef DEBUG
                Serial.print(F("new log file "));
                Serial.println(fileName);
                #endif
                firstEntry = false;
                utils.setLEDColor(13, 18, 229);
            } else {
                utils.error("could not save new log file");
            }
        }
    }
    if (!firstEntry && gps.location.isValid()) {
        if (gps.location.isUpdated()) {
            // get new dht value
            dht.acquire();
            char entry [40];
            serialize(entry);
            #ifdef DEBUG
            Serial.println(entry);
            #endif
            if (sd.writeToFile(fileName, entry)) {
                utils.setLEDColor(13, 18, 229);
            } else {
                utils.error("could not write data to file");
            }
        }
    } else {
        // green for invalid gps data
        utils.setLEDColor(200, 252, 2);
    }
    // reset watchdog timer
    wdt_reset();
}
