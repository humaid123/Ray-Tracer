

#ifndef VEC3_H
#define VEC3_H

/*
this will encapsulate an eigen vector 3 
*/ 


#include <cmath>
#include <iostream>
#include "utility.h"

using std::sqrt;

class Vec3
{
public:
    Vec3() : e{0, 0, 0} {}
    Vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double &operator[](int i) { return e[i]; }

    Vec3 &operator+=(const Vec3 &v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    Vec3 &operator*=(const double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Vec3 &operator/=(const double t) {
        return *this *= 1 / t;
    }

    double norm() const {
        return sqrt(normSquared());
    }

    double normSquared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    inline static Vec3 random() {
        return Vec3(random_double(), random_double(), random_double());
    }

    inline static Vec3 random(double min, double max) {
        return Vec3(random_double(min,max), random_double(min,max), random_double(min,max));
    }

    bool near_zero() const {
        // Return true if the vector is close to zero in all dimensions.
        const auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

    inline double dot(const Vec3 &v) const {
        return this->e[0] * v.e[0]
             + this->e[1] * v.e[1]
             + this->e[2] * v.e[2];
    }

    inline Vec3 cross(const Vec3 &v) {
        return Vec3(this->e[1] * v.e[2] - this->e[2] * v.e[1],
                    this->e[2] * v.e[0] - this->e[0] * v.e[2],
                    this->e[0] * v.e[1] - this->e[1] * v.e[0]);
    }

    inline void normalize() {
        auto norm = this->norm();
        e[0] /= norm;
        e[1] /= norm;
        e[2] /= norm;
    }


public:
    double e[3];
};

// Type aliases for vec3
typedef Vec3 Point3; // 3D point


inline std::ostream& operator<<(std::ostream &out, const Vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline Vec3 operator+(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}


inline Vec3 operator*(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline Vec3 operator*(double t, const Vec3 &v) {
    return Vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline Vec3 operator*(const Vec3 &v, double t) {
    return t * v;
}

inline Vec3 operator/(Vec3 v, double t) {
    return (1/t) * v;
}

Vec3 random_in_unit_sphere() {
    while (true) {
        auto p = Vec3::random(-1,1);
        if (p.normSquared() >= 1) continue;
        return p;
    }
}

Vec3 random_unit_vector() {
    Vec3 res = (random_in_unit_sphere());
    res.normalize();
    return res;
}

/*

vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}
*/


// assumes all the vectors are unit vectors
Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v - 2*v.dot(n)*n;
}

// assumes all the vectors are unit vectors
// etai_over_etat = n1/n2 where n1 is the index of refraction incident and n2 is the index refraction of new obj
Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat) {
    auto cos_theta = fmin((-uv).dot(n), 1.0);
    Vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.normSquared())) * n;
    return r_out_perp + r_out_parallel;
}

#endif