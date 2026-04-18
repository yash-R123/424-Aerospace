#ifndef SATELLITE_H
#define SATELLITE_H

#include "SpaceObject.h"
#include "Vector2D.h"

class Satellite : public SpaceObject {
private:
    Vector2D position;
    Vector2D velocity;
    double fuelMass;

public:
    Satellite();
    Satellite(const std::string& satelliteName,
              double satelliteMass,
              const Vector2D& initialPosition,
              const Vector2D& initialVelocity,
              double initialFuelMass);

    Vector2D getPosition() const;
    Vector2D getVelocity() const;
    double getFuelMass() const;

    void setPosition(const Vector2D& newPosition);
    void setVelocity(const Vector2D& newVelocity);
    void setFuelMass(double newFuelMass);
};

#endif
