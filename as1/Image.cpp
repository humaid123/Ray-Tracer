#include "Image.h"
#include "Vec3.h"

cv::Vec3b &Image::operator()(int row, int col)
{
	assert(row >= 0 && row < _rows);
	assert(col >= 0 && col < _cols);
	return image.at<cv::Vec3b>(cv::Point(col, row));
}

void Image::display()
{
	/// Display the image
	cv::imshow("image", image);
	cv::waitKey(0);
}

void Image::save(const std::string &filename)
{
	/// Save the image
	cv::imwrite(filename, image);
}

/*
We let the camera do this.
Vec3 Image::generatePixelPos(int i, int j)
{
	float u = l + (r - l) * (i + 0.5f) / _cols;
	float v = b + (t - b) * (j + 0.5f) / _rows;
	return Vec3(u, v, -1);
}
*/