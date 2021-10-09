#ifndef SPHERE_H
#define SPHERE_H

#include "Hittable.h"
#include "Vec3.h"
#include "string.h"

class Sphere : public Hittable {
    public:
        Sphere() {}
        Sphere(Point3 cen, double r, shared_ptr<Material> m)
        : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
        virtual bool bounding_box(aabb& output_box) const override;
        
        virtual std::string name() const override {
            return "sphere";
        }


    public:
        Point3 center;
        double radius;
        shared_ptr<Material> mat_ptr;
    
    private:
        // required for texturing a sphere => given the point of intersection, it returns (u, v) by reference...
        static void get_sphere_uv(const Point3& p, double& u, double& v) {
            // p: a given point on the sphere of radius one, centered at the origin.
            // u: returned value [0,1] of angle around the Y axis from X=-1.
            // v: returned value [0,1] of angle from Y=-1 to Y=+1.
            //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
            //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
            //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

            auto theta = acos(-p.y());
            auto phi = atan2(-p.z(), p.x()) + pi;

            u = phi / (2*pi);
            v = theta / pi;
        }
};

bool Sphere::bounding_box(aabb& output_box) const {
    output_box = aabb(
        center - Vec3(radius, radius, radius),
        center + Vec3(radius, radius, radius));
    return true;
}

bool Sphere::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    Vec3 oc = r.origin() - center;
    auto a = r.direction().squaredNorm();
    auto half_b = oc.dot(r.direction());
    auto c = oc.squaredNorm() - radius*radius;

    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    Vec3 outward_normal = (rec.p - center) / radius; //normalise the vector
    rec.set_face_normal(r, outward_normal);
    // set_face_normal sets the normal based on the face that was hit (front/exterior or back/interior) 
    // no need for this ->rec.normal = (rec.p - center) / radius;

    // set the material
    rec.mat_ptr = mat_ptr;

    // look up (u, v) coordinates
    get_sphere_uv(outward_normal, rec.u, rec.v);

    return true;
}

#endif