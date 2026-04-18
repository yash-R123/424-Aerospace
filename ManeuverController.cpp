#include "ManeuverController.h"
#include "Vector2D.h"

void ManeuverController::applyProgradeBurn(Satellite& satellite, double deltaV) const {
    Vector2D velocity = satellite.getVelocity();
    Vector2D direction = velocity.normalized();
    satellite.setVelocity(velocity + direction * deltaV);
}

void ManeuverController::applyRetrogradeBurn(Satellite& satellite, double deltaV) const {
    Vector2D velocity = satellite.getVelocity();
    Vector2D direction = velocity.normalized();
    satellite.setVelocity(velocity - direction * deltaV);
}

void ManeuverController::applyRadialOutBurn(Satellite& satellite, double deltaV) const {
    Vector2D position = satellite.getPosition();
    Vector2D radialDirection = position.normalized();
    Vector2D velocity = satellite.getVelocity();
    satellite.setVelocity(velocity + radialDirection * deltaV);
}
