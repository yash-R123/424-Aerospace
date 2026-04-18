#include "vec.h"
#include <cmath>

double vec::dot(const vec& o) const {
    return x*o.x + y*o.y + z*o.z;
}

vec vec::cross(const vec& o) const {
    return vec(y*o.z - z*o.y,
        z*o.x - x*o.z,
        x*o.y - y*o.x);
}

double vec::length() const {
    return std::sqrt(x*x + y*y + z*z);
}

vec vec::normalized() const {
    return *this / length();
}

std::ostream& operator<<(std::ostream& os, const vec& v) {
    os << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
    return os;
}
