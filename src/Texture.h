#ifndef TEXTURE_H
#define TEXTURE_H

#include "utility.h"
#include "Color.h"

/*
Defines a texture abstract class which takes the coordinates and the point of intersection to return a color

I only implemented away to add a checkerboard to a rectangle...
*/

class Texture {
    public:
        virtual Color value(double u, double v, const Vec3& p) const = 0;
};

// materials that only have one color value
// just a layer on Color so we can manipulate even single colors as textues
class SolidColor : public Texture {
    public:
        SolidColor() {}
        SolidColor(Color c) : color_value(c) {}

        SolidColor(double red, double green, double blue)
          : color_value(Color(red,green,blue)) {}

        virtual Color value(double u, double v, const Vec3& p) const override {
            return color_value;
        }

    private:
        Color color_value;
};

class RectCheckerTexture : public Texture {
    public:
        RectCheckerTexture() {}

        RectCheckerTexture(shared_ptr<Texture> _even, shared_ptr<Texture> _odd,  int _width, int _height, int _nx, int _ny)
            : even(_even), odd(_odd), width(_width), height(_height), nx(_nx), ny(_ny) {}

        RectCheckerTexture(Color c1, Color c2, int _width, int _height, int _nx, int _ny)
            : even(make_shared<SolidColor>(c1)) , odd(make_shared<SolidColor>(c2)), width(_width), height(_height), nx(_nx), ny(_ny) {}


        virtual Color value(double u, double v, const Vec3& p) const override {
            // u and v is between 0 and 1
            // u * width is the point on the width
            // v*height is the point on the height
            // we know that the width is divided into nx line segments
            // we know that the height is dividing into ny line segments
            // we find the length of each line segment along the width by width / nx
            // we find the length of each line segment along the height by height / ny

            // we then see in which line segment u*width is by u*width/length_one_line_segment_x
            // we then see in which line segment u*width is by v*height/length_one_line_segment_y

            // a checkerboard is such that if the point is in an even line segment along the x and an odd line segment on the y, we color it black
            // if odd line segment on along x and even along y we color it black,
            // else we color it white

            // to be in a checker pattern => 
            float length_one_line_segment_x = width  / nx;
            float length_one_line_segment_y = height / ny;

            int which_segment_x = static_cast<int>(u*width/length_one_line_segment_x);
            int which_segment_y = static_cast<int>(v*height/length_one_line_segment_y);

            if ( which_segment_x % 2 == 0 && which_segment_y %2 != 0) {
                return odd->value(u, v, p);
            }
            if ( which_segment_x % 2 != 0 && which_segment_y %2 == 0) {
                return odd->value(u, v, p);
            }

            return even->value(u, v, p);
        }


    public:
        shared_ptr<Texture> odd;
        shared_ptr<Texture> even;
        int nx, ny;
        float width, height;

};

#endif