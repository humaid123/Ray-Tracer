#ifndef AARECT_H
#define AARECT_H

#include "utility.h"
#include "Hittable.h"

/*
this file defines axis aligned rectangles.
It allows us to define an x=k, y=k or z=k rectangle
aligning 6 such rectangles create a box
*/

// z=k rectangle
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

        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
            // The bounding box must have non-zero width in each dimension, so pad the Z
            // dimension a small amount.
            output_box = aabb(Point3(x0,y0, k-0.0001), Point3(x1, y1, k+0.0001));
            return true;
        }

        virtual std::string name() const override {
            return "xy rect";
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
            //std::cout << "t:" << t << "\n"; //<< r.origin().y() << " " << r.direction().y() << "\n";
            if (t < t_min || t > t_max)    return false;
    
            // see if within the range
            auto x = r.origin().x() + t*r.direction().x();
            auto z = r.origin().z() + t*r.direction().z();
            //std::cout << "x:" << x << "z" <<  z <<"\n";
            //std::cout << x0 << " " << x << " " << x1 << "\n";
            //std::cout << z0 << " " << z << " " << z1 << "\n";
            if (x < x0 || x > x1 || z < z0 || z > z1) return false;

            //std::cout << "reached after checks\n";
            rec.u = (x-x0)/(x1-x0);
            rec.v = (z-z0)/(z1-z0);
            rec.t = t;
            auto outward_normal = Vec3(0, 1, 0);
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mp;
            rec.p = r.at(t);
            return true;
        }

        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
            // The bounding box must have non-zero width in each dimension, so pad the Y
            // dimension a small amount.
            output_box = aabb(Point3(x0,k-0.0001,z0), Point3(x1, k+0.0001, z1));
            return true;
        }

        virtual std::string name() const override {
            return "xz rect";
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

        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
            // The bounding box must have non-zero width in each dimension, so pad the X
            // dimension a small amount.
            output_box = aabb(Point3(k-0.0001, y0, z0), Point3(k+0.0001, y1, z1));
            return true;
        }

        virtual std::string name() const override {
            return "yz rect";
        }

    public:
        shared_ptr<Material> mp;
        double y0, y1, z0, z1, k;
};
#endif