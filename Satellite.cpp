#include "Satellite.h"

Satellite::Satellite()
    : SpaceObject("Unnamed Satellite", 100.0),
      position(0.0, 0.0),
      velocity(0.0, 0.0),
      fuelMass(0.0) {}

Satellite::Satellite(const std::string& satelliteName,
                     double satelliteMass,
                     const Vector2D& initialPosition,
                     const Vector2D& initialVelocity,
                     double initialFuelMass)
    : SpaceObject(satelliteName, satelliteMass),
      position(initialPosition),
      velocity(initialVelocity),
      fuelMass(initialFuelMass) {}

Vector2D Satellite::getPosition() const {
    return position;
}

Vector2D Satellite::getVelocity() const {
    return velocity;
}

double Satellite::getFuelMass() const {
    return fuelMass;
}

void Satellite::setPosition(const Vector2D& newPosition) {
    position = newPosition;
}

void Satellite::setVelocity(const Vector2D& newVelocity) {
    velocity = newVelocity;
}

void Satellite::setFuelMass(double newFuelMass) {
    fuelMass = newFuelMass;
}
