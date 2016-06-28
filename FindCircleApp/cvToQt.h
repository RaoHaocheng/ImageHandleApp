#ifndef CVTOQT_H
#define CVTOQT_H

#include "..\CameraCalibration\FindCircularMarker.h"
#include <QImage>

QImage Mat2QImage(const cv::Mat& mat);
cv::Mat QImage2Mat(QImage image);

QImage cvMat2QImage(const CvMat& mat);
CvMat QImage2cvMat(QImage image);

QImage IplImage2QImage(const IplImage& mat);
IplImage QImage2cvIplImage(QImage image);

#endif // !1