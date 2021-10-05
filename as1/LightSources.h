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

class LightSource : Hittable {
public:
    LightSource() {}

    LightSource(Point3 _pos, shared_ptr<Hittable> _object) : pos(_pos), object(_object) {}
    
    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override {
        return object->hit(r, t_min, t_max, rec);
    }

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
        return object->bounding_box(time0, time1, output_box);
    }

    virtual std::string name() const override {
        return object->name();
    }

    Vec3 position() {
        return pos;
    }
    
private:
    Vec3 pos;
    shared_ptr<Hittable> object;
};

class LightSources : Hittable {
public:
    LightSources() {}
    LightSources(shared_ptr<LightSource> object) { add(object); }

    void clear() { lights.clear(); }
    void add(shared_ptr<LightSource> object) { lights.push_back(object); }

    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override {
        HitRecord temp_rec;
        bool hit_anything = false;
        auto closest_so_far = t_max;

        // instead oh infinty for tmax, we use the closest poiny so far, this is why hit takes tmax
        for (const auto& light : lights) {
            //std::cout << "object is a " << object->name() << "\n";
            if (light->hit(r, t_min, closest_so_far, temp_rec)) {
                //std::cout << "object hit\n";
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
        
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
        if (lights.empty()) return false;

        aabb temp_box;
        bool first_box = true;

        for (const auto& light : lights) {
            if (!light->bounding_box(time0, time1, temp_box)) return false;
            output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
            first_box = false;
        }

        return true;
    }

    std::vector<Vec3> positions() const {
        std::vector<Vec3> light_positions;

        for (const auto& light : lights) {
            light_positions.push_back(light->position());
        }
        return light_positions;
    }

    virtual std::string name() const override {
        return "lights";
    }

private:
    std::vector<shared_ptr<LightSource>> lights;
};

#endif
