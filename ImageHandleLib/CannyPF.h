#ifndef _CANNY_PY_H_
#define _CANNY_PY_H_
#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

class CannyPF
{
public:
	CannyPF(void);
	~CannyPF(void);

	void cannyPF( cv::Mat &image, int gausHalfSize, float VMGradient, cv::Mat &edgeMap );

};
#endif //_CANNY_PY_H_
