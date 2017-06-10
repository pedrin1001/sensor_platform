#ifndef EEPROMHelper_h
#define EEPROMHelper_h

#include <EEPROM.h>

#define byte uint8_t

class EEPROMHelper {
    public:
        EEPROMHelper();
        void writeInt(uint8_t address, int const &value);
        void writeUInt(uint8_t address, unsigned int const &value);
        void writeDouble(uint8_t address, double const &value);
        double readDouble(uint8_t address);
        int readInt(uint8_t address);
        unsigned int readUInt(uint8_t address);
};

#endif
