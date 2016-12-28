//查找圆形标记点的类，主要是封装了OPENCV中的几个类
#ifndef FINDCIRCULARMARKER_H
#define FINDCIRCULARMARKER_H

// 数据结构定义库以及在OpenCV库中必须使用的内容
#include "define.h"                           // 一些基本的函数
#include "BaseImageHandleFunction.h"          // 一些基本的函数

// 用于找圆形标记点的方法,是在标定的基础上找圆，但是也可以脱离标定，这个地方以后需要修改
class DLLS_PORT FindCircularMarker
{
public:
	FindCircularMarker();
	~FindCircularMarker();
	void SetFileName(const char* fileName);                                           // 保存文件名

	// 找圆
	BOOL FindCircleCenter(IplImage* pImg,
		cv::SimpleBlobDetector::Params params,
		std::vector<cv::KeyPoint>& vct_ponits, BOOL bShow = FALSE);// OpenCv提供找圆
	BOOL FindCircle(cv::Mat& cvMatimage, cv::SimpleBlobDetector::Params params,
		std::vector<ST_CENTER>& vct_ponits, BOOL bShow = FALSE);

	// 三点找圆
	BOOL FindCircleByThreePoint(const std::vector<cv::Point> counter,
		ST_CENTER& center, const double dbPrecision = 0.1, const UINT iMinSuccessNum = 1, const UINT iMaxTestNum = 10);         // 通过三点确定一个圆
	BOOL FindCircleByThreePoint(const std::vector<cv::Point> counter,
		std::vector<ST_CENTER>& centers, const double dbPrecision = 0.1, const double dbMinConfidence = 0.3,
		const UINT iMinSuccessNum = 1, const UINT iMaxTestNum = 1000);                                                          // 通过三点确定一个圆
	BOOL FindCircleByThreePoint(IplImage*pImg, std::vector<ST_CENTER>& vct_ponits,
		BOOL bShow = FALSE, const double dbPrecision = 0.1, const UINT iMinSuccessNum = 1, const UINT iMaxTestNum = 10);        // 通过三点确定一个圆
	BOOL FindCircleByThreePoint(const cv::Mat& cvMatimage, std::vector<ST_CENTER>& vct_ponits,
		BOOL bShow = FALSE, const double dbPrecision = 0.1, const UINT iMinSuccessNum = 1, const UINT iMaxTestNum = 10);        // 通过三点确定一个圆

	// 论文中找圆的方法iTCID
	void CalCircleCentrePoint(IplImage* pImg, std::vector<cv::KeyPoint>& vct_ponits, int ipointscounts);   // 计算圆心坐标的内容
	void FindCircleBySamplingTriangles(IplImage* pImg, std::vector<ST_CENTER>& vct_ponits,
		BOOL bShow = FALSE);                                                                               // 论文中提供的找圆的方法
	void FindCircleBySamplingTriangles(const cv::Mat& cvMatimage, std::vector<ST_CENTER>& vct_ponits,
		BOOL bShow = FALSE);                                                                               // 论文中找圆的方法	
	void FindCircleBySamplingTrianglesImproved(IplImage* pImg, std::vector<ST_CENTER>& vct_ponits,
		BOOL bShow = FALSE);                                                                               // 论文中提供的找圆的方法
	void FindCircleBySamplingTrianglesImproved(const cv::Mat& cvMatimage, std::vector<ST_CENTER>& vct_ponits,
		BOOL bShow = FALSE);// 论文中找圆的方法

	// 我们提出的找圆算法
	// 通过EDPF边缘检测的CIC算法
	void FindCircleByCIC(const cv::Mat& cvMatImage,
		cv::SimpleBlobDetector::Params params, std::vector<ST_CENTER>& vct_ponits, double smoothingSigma = 1.0, BOOL bShow = FALSE);
	void FindCircleByCICImproved(const cv::Mat& cvMatImage,
		cv::SimpleBlobDetector::Params params, std::vector<ST_CENTER>& vct_ponits, double smoothingSigma = 1.0, BOOL bShow = FALSE);
	void FindCircleByCICImproved(const cv::Mat& cvMatimage, cv::SimpleBlobDetector::Params params,
		std::vector<ST_CENTER>& vct_ponits, BOOL bShow = FALSE);			// 我们的方法改进型

	// 基于二值化的找圆算法
	void FindCircleByThreadshold(const cv::Mat& cvMatimage, cv::SimpleBlobDetector::Params params,
		std::vector<ST_CENTER>& vct_points, BOOL bShow = FALSE);

	// 基于分水岭的找圆算法，不过已经废弃，原因是存在各种问题
	void FindCircleByWaterThed(const cv::Mat& cvMatimage, cv::SimpleBlobDetector::Params params,
		std::vector<ST_CENTER>& vct_ponits, BOOL bShow = FALSE);			// 我们的方法改进型

protected:
	// 提供给开发者需要的一些函数
	// 画圆
	void DrawCircle(IplImage* src, cv::Mat dst, std::vector<ST_CENTER>& vct_ponits, const char* saveFileName = NULL);
	// 提供显示图片需要使用的滚动条
	static void Trackbar_callback(int Pos, void* Usrdata);                                                 // 回调函数,用于显示进度条
	// 找圆方法
	BOOL FindCircle(IplImage* pImg, cv::SimpleBlobDetector::Params params,
		std::vector<ST_CENTER>& vct_ponits, BOOL bShow = FALSE);                                           // 修改后快速找圆方法
	// 重载函数，为了解决Mat的调用的问题
	BOOL FindCircleImproved(cv::Mat& cvMatimage, cv::SimpleBlobDetector::Params params,
		std::vector<ST_CENTER>& vct_ponits, BOOL bShow = FALSE);                                           // 重载函数，为了解决Mat的调用的问题
public:
	BOOL AccuaryCircleLocation(cv::Mat src, std::vector<ST_CENTER>& vct_ponits, BOOL bShow = FALSE);       // 精确的圆心定位

private:
	// 利用均方差检测是否是在正元
	BOOL CheckReliability(const std::vector<double> src,
		const double dbVarPrecision = 0.3, const double dbRaPrecision = 0.7);

private:
	static int m_siGobalThreHold;
	char* m_fileName;
};
#endif