#ifndef CAMERA_H
#define CAMERA_H

#include "utility.h"
#include "Vec3.h"

/*
positionable camera => 
give origin, view direction, up vector, height and aspect ratio of the image and it is able to cast rays...

Peter Shirley asks for the field of view angle and the aspect ratio instead of the height and width...
we can also take the origin and the point to look at and generate the view direction by lookat - lookfrom

we also take the up vector and the focus dist for perspective viewing


*/


class Camera {
    public:
        Camera(
            Point3 lookfrom,
            Point3 lookat,
            Vec3   vup,
            double vfov, // vertical field-of-view in degrees
            double aspect_ratio,
            double focus_dist
        ) {
            // get view height using the field of view angle => use the triangle formed from the origin, lookat and the 
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            // get a basis
            w = (lookfrom - lookat);
            w.normalize();
            u = vup.cross(w);
            u.normalize();
            v = w.cross(u);
            v.normalize();

            // instead of using the basis as a normalised vector, we scale everything based on the 
            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;
        }

        // the camera generates a ray given the pixel position / subpixel position and transforms it along the axis.
        Ray get_ray(double s, double t) const {
            return Ray(
                origin,
                lower_left_corner + s*horizontal + t*vertical - origin
            );
        }

    private:
        Point3 origin;
        Point3 lower_left_corner;
        Vec3 horizontal;
        Vec3 vertical;
        Vec3 u, v, w;
};

#endif