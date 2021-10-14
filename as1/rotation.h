
#ifndef ROTATION_H
#define ROTATION_H

#include "Hittable.h"
#include "utility"
#include "math.h"
#include "Vec3.h"
/*
We use rotations to make boxes no longer axis aligned

by using trigonometry, we can find that rotatin counter-clockwise on the z-axis => changing only x and y
by an angle theta is done with:
    x_new = cos(theta) * x - sin(theta)*y
    y_new = sin(theta) * x + cos(theta)*y
This works for any theta indepdent on the quadrant

The proofs are just PROJECTIONS...

rotating along the y axis is as such:
    x_new = cos(theta)*x + sin(theta)*z
    z_new = -sin(theta)*x + cos(theta)*z

rotating along the x axis is as such:
    y_new = cos(theta)*y - sin(theta)*z
    z_new = sin(theta)*y + cos(theta)*z 

You need to apply the same code to the surface normals to rotate them
on hit

A rotation is an instance:
    it has a pointer to the actual object but rotates everything that interacts with it

    to calculate ray intersection, the ray is rotated by the given angle of rotation => the rotated ray is thus 
    along the original object's frame and that object's old hit method can be used

    So that blinn-phong still works, we also rotate the surface normals and the point of intersections.

    to create a bouding box => we rotate the original llc and urc of the object inside and use the minimum rotated value
    for the llc and use the maximum rotated value for the urc...

Note:
    the ray is rotated into the original frame of reference
    the surface normals, point of intersection,p, the llc and urc are rotated from the old frame to the new one
*/

class rotate_y : public Hittable {
    public:
        rotate_y(shared_ptr<Hittable> p, double angle) : ptr(p) {
            auto radians = degrees_to_radians(angle);
            sin_theta = sin(radians);
            cos_theta = cos(radians);
            has_box = ptr->bounding_box(bbox); // has_box says if we can apply the rotation instance on the current object 

            Point3 min( infinity,  infinity,  infinity);
            Point3 max(-infinity, -infinity, -infinity);

            // we rotate the bounding box by finding the new llc and urc by finding the minimum of 
            // all minimum rotated coordinates and the maximum of all rotated maximum coordinates..
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    for (int k = 0; k < 2; k++) {
                        auto x = i*bbox.max().x() + (1-i)*bbox.min().x();
                        auto y = j*bbox.max().y() + (1-j)*bbox.min().y();
                        auto z = k*bbox.max().z() + (1-k)*bbox.min().z();

                        auto newx =  cos_theta*x + sin_theta*z;
                        auto newz = -sin_theta*x + cos_theta*z;

                        Vec3 tester(newx, y, newz);

                        for (int c = 0; c < 3; c++) {
                            min[c] = fmin(min[c], tester[c]);
                            max[c] = fmax(max[c], tester[c]);
                        }
                    }
                }
            }
            bbox = aabb(min, max);
        }

        virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override {
            auto origin = r.origin();
            auto direction = r.direction();

            // to test for intersection, we ROTATE THE RAY TO BE on the plane of the original item
            origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
            origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

            direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
            direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

            Ray rotated_r(origin, direction);

            // the rotated ray is now in the 'coordinate frame' of the original item 
            // we test if the rotated ray hits the original item
            if (!ptr->hit(rotated_r, t_min, t_max, rec))
                return false;

            // we get the point of intersection from the original item intersection
            // we then rotate them back
            auto p = rec.p;
            auto normal = rec.normal;

            p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[2];
            p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

            normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
            normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

            rec.p = p;
            rec.set_face_normal(rotated_r, normal);

            return true;
        }

        virtual bool bounding_box(aabb& output_box) const override {
            // return the bouding box from the constructor code...
            output_box = bbox;
            return has_box;
        }

        // function used when debugging => makes a hittable print out its name if it was hit
        virtual std::string name() const override {
            return "rotate y";
        }

        virtual Vec3 random_surface_point() const override {
            auto p = ptr->random_surface_point();

            p[0] =  cos_theta*p[0] + sin_theta*p[2];
            p[2] = -sin_theta*p[0] + cos_theta*p[2];
            return p;
        }

    public:
        shared_ptr<Hittable> ptr;
        double sin_theta;
        double cos_theta;
        bool has_box; // can only contruct a rotated bounding box if the original item had a bounding box
        aabb bbox;
};


class rotate_x : public Hittable {
    public:
        rotate_x(shared_ptr<Hittable> p, double angle) : ptr(p) {
            auto radians = degrees_to_radians(angle);
            sin_theta = sin(radians);
            cos_theta = cos(radians);
            has_box = ptr->bounding_box(bbox); // has_box says if we can apply the rotation instance on the current object 
            Point3 min( infinity,  infinity,  infinity);
            Point3 max(-infinity, -infinity, -infinity);

            // we rotate the bounding box by finding the new llc and urc by finding the minimum of 
            // all minimum rotated coordinates and the maximum of all rotated maximum coordinates..
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    for (int k = 0; k < 2; k++) {
                        auto x = i*bbox.max().x() + (1-i)*bbox.min().x();
                        auto y = j*bbox.max().y() + (1-j)*bbox.min().y();
                        auto z = k*bbox.max().z() + (1-k)*bbox.min().z();

                        // auto newx =  cos_theta*x + sin_theta*z;
                        // auto newz = -sin_theta*x + cos_theta*z;
                        auto newy = cos_theta*y - sin_theta*z;
                        auto newz = sin_theta*y + cos_theta*z;
            
                        Vec3 tester(x, newy, newz);

                        for (int c = 0; c < 3; c++) {
                            min[c] = fmin(min[c], tester[c]);
                            max[c] = fmax(max[c], tester[c]);
                        }
                    }
                }
            }
            bbox = aabb(min, max);
        }

        virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override {
            auto origin = r.origin();
            auto direction = r.direction();

            // to test for intersection, we ROTATE THE RAY TO BE on the plane of the original item
            origin[1] = cos_theta*r.origin()[1] + sin_theta*r.origin()[2];
            origin[2] = -sin_theta*r.origin()[1] + cos_theta*r.origin()[2]; 

            direction[1] = cos_theta*r.direction()[1] + sin_theta*r.direction()[2];
            direction[2] = -sin_theta*r.direction()[1] + cos_theta*r.direction()[2];

            Ray rotated_r(origin, direction);

            // the rotated ray is now in the 'coordinate frame' of the original item 
            // we test if the rotated ray hits the original item
            if (!ptr->hit(rotated_r, t_min, t_max, rec))
                return false;

            // we get the point of intersection from the original item intersection
            // we then rotate them back
            auto p = rec.p;
            auto normal = rec.normal;

            p[1] = cos_theta*rec.p[1] - sin_theta*rec.p[2];
            p[2] = sin_theta*rec.p[1] + cos_theta*rec.p[2];

            normal[1] = cos_theta*rec.normal[1] - sin_theta*rec.normal[2];
            normal[2] = sin_theta*rec.normal[1] + cos_theta*rec.normal[2];

            rec.p = p;
            rec.set_face_normal(rotated_r, normal);

            return true;
        }

        virtual bool bounding_box(aabb& output_box) const override {
            // return the bouding box from the constructor code...
            output_box = bbox;
            return has_box;
        }

        // function used when debugging => makes a hittable print out its name if it was hit
        virtual std::string name() const override {
            return "rotate x";
        }

        virtual Vec3 random_surface_point() const override {
            auto p = ptr->random_surface_point();

            p[1] = cos_theta*p[1] - sin_theta*p[2];
            p[2] = sin_theta*p[1] + cos_theta*p[2];
            return p;
        }

    public:
        shared_ptr<Hittable> ptr;
        double sin_theta;
        double cos_theta;
        bool has_box; // can only contruct a rotated bounding box if the original item had a bounding box
        aabb bbox;
};

class rotate_z : public Hittable {
    public:
        rotate_z(shared_ptr<Hittable> p, double angle) : ptr(p) {
            auto radians = degrees_to_radians(angle);
            sin_theta = sin(radians);
            cos_theta = cos(radians);
            has_box = ptr->bounding_box(bbox); // has_box says if we can apply the rotation instance on the current object 
            Point3 min( infinity,  infinity,  infinity);
            Point3 max(-infinity, -infinity, -infinity);

            // we rotate the bounding box by finding the new llc and urc by finding the minimum of 
            // all minimum rotated coordinates and the maximum of all rotated maximum coordinates..
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    for (int k = 0; k < 2; k++) {
                        auto x = i*bbox.max().x() + (1-i)*bbox.min().x();
                        auto y = j*bbox.max().y() + (1-j)*bbox.min().y();
                        auto z = k*bbox.max().z() + (1-k)*bbox.min().z();

                        // x_new = cos(theta) * x - sin(theta)*y
                        // y_new = sin(theta) * x + cos(theta)*y
                        auto newX = cos_theta*x - sin_theta*y;
                        auto newY = sin_theta*x + cos_theta*y;
            
                        Vec3 tester(newX, newY, z);

                        for (int c = 0; c < 3; c++) {
                            min[c] = fmin(min[c], tester[c]);
                            max[c] = fmax(max[c], tester[c]);
                        }
                    }
                }
            }
            bbox = aabb(min, max);
        }

        virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override {
            auto origin = r.origin();
            auto direction = r.direction();

            // x_new = cos(theta) * x - sin(theta)*y => rotate into plane => cos(theta) * x + sin(theta)*y
            // y_new = sin(theta) * x + cos(theta)*y => rotate into plane => -sin(theta) * x + cos(theta)*y

            // to test for intersection, we ROTATE THE RAY TO BE on the plane of the original item
            origin[0] = cos_theta*r.origin()[0] + sin_theta*r.origin()[1];
            origin[1] = -sin_theta*r.origin()[0] + cos_theta*r.origin()[1]; 

            direction[0] = cos_theta*r.direction()[0] + sin_theta*r.direction()[1];
            direction[1] = -sin_theta*r.direction()[0] + cos_theta*r.direction()[1];

            Ray rotated_r(origin, direction);

            // the rotated ray is now in the 'coordinate frame' of the original item 
            // we test if the rotated ray hits the original item
            if (!ptr->hit(rotated_r, t_min, t_max, rec))
                return false;

            // we get the point of intersection from the original item intersection
            // we then rotate them back
            auto p = rec.p;
            auto normal = rec.normal;

            // x_new = cos(theta) * x - sin(theta)*y
            // y_new = sin(theta) * x + cos(theta)*y
            p[0] = cos_theta*rec.p[0] - sin_theta*rec.p[1];
            p[1] = sin_theta*rec.p[0] + cos_theta*rec.p[1];

            normal[0] = cos_theta*rec.normal[0] - sin_theta*rec.normal[1];
            normal[1] = sin_theta*rec.normal[0] + cos_theta*rec.normal[1];

            rec.p = p;
            rec.set_face_normal(rotated_r, normal);

            return true;
        }

        virtual bool bounding_box(aabb& output_box) const override {
            // return the bouding box from the constructor code...
            output_box = bbox;
            return has_box;
        }

        // function used when debugging => makes a hittable print out its name if it was hit
        virtual std::string name() const override {
            return "rotate x";
        }

        virtual Vec3 random_surface_point() const override {
            auto p = ptr->random_surface_point();

            // x_new = cos(theta) * x - sin(theta)*y
            // y_new = sin(theta) * x + cos(theta)*y
            p[0] = cos_theta*p[0] - sin_theta*p[1];
            p[1] = sin_theta*p[0] + cos_theta*p[1];
            return p;
        }

    public:
        shared_ptr<Hittable> ptr;
        double sin_theta;
        double cos_theta;
        bool has_box; // can only contruct a rotated bounding box if the original item had a bounding box
        aabb bbox;
};


#endif