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

	// edpf算法
	BOOL findContoursByEDPF(const cv::Mat& src, cv::Mat& dst, double smoothingSigma = 1.0);
	BOOL findContoursByEDPF(const cv::Mat& src,cv::Mat& dst);
	// ed算法
	BOOL findContoursByED(const cv::Mat& src, cv::Mat& dst, const int GRADIENT_THRESH = 36, 
		const int ANCHOR_THRESH = 8, const double smoothingSigma= 1);
	BOOL findContoursByEDLib(const cv::Mat& src, cv::Mat& dst, const int GRADIENT_THRESH = 16,
		const int ANCHOR_THRESH = 0, const double smoothingSigma = 1);
	// CannyPf算法
	BOOL findContoursByCannyPF(const cv::Mat& src, cv::Mat& dst);
	BOOL findContoursByCannyLine(const cv::Mat& src, cv::Mat& dst);
	// 查找连通域
	BOOL findConnectedDomain(const cv::Mat& src, cv::Mat& dst, int& iTotal);

protected:
	enum DirectionType
	{
		Left = 0,
		Right = 1,
		Top = 2,
		Down = 3
	};

	virtual BOOL isAnchor(int x, int y, const cv::Mat& gradMap, 
		const BOOL direction, const int ANCHOR_THRESH = 4);
	virtual BOOL SmartRouting(const std::vector<ST_COORDINATE_INFO> anchorsInfo
		, const cv::Mat& gradMap, const cv::Mat& directionMap, const cv::Mat& edgeAreasMap, cv::Mat& dst);
	virtual BOOL SmartRouting(ST_COORDINATE_INFO anchors
		, const cv::Mat& gradMap, const cv::Mat& directionMap, const cv::Mat& edgeAreasMap, cv::Mat& dst);
	virtual BOOL OriginRouting(ST_COORDINATE_INFO anchors
		, const cv::Mat& gradMap, const cv::Mat& directionMap, const cv::Mat& edgeAreasMap, cv::Mat& dst
		, DirectionType direction);

	virtual BOOL GoLeft(ST_COORDINATE_INFO& anchors
		, const cv::Mat& gradMap, const cv::Mat& directionMap, const cv::Mat& edgeAreasMap, cv::Mat& dst,
		DirectionType& direction);
	virtual BOOL GoRight(ST_COORDINATE_INFO& anchors
		, const cv::Mat& gradMap, const cv::Mat& directionMap, const cv::Mat& edgeAreasMap, cv::Mat& dst,
		DirectionType& direction);
	virtual BOOL GoTop(ST_COORDINATE_INFO& anchors
		, const cv::Mat& gradMap, const cv::Mat& directionMap, const cv::Mat& edgeAreasMap, cv::Mat& dst, 
		DirectionType& direction);
	virtual BOOL GoDown(ST_COORDINATE_INFO& anchors
		, const cv::Mat& gradMap, const cv::Mat& directionMap, const cv::Mat& edgeAreasMap, cv::Mat& dst, 
		DirectionType& direction);

private:
	// 需要使用的一些常见变量
	const uchar HORIZONTAL = 0;
	const uchar VERTICAL = 255;
	const uchar EDGEL = 255;
};




#endif