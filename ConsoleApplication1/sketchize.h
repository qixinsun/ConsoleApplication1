#pragma once
#include <opencv2/highgui/highgui.hpp>
class sketchize
{
private:
	int strokeWidth;
	cv::Mat result;
public:
	sketchize();
	void setStrokeWidth(int w);
	int getStrokeWidth() const;
	cv::Mat process(const cv::Mat &image);
private:
	// remove black dots (upto 4x4 in size) of noise from a pure black & white image.
	void removePepperNoise(cv::Mat &mask);
};

