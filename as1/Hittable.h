#ifndef HITTABLE_H
#define HITTABLE_H

#include "utility.h"
#include "aabb.h"
#include "string.h"
// Cannot do this as circular dependency => #include "Material.h"
class Material; // alerts the C++ compiler that material is a class

struct HitRecord {
    Point3 p;
    Vec3 normal;
    shared_ptr<Material> mat_ptr;
    double t;
    bool front_face;

    // (u, v) coordinates for textures
    double u;
    double v;

    inline void set_face_normal(const Ray& r, const Vec3& outward_normal) {
        front_face = r.direction().dot(outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};

class Hittable {
    public:
        virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const = 0;
        virtual bool bounding_box(double time0, double time1, aabb& output_box) const = 0;
        virtual std::string name() const = 0;
};

#endif