#ifndef Barometer_h
#define Barometer_h

#include <SFE_BMP180.h>

class Barometer {
    public:
        Barometer();
        int begin();
        int getAltitude(double &alt);
    private:
        double _getPressure(double &P);
        SFE_BMP180 _pressure;
        double _baseline;
};

#endif
