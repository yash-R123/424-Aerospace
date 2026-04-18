#ifndef VECTOR2D_H
#define VECTOR2D_H

class Vector2D {
private:
    double x;
    double y;

public:
    Vector2D();
    Vector2D(double xValue, double yValue);

    double getX() const;
    double getY() const;

    void setX(double xValue);
    void setY(double yValue);

    double magnitude() const;
    double magnitudeSquared() const;
    Vector2D normalized() const;

    Vector2D operator+(const Vector2D& other) const;
    Vector2D operator-(const Vector2D& other) const;
    Vector2D operator*(double scalar) const;
    Vector2D operator/(double scalar) const;

    Vector2D& operator+=(const Vector2D& other);
    Vector2D& operator-=(const Vector2D& other);
};

#endif
