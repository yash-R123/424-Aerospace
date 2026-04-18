#include "SpaceObject.h"

SpaceObject::SpaceObject() : name("Unnamed Object"), mass(0.0) {}

SpaceObject::SpaceObject(const std::string& objectName, double objectMass)
    : name(objectName), mass(objectMass) {}

SpaceObject::~SpaceObject() {}

std::string SpaceObject::getName() const {
    return name;
}

double SpaceObject::getMass() const {
    return mass;
}

void SpaceObject::setName(const std::string& objectName) {
    name = objectName;
}

void SpaceObject::setMass(double objectMass) {
    mass = objectMass;
}
