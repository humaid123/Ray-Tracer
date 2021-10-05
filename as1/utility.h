// file is called rtweekend.h in the book

#ifndef UTILITY_H
#define UTILITY_H

#include <cmath> 
#include <limits> // gives INT_MAX, ...
#include <memory> // gives shared_ptr
#include <cstdlib> // gives rand()

// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;
const float epsilon = 0.0001;

// Utility Functions

/*
dont need as we wont do the vfov thing
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}
*/

inline double random_double() {
    // Returns a random real in [0,1).
    // rand gives an integer between 0 to RAND_MAX, divide to get a real
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

// generate random integer
inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max+1));
}
// Common Headers

#include "Ray.h"
#include "Vec3.h"

inline static Vec3 random(double min, double max) {
    return Vec3(random_double(min,max), random_double(min,max), random_double(min,max));
}

// IMPORTANT FUNCTIONS
Vec3 random_in_unit_sphere() {
    while (true) {
        auto p = random(-1,1);
        if (p.squaredNorm() >= 1) continue;
        return p;
    }
}

Vec3 random_unit_vector() {
    Vec3 res = (random_in_unit_sphere());
    res.normalize();
    return res;
}


// assumes all the vectors are unit vectors
Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v - 2*v.dot(n)*n;
}

// assumes all the vectors are unit vectors
// etai_over_etat = n1/n2 where n1 is the index of refraction incident and n2 is the index refraction of new obj
Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat) {
    auto cos_theta = fmin((-uv).dot(n), 1.0);
    Vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.squaredNorm())) * n;
    return r_out_perp + r_out_parallel;
}

#endif