#pragma once
#include <ostream>

class vec {
public:
    vec(double a, double b, double c) : x(a), y(b), z(c) {}

    double operator[](int i) const {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    vec operator+(const vec& o) const { return vec(x+o.x, y+o.y, z+o.z); }
    vec operator-(const vec& o) const { return vec(x-o.x, y-o.y, z-o.z); }
    vec operator-()             const { return vec(-x, -y, -z); }
    vec operator*(double s)     const { return vec(x*s, y*s, z*s); }
    vec operator/(double s)     const { return vec(x/s, y/s, z/s); }

    vec& operator+=(const vec& o) { x+=o.x; y+=o.y; z+=o.z; return *this; }
    vec& operator-=(const vec& o) { x-=o.x; y-=o.y; z-=o.z; return *this; }
    vec& operator*=(double s)     { x*=s;   y*=s;   z*=s;   return *this; }
    vec& operator/=(double s)     { x/=s;   y/=s;   z/=s;   return *this; }

    double dot(const vec& o)   const;
    vec    cross(const vec& o) const;
    double length()            const;
    vec    normalized()        const;

    friend std::ostream& operator<<(std::ostream& os, const vec& v);

private:
    double x, y, z;
};

inline vec operator*(double s, const vec& v) { return v * s; }
