#include "ThermalEquilibrium.h"

ThermalEquilibirium::ThermalEquilibirium(double m, double c, double temp) {
    mass = m;
    specificHeat = c;
    temperature = temp;
}

double ThermalEquilibirium::ThermalEquilibirium() const {
    return temperature;
}

void ThermalEquilibirium::setTemperature(double temp) {
    temperature = temp;
}

void ThermalEquilibirium::exchangeHeat(ThermalSystem& other) {
    double finalTemp =
        (mass * specificHeat * temperature +
         other.mass * other.specificHeat * other.temperature)
        /
        (mass * specificHeat + other.mass * other.specificHeat);

    temperature = finalTemp;
    other.temperature = finalTemp;
}
