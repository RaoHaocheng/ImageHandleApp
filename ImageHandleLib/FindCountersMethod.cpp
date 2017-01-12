#include "FindCountersMethod.h"
#include "BaseImageHandleFunction.h"

#include "EDLib.h"
#include "EdgeMap.h"
#include "CannyLine.h"
#include "CannyPF.h"
#include "ChainFromEdgeMap.h"

#pragma comment(lib, "opencv_imgproc245.lib")   // OpenCV
#pragma comment(lib, "opencv_core245.lib")      // OpenCV - cxcore
#pragma comment(lib, "EDLib.lib")          // DetectEdgesByED, DetectEdgesByEDPF, DetectEdgesByCannySR, DetectEdgesByCannySRPF functions


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

	return FALSE;
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

// 	// ʹ����ȿ����������������
// 	cv::Mat src_gray = src.clone();
//  	cv::Mat grad_x, grad_y;
//  	cv::Mat gray;
//  	int scale = 1;
//  	int delta = 0;
//  	int ddepth = CV_16S;
 
//  	// ���ͼƬû�����ݾͷ���
//  	if (!src.data)
//  		return FALSE;
//  
//  	// ��ͼƬ�����˹�˺���
//  	GaussianBlur(src_gray, src_gray, cv::Size(7, 7), 0, 1.28, cv::BORDER_DEFAULT);
//  
//  	// ��������ͼƬת�ɻҶ�ͼ
//  	//	cvtColor(src, src_gray, CV_BGR2GRAY);
//  
//  	cv::Mat tem_grad_x, tem_grad_y;
//  
//  	// �����ݶ�
//  	/// Gradient X
//  	Sobel(src_gray, tem_grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
//  	/// Gradient Y
//  	Sobel(src_gray, tem_grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
//  
//  	// ת����8λ�õ�ͼƬ
//  	convertScaleAbs(tem_grad_x, grad_x);
//  	convertScaleAbs(tem_grad_y, grad_y);
//  
//  	addWeighted(grad_x, 0.5, grad_y, 0.5, 0, gray);
// 
//   	IplImage *pBinaryImg = cvCreateImage(cvGetSize(&IplImage(src_gray)), IPL_DEPTH_8U, (&IplImage(src_gray))->nChannels);
//   	
//   	int threshold = Otsu(&IplImage(gray));
//   	cvThreshold(&IplImage(gray), pBinaryImg, threshold, 1, CV_THRESH_BINARY);
//   
//   	dst = cv::Mat(pBinaryImg).clone();
//   
//   	cvReleaseImage(&pBinaryImg);
//  
//  	dst = thinImage(dst);
// 	dst *= 255;
// 	return TRUE;
}

BOOL FindContoursMethod::findContoursByED(const cv::Mat& src, cv::Mat& dst, 
										  const int GRADIENT_THRESH, const int ANCHOR_THRESH, 
										  const double smoothingSigma)
{
	// ����ʹ�õ���ED�㷨
	cv::Mat cvMatTemImg = src.clone();  // ����ͼƬ�������������
	cv::Mat src_gray;                   // �Ҷ�ͼ   

	// ��һ������ͼ����и�˹�˲�
	cv::Mat cvMatGaussianImg = src.clone();           // ��˹�˲����ͼƬ
	// ����������ʹ�ø�˹�ں�Ϊ5*5 ƫ��Ϊ1�ĸ�˹�˲�;����֤ʵ�������ƫ��Ϊ1,��õ�����Ĵ�
	cv::GaussianBlur(cvMatTemImg, cvMatGaussianImg, cv::Size(5, 5), 0, 0, cv::BORDER_DEFAULT);
	//cvSmooth(&IplImage(cvMatTemImg), &IplImage(cvMatGaussianImg), CV_GAUSSIAN, 5, 5,1);

	// ����ֻ����Ҷ�ͼ��
	if (CV_8UC1 != cvMatTemImg.type())
		cvtColor(cvMatGaussianImg, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatGaussianImg.clone();

	// �ڶ�����ͨ��Sobel���Ӽ����ݶ�
	cv::Mat tem_grad_x, tem_grad_y;
	cv::Mat grad_x, grad_y;
	cv::Mat grad_map, direction_map, edgeAreas_map; // �ݶ�ͼ������ͼ
	const int scale = 1;
	const int delta = 0;

	// �����ݶ�
	/// Gradient X
	Sobel(src_gray, tem_grad_x, CV_64F, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
	/// Gradient Y
	Sobel(src_gray, tem_grad_y, CV_64F, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);

	// ת����8λ�õ�ͼƬ
	convertScaleAbs(tem_grad_x, grad_x);
	convertScaleAbs(tem_grad_y, grad_y);

	// �γ��ݶ�ͼ������ͼ
	//addWeighted(grad_x, 1, grad_y, 1, 0, grad_map);
	grad_map.create(src_gray.size(), CV_16UC1);
	edgeAreas_map.create(src_gray.size(), src_gray.type());
	direction_map.create(src_gray.size(), src_gray.type());

	for (int i = 0; i < src_gray.rows; i++)
	{
		// ÿһ��ͼ���ָ��
		const uchar* grad_x_data = grad_x.ptr<uchar>(i);
		const uchar* grad_y_data = grad_y.ptr<uchar>(i);
	
		// �����ָ��
		uchar* direction_map_outData = direction_map.ptr<uchar>(i);
		uchar* edgeAreas_map_outData = edgeAreas_map.ptr<uchar>(i);
		ushort* grad_map_outData = grad_map.ptr<ushort>(i);

		for (int j = 0; j < src_gray.cols*src_gray.channels(); j++)
		{
			// ���㷽��ͼ
			if (grad_x_data[j] >= grad_y_data[j])
				direction_map_outData[j] = grad_x_data[j];
			else
				direction_map_outData[j] = grad_y_data[j];

			// �����ݶȵȼ���Ϣ
			grad_map_outData[j] =  grad_x_data[j] + grad_y_data[j]; // ��ֹ����������*0.5

			if (i == 56 && j== 20)
			{
				int x = 0;
				x++;
			}

			if (grad_map_outData[j] >= GRADIENT_THRESH)
				edgeAreas_map_outData[j] = 255;
			else
				edgeAreas_map_outData[j] = 0;
		}
	}

	// Step3:Extraction of the Anchors
	cv::Mat anchors_map = cv::Mat(src_gray.size(), src_gray.type(), double(0));
	const int iscanInterval = 4;                       // ��ɨ����

	for (int i = iscanInterval; i < grad_map.rows - 1; i = i + iscanInterval)// ����һ�п�ʼ��Anchors
	{
		// ÿһ��ͼ���ָ��
		const uchar* grad_x_data = grad_x.ptr<uchar>(i);

		// �����ָ��
		uchar* direction_map_outData = direction_map.ptr<uchar>(i);
		uchar* anchors_map_outData = anchors_map.ptr<uchar>(i);
		uchar* anchors_map_next_outData = anchors_map.ptr<uchar>(i+1);

		for (int j = iscanInterval; j < (grad_map.cols - 1)*grad_map.channels(); j = j + iscanInterval)
		{
			if (grad_x_data[j] == direction_map_outData[j])
			{
				if (isAnchor(i, j, grad_map, TRUE))
				{
					anchors_map_outData[j] = 255;
					anchors_map_next_outData[j] = 255;
					anchors_map_outData[j+1] = 255;
					anchors_map_next_outData[j+1] = 255;
				}
				else
				{
					anchors_map_outData[j] = 0;
				}
			}
			else
			{
				if (isAnchor(i, j, grad_map, FALSE))
				{
					anchors_map_outData[j] = 255;
					anchors_map_next_outData[j] = 255;
					anchors_map_outData[j + 1] = 255;
					anchors_map_next_outData[j + 1] = 255;
				}
				else
				{
					anchors_map_outData[j] = 0;
				}
			}
		}
	}


	//dst = anchors_map.clone();
	addWeighted(grad_x, 0.5, grad_y, 0.5, 0, dst);

	return TRUE;
}

/***************************************************************************
* �������ƣ�   isAnchor
* ժ����Ҫ��   
* ȫ��Ӱ�죺   virtual protected 
* �Ρ�������   [in]  int x
* �Ρ�������   [in]  int y
* �Ρ�������   [in]  const cv::Mat & gradMap
* �Ρ�������   [in]  const BOOL direction
* �Ρ�������   [in]  const int ANCHOR_THRESH
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/10/11      ���ǲ�        ���
***************************************************************************/
BOOL FindContoursMethod::isAnchor(int x, int y, 
								const cv::Mat& gradMap, const BOOL direction, 
								const int ANCHOR_THRESH /* = 8 */)
{
	const ushort* grad_map_Data = gradMap.ptr<ushort>(x);

	if (direction) //˵���Ǵ�ֱ����
	{
		if (grad_map_Data[y] - grad_map_Data[y - 1] >= ANCHOR_THRESH &&
			grad_map_Data[y] - grad_map_Data[y + 1] >= ANCHOR_THRESH)
			return TRUE;
	}
	else           // ˵����ˮƽ����
	{
		const ushort* grad_map_Data_X1 = gradMap.ptr<ushort>(x - 1);
		const ushort* grad_map_Data_X2 = gradMap.ptr<ushort>(x + 1);

		if (grad_map_Data[y] - grad_map_Data_X1[y] >= ANCHOR_THRESH &&
			grad_map_Data[y] - grad_map_Data_X2[y] >= ANCHOR_THRESH)
			return TRUE;
	}

	return FALSE;
}

/***************************************************************************
* �������ƣ�   findContoursByEDPF
* ժ����Ҫ��   �������������Ҫ�����潫mat���Ϳ��ٺÿռ䣬���򽫻��������
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  unsigned char * srcImg
* �Ρ�������   [in]  int width
* �Ρ�������   [in]  int height
* �Ρ�������   [in]  cv::Mat & dst
* �Ρ�������   [in]  double smoothingSigma
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/10/12      ���ǲ�        ���
***************************************************************************/
BOOL FindContoursMethod::findContoursByEDPF(const cv::Mat& src, cv::Mat& dst, double smoothingSigma /* = 1.0 */)
{
	// �ȸ���
	cv::Mat src_gray;
	// ����ֻ����Ҷ�ͼ��
	if (CV_8UC1 != src.type())
		cvtColor(src, src_gray, CV_BGR2GRAY);
	else
		src_gray = src.clone();

	dst = src_gray.clone();

	int width = src.cols;
	int height = src.rows;
	unsigned char *srcImg=new unsigned char[width*height];
	
	for (int i = 0; i <height ; i++)
	{
		uchar* src_Data = dst.ptr<uchar>(i);
		for (int j = 0; j < width; j++)
			srcImg[i*width + j] = src_Data[j];
	}

	// �õ�EDPF���еı�Ե�����
	EdgeMap *map =  //DetectEdgesByED(srcImg, width, height, ::SOBEL_OPERATOR, 16, 0, 1.0);
		DetectEdgesByEDPF(srcImg,width,height, 1.0);

	// ����Ľ��ת���ɱ�Եͼ��
	memset(map->edgeImg, 0, width*height);
	for (int i = 0; i < map->noSegments; i++){
		for (int j = 0; j < map->segments[i].noPixels; j++){
			int r = map->segments[i].pixels[j].r;
			int c = map->segments[i].pixels[j].c;

			map->edgeImg[r*width + c] = 255;
		} //end-for
	} //end-for

	for (int i = 0; i < height; i++)
	{
		uchar* EDPF_edge_outData = dst.ptr<uchar>(i);
		for (int j = 0; j < width; j++)
			EDPF_edge_outData[j] = map->edgeImg[i*width + j];
	}

	delete [] srcImg;

	return TRUE;
}

/***************************************************************************
* �������ƣ�   findContoursByCannyLine
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Mat & src
* �Ρ�������   [in]  cv::Mat & dst
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/09      ���ǲ�        ���
***************************************************************************/
BOOL FindContoursMethod::findContoursByCannyLine(const cv::Mat& src, cv::Mat& dst)
{
 	CannyLine detector;
 	std::vector<std::vector<float> > lines;
 	cv::Mat cvMatTemImg = src.clone();  // ����ͼƬ�������������
 
 	detector.cannyLine(cvMatTemImg, lines);
 
 	// show
 	cv::Mat imgShow(cvMatTemImg.rows, cvMatTemImg.cols, CV_8UC3, cv::Scalar(255, 255, 255));
  	for (int m = 0; m < (int)lines.size(); ++m)
  	{
		cv::line(imgShow, cv::Point((int)lines[m][0], (int)lines[m][1]), cv::Point((int)lines[m][2], (int)lines[m][3]), cv::Scalar(0, 0, 0), 1, CV_AA);
  	}

	dst = imgShow.clone();

	return TRUE;
}

/***************************************************************************
* �������ƣ�   findContoursByCannyPF
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Mat & src
* �Ρ�������   [in]  cv::Mat & dst
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/09      ���ǲ�        ���
***************************************************************************/
BOOL FindContoursMethod::findContoursByCannyPF(const cv::Mat& src, cv::Mat& dst)
{
	CannyPF cannyer;
	cv::Mat edgeMap;
	float VMGradient = 70.0;
	cv::Mat cvMatTemImg = src.clone();  // ����ͼƬ�������������
	cv::Mat src_gray;

	if (CV_8UC1 != cvMatTemImg.type())
		cvtColor(cvMatTemImg, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatTemImg.clone();
//	cvMatTemImg = src_gray.clone();

	cannyer.cannyPF(src_gray, 3, VMGradient, edgeMap);

	// get edge chain
	ChainFromEdgeMap chainer;
	std::vector<std::vector<cv::Point> > edgeChains;
	chainer.run(src_gray, edgeMap, edgeChains);

	// show
	cv::Mat imgShow(src_gray.rows, src_gray.cols, CV_8UC1, cv::Scalar(0));
	uchar* ptrS = (uchar*)imgShow.data;
	for (int i = 0; i < edgeChains.size(); ++i)
	{
		for (int j = 0; j < edgeChains[i].size(); ++j)
		{
			int loc = edgeChains[i][j].y * src_gray.cols + edgeChains[i][j].x;
			ptrS[loc] = 255;
		}
	}
	dst = imgShow.clone();

	return TRUE;
}

/***************************************************************************
* �������ƣ�   findConnectedDomain
* ժ����Ҫ��   ��Ҫע������������ֻ���ǻҶ�ͼ��
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Mat & src
* �Ρ�������   [in]  cv::Mat & dst
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/22      ���ǲ�        ���
***************************************************************************/
BOOL FindContoursMethod::findConnectedDomain(const cv::Mat& src, cv::Mat& dst, int& iTotal /* = 0 */)
{
	cv::Mat cvMatTemImg = src.clone();  // ����ͼƬ�������������
	cv::Mat src_gray;
	iTotal = 0;

	// this function only for gray image
	if (CV_8UC1 != cvMatTemImg.type())
		cvtColor(cvMatTemImg, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatTemImg.clone();

	// find contours
	std::vector < std::vector<cv::Point> > contours; // ������ǵ�洢��λ��
	findContours(src_gray, contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

	// get connected domain contours image
	cv::Mat contoursImage(src_gray.rows, src_gray.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	//cvtColor(src_gray, contoursImage, CV_GRAY2RGB);
	const int COLOR_NUM = 7; // the color number, seven color

	// draw contoursImage
	for (int i = 0; i < (int)contours.size(); i++)
	{
		std::vector < std::vector<cv::Point> > contour;

		// check connected domain
		cv::Moments moms = cv::moments(cv::Mat(contours[i]), true);
		double area = moms.m00;
		if (area <= contours[i].size())
			continue;

		iTotal++;
		contour.push_back(contours.at(i));
		// draw the contour by the different color
		drawContours(contoursImage, contour, -1, GetSevenColor(i%COLOR_NUM));
	}

	dst = contoursImage.clone();
	return TRUE;
}