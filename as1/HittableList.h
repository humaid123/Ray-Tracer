#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "Hittable.h"

#include <memory>
#include <vector>
#include <iostream>

using std::shared_ptr;
using std::make_shared;

class HittableList : public Hittable {
    public:
        HittableList() {}
        HittableList(shared_ptr<Hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<Hittable> object) { objects.push_back(object); }

        virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
        
        virtual bool bounding_box(aabb& output_box) const override;

        virtual std::string name() const override {
            return "hittable list";
        }

    public:
        std::vector<shared_ptr<Hittable>> objects;
};


bool HittableList::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    HitRecord temp_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    // instead oh infinty for tmax, we use the closest poiny so far, this is why hit takes tmax
    for (const auto& object : objects) {
        //std::cout << "object is a " << object->name() << "\n";
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            //std::cout << "object hit\n";
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

bool HittableList::bounding_box( aabb& output_box) const {
    if (objects.empty()) return false;

    aabb temp_box;
    bool first_box = true;

    for (const auto& object : objects) {
        if (!object->bounding_box(temp_box)) return false;
        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }

    return true;
}

#endif