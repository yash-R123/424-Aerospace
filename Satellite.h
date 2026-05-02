#pragma once
#include "vec.h"
#include "thermalmodel.h"
#include <string>
#include <algorithm>  // for std::clamp

class Satellite {
public:
    Satellite(const std::string& name, vec pos, vec vel, double powerGen, double batCap, double powerUsage)
        : name(name), pos(pos), vel(vel), power(0.0), temp(273.0), PowerGen(powerGen), BatCap(batCap), PowerUsage(powerUsage), thermal(100.0, 4.0, 0.3, 0.8, 900.0) {}

    std::string  name;
    vec          pos;
    vec          vel;
    double       power;
    double       temp;
    double       PowerGen;
    double       BatCap;
    double       PowerUsage;
    ThermalModel thermal;
    bool         inEclipse = false;

    void setThermal(const ThermalModel& t) { thermal = t; }

    void updatePower(double dt) {
        inEclipse  = thermal.inEclipse(pos[0], pos[1], pos[2]);
        double gen = inEclipse ? 0.0 : PowerGen;
        power     += (gen - PowerUsage) * dt *1/3600;
        if (power < 0.0)    power = 0.0;    
        if (power > BatCap) power = BatCap;
    }

    void updateTemp(double dt) {
        temp = thermal.update(temp, dt,
            pos[0], pos[1], pos[2],
            PowerUsage);
    }
};
