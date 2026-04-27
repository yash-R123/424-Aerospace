#include "ThermalEquilibrium.h"

ThermalSystem::ThermalSystem(double m, double c, double temp) {
    mass = m;
    specificHeat = c;
    temperature = temp;
}

double ThermalSystem::getTemperature() const {
    return temperature;
}

void ThermalSystem::setTemperature(double temp) {
    temperature = temp;
}

void ThermalSystem::exchangeHeat(ThermalSystem& other) {
    double finalTemp =
        (mass * specificHeat * temperature +
         other.mass * other.specificHeat * other.temperature)
        /
        (mass * specificHeat + other.mass * other.specificHeat);

    temperature = finalTemp;
    other.temperature = finalTemp;
}
