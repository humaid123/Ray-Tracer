#ifndef AARECT_H
#define AARECT_H

#include "utility.h"
#include "Hittable.h"

/*
this file defines axis aligned rectangles.
It allows us to define an x=k, y=k or z=k rectangle
aligning 6 such rectangles create a box

To build an axis aligned rectangle only the z-axis for example
     => we just need to store a value k for z= k
     => we need to  store the llc and urc to know which range is fine => (x0, y0) to (x1, y1)
     to intersect => we do plane intersection, then check the range
     a boudning box => just need to give some space around k
*/

// z=k rectangle in region (x0, x1) to (y0, y1)
class xy_rect : public Hittable {
    public:
        xy_rect() {}

        xy_rect(double _x0, double _x1, double _y0, double _y1, double _k, 
            shared_ptr<Material> mat)
            : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};

        virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override {
            auto t = (k-r.origin().z()) / r.direction().z(); // match z component
            if (t < t_min || t > t_max) return false;

            auto x = r.origin().x() + t*r.direction().x();
            auto y = r.origin().y() + t*r.direction().y();
            if (x < x0 || x > x1 || y < y0 || y > y1) return false;
    
            rec.u = (x-x0)/(x1-x0);
            rec.v = (y-y0)/(y1-y0);
            rec.t = t;
            auto outward_normal = Vec3(0, 0, 1);
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mp;
            rec.p = r.at(t);
            return true;
        }

        virtual bool bounding_box(aabb& output_box) const override {
            // need to pad around to give it some thickness
            output_box = aabb(Point3(x0,y0, k-epsilon), Point3(x1, y1, k+epsilon));
            return true;
        }

        virtual std::string name() const override {
            return "xy rect";
        }

        virtual Vec3 random_surface_point() const override {
            return Vec3(
                x0 + (x1-x0) * random_double(0, 1),
                y0 + (y1-y0) * random_double(0, 1), 
                k
            );
        }

    public:
        shared_ptr<Material> mp;
        double x0, x1, y0, y1, k;
};

// y=k rectangle
class xz_rect : public Hittable {
    public:
        xz_rect() {}

        xz_rect(double _x0, double _x1, double _z0, double _z1, double _k,
            shared_ptr<Material> mat)
            : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

        virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override {
            auto t = ( k - r.origin().y()) / r.direction().y(); // check y component for t

            if (t < t_min || t > t_max)  return false;

            // see if within the range
            auto x = r.origin().x() + t*r.direction().x();
            auto z = r.origin().z() + t*r.direction().z();

            if (x < x0 || x > x1 || z < z0 || z > z1) return false;

            // scale u, v in range (0, 1) in case we want to apply a texture
            rec.u = (x-x0)/(x1-x0);
            rec.v = (z-z0)/(z1-z0);
            rec.t = t;
            auto outward_normal = Vec3(0, 1, 0);
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mp;
            rec.p = r.at(t);
            return true;
        }

        virtual bool bounding_box(aabb& output_box) const override {
            // The bounding box must have non-zero width in each dimension, so pad the Y
            // dimension a small amount.
            output_box = aabb(Point3(x0,k-epsilon,z0), Point3(x1, k+epsilon, z1));
            return true;
        }

        virtual std::string name() const override {
            return "xz rect";
        }

        virtual Vec3 random_surface_point() const override {
            return Vec3(
                x0 + (x1-x0) * random_double(0, 1),
                k,
                z0 + (z1-z0) * random_double(0, 1)
            );
        }

    public:
        shared_ptr<Material> mp;
        double x0, x1, z0, z1, k;
};

// x=k rectangle
class yz_rect : public Hittable {
    public:
        yz_rect() {}

        yz_rect(double _y0, double _y1, double _z0, double _z1, double _k,
            shared_ptr<Material> mat)
            : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

        virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override {
            auto t = (k-r.origin().x()) / r.direction().x(); // match x component for t
            if (t < t_min || t > t_max) return false;

            // check if t is within the limit
            auto y = r.origin().y() + t*r.direction().y();
            auto z = r.origin().z() + t*r.direction().z();
            if (y < y0 || y > y1 || z < z0 || z > z1)   return false;

            // set up for texturing
            rec.u = (y-y0)/(y1-y0);
            rec.v = (z-z0)/(z1-z0);
            rec.t = t;
            auto outward_normal = Vec3(1, 0, 0);
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mp;
            rec.p = r.at(t);
    
            return true;
        }

        virtual bool bounding_box(aabb& output_box) const override {
            // The bounding box must have non-zero width in each dimension, so pad the X
            // dimension a small amount.
            output_box = aabb(Point3(k-epsilon, y0, z0), Point3(k+epsilon, y1, z1));
            return true;
        }

        virtual std::string name() const override {
            return "yz rect";
        }
        virtual Vec3 random_surface_point() const override {
            return Vec3(
                k,
                y0 + (y1-y0) * random_double(0, 1), 
                z0 + (z1-z0) * random_double(0, 1)
            );
        }

    public:
        shared_ptr<Material> mp;
        double y0, y1, z0, z1, k;
};
#endif