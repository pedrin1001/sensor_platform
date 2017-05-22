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

void SDCard::writeToFile(char const* fileName, char const* data) {
    // create or append if file already exists
    File file = SD.open(fileName, FILE_WRITE);
    if (file) {
        file.println(data);
        file.close();
    }
}

void SDCard::readFromFile(char const* fileName) {
    // if the file is available, read from it
    if (fileName) {
        File file = SD.open(fileName, FILE_READ);
        while (file && file.available()) {
            Serial.write(file.read());
        }
        file.close();
    }
}
