/**
 * The circuit:
 * SD card attached to SPI bus as follows:
 * - MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 * - MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 * - CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 * - CS - depends on your SD card shield or module.
 * @see https://www.arduino.cc/en/Tutorial/CardInfo
 * @see https://www.amazon.de/gp/product/B06XHJTGGC/
 */

#include "SDCard.h"

SDCard::SDCard(int chipSelect) {
    _cs = chipSelect;
}

int SDCard::begin() {
    if (!SD.begin(_cs)) {
        return 0;
    }
    return 1;
}

bool SDCard::exists(char* fileName) {
    return SD.exists(fileName);
}

int SDCard::writeToFile(char* fileName, char const* data, bool ow /*= false*/) {
    if (ow == true) {
        #ifdef DEBUG
        Serial.print(F("overwriting "));
        Serial.println(fileName);
        #endif
        SD.remove(fileName);
    }
    // create or append if file already exists
    File file = SD.open(fileName, FILE_WRITE);
    if (file) {
        file.println(data);
        file.close();
        return 1;
    } else {
        return 0;
    }
}

int SDCard::readFromFile(char const* fileName) {
    // if the file is available, read from it
    if (fileName) {
        File file = SD.open(fileName, FILE_READ);
        if (file) {
            while (file.available()) {
                Serial.write(file.read());
            }
            file.close();
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}
