#pragma once
#include <cmath>

class ThermalModel {
public:
    double mass;
    double surfaceArea;
    double absorptivity;
    double emissivity;
    double heatCapacity;

    static constexpr double SOLAR_FLUX    = 1361.0;
    static constexpr double STEFAN_BOLTZ  = 5.67e-8;
    static constexpr double EARTH_RADIUS  = 6371000.0;
    static constexpr double EARTH_IR_FLUX = 237.0;

    ThermalModel(double mass, double surfaceArea,
        double absorptivity, double emissivity,
        double heatCapacity)
        : mass(mass), surfaceArea(surfaceArea),
        absorptivity(absorptivity), emissivity(emissivity),
        heatCapacity(heatCapacity) {}

    bool inEclipse(double px, double py, double pz) const {
        double distFromAxis = sqrt(py*py + pz*pz);
        return (px < 0) && (distFromAxis < EARTH_RADIUS);
    }

    double update(double temp, double dt,
        double px, double py, double pz,
        double internalPowerW) const {
        bool   eclipse = inEclipse(px, py, pz);
        double solarIn = eclipse ? 0.0 : absorptivity * surfaceArea * SOLAR_FLUX * 0.25;
        double earthIR = absorptivity * surfaceArea * EARTH_IR_FLUX * 0.25;
        double Qin     = solarIn + earthIR + internalPowerW;
        double Qout    = emissivity * STEFAN_BOLTZ * surfaceArea * pow(temp, 4);
        double dT      = (Qin - Qout) / (mass * heatCapacity);
        double newTemp = temp + dT * dt;

        // clamp to physical range (2.7K = deep space, 500K = upper limit)
        if (newTemp < 2.7)  newTemp = 2.7;
        if (newTemp > 500.0) newTemp = 500.0;
        return newTemp;
    }
};
