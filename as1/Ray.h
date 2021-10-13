#ifndef RAY_H
#define RAY_H

#include "Vec3.h"

/*
Defines a simple ray which is two vectors, an origin and direction
*/

class Ray {
    public:
        Ray() {}
        Ray(const Point3& origin, const Vec3& direction)
            : orig(origin), dir(direction)
        {}

        Point3 origin() const  { return orig; }
        Vec3 direction() const { return dir; }

        Point3 at(double t) const {
            return orig + t*dir;
        }

    public:
        Point3 orig;
        Vec3 dir;
};

#endif