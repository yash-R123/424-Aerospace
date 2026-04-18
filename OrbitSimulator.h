#ifndef ORBITSIMULATOR_H
#define ORBITSIMULATOR_H

#include "Satellite.h"
#include "OrbitState.h"
#include "Vector2D.h"

class OrbitSimulator {
private:
    double gravitationalParameter;
    double planetRadius;
    double timeStep;
    double currentTime;

public:
    OrbitSimulator(double mu, double radius, double dt);

    Vector2D computeAcceleration(const Satellite& satellite) const;
    void updateSatellite(Satellite& satellite);
    OrbitState getState(const Satellite& satellite) const;

    double getTimeStep() const;
    double getCurrentTime() const;
};

#endif
