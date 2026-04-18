#include "OrbitSimulator.h"
#include <cmath>

OrbitSimulator::OrbitSimulator(double mu, double radius, double dt)
    : gravitationalParameter(mu),
      planetRadius(radius),
      timeStep(dt),
      currentTime(0.0) {}

Vector2D OrbitSimulator::computeAcceleration(const Satellite& satellite) const {
    Vector2D position = satellite.getPosition();
    double r = position.magnitude();

    if (r == 0.0) {
        return Vector2D(0.0, 0.0);
    }

    double factor = -gravitationalParameter / (r * r * r);
    return position * factor;
}

void OrbitSimulator::updateSatellite(Satellite& satellite) {
    Vector2D acceleration = computeAcceleration(satellite);

    Vector2D velocity = satellite.getVelocity();
    velocity += acceleration * timeStep;

    Vector2D position = satellite.getPosition();
    position += velocity * timeStep;

    satellite.setVelocity(velocity);
    satellite.setPosition(position);

    currentTime += timeStep;
}

OrbitState OrbitSimulator::getState(const Satellite& satellite) const {
    Vector2D position = satellite.getPosition();
    Vector2D velocity = satellite.getVelocity();

    OrbitState state;
    state.time = currentTime;
    state.x = position.getX();
    state.y = position.getY();
    state.vx = velocity.getX();
    state.vy = velocity.getY();
    state.radius = position.magnitude();
    state.altitude = state.radius - planetRadius;
    state.speed = velocity.magnitude();

    return state;
}

double OrbitSimulator::getTimeStep() const {
    return timeStep;
}

double OrbitSimulator::getCurrentTime() const {
    return currentTime;
}
