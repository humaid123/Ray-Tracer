#ifndef BVH_H
#define BVH_H

#include "utility.h"
#include "HittableList.h"
#include "aabb.h"
#include "Ray.h"
#include "Hittable.h"
#include <vector>
#include <algorithm>

/*
A BVH tree is stored recursively using its nodes

the constructor can either take a hittable_list or a vector of Hittable pointers
What it will create a Binary tree by recursively dividing the list by two until the leaves contain a single object
the idea is that when a ray wants to check if there is a hit, we go down the tree in O(log n) rather than iterating over a list in O(n)

To check what a ray hits is as simple as doing this:
    if (!ray->hit(root, tmin, tmax, rec))
        return false
    // need to do it with two bools as shortcircuiting will mess things up 
    hit_left = ray->hit(root.left, tmin, rec.t, rec);
    hit_right = ray->hit(root.right, tmin, rec.t, rec);
    return hit_left or hit_right // rec will then contain the FINAL OBJECT by recursively exploring the tree

the hardest part about building a BVH is finding how to divide the space during construction

Peter divides the space using a simple heuristic
    if the vector contains two elements, put each element in a child node and stop the recursion
    else:
        randomly choose an axis x, y, z
        sort the objects in the list along that axis so that smaller x/y/z means the object is at the start
        mid = size/2
        leftNode = BVH(vector[:mid])
        rightNode = BVH(vector[mid:])


To do this in C++ => you need to use std::sort
    sort takes an iterator at the start and at the end of a vector and a comparator function
    in our case in the comparator function => we just compare the llcs/min of the two boxes along, x, y, or z depending on which axis was choosen

Doing BVH(list) returns a hittable that is A TREE as the constructor recursively divides everything on its own...

*/

// we need to create a comparator to sort hte objects based on the axes
// compare two boxes along a chosen axis to sort a vector by smallest llc[axis] being at the start
inline bool box_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axis) {
    aabb box_a, box_b;

    if (!a->bounding_box(box_a) || !b->bounding_box(box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    if (axis == 0) return box_a.min().x() < box_b.min().x();
    if (axis == 1) return box_a.min().y() < box_b.min().y();
    if (axis == 2) return box_a.min().z() < box_b.min().z();

    std::cerr << "generated a wrong axis\n";
    return false;
}

// the comparator function only takes two arguments so we are forced to do this => create 3 functions
// pick the correct comparator based on what is needed...
bool box_x_compare (const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return box_compare(a, b, 0);
}

bool box_y_compare (const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return box_compare(a, b, 1);
}

bool box_z_compare (const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return box_compare(a, b, 2);
}

class BVH : public Hittable {
    public:
        BVH();
        // calls the comparator that does the work with a start and an end pointer into the list. Needs to specify which axis to use as well
        BVH(const HittableList& list) : BVH(list.objects, 0, list.objects.size(), 0) {}
        BVH(const std::vector<shared_ptr<Hittable>>& src_objects, size_t start, size_t end) : BVH(src_objects, start, end, 0) {}

        BVH(const std::vector<shared_ptr<Hittable>>& src_objects, size_t start, size_t end, int axis) {
            auto objects = src_objects; // Create a modifiable array of the source scene objects

            // pick a comparator based on the axis
            auto comparator = (axis == 0) ? box_x_compare
                                          : (axis == 1) ? box_y_compare
                                          : box_z_compare;

            size_t object_span = end - start;            
            if (object_span == 1) {
                
                // we make a copy instead of having nullptrs around
                left = right = objects[start];

            } else if (object_span == 2) {

                // two objects so we sort them and place closest to the left
                if (comparator(objects[start], objects[start+1])) {
                    left = objects[start];
                    right = objects[start+1];
                } else {
                    left = objects[start+1];
                    right = objects[start];
                }

            } else {
                
                // sort the objects based on the given axis
                std::sort(objects.begin() + start, objects.begin() + end, comparator);

                // find midpoint and recurse on mid...
                // pick the next logical axis using % => we divide on x, then on y, then on z and so on...
                auto mid = start + object_span/2;
                left = make_shared<BVH>(objects, start, mid, (axis + 1) % 3);
                right = make_shared<BVH>(objects, mid, end, (axis + 1) % 3);
            }

            aabb box_left, box_right;
            if (  !left->bounding_box (box_left) || !right->bounding_box(box_right)  )
                std::cerr << "No bounding box in bvh_node constructor.\n";

            box = surrounding_box(box_left, box_right); // the root is a box that contains the other two boxes
        }

        virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override {
            if (!box.hit(r, t_min, t_max)) return false;

            // the left and the right are HITTABLE OBJECTS...
            // so we can recurse into a hit record with just this
            bool hit_left = left->hit(r, t_min, t_max, rec);
            bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);
            return hit_left || hit_right;
        }

        virtual bool bounding_box(aabb& output_box) const override {
            output_box = box;
            return true;
        }

        virtual std::string name() const override {
            return left->name() +  " BVH " + right->name();
        }

        virtual Vec3 random_surface_point() const override {
            int rand = random_int(0, 1);
            return rand == 0 ? left->random_surface_point() : right->random_surface_point();
        }

    public:
        std::shared_ptr< Hittable > left;
        std::shared_ptr< Hittable > right;
        aabb box;
};



#endif
