#ifndef FINDCONTOURSMETHOD_H
#define FINDCONTOURSMETHOD_H

#include "define.h"

class DLLS_PORT FindContoursMethod
{
public:
	FindContoursMethod();
	~FindContoursMethod();

	// �ݶ����ӣ�Ŀǰ��ʵ��һ��
	enum GradientOperator { PREWITT_OPERATOR = 101, SOBEL_OPERATOR = 102, SCHARR_OPERATOR = 103 };

	// edpf�㷨
	BOOL findContoursByEDPF(const cv::Mat& src, cv::Mat& dst, double smoothingSigma = 1.0);
	BOOL findContoursByEDPF(const cv::Mat& src,cv::Mat& dst);
	// ed�㷨
	BOOL findContoursByED(const cv::Mat& src, cv::Mat& dst, const int GRADIENT_THRESH = 36, const int ANCHOR_THRESH = 1, const double smoothingSigma= 1);
	// CannyPf�㷨
	BOOL findContoursByCannyPF(const cv::Mat& src, cv::Mat& dst);
	BOOL findContoursByCannyLine(const cv::Mat& src, cv::Mat& dst);
	// ������ͨ��
	BOOL findConnectedDomain(const cv::Mat& src, cv::Mat& dst, int& iTotal);

protected:
	virtual BOOL isAnchor(int x, int y, const cv::Mat& gradMap, const BOOL direction, const int ANCHOR_THRESH = 8);
	
private:

};




#endif