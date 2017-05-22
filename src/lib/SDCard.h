#ifndef SDCard_h
#define SDCard_h

#include <SD.h>

class SDCard {
    public:
        SDCard(int pin);
        int begin();
        void writeToFile(char const*, char const*);
        void readFromFile(char const*);
    private:
        int _cs;
};

#endif
