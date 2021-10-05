#ifndef MATERIAL_H
#define MATERIAL_H

#include "utility.h"
#include "Hittable.h"
#include "Texture.h"
#include "Color.h"
// I have three types of materials => a blinn_phong/reflective, glassy/refractive, light_emitter/my light source is an object in the scene
enum MatTypes { blinn_phong, glassy, light_emitter };

struct ScatterRec {
    Ray ray_to_trace;
    Color local_color;
};

class Material {
    public:
        // Does either reflection or refraction based on the object type
        virtual ScatterRec scatter(const Ray& r_in, const HitRecord& rec) const = 0;

        // most material do not emit any light 
        // however some material can emit a diffuse light 
        // when that happens, we call the emit function of the provided material to know which color is emitted...
        virtual Color emitted(double u, double v, const Point3& p) const {
            return Color(0,0,0);
        }

        // used for the swtich statement => I added a way for reflection, for refraction and for emission
        virtual MatTypes type() const = 0;
    public:
        float ka, ks, kd, km, p;
};
// this is a matte material => it scatters ray in a random direction
class Matte : public Material {
    public:
        Matte(const Color& a) 
            : texture(make_shared<SolidColor>(a)) {
            ka = 0.1;
            kd = 0.2;
            km = 0.1;
            ks = 0.05;
        }
        Matte(shared_ptr<Texture> a) : texture(a) {
            ka = 0.1;
            kd = 0.2;
            km = 0.1;
            ks = 0.05;
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
            km = 1.0;
            ks = 0.1;
        }

        Metal(const Color& a, float _km) : texture(make_shared<SolidColor>(a))  {
            ka = 0.01;
            kd = 0.01;
            km = _km;
            ks = 0.1;
        }

        virtual ScatterRec scatter(const Ray& r, const HitRecord& rec) const override {
            ScatterRec res;
            Vec3 r_in = r.direction();
            r_in.normalize();
            Vec3 reflected = reflect(r_in, rec.normal);
            
            res.ray_to_trace = Ray(rec.p, reflected);
            res.local_color = texture->value(rec.u, rec.v, rec.p);

            if (res.ray_to_trace.direction().dot(rec.normal) <= 0) {
                rec.mat_ptr->km = 0; // Do not trace the ray as NaNs/Infinity
            }

            return res;
        }

        virtual MatTypes type() const {
            return blinn_phong;
        } 

    public:
        shared_ptr<Texture> texture;
};

// this material has a fuzzy reflection
// fuzz = 0 => perfect reflection
// fuzz = 1 => matte
class FuzzyMetal : public Material {
    public:
        FuzzyMetal(const Color& a, double f) 
        : texture(make_shared<SolidColor>(a)), fuzz(f < 1 ? f : 1) {
            ka = 0.1;
            kd = 0.1;
            km = 0.1;
            ks = 0.9;
        }

        virtual ScatterRec scatter(const Ray& r, const HitRecord& rec) const override {
            ScatterRec res;
            Vec3 r_in = r.direction();
            r_in.normalize();
            Vec3 reflected = reflect(r_in, rec.normal);
            
            res.ray_to_trace = Ray(rec.p, reflected+ fuzz*random_in_unit_sphere());
            res.local_color = texture->value(rec.u, rec.v, rec.p);

            if (res.ray_to_trace.direction().dot(rec.normal) <= 0) {
                rec.mat_ptr->km = 0; // Do not trace the ray as NaNs/Infinity
            }

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

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            Vec3 direction;

            // reflectance check allows reflectivity based  on the viewing angle
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
        DiffuseLight(shared_ptr<Texture> a) : emit(a) {}
        DiffuseLight(Color c) : emit(make_shared<SolidColor>(c)) {}

        virtual ScatterRec scatter(const Ray& r_in, const HitRecord& rec) const override {
            ScatterRec res;
            return res; // is never called
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