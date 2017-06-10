#include "EEPROMHelper.h"

EEPROMHelper::EEPROMHelper() {
}

void EEPROMHelper::writeUInt(uint8_t address, unsigned int const &value) {
    byte* p = (byte*)(void*)&value;
    for (uint8_t i = 0; i < sizeof(value); i++) {
        EEPROM.update(address++, *p++);
    }
}

void EEPROMHelper::writeInt(uint8_t address, int const &value) {
    byte* p = (byte*)(void*)&value;
    for (uint8_t i = 0; i < sizeof(value); i++) {
        EEPROM.update(address++, *p++);
    }
}

void EEPROMHelper::writeDouble(uint8_t address, double const &value) {
    byte* p = (byte*)(void*)&value;
    for (uint8_t i = 0; i < sizeof(value); i++) {
        EEPROM.update(address++, *p++);
    }
}

double EEPROMHelper::readDouble(uint8_t address) {
    // Serial.println(address);
    double value = 0.0;
    byte* p = (byte*)(void*)&value;
    for (uint8_t i = 0; i < sizeof(value); i++) {
        *p++ = EEPROM.read(address++);
    }
    return value;
}

int EEPROMHelper::readInt(uint8_t address) {
    int value = 0;
    byte* p = (byte*)(void*)&value;
    for (uint8_t i = 0; i < sizeof(value); i++) {
        *p++ = EEPROM.read(address++);
    }
    return value;
}

unsigned int EEPROMHelper::readUInt(uint8_t address) {
    unsigned int value = 0;
    byte* p = (byte*)(void*)&value;
    for (uint8_t i = 0; i < sizeof(value); i++) {
        *p++ = EEPROM.read(address++);
    }
    return value;
}
