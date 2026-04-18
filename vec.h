#pragma once

class vec {
public:
    vec(double a, double b, double c) : x(a), y(b), z(c) {}

    double operator[](int i) const {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

  
    double dot(const vec& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    vec cross(const vec& v) const {
        return vec(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

private:
    double x, y, z;
};
