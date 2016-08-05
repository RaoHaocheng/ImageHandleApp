#include "FindCountersMethod.h"
#include "BaseImageHandleFunction.h"


/***************************************************************************
* �������ƣ�   FindContoursMethod
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/20      ���ǲ�        ���
***************************************************************************/
FindContoursMethod::FindContoursMethod()
{
}

/***************************************************************************
* �������ƣ�   ~FindContoursMethod
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/20      ���ǲ�        ���
***************************************************************************/
FindContoursMethod::~FindContoursMethod()
{

}

/***************************************************************************
* �������ƣ�   findContoursByEDPF
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Mat & src
* �Ρ�������   [out] cv::Mat & dst
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/20      ���ǲ�        ���
***************************************************************************/
BOOL FindContoursMethod::findContoursByEDPF(const cv::Mat& src, cv::Mat& dst)
{
//  	cv::Mat cvMatTemImg = src.clone();  // ����ͼƬ�������������
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

	// ʹ����ȿ����������������
	cv::Mat src_gray = src.clone();
 	cv::Mat grad_x, grad_y;
 	cv::Mat gray;
 	int scale = 1;
 	int delta = 0;
 	int ddepth = CV_16S;
 
 	// ���ͼƬû�����ݾͷ���
 	if (!src.data)
 		return FALSE;
 
 	// ��ͼƬ�����˹�˺���
 	GaussianBlur(src_gray, src_gray, cv::Size(7, 7), 0, 1.28, cv::BORDER_DEFAULT);
 
 	// ��������ͼƬת�ɻҶ�ͼ
 	//	cvtColor(src, src_gray, CV_BGR2GRAY);
 
 	cv::Mat tem_grad_x, tem_grad_y;
 
 	// �����ݶ�
 	/// Gradient X
 	Sobel(src_gray, tem_grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
 	/// Gradient Y
 	Sobel(src_gray, tem_grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
 
 	// ת����8λ�õ�ͼƬ
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