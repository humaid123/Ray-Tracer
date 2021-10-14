#include "utility.h" // includes vec3 and ray
#include "Color.h"
#include "HittableList.h"
#include "Sphere.h"
#include "Camera.h"
#include "Material.h"
#include "aarect.h"
#include "Box.h"
#include <iostream>
#include "Shader.h"
#include "Image.h"
#include "BVH.h"
#include "rotation.h"

void cornell_box(HittableList& objects, LightSources& lights) {
    auto cube_side = 555; // can change the size of the box right here
    
    // light sources
    auto light = make_shared<DiffuseLight>(create_color(255, 255, 255));    
    
    // far away light => required to add  reflections if other lights are behind the reflective sphere
    Point3 light_position(80, 200, -800);
    shared_ptr<Hittable> light_object = make_shared<Sphere>(light_position, 20, light);
    lights.add(light_object);
    
    // light sphere inside the room
    Point3 third_light_position(80, 370, 150);
    shared_ptr<Hittable> third_light_object = make_shared<Sphere>(third_light_position, 50, light);
    lights.add(third_light_object);
    
    /*
    // light rectangle
    double size_light = cube_side/3;
    shared_ptr<Hittable> light_rect = make_shared<xz_rect>(
            cube_side/2 - size_light/2, 
            cube_side/2 + size_light/2,  // x points from right to left, y points along vertical, z points deeper into the box..
            cube_side/2 - size_light/2, 
            cube_side/2 + size_light/2, 
            cube_side - 10, light);
    lights.add(light_rect);
    */

    // objects
    auto ue_red = create_color(193, 2, 6);
    auto carmine_red = create_color(165, 1, 19);
    auto floral_white = create_color(255, 251, 242);
    auto chinese_grey = create_color(223, 226, 219);
    auto raisin_black = create_color(33, 29, 33);
    auto rich_black = create_color(1, 10, 16);

    // Materials --- I made all materials have default ka, kd, km, ks, p that I wanted    
    auto glass = make_shared<Dielectric>(1.5); // refraction -> basically glass
    auto fuzzy_red = make_shared<FuzzyMetal>(ue_red, 0.2);
    auto metal_red = make_shared<FuzzyMetal>(carmine_red, 0.1);
    auto matte_white = make_shared<Matte>(floral_white);
    auto matte_black = make_shared<Matte>(rich_black);
    auto metal_grey = make_shared<Metal>(chinese_grey); 

    HittableList tmp; // temporary list to build up BVH
    
    // the sides => give range and k example => the plane x = k  is [y0, z0] [y1, z1] and k creates the plane
    tmp.add(make_shared<yz_rect>(0, cube_side, 0, cube_side, cube_side, matte_black));
    tmp.add(make_shared<yz_rect>(0, cube_side, 0, cube_side, 0, matte_white));
    // roof
    tmp.add(make_shared<xz_rect>(0, cube_side, 0, cube_side, cube_side, metal_grey));
    // back
    tmp.add(make_shared<xy_rect>(0, cube_side, 0, cube_side, cube_side, metal_red));
    
    // make floor textured
    auto num_squares_along_side = 20; // can change the grid pattern
    auto checker = make_shared<RectCheckerTexture>(floral_white, raisin_black, cube_side, cube_side, num_squares_along_side, num_squares_along_side);
    tmp.add(make_shared<xz_rect>(0, cube_side, 0, cube_side, 0, make_shared<Matte>(checker)));

    // reflective metal sphere
    tmp.add(make_shared<Sphere>(Point3(150, 100, 400), 120, metal_grey));    
    // glass sphere
    tmp.add(make_shared<Sphere>(Point3(385, 80, 195), 80, glass));
    // small Matte sphere
    tmp.add(make_shared<Sphere>(Point3(90, 40, 60), 40, fuzzy_red));

    // two small rotated boxes
    tmp.add(
        make_shared<rotate_x> (
            make_shared<rotate_y> (
                //make_shared<Box>(Point3(100, 350, 400), Point3(150, 400, 450), metal_red, matte_black),
                make_shared<Box>(Point3(200, 350, 350), Point3(250, 400, 400), metal_red, matte_black),
                30
            ),
            300
        )
    );

    tmp.add(
        make_shared<rotate_y> (
            make_shared<rotate_z> (
                make_shared<Box>(Point3(250, 360, 350), Point3(300, 420, 400), metal_red, matte_white),
                45
            ), 
            45
        )
    );

    // big box inside the room with one side checkered
    auto num_squares_along_box = 3; // can change the grid pattern
    auto box_size = 120;
    auto box_checker = make_shared<RectCheckerTexture>(floral_white, carmine_red, box_size, box_size, num_squares_along_box, num_squares_along_box);
    //auto x = 200, y = 400, z = 150;
    auto x = -20, y = 100, z = 350;
    tmp.add(
        make_shared<rotate_x>(
            make_shared<rotate_y> (
                make_shared<Box>(Point3(x, y, z), Point3(x + box_size, y+box_size, z+box_size), make_shared<Matte>(box_checker), fuzzy_red),
                45
            ),
            315
        )
    );

    objects.add(make_shared<BVH>(tmp));
}


int main(int, char**) {
    // Image
    const auto aspect_ratio = 1.0;
    const int image_width = 1000;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // use 3, 3, 3 with 400 width during presentation...
    const int samples_per_pixel = 7;
    const int max_depth = 5;
    const int num_sample_lights = 8;
    // OpenCV (0, 0) is top-left = so I address the code by image(j, i)...
    Image image(image_height, image_width);

    // Camera
    double field_of_view = 40.0; // can change how wide the camera view is => WE DO NOT PASS IN THE viewport width and height..
    Point3 lookfrom(278, 278, -800);
    Point3 lookat(278, 278, 0);

    // CAN CHANGE CAMERA POSITION and field of view
    // wide view from inside the room => can see the reflective material roof..
    // double field_of_view = 60.0;
    // Point3 lookfrom(150, 50, -200);
    // Point3 lookat(370, 350, 400);

    Vec3 view_direction = -(lookat - lookfrom);
    Vec3 vup(0,1,0);
    float focus_dist = 1.0;
    Camera cam(lookfrom, lookat, vup, field_of_view, aspect_ratio, focus_dist);

    // Scene
    Color background(0, 0, 0); // ambient light
    LightSources lights;
    HittableList objects;
    cornell_box(objects, lights);
    
    // encapsulates blinn_phong, refraction, light object => we no multiple iterations for each light source
    // we do even more iterations to add
    Shader shader(background, objects, lights, num_sample_lights); 

    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color(0, 0, 0); // JUST A Vec3 of floats

            // jittering antialiasing
            for (int p = 0; p < samples_per_pixel; p++) {
                for (int q = 0; q < samples_per_pixel; q++) {
                    // generate ray at (u, v), random_double is in [0, 1)
                    auto u = (i + (p + random_double())/samples_per_pixel ) / (image_width-1);
                    auto v = (j + (q + random_double())/samples_per_pixel ) / (image_height-1);
                    Ray r = cam.get_ray(u, v);
                    pixel_color += shader.trace(r, max_depth); 
                }
            }

            RGB color = scale_color(pixel_color, samples_per_pixel * samples_per_pixel); // actual RGB values in uchar
            image(image_height - 1 - j, image_width - 1 -i) = color;
        }
    }
    image.display();
    image.save("result.png");
}
