#include <iostream>
#include <iomanip>
#include "Vector2D.h"
#include "Satellite.h"
#include "OrbitSimulator.h"
#include "ManeuverController.h"

using namespace std;

int main() {
    const double earthRadius = 6371000.0;
    const double earthMu = 3.986004418e14;
    const double altitude = 400000.0;
    const double orbitRadius = earthRadius + altitude;
    const double circularSpeed = 7670.0;
    const double timeStep = 1.0;

    Vector2D initialPosition(orbitRadius, 0.0);
    Vector2D initialVelocity(0.0, circularSpeed);

    Satellite satellite("DemoSat", 500.0, initialPosition, initialVelocity, 20.0);
    OrbitSimulator simulator(earthMu, earthRadius, timeStep);
    ManeuverController controller;

    cout << fixed << setprecision(2);

    for (int i = 0; i < 120; ++i) {
        if (i == 30) {
            controller.applyProgradeBurn(satellite, 50.0);
        }

        simulator.updateSatellite(satellite);
        OrbitState state = simulator.getState(satellite);

        cout << "t = " << setw(6) << state.time
             << " s | x = " << setw(12) << state.x
             << " | y = " << setw(12) << state.y
             << " | speed = " << setw(10) << state.speed
             << " | altitude = " << setw(10) << state.altitude
             << endl;
    }

    return 0;
}
