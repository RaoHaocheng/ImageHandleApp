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

	return FALSE;
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

// 	// 使用深度拷贝，以免出现问题
// 	cv::Mat src_gray = src.clone();
//  	cv::Mat grad_x, grad_y;
//  	cv::Mat gray;
//  	int scale = 1;
//  	int delta = 0;
//  	int ddepth = CV_16S;
 
//  	// 如果图片没有内容就返回
//  	if (!src.data)
//  		return FALSE;
//  
//  	// 将图片加入高斯核函数
//  	GaussianBlur(src_gray, src_gray, cv::Size(7, 7), 0, 1.28, cv::BORDER_DEFAULT);
//  
//  	// 将变异后的图片转成灰度图
//  	//	cvtColor(src, src_gray, CV_BGR2GRAY);
//  
//  	cv::Mat tem_grad_x, tem_grad_y;
//  
//  	// 计算梯度
//  	/// Gradient X
//  	Sobel(src_gray, tem_grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
//  	/// Gradient Y
//  	Sobel(src_gray, tem_grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
//  
//  	// 转换成8位置的图片
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
	// 这里使用的是ED算法
	cv::Mat cvMatTemImg = src.clone();  // 复制图片，以免出现问题
	cv::Mat src_gray;                   // 灰度图   

	// 第一步：对图像进行高斯滤波
	cv::Mat cvMatGaussianImg = src.clone();           // 高斯滤波后的图片
	// 在这里我们使用高斯内核为5*5 偏差为1的高斯滤波;经过证实如果设置偏差为1,会得到错误的答案
	cv::GaussianBlur(cvMatTemImg, cvMatGaussianImg, cv::Size(5, 5), 0, 0, cv::BORDER_DEFAULT);
	//cvSmooth(&IplImage(cvMatTemImg), &IplImage(cvMatGaussianImg), CV_GAUSSIAN, 5, 5,1);

	// 我们只处理灰度图像
	if (CV_8UC1 != cvMatTemImg.type())
		cvtColor(cvMatGaussianImg, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatGaussianImg.clone();

	// 第二步：通过Sobel算子计算梯度
	cv::Mat tem_grad_x, tem_grad_y;
	cv::Mat grad_x, grad_y;
	cv::Mat grad_map, direction_map, edgeAreas_map; // 梯度图，方向图
	const int scale = 1;
	const int delta = 0;

	// 计算梯度
	/// Gradient X
	Sobel(src_gray, tem_grad_x, CV_64F, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
	/// Gradient Y
	Sobel(src_gray, tem_grad_y, CV_64F, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);

	// 转换成8位置的图片
	convertScaleAbs(tem_grad_x, grad_x);
	convertScaleAbs(tem_grad_y, grad_y);

	// 形成梯度图，方向图
	//addWeighted(grad_x, 1, grad_y, 1, 0, grad_map);
	grad_map.create(src_gray.size(), CV_16UC1);
	edgeAreas_map.create(src_gray.size(), src_gray.type());
	direction_map.create(src_gray.size(), src_gray.type());

	for (int i = 0; i < src_gray.rows; i++)
	{
		// 每一行图像的指针
		const uchar* grad_x_data = grad_x.ptr<uchar>(i);
		const uchar* grad_y_data = grad_y.ptr<uchar>(i);
	
		// 输出的指针
		uchar* direction_map_outData = direction_map.ptr<uchar>(i);
		uchar* edgeAreas_map_outData = edgeAreas_map.ptr<uchar>(i);
		ushort* grad_map_outData = grad_map.ptr<ushort>(i);

		for (int j = 0; j < src_gray.cols*src_gray.channels(); j++)
		{
			// 计算方向图
			if (grad_x_data[j] >= grad_y_data[j])
				direction_map_outData[j] = grad_x_data[j];
			else
				direction_map_outData[j] = grad_y_data[j];

			// 计算梯度等级信息
			grad_map_outData[j] =  grad_x_data[j] + grad_y_data[j]; // 防止他过界所以*0.5

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
	const int iscanInterval = 4;                       // 行扫描间隔

	for (int i = iscanInterval; i < grad_map.rows - 1; i = i + iscanInterval)// 从下一行开始找Anchors
	{
		// 每一行图像的指针
		const uchar* grad_x_data = grad_x.ptr<uchar>(i);

		// 输出的指针
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
* 函数名称：   isAnchor
* 摘　　要：   
* 全局影响：   virtual protected 
* 参　　数：   [in]  int x
* 参　　数：   [in]  int y
* 参　　数：   [in]  const cv::Mat & gradMap
* 参　　数：   [in]  const BOOL direction
* 参　　数：   [in]  const int ANCHOR_THRESH
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/10/11      饶智博        添加
***************************************************************************/
BOOL FindContoursMethod::isAnchor(int x, int y, 
								const cv::Mat& gradMap, const BOOL direction, 
								const int ANCHOR_THRESH /* = 8 */)
{
	const ushort* grad_map_Data = gradMap.ptr<ushort>(x);

	if (direction) //说明是垂直方向
	{
		if (grad_map_Data[y] - grad_map_Data[y - 1] >= ANCHOR_THRESH &&
			grad_map_Data[y] - grad_map_Data[y + 1] >= ANCHOR_THRESH)
			return TRUE;
	}
	else           // 说明是水平方向
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
* 函数名称：   findContoursByEDPF
* 摘　　要：   利用这个函数需要在外面将mat类型开辟好空间，否则将会出现问题
* 全局影响：   public 
* 参　　数：   [in]  unsigned char * srcImg
* 参　　数：   [in]  int width
* 参　　数：   [in]  int height
* 参　　数：   [in]  cv::Mat & dst
* 参　　数：   [in]  double smoothingSigma
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/10/12      饶智博        添加
***************************************************************************/
BOOL FindContoursMethod::findContoursByEDPF(const cv::Mat& src, cv::Mat& dst, double smoothingSigma /* = 1.0 */)
{
	// 先复制
	cv::Mat src_gray;
	// 我们只处理灰度图像
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

	// 得到EDPF库中的边缘检测结果
	EdgeMap *map =  //DetectEdgesByED(srcImg, width, height, ::SOBEL_OPERATOR, 16, 0, 1.0);
		DetectEdgesByEDPF(srcImg,width,height, 1.0);

	// 将其的结果转换成边缘图像
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
* 函数名称：   findContoursByCannyLine
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Mat & src
* 参　　数：   [in]  cv::Mat & dst
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/09      饶智博        添加
***************************************************************************/
BOOL FindContoursMethod::findContoursByCannyLine(const cv::Mat& src, cv::Mat& dst)
{
 	CannyLine detector;
 	std::vector<std::vector<float> > lines;
 	cv::Mat cvMatTemImg = src.clone();  // 复制图片，以免出现问题
 
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
* 函数名称：   findContoursByCannyPF
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Mat & src
* 参　　数：   [in]  cv::Mat & dst
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/09      饶智博        添加
***************************************************************************/
BOOL FindContoursMethod::findContoursByCannyPF(const cv::Mat& src, cv::Mat& dst)
{
	CannyPF cannyer;
	cv::Mat edgeMap;
	float VMGradient = 70.0;
	cv::Mat cvMatTemImg = src.clone();  // 复制图片，以免出现问题
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
* 函数名称：   findConnectedDomain
* 摘　　要：   需要注意的输入进来的只能是灰度图像
* 全局影响：   public 
* 参　　数：   [in]  const cv::Mat & src
* 参　　数：   [in]  cv::Mat & dst
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/22      饶智博        添加
***************************************************************************/
BOOL FindContoursMethod::findConnectedDomain(const cv::Mat& src, cv::Mat& dst, int& iTotal /* = 0 */)
{
	cv::Mat cvMatTemImg = src.clone();  // 复制图片，以免出现问题
	cv::Mat src_gray;
	iTotal = 0;

	// this function only for gray image
	if (CV_8UC1 != cvMatTemImg.type())
		cvtColor(cvMatTemImg, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatTemImg.clone();

	// find contours
	std::vector < std::vector<cv::Point> > contours; // 轮廓标记点存储的位置
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