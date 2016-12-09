#ifndef FINDCONTOURSMETHOD_H
#define FINDCONTOURSMETHOD_H

#include "define.h"

class DLLS_PORT FindContoursMethod
{
public:
	FindContoursMethod();
	~FindContoursMethod();

	// 梯度算子，目前先实现一个
	enum GradientOperator { PREWITT_OPERATOR = 101, SOBEL_OPERATOR = 102, SCHARR_OPERATOR = 103 };

	BOOL findContoursByEDPF(const cv::Mat& src, cv::Mat& dst, double smoothingSigma = 1.0);
	BOOL findContoursByEDPF(const cv::Mat& src,cv::Mat& dst);
	BOOL findContoursByED(const cv::Mat& src, cv::Mat& dst, const int GRADIENT_THRESH = 36, const int ANCHOR_THRESH = 1, const double smoothingSigma= 1);
	BOOL findContoursByCannyPF(const cv::Mat& src, cv::Mat& dst);
	BOOL findContoursByCannyLine(const cv::Mat& src, cv::Mat& dst);

protected:
	virtual BOOL isAnchor(int x, int y, const cv::Mat& gradMap, const BOOL direction, const int ANCHOR_THRESH = 8);
	
private:

};




#endif