#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "Vec3.h"

class Image
{
public:
	Image(){};
	Image(unsigned xRes, unsigned yRes) : _cols(xRes), _rows(yRes){};

	cv::Vec3b &operator()(int row, int col);

	/// Displays the image and pauses program execution (until key pressed)
	void display();

	/// Use files with "*.png" extension
	void save(const std::string &filename);

	unsigned rows() { return _rows; }
	unsigned cols() { return _cols; }

	Vec3 generatePixelPos(int, int);

private:
	/// Image resolution
	unsigned _cols = 100;
	unsigned _rows = 100;
	int r = -1, l = 1, t = 1, b = -1;

	///Image matrix - 8 bit depth unsigned with 3 channels
	cv::Mat image = cv::Mat(_rows, _cols, CV_8UC3, cv::Scalar(255, 255, 255));
};
