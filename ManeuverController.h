#ifndef MANEUVERCONTROLLER_H
#define MANEUVERCONTROLLER_H

#include "Satellite.h"

class ManeuverController {
public:
    void applyProgradeBurn(Satellite& satellite, double deltaV) const;
    void applyRetrogradeBurn(Satellite& satellite, double deltaV) const;
    void applyRadialOutBurn(Satellite& satellite, double deltaV) const;
};

#endif
