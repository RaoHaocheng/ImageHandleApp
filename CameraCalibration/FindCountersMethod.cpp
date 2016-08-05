#include "FindCountersMethod.h"
#include "BaseImageHandleFunction.h"


/***************************************************************************
* 函数名称：   FindContoursMethod
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/20      饶智博        添加
***************************************************************************/
FindContoursMethod::FindContoursMethod()
{
}

/***************************************************************************
* 函数名称：   ~FindContoursMethod
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/20      饶智博        添加
***************************************************************************/
FindContoursMethod::~FindContoursMethod()
{

}

/***************************************************************************
* 函数名称：   findContoursByEDPF
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Mat & src
* 参　　数：   [out] cv::Mat & dst
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/20      饶智博        添加
***************************************************************************/
BOOL FindContoursMethod::findContoursByEDPF(const cv::Mat& src, cv::Mat& dst)
{
//  	cv::Mat cvMatTemImg = src.clone();  // 复制图片，以免出现问题
//   	IplImage* img = &IplImage(cvMatTemImg);
//   	IplImage *gaussianImg = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, img->nChannels);
//   	cvSmooth(img, gaussianImg, CV_GAUSSIAN, 7, img->nChannels);
//   
//  	dst = cv::Mat(gaussianImg);
// // 	cvReleaseImage(&gaussianImg);
//    	cv::Mat sharpenedLena;
//    	cv::Mat kernel = (cv::Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
// 	cv::filter2D(dst, sharpenedLena, dst.depth(), kernel);
//    	dst = sharpenedLena;

	// 使用深度拷贝，以免出现问题
	cv::Mat src_gray = src.clone();
 	cv::Mat grad_x, grad_y;
 	cv::Mat gray;
 	int scale = 1;
 	int delta = 0;
 	int ddepth = CV_16S;
 
 	// 如果图片没有内容就返回
 	if (!src.data)
 		return FALSE;
 
 	// 将图片加入高斯核函数
 	GaussianBlur(src_gray, src_gray, cv::Size(7, 7), 0, 1.28, cv::BORDER_DEFAULT);
 
 	// 将变异后的图片转成灰度图
 	//	cvtColor(src, src_gray, CV_BGR2GRAY);
 
 	cv::Mat tem_grad_x, tem_grad_y;
 
 	// 计算梯度
 	/// Gradient X
 	Sobel(src_gray, tem_grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
 	/// Gradient Y
 	Sobel(src_gray, tem_grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
 
 	// 转换成8位置的图片
 	convertScaleAbs(tem_grad_x, grad_x);
 	convertScaleAbs(tem_grad_y, grad_y);
 
 	addWeighted(grad_x, 0.5, grad_y, 0.5, 0, gray);

  	IplImage *pBinaryImg = cvCreateImage(cvGetSize(&IplImage(src_gray)), IPL_DEPTH_8U, (&IplImage(src_gray))->nChannels);
  	
  	int threshold = Otsu(&IplImage(gray));
  	cvThreshold(&IplImage(gray), pBinaryImg, threshold, 1, CV_THRESH_BINARY);
  
  	dst = cv::Mat(pBinaryImg).clone();
  
  	cvReleaseImage(&pBinaryImg);
 
 	dst = thinImage(dst);
	dst *= 255;
	return TRUE;
}