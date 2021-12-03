#ifndef SHADER_H
#define SHADER_H

#include "Hittable.h"
#include "Material.h"
#include "Ray.h"
#include "math.h"
#include <iostream>
#include <vector>
#include "LightSources.h"

/*
As per the book, I define a shader but also make it do the ray intersection code

the shader gets the best intersection, looks at the material type
and based on the material type runs a blinn_phong, light emission or ray refraction routine to get a color
*/

class Shader
{
public:
    Shader(const Color &_background, const Hittable &_world, const LightSources &_light_sources, int _num_light_samples)
        : background(_background), light_sources(_light_sources), world(_world), num_light_samples(_num_light_samples) {
        light_positions = light_sources.generate_random_positions(num_light_samples);
    }

    Color trace(const Ray &r, int depth)
    {
        HitRecord rec;
        if (depth <= 0)
            return background;

        if (!world.hit(r, epsilon, infinity, rec))
            return background;
        
        light_sources.hit(r, epsilon, rec.t, rec); // if there is a hit, the light emit code below will be run...

        switch (rec.mat_ptr->type())
        {
        case blinn_phong:
            return perform_blinn_phong(r, rec, depth);
            break;
        case glassy:
            return refract_ray(r, rec, depth);
            break;
        case light_emitter:
            return emit_light(r, rec, depth);
            break;
        default:
            return background;
        }
    }

private:
    Color perform_blinn_phong(const Ray &r, const HitRecord &rec, int depth)
    {
        ScatterRec srec = rec.mat_ptr->scatter(r, rec);
        Color local = srec.local_color;
        Ray reflected_ray = srec.ray_to_trace;
        Vec3 view_vector = -r.direction();
        Vec3 normal = rec.normal;

        Color c = rec.mat_ptr->ka * local;

        // performing blinn bhong
        Color toAdd = rec.mat_ptr->emitted(rec.u, rec.v, rec.normal); // we add if the material emits a little bit
        for (const auto& light_position : light_positions) {
            Vec3 light_vector = light_position - rec.p;
            light_vector.normalize();

            Ray shadow_ray(rec.p, light_vector);
            HitRecord shadow_rec;
            if (world.hit(shadow_ray, epsilon, infinity, shadow_rec)) continue;

            Vec3 half_vector = view_vector + light_vector;
            half_vector.normalize();
            
            // multiplication is item by item => we are scaling floats between 0 and 1 => we scale to 255 at the end
            toAdd += rec.mat_ptr->kd * local * std::max((float)0.0, rec.normal.dot(light_vector)) // diffusion
                    + rec.mat_ptr->ks * local * std::pow(std::max((float)0.0, rec.normal.dot(half_vector)), rec.mat_ptr->p); // specular highlights
        }

        return c 
                + (toAdd / (light_positions.size()))  
                + rec.mat_ptr->km * trace(reflected_ray, depth - 1); // reflection does not depend on light position, only on material scatter
    }

    Color refract_ray(const Ray &r, const HitRecord &rec, int depth)
    {
        ScatterRec srec = rec.mat_ptr->scatter(r, rec);
        Color local = srec.local_color;
        Ray refracted_ray = srec.ray_to_trace;

        // need to use c_wise product NOT *
        return local.cwiseProduct(trace(refracted_ray, depth - 1));
    }

    Color emit_light(const Ray &r, const HitRecord &rec, int depth) {
        Color emitted(0, 0, 0);
        Vec3 view_vector = -r.direction();
        for (const auto& light_position : light_positions) {
            Vec3 light_vector = light_position - rec.p;
            light_vector.normalize();
        
            // give it some shape.
            emitted += rec.mat_ptr->kd 
                * rec.mat_ptr->emitted(rec.u, rec.v, rec.p) 
                * ( 1 - std::max((float) 0.0, rec.normal.dot(light_vector)) );

            
            Vec3 half_vector = view_vector + light_vector;

            emitted += rec.mat_ptr->ks *  
                        rec.mat_ptr->emitted(rec.u, rec.v, rec.p) * 
                        std::pow(std::max((float)0.0, rec.normal.dot(half_vector)), rec.mat_ptr->p);
        }
        return emitted / (light_positions.size());
        
        // can just do this if no shape -> return rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    }

private:
    const Color &background;
    const Hittable &world;
    const LightSources &light_sources;
    std::vector<Point3> light_positions;
    const int num_light_samples;
};

#endif