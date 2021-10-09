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

// compare two boxes along a chosen axis to sort a vector by smallest llc[axis] being at the start
inline bool my_box_compare(shared_ptr<Hittable> a, shared_ptr<Hittable> b, int axis) {
    aabb box_a, box_b;

    if (!a->bounding_box(box_a) || !b->bounding_box(box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    // compare the llcs along the chosen axis
    if (axis == 0)
        return box_a.min().x() < box_b.min().x();
    else if (axis == 1)
        return box_a.min().y() < box_b.min().y();
    else
        return box_a.min().z() < box_b.min().z();
}

// wrappers so that box_compare() follows the required signature
bool box_x_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return my_box_compare(a, b, 0);
}
bool box_y_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return my_box_compare(a, b, 1);
}
bool box_z_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return my_box_compare(a, b, 2);
}

class BVH : public Hittable {
    public:
        BVH();

        BVH(const HittableList& list) : BVH(list.objects, 0, list.objects.size()) {}

        BVH(const std::vector<shared_ptr<Hittable>>& src_objects, size_t start, size_t end) {
            auto objects = src_objects; // Create a modifiable array of the source scene objects

            int axis = random_int(0,2);
            auto comparator = box_x_compare; // axis == 0
            if (axis == 1) 
                comparator = box_y_compare;
            else 
                comparator = box_z_compare;

            size_t object_span = end - start;

            if (object_span == 1) {
                // instead of adding checks for null in the middle of the code, we make a duplicate of the object for each child
                this->left = this->right = objects[start];
            } else if (object_span == 2) {
                // there are only two objects => compare and put smallest llc[axis] in left
                if (comparator(objects[start], objects[start+1])) {
                    this->left = objects[start];
                    this->right = objects[start+1];
                } else {
                    this->left = objects[start+1];
                    this->right = objects[start];
                }
            } else {
                // more that two objects, SORT, then create children
                std::sort(objects.begin() + start, objects.begin() + end, comparator);
                auto mid = start + object_span/2;
                this->left = make_shared<BVH>(objects, start, mid);
                this->right = make_shared<BVH>(objects, mid, end);
            }

            // create the root node by creating a box than encompasses both children
            aabb box_left, box_right;

            if (  !left->bounding_box (box_left) || !right->bounding_box(box_right)  )
                std::cerr << "No bounding box in bvh_node constructor.\n";

            // the ROOT box is a box that ENGLOBES BOTH OF ITS CHILDREN !!!!
            box = surrounding_box(box_left, box_right);
        }

        virtual bool hit(const Ray& r, double tmin, double tmax, HitRecord& rec) const override {
            if (!box.hit(r, tmin, tmax))
                return false;

            bool hit_left = left->hit(r, tmin, tmax, rec);
            bool hit_right = right->hit(r, tmin, hit_left ? rec.t : tmax, rec);

            return hit_left || hit_right;
        }

        virtual bool bounding_box(aabb& output_box) const override {
            output_box = box;
            return true;
        }

        virtual std::string name() const override {
            return "BVH";
        }

    public:
        std::shared_ptr< Hittable > left;
        std::shared_ptr< Hittable > right;
        aabb box;
};



#endif
