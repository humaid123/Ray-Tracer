#ifndef AABB_H
#define AABB_H

#include "utility.h"

/*
an aabb is a bounding box around a primitive

if a ray hits the box, we calculate if the ray hits its content => other wise we ignore the box
By hierarchically dividing the space into boxes => boxes within boxes within boxes
    we get o run the ray intersection code ONLY WHEN NEEDED => not against every object in the space

note that to check if something hits an axis aligned plane, say x = k
    You just need to get the value of t when that component of the ray = k => o.x + t*d.x = k
        => t = (k - o.x) / d.x
    we then put t in x and y
    x(t), y(t), z(t) is when the ray intersects that plan

in 2D we have bounding rectangles which are formed by 4 lines: x=x0, x=x1, y=y0, y=y1
    we can get 4 ts when the the ray hits the 4 lines by solving equations similar to t = t = (k - o.x) / d.x ;; just put k = x0, x1, y0, y1
    Note that there is a hit when the t values from solving the x's overlap with the ts from solving with the ys
    hit if [tx0, tx1] SUBSET [ty0, ty1]
        No need to be proper subsets
    DRAW A PICTURE TO CONVINCE YOURSELF THAT ONLY AN OVERLAP IS NEEDED
    
    we can test for intersections in 2D by doing 4 subtraction and 4 division and checking for overlaps (some calls to min/max)

this translates exactly to 3D
    a box is 6 planes
    an aabb can be stored using only its llc and urc (x0, y0, z0) and (x1, y1, z1)
        x=x0, x1 
        y=y0, y1
        z=z0, z1
    you can solve for 6 equations to know at what ts, the ray hits each plane
        you get tx0, tx1, ty0, ty1, tz0, tz1
    THERE IS A HIT IF THE FOLLOWING OVERLAPS:
        [tx0, tx1] SUBSET [ty0, ty1] SUBSET [tz0, tz1]
    If ALL three intervals overlap, there is a hit


Peter Shirley does some defensive programming stuff to prevent errors
    See the non-optimized hit method for a more readable code

    Peter stores the llc in 'minium' and the urc in 'maximum'

    The following code does the following
    for each of the axes x, y, x:
        calculate t0 and t1 using a defensive programming method that solves the issue of r.d contains 0
    From this, you will get tx0, tx1, ty0, ty1, tz0, tz1

    after each time that you calculate the interval:
        compare using the min the new t0 with the previous t0 from the previous axis => when you do for x, you use the provided tmin
            this calculates the minimum of [tx0, ty0, tz0]
        compare using the max the new t1 against previous t1 with initial t1 being t_max
            this calculates the maximum of [tx1, ty1, tz1]
        Notice that there is an intersection => all three subsets overlap if min(t0) < max(t1)

            bool hit(const ray& r, double t_min, double t_max) const {
            for (int a = 0; a < 3; a++) {
                auto t0 = fmin((minimum[a] - r.origin()[a]) / r.direction()[a],
                               (maximum[a] - r.origin()[a]) / r.direction()[a]);
                auto t1 = fmax((minimum[a] - r.origin()[a]) / r.direction()[a],
                               (maximum[a] - r.origin()[a]) / r.direction()[a]);
                t_min = fmax(t0, t_min);
                t_max = fmin(t1, t_max);
                if (t_max <= t_min)
                    return false;
            }
            return true;
        }

        Peter Shirley then presents an optimised version of that hit method...

To implement the BVH data strucuture => you need to have all of your geometric primitives return a bounding box
    Each geometric primitive will have a different way to calculate the box
        Sphere => each side is 2*radius => to get llc and urc => you just need to go 
                x0 = center.x - radius, x1 = center.x + radius 
                y0 = center.y - radius, y1 = center.y + radius 
                z0 = center.z - radius, z1 = center.z + radius 
        AArect => just return itself
        plane => return the llc and urc of the place with a little thickness
        a group of objects => make each object calculate their bounding box 
                => llc = min of all llc
                => urc = max of all urc
            Just go 2 by 2 using the surrounding_box() method provided below
*/

class aabb {
    public:
        aabb() {}
        aabb(const Point3& a, const Point3& b) { minimum = a; maximum = b;}

        Point3 min() const {return minimum; }
        Point3 max() const {return maximum; }

        // optimised hit method from Peter Shirley's book, does the same thing as explained above...
       inline bool hit(const Ray& r, double t_min, double t_max) const {
            for (int a = 0; a < 3; a++) {
                auto t0 = fmin((minimum[a] - r.origin()[a]) / r.direction()[a],
                               (maximum[a] - r.origin()[a]) / r.direction()[a]);
                auto t1 = fmax((minimum[a] - r.origin()[a]) / r.direction()[a],
                               (maximum[a] - r.origin()[a]) / r.direction()[a]);
                t_min = fmax(t0, t_min);
                t_max = fmin(t1, t_max);
                if (t_max <= t_min)
                    return false;
            }
            return true;
            return true;
        }       

        Point3 minimum;
        Point3 maximum;
};

// merge boxes => given two boxes => ths code returns a box that contains both boxes
// basically find the lowest llc and the biggest urc
aabb surrounding_box(aabb box0, aabb box1) {
    Vec3 small(fmin(box0.min().x(), box1.min().x()),
               fmin(box0.min().y(), box1.min().y()),
               fmin(box0.min().z(), box1.min().z()));

    Vec3 big  (fmax(box0.max().x(), box1.max().x()),
               fmax(box0.max().y(), box1.max().y()),
               fmax(box0.max().z(), box1.max().z()));

    return aabb(small,big);
}
#endif