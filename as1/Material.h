#ifndef MATERIAL_H
#define MATERIAL_H

#include "utility.h"
#include "Hittable.h"
#include "Texture.h"
#include "Color.h"

/*
This file defines all possible types of materials used in this project

We first define an interface for materials.
    a material mist have a way to emit a color
    a way to generate a secondary ray to trace / scatter
    a way to say which type it is

NOT all my materials are Blinn-Phong ones
    I have a light emitter materials and a dielectric

materials may also take textures which are classes that take coordinates (u, v) and possibly the point of intersection
to return a color..

I GIVE DEFAULT VALUES OF ka, km, kd, ks and p...
Then if I feel that I need to change some values for a particular material, I added ONLY these coefficients

I have 3 Blinn-Phong materials
    A Matte object whose secondary ray is random
    A Metal object which does perfect reflection using d - 2(n . d)n
    A fuzzy metal object which have lower reflection and disturbs the reflected ray a little bit
*/

// I have three types of materials => 
    // a blinn_phong/reflective, 
    // glassy/refractive, 
    // light_emitter
    // based on which material type, we run a different shading algo...
enum MatTypes { blinn_phong, glassy, light_emitter };

/*
values of p
10—“eggshell”;
100—mildly shiny;
1000—really glossy;
10,000—nearlymirror-like.
*/

// we get the next ray to trace using the struct... can be a reflective ray, a refractive ray or just a diffuse scatter ray..
struct ScatterRec {
    Ray ray_to_trace;
    Color local_color;
};

class Material {
    public:
        // Does either reflection or refraction based on the object type
        virtual ScatterRec scatter(const Ray& r_in, const HitRecord& rec) const = 0;

        // need for materials that emit lights
        virtual Color emitted(double u, double v, const Point3& p) const {
            return Color(0,0,0);
        }

        // used for the swtich statement => I added a way for reflection, for refraction and for emission
        virtual MatTypes type() const = 0;
    public:
        float ka, // ambience
        km, // reflectivity
        kd, // diffusion
        ks, p;   // specular highlights
};

// this is a matte material => it scatters ray in a random direction
class Matte : public Material {
    public:
        Matte(const Color& a) 
            : texture(make_shared<SolidColor>(a)) {
            ka = 0.1;
            kd = 0.6;
            km = 0.1;
            ks = 0.05;
            p = 10;
        }
        Matte(shared_ptr<Texture> a) : texture(a) {
            ka = 0.1;
            kd = 0.6;
            km = 0.1;
            ks = 0.05;
            p = 10;
        }
        
        virtual ScatterRec scatter(const Ray& r_in, const HitRecord& rec) const override {
            // scatter randomly 
            auto scatter_direction = rec.normal + random_unit_vector();

            ScatterRec res;
            res.ray_to_trace = Ray(rec.p, scatter_direction);
            res.local_color = texture->value(rec.u, rec.v, rec.p); // called the texture.value() function to set a texture
            return res;
        }

         virtual MatTypes type() const {
             return blinn_phong;
         } 

    public:
        shared_ptr<Texture> texture;
};

// this material reflects perfectly => mirror effect
class Metal : public Material {
    public:
        Metal(const Color& a) : texture(make_shared<SolidColor>(a))  {
            ka = 0.01;
            kd = 0.01;
            km = 0.75;
            ks = 0.1;
            p = 10000;
        }

        Metal(const Color& a, float _km) : texture(make_shared<SolidColor>(a))  {
            ka = 0.01;
            kd = 0.01;
            km = _km;
            ks = 0.1;
            p = 10000;
        }

        virtual ScatterRec scatter(const Ray& r, const HitRecord& rec) const override {
            ScatterRec res;
            Vec3 r_in = r.direction();
            r_in.normalize();
            Vec3 reflected = reflect(r_in, rec.normal);
            
            res.ray_to_trace = Ray(rec.p, reflected);
            res.local_color = texture->value(rec.u, rec.v, rec.p);

            /*
            if (res.ray_to_trace.direction().dot(rec.normal) <= 0) {
                rec.mat_ptr->km = 0; // Do not trace the ray as NaNs/Infinity as d.n === 0
            }
            */

            return res;
        }

        virtual MatTypes type() const {
            return blinn_phong;
        } 

    public:
        shared_ptr<Texture> texture;
};

// this material has a fuzzy reflection
class FuzzyMetal : public Material {
    public:
        FuzzyMetal(const Color& a, double f) 
        : texture(make_shared<SolidColor>(a)), fuzz(f < 1 ? f : 1) {
            ka = 0.1;
            kd = 0.4;
            km = f;
            ks = 0.9;
            p = 1000;
        }

        virtual ScatterRec scatter(const Ray& r, const HitRecord& rec) const override {
            ScatterRec res;
            Vec3 r_in = r.direction();
            r_in.normalize();
            Vec3 reflected = reflect(r_in, rec.normal);
            
            res.ray_to_trace = Ray(rec.p, reflected+ fuzz*random_in_unit_sphere());
            res.local_color = texture->value(rec.u, rec.v, rec.p);

            /*
            if (res.ray_to_trace.direction().dot(rec.normal) <= 0) {
                rec.mat_ptr->km = 0; // Do not trace the ray as NaNs/Infinity
            }*/

            return res;
        }

        virtual MatTypes type() const {
            return blinn_phong;
        } 

    public:
        shared_ptr<Texture> texture;
        double fuzz;
};

// pure internal reflection without taking into account Schilck approximation for reflection based on angle
// pure internal reflection when there cannot be refraction + angle based reflection
// this way we have a true GLASSY, WATER_DROP material
class Dielectric : public Material {
    public:
        Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        virtual ScatterRec scatter(const Ray& r, const HitRecord& rec) const override {
            ScatterRec res;
            res.local_color = Color(1.0, 1.0, 1.0);
            double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

            Vec3 unit_direction = r.direction();
            unit_direction.normalize();

            // Snell's Law to get the angles
            double cos_theta = fmin((-unit_direction).dot(rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

            // need to decide if we refract or reflect...
            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            Vec3 direction;
            // reflectance does Schlicks approximation to decide if total internal reflection or refraction...
            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refraction_ratio);

            res.ray_to_trace = Ray(rec.p, direction);
            return res;
        }


        virtual MatTypes type() const {
            return glassy;
        } 

    public:
        double ir; // Index of Refraction

    private:
        static double reflectance(double cosine, double ref_idx) {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1-ref_idx) / (1+ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1 - cosine),5);
        }
};

// material that can emit light => we have an area light kind of effect...
class DiffuseLight : public Material  {
    public:
        DiffuseLight(shared_ptr<Texture> a) : emit(a) {
            kd = 0.95; // add some shape
            ks = 0.9;
            p = 0.1;
        }
        DiffuseLight(Color c) : emit(make_shared<SolidColor>(c)) {
            kd = 0.95; // add some shape
            ks = 0.9;
            p = 0.1;
        }

        virtual ScatterRec scatter(const Ray& r, const HitRecord& rec) const override {
            ScatterRec res;
            return res; // never called
        }

        virtual Color emitted(double u, double v, const Point3& p) const override {
            return emit->value(u, v, p);
        }

        virtual MatTypes type() const {
            return light_emitter;
        } 

    public:
        shared_ptr<Texture> emit;
};
#endif