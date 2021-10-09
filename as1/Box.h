#ifndef BOX_H
#define BOX_H

#include "utility.h"

#include "aarect.h"
#include "HittableList.h"

// A box is 6 rectangles => we use aarects here

class Box : public Hittable  {
    public:
        Box() {}
        Box(const Point3& p0, const Point3& p1, shared_ptr<Material> ptr) {
            box_min = p0;
            box_max = p1;

            sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
            sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

            sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
            sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));

            sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
            sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
        }

        virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override {
            return sides.hit(r, t_min, t_max, rec);
        }

        virtual bool bounding_box(aabb& output_box) const override {
            output_box = aabb(box_min, box_max);
            return true;
        }
        virtual std::string name() const override {
            return "box";
        }

    public:
        Point3 box_min;
        Point3 box_max;
        HittableList sides;
};

#endif