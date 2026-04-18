#include <iostream>
#include <iomanip>
#include "Vector2D.h"
#include "Satellite.h"
#include "OrbitSimulator.h"
#include "ManeuverController.h"

using namespace std;

int main() {
    // Earth constants
    const double earthRadius = 6371000.0;      // meters
    const double earthMu = 3.986004418e14;     // m^3/s^2

    // Start the satellite 400 km above Earth's surface
    const double altitude = 400000.0;          
    const double orbitRadius = earthRadius + altitude;

    // Approximate circular orbit speed
    const double circularSpeed = 7670.0;       

    // Simulation timestep
    const double timeStep = 1.0;               

    // Initial position: satellite starts on x-axis
    Vector2D initialPosition(orbitRadius, 0.0);

    // Initial velocity: perpendicular to radius vector for orbit
    Vector2D initialVelocity(0.0, circularSpeed);

    // Create the satellite object
    Satellite satellite("DemoSat", 500.0, initialPosition, initialVelocity, 20.0);

    // Create the simulator
    OrbitSimulator simulator(earthMu, earthRadius, timeStep);

    // Create maneuver controller
    ManeuverController controller;

    // Make output cleaner
    cout << fixed << setprecision(2);

    // Run simulation for 120 seconds
    for (int i = 0; i < 120; ++i) {
        // Apply a burn at t = 30 s
        if (i == 30) {
            controller.applyProgradeBurn(satellite, 50.0);
        }

        // Advance orbit one step
        simulator.updateSatellite(satellite);

        // Get current state
        OrbitState state = simulator.getState(satellite);

        // Print useful information
        cout << "t = " << setw(6) << state.time
             << " s | x = " << setw(12) << state.x
             << " | y = " << setw(12) << state.y
             << " | speed = " << setw(10) << state.speed
             << " | altitude = " << setw(10) << state.altitude
             << endl;
    }

    return 0;
}
