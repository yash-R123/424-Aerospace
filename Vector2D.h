#ifndef VECTOR2D_H
#define VECTOR2D_H

// A simple 2D vector class used for position, velocity, and acceleration.
class Vector2D {
private:
    double x; // x-component of the vector
    double y; // y-component of the vector

public:
    // Default constructor sets vector to (0,0)
    Vector2D();

    // Constructor that sets vector to (xValue, yValue)
    Vector2D(double xValue, double yValue);

    // Gets values
    double getX() const;
    double getY() const;

    // Sets Values
    void setX(double xValue);
    void setY(double yValue);

    // Returns the vector length: sqrt(x^2 + y^2)
    double magnitude() const;

    // Returns x^2 + y^2
    double magnitudeSquared() const;

    // Returns a unit vector (same direction)
    Vector2D normalized() const;

    // Vector math operators
    Vector2D operator+(const Vector2D& other) const;
    Vector2D operator-(const Vector2D& other) const;
    Vector2D operator*(double scalar) const;
    Vector2D operator/(double scalar) const;

    // Compound assignment operators
    Vector2D& operator+=(const Vector2D& other);
    Vector2D& operator-=(const Vector2D& other);
};

#endif
