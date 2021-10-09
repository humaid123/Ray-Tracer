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
        // make all geometries return if a ray hits it or not. We pass rec by ref for clearner code for list of hittables
        virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const = 0;

        // make a hittable return a bouding box. again we use a ref so that we can group objects into one box if needed
        virtual bool bounding_box(aabb& output_box) const = 0;
        
        // function used when debugging => makes a hittable print out its name if it was hit
        virtual std::string name() const = 0;
};

#endif