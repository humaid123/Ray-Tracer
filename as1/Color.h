#ifndef COLOR_H
#define COLOR_H

#include "Vec3.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"

#include <iostream>
using Color = Vec3;  // vec3 from 0 to 1 => this is the color we manipulate until the end

// when it comes time to store into the Image, we store as a char[], it is called cv::Vec3b

// emulating cv Vec3b
using RGB = cv::Vec3b; // RGB Value
/*
class RGB {
    public: 
        RGB() {}

        RGB(unsigned char _r, unsigned char _g, unsigned char _b) : r(_r), g(_g), b(_b) {}

    public:
        unsigned char r, g, b;
};*/



/*
color is a vec3 because of the 'using' clause

we add some helper functions to make the color easier to generate

WE KEEP COLORS between 0 and 1 until we store them into the matrix

this makes it so we can apply Blinn-Phong without casting to int until the last moment

*/

/** with anti aliasing*/
inline double clamp(double x) {
    if (x < 0.000) return 0;
    if (x > 0.999) return 0.999;
    return x;
}
/*
void write_color(std::ostream &out, Color pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

   // gamma - 2 correction
    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r)) << ' '
        << static_cast<int>(256 * clamp(g)) << ' '
        << static_cast<int>(256 * clamp(b)) << '\n';
}
*/
// HUMAID START

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

/*
// WE WILL NOT NEED THIS WHEN WE CHANGE TO OPENCV....
void my_write_color(std::ostream &out, RGB pixel_color) {
    //std::cout << "reached" << " " << pixel_color.x() << " " << pixel_color.y();
    out << static_cast<int>(pixel_color.r) << ' '
        << static_cast<int>(pixel_color.g) << ' '
        << static_cast<int>(pixel_color.b) << '\n';
}*/

#endif