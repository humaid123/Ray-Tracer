#include "utility.h" // includes vec3 and ray
#include "Color.h"
#include "HittableList.h"
#include "Sphere.h"
#include "Camera.h"
#include "Material.h"
#include "aarect.h"
#include "Box.h"
#include <iostream>
#include "RayTracer.h"
#include "Image.h"
#include "BVH.h"

/*
// considering materials and reflectance
color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return background; //color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth-1);
}*/

class Scene {
public:
    LightSources light_sources;
    HittableList objects;
};

Scene cornell_box() {
    Scene scene;
    auto cube_side = 555; // can change the size of the box right here

    // light sources
    auto light = make_shared<DiffuseLight>(Color(1, 1, 1));
    
    // far away light
    Point3 light_position(80, 200, -800);
    shared_ptr<Hittable> light_object = make_shared<Sphere>(light_position, 20, light);
    scene.light_sources.add(make_shared<LightSource>(light_position, light_object));
    
    // light sphere inside the room
    Point3 third_light_position(80, 400, 350);
    shared_ptr<Hittable> third_light_object = make_shared<Sphere>(third_light_position, 50, light);
    scene.light_sources.add(make_shared<LightSource>(third_light_position, third_light_object));
    
    double size_light = cube_side/3;
    shared_ptr<xz_rect> light_rect = make_shared<xz_rect>(
            cube_side/2 - size_light/2, 
            cube_side/2 + size_light/2,  // x points from right to left, y points along vertical, z points deeper into the box..
            cube_side/2 - size_light/2, 
            cube_side/2 + size_light/2, 
            cube_side - 10, light);
    // center of the room
    Point3 second_light_position(
        (light_rect->x0 + light_rect->x1) / 2,
        cube_side - 10,
        (light_rect->z0 + light_rect->z1) / 2
    );
    scene.light_sources.add(make_shared<LightSource>(second_light_position, light_rect)); 
    
    // objects
    // colors the colors are actual B G R
    auto red = Color(0.3, 0.3, 0.85);
    auto green = Color(0.15, 0.65, 0.12);
    auto blue = Color(0.97, 0.63, 0.3);
    auto light_blue = Color(0.77, 0.43, 0.1);
    auto white = Color(0.9, 0.85, 0.9);
    auto black = Color(0, 0, 0);

    // Materials --- I made all materials have default ka, kd, km, ks, p that I wanted
    auto red_matte   = make_shared<Matte>(red);
    auto white_metal = make_shared<FuzzyMetal>(white, 0.1);
    auto green_matte = make_shared<Matte>(green);
    //auto black_metal = make_shared<Metal>(black);
    auto blue_metal = make_shared<Metal>(blue, 0.1); 
    auto lime_fuzzy = make_shared<FuzzyMetal>(green, 0.5);
    auto blue_reflective = make_shared<Metal>(light_blue);

    // the sides
    // give (x0, y0) and (x1, y1)
    scene.objects.add(make_shared<yz_rect>(0, cube_side, 0, cube_side, cube_side, green_matte));
    scene.objects.add(make_shared<yz_rect>(0, cube_side, 0, cube_side, 0, red_matte));

    // roof
    scene.objects.add(make_shared<xz_rect>(0, cube_side, 0, cube_side, cube_side, white_metal));
    // back, I need the back to be a color the light sphere to show
    scene.objects.add(make_shared<xy_rect>(0, cube_side, 0, cube_side, cube_side, blue_metal));
    
    // box => give llc and urc
    scene.objects.add(make_shared<Box>(Point3(300, 200, 200), Point3(400, 365, 430), white_metal));

    // make floor textured
    auto num_squares_along_side = 20; // can change the grid pattern
    auto checker = make_shared<RectCheckerTexture>(white, black, cube_side, cube_side, num_squares_along_side, num_squares_along_side);
    scene.objects.add(make_shared<xz_rect>(0, cube_side, 0, cube_side, 0, make_shared<Matte>(checker)));


    // reflective metal sphere
    // I need a lighter blue so that we can see the reflections more clearly
    //auto reflective_metal = make_shared<metal>(color(0, 0, 0.25));
    scene.objects.add(make_shared<Sphere>(Point3(150, 100, 400), 120, blue_reflective ));

    // glass sphere
    auto glass = make_shared<Dielectric>(1.5); // refraction -> basically glass
    scene.objects.add(make_shared<Sphere>(Point3(385, 80, 195), 80, glass));

    // small sphere / mesh 
    scene.objects.add(make_shared<Sphere>(Point3(90, 40, 60), 40, lime_fuzzy));
    return scene;
}

int main() {
    // Image
    const auto aspect_ratio = 1.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 15; // this needs to be a very big number to remove graininess, Peter used 10000, we need to consider bvh and cuda
    const int max_depth = 10;

    // World
    Scene scene = cornell_box();

    // Camera
    // original
    Point3 lookfrom(278, 278, -800);
    Point3 lookat(278, 278, 0);

    // CAN CHANGE CAMERA POSITION -- 
    //Point3 lookfrom(50, 400, -800);
    //Point3 lookat(378, 178, 0);

    Vec3 view_direction = -(lookat - lookfrom);
    Vec3 vup(0,1,0);
    float focus_dist = 1.0;

    // some ambient light
    //Color background(0.2, 0.2, 0.2);
    Color background(0, 0, 0);

    Camera cam(lookfrom, lookat, vup, 40.0, aspect_ratio, 0.01, focus_dist);

    // Render
    // OpenCV (0, 0) is top-left = so I address the code by image(j, i)...
    Image image(image_height, image_width);

    // encapsulates blinn_phong, refraction, light object
    RayTracer raytracer(background, BVH(scene.objects), scene.light_sources); 

    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color(0, 0, 0); // JUST A Vec3 of floats

            // jittering antialiasing
            for (int p = 0; p < samples_per_pixel; p++) {
                for (int q = 0; q < samples_per_pixel; q++) {
                // generate ray at (u, v), random_double is in [0, 1)
                auto u = (i + (p + random_double())/samples_per_pixel ) / (image_width-1);
                auto v = (j + (q + random_double())/samples_per_pixel ) / (image_height-1);
                Ray r = cam.get_ray(u, v);
                pixel_color += raytracer.trace(r, max_depth); 
                }
            }

            RGB color = scale_color(pixel_color, samples_per_pixel * samples_per_pixel); // actual RGB values in uchar
            image(image_height - 1 - j, image_width - 1 -i) = color;
        }
    }
    image.display();
    image.save("result.png");
}
