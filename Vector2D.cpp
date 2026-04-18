#include "Vector2D.h"
#include <cmath>

Vector2D::Vector2D() : x(0.0), y(0.0) {}

Vector2D::Vector2D(double xValue, double yValue) : x(xValue), y(yValue) {}

double Vector2D::getX() const {
    return x;
}

double Vector2D::getY() const {
    return y;
}

void Vector2D::setX(double xValue) {
    x = xValue;
}

void Vector2D::setY(double yValue) {
    y = yValue;
}

double Vector2D::magnitude() const {
    return std::sqrt(x * x + y * y);
}

double Vector2D::magnitudeSquared() const {
    return x * x + y * y;
}

Vector2D Vector2D::normalized() const {
    double mag = magnitude();
    if (mag == 0.0) {
        return Vector2D(0.0, 0.0);
    }
    return Vector2D(x / mag, y / mag);
}

Vector2D Vector2D::operator+(const Vector2D& other) const {
    return Vector2D(x + other.x, y + other.y);
}

Vector2D Vector2D::operator-(const Vector2D& other) const {
    return Vector2D(x - other.x, y - other.y);
}

Vector2D Vector2D::operator*(double scalar) const {
    return Vector2D(x * scalar, y * scalar);
}

Vector2D Vector2D::operator/(double scalar) const {
    return Vector2D(x / scalar, y / scalar);
}

Vector2D& Vector2D::operator+=(const Vector2D& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}
