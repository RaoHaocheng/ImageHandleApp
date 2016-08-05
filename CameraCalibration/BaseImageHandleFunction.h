#ifndef BASEIMAGEHANDLEFUNCTION_H
#define BASEIMAGEHANDLEFUNCTION_H

// ���ݽṹ������Լ���OpenCV���б���ʹ�õ�����
#include "define.h"

// Otsu��ֵ�ָ��
DLLS_PORT int  Otsu(IplImage* src);

// ��ì�ܷ�����
DLLS_PORT void  Get3DPos(double* pdbQ, double* pdb2D, double *pdbSolution);

// ��˹����
DLLS_PORT IplImage*  AddGaussianNoise(IplImage* src, double dbVariance);
DLLS_PORT IplImage*  AddGaussianNoise(IplImage* src, double dbMean, double dbVariance = GAUSSIAN_VARIANCE);


// ����ͼƬ�ݶ�
DLLS_PORT void GradientEstimation(IplImage* src, cv::Mat &grad_x, cv::Mat &grad_y, BOOL bShow = false);
DLLS_PORT void GradientEstimation(cv::Mat src, cv::Mat &grad_x, cv::Mat &grad_y, BOOL bShow = false);
DLLS_PORT void GradientEstimation(cv::Mat src, cv::Mat &dst);                                       

// �������õ�Բ�ĺͰ뾶
DLLS_PORT void GetCircle(const cv::Point& p1, const cv::Point& p2,
						 const cv::Point& p3, cv::Point2d& center, double& radius);
// �õ�����֮��ľ���
DLLS_PORT double GetDistance(const cv::Point2f p1, const cv::Point2f p2);

// ����Ƿ���Բ��
DLLS_PORT BOOL IsInCircle(const cv::Point point, const ST_CENTER center, const double dbPrecision);

// ����Ƿ���Բ��
DLLS_PORT BOOL IsInCircle(const std::vector<cv::Point> contour, std::vector<ST_CENTER>& center, const double dbPrecision);

// ���������ĳ���
DLLS_PORT double CalculateVectorDistance(const cv::Point2f twoPointVector);

// �������ݵķ�������
DLLS_PORT double CalculateGradientDrectAngle(const cv::Point2f grad,const cv::Point2f twoPointVector);

// ������������
DLLS_PORT double DotProduct(const cv::Point2f p1, const cv::Point2f p2);

// ��ĳ����й�һ��
DLLS_PORT BOOL NormalizerPoint(const cv::Point2f p1, cv::Point2f& p2);

// ������������
DLLS_PORT BOOL DetectIsoscelesTriangles(const cv::Mat src, const cv::Point2f p1, const cv::Point2f& p2);
DLLS_PORT BOOL DetectIsoscelesTriangles(const cv::Mat grad_x, const cv::Mat grad_y,
	const cv::Point2f p1, const cv::Point2f p2,
	const double dbDifPrecision = 1, const double dbDotPrecision = 1);

// �����
DLLS_PORT BOOL AnalysisCluster(const std::vector<cv::Point> contour, const cv::Mat grad_x,
	const cv::Mat grad_y, const double dbDifPrecision = 0.2, const double dbDotPrecision = 1);

// �Ƿ���
DLLS_PORT BOOL IsThreePointInLine(const cv::Point2f p1, const cv::Point2f p2, const cv::Point2f p3
	, const double dbDifPrecision = 0.2);

// ��Բ
DLLS_PORT void ConvertClosedContour(std::vector<cv::Point> &contours);

// ����ȱʧ��
DLLS_PORT BOOL FindNick(std::vector<cv::Point> &contours);

// rgba ת yuv
DLLS_PORT BOOL ConvertRgbToYuv(BYTE* src, BYTE* dst,const int width,const int height);

// ���ƶ���λ���ϻ�ԲȦ
DLLS_PORT BOOL DrawCircle(cv::Mat& src, cv::Mat& keypointsImage, std::vector<ST_CENTER> centers);

// ���㷽��
DLLS_PORT double Variance(std::vector<double> src);

// ϸ��
DLLS_PORT cv::Mat thinImage(const cv::Mat & src, const int maxIterations = -1);

// ���컯
DLLS_PORT double DifferntPoint(const std::vector<double> src, const double dbDotPrecision = 1);

// �Զ�canny����
DLLS_PORT void AutoCanny(const cv::Mat & src, cv::Mat & dst);


#endif // !BASEIMAGEHANDLEFUNCTION_H
