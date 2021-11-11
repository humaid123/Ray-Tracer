#ifndef VEC3_H
#define VEC3_H

#include <Eigen/Dense>

/*
File to encapsulate an Eigen vector

I created it so that I could include it and turn off IDE errors...
*/

typedef Eigen::Matrix<float, 3, 1> Vec3;
using Point3 = Vec3; // 3D point

#endif // VEC3_H