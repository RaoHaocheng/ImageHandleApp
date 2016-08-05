#ifndef BASEIMAGEHANDLEFUNCTION_H
#define BASEIMAGEHANDLEFUNCTION_H

// 数据结构定义库以及在OpenCV库中必须使用的内容
#include "define.h"

// Otsu阈值分割函数
DLLS_PORT int  Otsu(IplImage* src);

// 解矛盾方程组
DLLS_PORT void  Get3DPos(double* pdbQ, double* pdb2D, double *pdbSolution);

// 高斯噪声
DLLS_PORT IplImage*  AddGaussianNoise(IplImage* src, double dbVariance);
DLLS_PORT IplImage*  AddGaussianNoise(IplImage* src, double dbMean, double dbVariance = GAUSSIAN_VARIANCE);


// 计算图片梯度
DLLS_PORT void GradientEstimation(IplImage* src, cv::Mat &grad_x, cv::Mat &grad_y, BOOL bShow = false);
DLLS_PORT void GradientEstimation(cv::Mat src, cv::Mat &grad_x, cv::Mat &grad_y, BOOL bShow = false);
DLLS_PORT void GradientEstimation(cv::Mat src, cv::Mat &dst);                                       

// 随机三点得到圆心和半径
DLLS_PORT void GetCircle(const cv::Point& p1, const cv::Point& p2,
						 const cv::Point& p3, cv::Point2d& center, double& radius);
// 得到两点之间的距离
DLLS_PORT double GetDistance(const cv::Point2f p1, const cv::Point2f p2);

// 检查是否在圆上
DLLS_PORT BOOL IsInCircle(const cv::Point point, const ST_CENTER center, const double dbPrecision);

// 检查是否在圆上
DLLS_PORT BOOL IsInCircle(const std::vector<cv::Point> contour, std::vector<ST_CENTER>& center, const double dbPrecision);

// 计算向量的长度
DLLS_PORT double CalculateVectorDistance(const cv::Point2f twoPointVector);

// 计算数据的方向数据
DLLS_PORT double CalculateGradientDrectAngle(const cv::Point2f grad,const cv::Point2f twoPointVector);

// 两个坐标点相乘
DLLS_PORT double DotProduct(const cv::Point2f p1, const cv::Point2f p2);

// 对某点进行归一化
DLLS_PORT BOOL NormalizerPoint(const cv::Point2f p1, cv::Point2f& p2);

// 侦查等腰三角形
DLLS_PORT BOOL DetectIsoscelesTriangles(const cv::Mat src, const cv::Point2f p1, const cv::Point2f& p2);
DLLS_PORT BOOL DetectIsoscelesTriangles(const cv::Mat grad_x, const cv::Mat grad_y,
	const cv::Point2f p1, const cv::Point2f p2,
	const double dbDifPrecision = 1, const double dbDotPrecision = 1);

// 族分析
DLLS_PORT BOOL AnalysisCluster(const std::vector<cv::Point> contour, const cv::Mat grad_x,
	const cv::Mat grad_y, const double dbDifPrecision = 0.2, const double dbDotPrecision = 1);

// 是否共线
DLLS_PORT BOOL IsThreePointInLine(const cv::Point2f p1, const cv::Point2f p2, const cv::Point2f p3
	, const double dbDifPrecision = 0.2);

// 补圆
DLLS_PORT void ConvertClosedContour(std::vector<cv::Point> &contours);

// 查找缺失点
DLLS_PORT BOOL FindNick(std::vector<cv::Point> &contours);

// rgba 转 yuv
DLLS_PORT BOOL ConvertRgbToYuv(BYTE* src, BYTE* dst,const int width,const int height);

// 在制定的位置上画圆圈
DLLS_PORT BOOL DrawCircle(cv::Mat& src, cv::Mat& keypointsImage, std::vector<ST_CENTER> centers);

// 计算方差
DLLS_PORT double Variance(std::vector<double> src);

// 细化
DLLS_PORT cv::Mat thinImage(const cv::Mat & src, const int maxIterations = -1);

// 差异化
DLLS_PORT double DifferntPoint(const std::vector<double> src, const double dbDotPrecision = 1);

// 自动canny算子
DLLS_PORT void AutoCanny(const cv::Mat & src, cv::Mat & dst);


#endif // !BASEIMAGEHANDLEFUNCTION_H
