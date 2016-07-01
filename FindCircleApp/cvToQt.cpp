#include "cvToQt.h"
#include <QDebug>

/***************************************************************************
* 函数名称：   Mat2QImage
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Mat & mat
* 返回值　：   QT_NAMESPACE::QImage
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
QImage Mat2QImage(const cv::Mat& mat)
{
	// 8-bits unsigned, NO. OF CHANNELS = 1  
	if (mat.type() == CV_8UC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		// Set the color table (used to translate colour indexes to qRgb values)  
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		// Copy input Mat  
		uchar *pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	// 8-bits unsigned, NO. OF CHANNELS = 3  
	else if (mat.type() == CV_8UC3)
	{
		// Copy input Mat  
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat  
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if (mat.type() == CV_8UC4)
	{
		qDebug() << "CV_8UC4";
		// Copy input Mat  
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat  
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else
	{
		qDebug() << "ERROR: Mat could not be converted to QImage.";
		return QImage();
	}
}

/***************************************************************************
* 函数名称：   QImage2Mat
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  QImage image
* 返回值　：   cv::Mat
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
cv::Mat QImage2Mat(QImage image)
{
	cv::Mat mat;
	qDebug() << image.format();
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
		cv::cvtColor(mat, mat, CV_BGR2RGB);
		break;
	case QImage::Format_Indexed8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	}
	return mat;
}

/***************************************************************************
* 函数名称：   cvMat2QImage
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const CvMat & mat
* 返回值　：   QT_NAMESPACE::QImage
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
QImage cvMat2QImage(const CvMat& mat)
{
	return Mat2QImage(cv::Mat(&mat));
}

/***************************************************************************
* 函数名称：   QImage2cvMat
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  QImage image
* 返回值　：   CvMat
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
CvMat QImage2cvMat(QImage image)
{
	return QImage2Mat(image);
}

/***************************************************************************
* 函数名称：   IplImage2QImage
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const IplImage & img
* 返回值　：   QT_NAMESPACE::QImage
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
QImage IplImage2QImage(const IplImage& img)
{
	return Mat2QImage(cv::Mat(&img));
}

/***************************************************************************
* 函数名称：   QImage2cvIplImage
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  QImage image
* 返回值　：   IplImage
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
IplImage QImage2cvIplImage(QImage image)
{
	return QImage2Mat(image);
}
