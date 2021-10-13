#ifndef LIGHTSOURCES_H
#define LIGHTSOURCES_H

#include "Hittable.h"
#include "Material.h"
#include "Ray.h"
#include "math.h"
#include "utility.h"
#include <vector>
#include <iostream>
#include "aabb.h"

/*
My light sources are an area of hittable objects with Diffuse Light materials

they additionally have a way to generate random surface points

the lightsources object generate a user-defined amount number of points from each light source and we use
this to have an area light

It is also a hittable object so that we can compute shadow ray intersections
*/

class LightSources : Hittable {
public:
    
    LightSources() {}
    
    LightSources(shared_ptr<Hittable> object) { add(object); }

    void clear() { lights.clear(); }

    void add(shared_ptr<Hittable> object) { lights.push_back(object); }

    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override {
        HitRecord temp_rec;
        bool hit_anything = false;
        auto closest_so_far = t_max;

        // instead oh infinity for tmax, we use the closest poiny so far, this is why hit starts with tmax
        for (const auto& light : lights) {
            if (light->hit(r, t_min, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
        
    virtual bool bounding_box(aabb& output_box) const override {
        if (lights.empty()) return false;

        aabb temp_box;
        bool first_box = true;

        for (const auto& light : lights) {
            if (!light->bounding_box(temp_box)) return false;
            output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
            first_box = false;
        }

        return true;
    }

    virtual std::string name() const override {
        return "lights";
    }

    std::vector<Point3> generate_random_positions(int num_light_samples) const {
        std::vector<Point3> res;

        // jitter for every other light sources
        for (const auto &light : lights) {
            for (int i = 0; i < num_light_samples; i ++) {
                res.push_back(light->random_surface_point());
            }
        }
        return res;
    }

    virtual Vec3 random_surface_point() const override {
        int which = random_int(0, lights.size());
        return lights[which]->random_surface_point();
    }
private:
    std::vector<shared_ptr<Hittable>> lights;
};

#endif
