#ifndef HITTABLE_H
#define HITTABLE_H

#include "utility.h"
#include "aabb.h"
#include "string.h"

/*
File defines what a SURFACE is.
I used Hittable as per Peter Shirley but this is just an abstract class/interface for 
any geometric primitive that we might want to use in a scene

We also define the HitRecord object which will be passed by reference to each object
until we get the closest hit

My hittable object also has a name() method which can be used for debugging
and a random_surface_point() which is used to do area_lights.
*/

// Cannot do this as circular dependency => #include "Material.h"
class Material; // alerts the C++ compiler that material is a class

struct HitRecord {
    Point3 p;
    Vec3 normal;
    shared_ptr<Material> mat_ptr;
    double t;
    bool front_face; // says if the normal is pointing inwards or outwards

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

        virtual Vec3 random_surface_point() const = 0;
        /* {
            std::cerr << "Object " << name() << " does not support random_surface_point()\n";
            return Vec3(0, 0, 0);
        }*/
};

#endif