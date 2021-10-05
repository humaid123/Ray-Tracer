
#ifndef IMAGE_H
#define IMAGE_H

/*
#pragma once
#include "opencv2/core/core.hpp"       
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"

this class is just an abstraction of the one from openCV

we give it the image height and width in number of pixels
we also give it the positions of the image plane

It is also capable of generating the (u,v) position of a pixel on the plane
the main loop does the random sampling => it just gets an (i, j) based on the x-y coordinates
and computes a (u, v) based on the position of the rectangle...



openCV stores colors as unsigned chars array

we manipulate colors using a float from 0 to 1
then when it is time to store a color, we use the RGB struct...
*/


#include "string.h"
#include "Vec3.h"
#include <vector>
#include "Color.h"
#include <iostream>


class Image
{
public:
	Image() {};
	Image(const int& xRes, const int& yRes) : cols(xRes), rows(yRes) {
        for (int i = 0; i < xRes; i++) {
            image.push_back( std::vector<RGB>(yRes) ); 
            for (int j = 0; j < yRes; j++) {
                image[i].push_back(RGB(0, 0, 0));
            }
        }
    };

    RGB operator()(int row, int col) {
        /*
	    assert(row >= 0 && row < rows);
	    assert(col >= 0 && col < cols);
	    return image.at<cv::Vec3b>(cv::Point(col, row));*/
        return image[row][col];
    }  

    void update(int row, int col, RGB c) {
        image[row][col] = c;
    }

    void save() {
        std::cout << "P3\n" << rows << ' ' << cols << "\n255\n";
        for (int j = cols-1; j >= 0; --j) {
            for (int i = 0; i < rows; ++i) {
                my_write_color(std::cout, image[i][j]);
            }
        }
    }


private:
    int cols, rows;
    std::vector< std::vector<RGB> > image; 
};



#endif