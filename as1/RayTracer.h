#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include "Hittable.h"
#include "Material.h"
#include "Ray.h"
#include "math.h"
#include <iostream>
#include "LightSources.h"



class RayTracer {
public:
    RayTracer(const Color& _background, const Hittable& _world, const LightSources& _light_sources) 
    : background(_background), light_sources(_light_sources), world(_world) {}; 

    Color trace(const Ray& r, int depth) {
        HitRecord rec;
        if (depth <= 0) return background;
        //std::cout << "trace reached\n";
        rec.t = infinity; // will change as it hits objects
        bool hit_world = world.hit(r, epsilon, rec.t, rec);
        bool hit_lights = light_sources.hit(r, epsilon, rec.t, rec);
        if (!hit_world) return background;

        switch(rec.mat_ptr->type()) {
            case blinn_phong: return perform_blinn_phong(r, rec, depth);  break;
            case glassy: return refract_ray(r, rec, depth); break;
            case light_emitter: return emit_light(r, rec, depth); break;
            default: return background;
        } 
    }

private:
    Color perform_blinn_phong(const Ray& r, const HitRecord& rec, int depth) {
        std::vector<Vec3> light_positions = light_sources.positions();

        ScatterRec srec = rec.mat_ptr->scatter(r, rec); 
        Color local = srec.local_color;
        Ray reflected_ray = srec.ray_to_trace;
        Vec3 view_vector = -r.direction(); 
        Vec3 normal = rec.normal;

        Color c = rec.mat_ptr->ka*local;
        HitRecord light_rec;
        // adding light value if it hits light a little bit => ARE LIGHTS...
        if (light_sources.hit(reflected_ray, 0, infinity, light_rec)) {
            c += (rec.mat_ptr->km + 0.15) * light_rec.mat_ptr->emitted(light_rec.u, light_rec.v, light_rec.p);
        }

        // performing blinn bhong
        Color toAdd = rec.mat_ptr->emitted(rec.u, rec.v, rec.normal); // we add if the material emits a little bit
        for (int i = 0; i < light_positions.size(); i++) {
            Point3 light_position = light_positions[i];
            Vec3 light_vector = light_position - rec.p;
            light_vector.normalize();
            
            Ray shadow_ray(rec.p, light_vector);
            HitRecord shadow_rec;
            if (world.hit(shadow_ray, epsilon, infinity, shadow_rec)) {
                continue; 
            }


            Vec3 half_vector = view_vector + light_vector;
            half_vector.normalize();

            // check if reflected ray hits a light source
            HitRecord light_rec;
            Color fromLight(0, 0, 0);
            if (light_sources.hit(reflected_ray, epsilon, infinity, light_rec)) {
                // reflected ray hits a light source so we just return the light from the light source
                fromLight = light_rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
            }

            // multiplication is item by item => we are scaling floats between 0 and 1 => we scale to 255 at the end
            toAdd += rec.mat_ptr->kd * local * std::max((float) 0.0, rec.normal.dot(light_vector)) 
            + rec.mat_ptr->ks * local * std::pow(std::max((float)0.0, rec.normal.dot(half_vector)), rec.mat_ptr->p)
            + rec.mat_ptr->km * trace(reflected_ray, depth-1)
            + rec.mat_ptr->ks * fromLight; // make even low km reflect so that light emitters can do their thing even on matte materials
        }
        // added based on too many light positions so we scale back
        // I dont think we need to scale toAdd = toAdd / light_positions.size();
        return c + toAdd;
    }

    Color refract_ray(const Ray& r, const HitRecord& rec, int depth) {
        ScatterRec srec = rec.mat_ptr->scatter(r, rec);
        Color local = srec.local_color;
        Ray refracted_ray = srec.ray_to_trace;    

        // need to use c_wise product NOT *
        return local.cwiseProduct(trace(refracted_ray, depth-1));
    }

    Color emit_light(const Ray& r, const HitRecord& rec, int depth) {
        Color emitted(0, 0, 0);
        std::vector<Vec3> light_positions = light_sources.positions();
        for (int i = 0; i < light_positions.size(); i++) {
            Point3 light_position = light_positions[i];
            Vec3 light_vector = light_position - rec.p;
            light_vector.normalize();
            emitted += rec.mat_ptr->kd * rec.mat_ptr->emitted(rec.u, rec.v, rec.p) * std::max((float) 0.0, rec.normal.dot(light_vector));
        }
        return emitted;
    }

private:
    const Color& background;
    const Hittable& world;
    const LightSources& light_sources;
};


#endif