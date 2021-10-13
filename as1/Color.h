#ifndef COLOR_H
#define COLOR_H

#include "Vec3.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include <iostream>

/*
 this file contains a bunch of utility functions require to use colors
 we note that opencv takes bgr
 we want to use numbers from 0 to 1 in Blinn Phong
 we also add gamma correction to make the image appear brighter -> sqrt a number less than 0...
*/
using Color = Vec3;  // vec3 from 0 to 1 => this is the color we manipulate until the end
using RGB = cv::Vec3b; // RGB Value

Color create_color(float r, float g, float b) {
    return Vec3(
        b / 255.0,
        g / 255.0,
        r / 255.0
    );
}

inline double clamp(float x) {
    if (x < 0.000) return 0;
    if (x > 0.999) return 0.999;
    return x;
}

RGB scale_color(Color pixel_color, int samples_per_pixel) {
    // gamma - 2 correction
    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    auto r = sqrt(scale * pixel_color.x());
    auto g = sqrt(scale * pixel_color.y());
    auto b = sqrt(scale * pixel_color.z());


    //std::cout << "1 scale " << r << " " << g << " " << b << "\n";
    auto c = RGB(
        (256 * clamp(r)),
        (256 * clamp(g)),
        (256 * clamp(b))
    );

    //std::cout << "2 scale " << c.x() << " " << c.y() << " " << c.z() << "\n";
    return c;
}

#endif