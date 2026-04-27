#pragma once

class ThermalEquilibirum {
private:
    double mass;
    double specificHeat;
    double temperature;

public:
    ThermalEquilibirum(double m, double c, double temp);

    double getTemperature() const;
    void setTemperature(double temp);

    void exchangeHeat(ThermalSystem& other);
};
