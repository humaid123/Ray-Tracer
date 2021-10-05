#ifndef CAMERA_H
#define CAMERA_H

#include "utility.h"
#include "Vec3.h"

/*
positionable camera => 
give origin, view direction, up vector, height and aspect ratio of the image and it is able to cast rays...
*/
/*
class Camera {
    public:
        Camera(
            Point3 origin,
            Vec3 view_direction,
            Vec3 vup,
            float viewport_height,
            float aspect_ratio
        ) {
            auto viewport_width = aspect_ratio * viewport_height;

            w = view_direction;
            w.normalize();

            v = vup.cross(w);
            v.normalize();

            u = w.cross(v);
            u.normalize();

            // required for positionable camera
            origin = origin;
            horizontal = viewport_width * u;
            vertical = viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2;
        }

        Ray cast_ray(Point3 pixelPos) const {
            return Ray(origin, pixelPos.x()*u + pixelPos.y()*v -w - origin);

            //return Ray(origin, lower_left_corner + pixelPos.x()*horizontal + pixelPos.y()*vertical - origin);
            //return Ray(origin, lower_left_corner + s*horizontal + t*vertical - origin);
        }

    private:
        Point3 origin;
        Vec3 u, v, w;
        Point3 lower_left_corner;
        Vec3 horizontal;
        Vec3 vertical;
};*/

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

Vec3 random_in_unit_disk() {
    while (true) {
        auto p = Vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.squaredNorm() >= 1) continue;
        return p;
    }
}

class Camera {
    public:
        Camera(
            Point3 lookfrom,
            Point3 lookat,
            Vec3   vup,
            double vfov, // vertical field-of-view in degrees
            double aspect_ratio,
            double aperture,
            double focus_dist
        ) {
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            w = (lookfrom - lookat);
            w.normalize();
            u = vup.cross(w);
            u.normalize();
            v = w.cross(u);
            v.normalize();

            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;

            lens_radius = aperture / 2;
        }


        Ray get_ray(double s, double t) const {
            //Vec3 rd = lens_radius * random_in_unit_disk();
            //Vec3 offset = u * rd.x() + v * rd.y();

            return Ray(
                origin /*+ offset*/,
                lower_left_corner + s*horizontal + t*vertical - origin /*- offset*/
            );
        }

    private:
        Point3 origin;
        Point3 lower_left_corner;
        Vec3 horizontal;
        Vec3 vertical;
        Vec3 u, v, w;
        double lens_radius;
};

#endif