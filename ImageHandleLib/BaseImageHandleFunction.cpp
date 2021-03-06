#include "BaseImageHandleFunction.h"

/***************************************************************************
* 函数名称：   Otsu
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  IplImage * src
* 返回值　：   int
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/14      饶智博        添加
***************************************************************************/
DLLS_PORT int  Otsu(IplImage* src)
{
	int iHeight = src->height;			 // 图片高度
	int iWidth = src->width;             // 图片宽度
	long lSize = iHeight * iWidth;       // 图片大小

	float fHistogram[GRAY_SCALE_VALUE] = { 0 };
	for (int i = 0; i < iHeight; i++){
		unsigned char* p = (unsigned char*)src->imageData + src->widthStep * i;
		for (int j = 0; j < iWidth; j++)
			fHistogram[int(*p++)]++;
	}

	int    iThreshold;                         // 返回的阈值
	long   lFrontgroundTotalGrayValue = 0,
		lBackgroundTotalGrayValue = 0;      // 存储前景的灰度总和和背景灰度总和  
	long   lFrontgroundTotalNum = 0,
		lBackgroundTotalNum = 0;            // 前景的总个数和背景的总个数  
	double dbFrontgroundRation = 0,
		dbBackgroundRation = 0;             // 前景和背景所占整幅图像的比例  
	double dbFrontgroundAveGrayValue = 0,
		dbBackgroundAveGrayValue = 0;       // 前景和背景的平均灰度  
	double dbVariance = 0;                     // 最大类间方差  
	double dbAveGrayValue = 0;
	double dbMaxVariance = 0;

	for (int i = 1; i < GRAY_SCALE_VALUE; i++) //一次遍历每个像素  
	{
		// 重新初始化
		lFrontgroundTotalGrayValue = 0;
		lBackgroundTotalGrayValue = 0;
		lFrontgroundTotalNum = 0;
		lBackgroundTotalNum = 0;
		dbFrontgroundRation = 0;
		dbBackgroundRation = 0;

		for (int j = 0; j < i; j++)
		{
			lFrontgroundTotalNum += (long)(fHistogram[j]);
			lFrontgroundTotalGrayValue += (long)(j * fHistogram[j]);
		}

		dbFrontgroundAveGrayValue = (double)lFrontgroundTotalGrayValue / lFrontgroundTotalNum;
		dbFrontgroundRation = (double)lFrontgroundTotalNum / lSize;

		for (int j = i; j < GRAY_SCALE_VALUE; j++)
		{
			lBackgroundTotalNum += (long)fHistogram[j];
			lBackgroundTotalGrayValue += (long)(j * fHistogram[j]);
		}

		dbBackgroundAveGrayValue = (double)lBackgroundTotalGrayValue / lBackgroundTotalNum;
		dbBackgroundRation = 1 - dbFrontgroundRation; // (double)lBackgroundTotalNum / size;  

		dbAveGrayValue = dbFrontgroundAveGrayValue * dbFrontgroundRation
			+ dbBackgroundAveGrayValue * dbBackgroundRation; //图像的平均灰度  
		dbVariance = dbFrontgroundRation * dbBackgroundRation *  (dbFrontgroundAveGrayValue - dbBackgroundAveGrayValue)
			* (dbFrontgroundAveGrayValue - dbBackgroundAveGrayValue);
		if (dbVariance > dbMaxVariance)
		{
			dbMaxVariance = dbVariance;
			iThreshold = i;
		}
	}

	printf("iThreshold = %d\n", iThreshold);
	return iThreshold;
}



/***************************************************************************
* 函数名称：   Get3DPos
* 摘　　要：   ///解矛盾方程组
* 全局影响：   public
* 参　　数：   [inout]  double * pdbQ
* 参　　数：   [inout]  double * pdb2D
* 参　　数：   [inout]  double * pdbSolution
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/21      饶智博        添加
***************************************************************************/
DLLS_PORT void  Get3DPos(double* pdbQ, double* pdb2D, double *pdbSolution)
{
	// 计算XYZW4个参数的解
	double* pdb4D = new double[PARAMETER_XYZW_NUM];
	for (int i = 0; i < PARAMETER_XYZW_NUM; i++){
		pdb4D[i] = 0;
		for (int j = 0; j < PARAMETER_XYZW_NUM; j++){
			pdb4D[i] += *(pdbQ + i * PARAMETER_XYZW_NUM + j) * *(pdb2D + j);
		}
	}

	// 计算XYZ3个参数的解
	for (int i = 0; i < PARAMETER_XYZ_NUM; i++)
		*(pdbSolution + i) = (*(pdb4D + i) / *(pdb4D + PARAMETER_XYZ_NUM));

	delete[] pdb4D;
	pdb4D = NULL;
}

/***************************************************************************
* 函数名称：   AddGaussianNoise
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  IplImage * src
* 参　　数：   [in]  double dbVariance
* 返回值　：   DLLS_PORT IplImage*
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/22      饶智博        添加
***************************************************************************/
DLLS_PORT IplImage*  AddGaussianNoise(IplImage* src, double dbVariance)
{
	if (!src)
		return FALSE;

	return AddGaussianNoise(src, GAUSSIAN_MEAN, dbVariance);
}

/***************************************************************************
* 函数名称：   AddGaussianNoise
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  IplImage * src
* 参　　数：   [in]  double dbMean
* 参　　数：   [in]  double dbVariance
* 返回值　：   DLLS_PORT IplImage*
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/22      饶智博        添加
***************************************************************************/
DLLS_PORT IplImage*  AddGaussianNoise(IplImage* src, double dbMean, double dbVariance /* = GAUSSIAN_VARIANCE */)
{
	if (!src)
		return FALSE;

	// 复制图片
	IplImage* temImg = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	cvCopy(src, temImg);

	// 初始化数据,这些数据是临时需要使用的
	cv::Mat cvMatImg(temImg);                    // 转换成为一个矩阵
	cv::Mat cvMatDst(temImg, true);
	cv::Mat cvMatNoise(cvMatImg.size(), CV_64F);

	// 图片归一化处理
	normalize(cvMatImg, cvMatDst, GAUSSIAN_MEAN, GAUSSIAN_VARIANCE, CV_MINMAX, CV_64F);
	// 生成噪声
	cv::randn(cvMatNoise, dbMean, dbVariance);
	// 叠加噪声
	cvMatDst = cvMatDst + cvMatNoise;
	// 归一化处理
	normalize(cvMatDst, cvMatDst, GAUSSIAN_MEAN, GAUSSIAN_VARIANCE, CV_MINMAX, CV_64F);

	// 复制我们需要图片出来
	IplImage temRelsutImg = (IplImage)(cvMatDst);
	IplImage* relsutImg = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);

	// 由于计算都是8位的灰度图，所以需要把原来64为的数据转换成8未，比例为255
	cvConvertScaleAbs(&temRelsutImg, relsutImg, CONVERT_SCALE);

	// 删除临时的变量,以免内存泄露
	cvReleaseImage(&temImg);

	return relsutImg;
}

/***************************************************************************
* 函数名称：   GradientEstimation
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  IplImage * src
* 参　　数：   [in]  cv::Mat & grad_x
* 参　　数：   [in]  cv::Mat & grad_y
* 参　　数：   [in]  BOOL bShow
* 返回值　：   DLLS_PORT void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/05/09      饶智博        添加
***************************************************************************/
DLLS_PORT void GradientEstimation(IplImage* src, cv::Mat &grad_x, cv::Mat &grad_y, BOOL bShow /* = false */)
{
	cv::Mat cvMatImg(src);
	return GradientEstimation(cvMatImg, grad_x, grad_y, bShow);
}

/***************************************************************************
* 函数名称：   GradientEstimation
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  cv::Mat src
* 参　　数：   [in]  cv::Mat & grad_x
* 参　　数：   [in]  cv::Mat & grad_y
* 参　　数：   [in]  BOOL bShow
* 返回值　：   DLLS_PORT void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/05/09      饶智博        添加
***************************************************************************/
DLLS_PORT void GradientEstimation(cv::Mat src, cv::Mat &grad_x, cv::Mat &grad_y, BOOL bShow)
{
	// 使用深度拷贝，以免出现问题
	cv::Mat src_gray = src.clone();
	int scale = 1;
	int delta = 1;
	int ddepth = CV_16S;

	// 如果图片没有内容就返回
	if (!src.data)
		return;

	// 将图片加入高斯核函数
	GaussianBlur(src_gray, src_gray, cv::Size(5, 5), 0, 1.28, cv::BORDER_DEFAULT);

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

	if (bShow)
	{	// 显示窗口的名字
		char* window_name = "GradientEstimation";
		cv::Mat grad;
		addWeighted(grad_x, 0.5, grad_y, 0.5, 0, grad);
		imshow(window_name, grad);
		cvWaitKey(0);
	}

}

/***************************************************************************
* 函数名称：   GradientEstimation
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  cv::Mat src
* 参　　数：   [in]  cv::Mat & dst
* 返回值　：   DLLS_PORT void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/05/09      饶智博        添加
***************************************************************************/
DLLS_PORT void GradientEstimation(cv::Mat src, cv::Mat &dst)
{
	// 使用深度拷贝，以免出现问题
	cv::Mat src_gray = src.clone();
	int scale = 1;
	int delta = 1;
	int ddepth = CV_16S;

	// 如果图片没有内容就返回
	if (!src.data)
		return;

	// 将图片加入高斯核函数
	GaussianBlur(src_gray, src_gray, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);

	// 将变异后的图片转成灰度图
	//	cvtColor(src, src_gray, CV_BGR2GRAY);

	cv::Mat tem_grad_x, tem_grad_y;
	cv::Mat grad_x, grad_y;

	// 计算梯度
	/// Gradient X
	Sobel(src_gray, tem_grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
	/// Gradient Y
	Sobel(src_gray, tem_grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);

	// 转换成8位置的图片
	convertScaleAbs(tem_grad_x, grad_x);
	convertScaleAbs(tem_grad_y, grad_y);

	// 画出梯度图
	cv::Mat grad;
	addWeighted(grad_x, 0.5, grad_y, 0.5, 0, grad);

	dst = grad.clone();
}

/***************************************************************************
* 函数名称：   GetCircle
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const cv::Point2f & p1
* 参　　数：   [in]  const cv::Point2f & p2
* 参　　数：   [in]  const cv::Point2f & p3
* 参　　数：   [out]  cv::Point2f & center
* 参　　数：   [out]  float & radius
* 返回值　：   DLLS_PORT void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/22      饶智博        添加
***************************************************************************/
DLLS_PORT void GetCircle(const cv::Point& p1, const cv::Point& p2,
	const cv::Point& p3, cv::Point2d& center, double& radius)
{
	float x1 = (float)p1.x;
	float x2 = (float)p2.x;
	float x3 = (float)p3.x;

	float y1 = (float)p1.y;
	float y2 = (float)p2.y;
	float y3 = (float)p3.y;

	// PLEASE CHECK FOR TYPOS IN THE FORMULA :)
	// 计算圆心坐标
	center.x = (x1*x1 + y1*y1)*(y2 - y3) + (x2*x2 + y2*y2)*(y3 - y1) + (x3*x3 + y3*y3)*(y1 - y2);
	center.x /= (2 * (x1*(y2 - y3) - y1*(x2 - x3) + x2*y3 - x3*y2));

	center.y = (x1*x1 + y1*y1)*(x3 - x2) + (x2*x2 + y2*y2)*(x1 - x3) + (x3*x3 + y3*y3)*(x2 - x1);
	center.y /= (2 * (x1*(y2 - y3) - y1*(x2 - x3) + x2*y3 - x3*y2));

	// 计算圆形半径
	radius = sqrt((center.x - x1)*(center.x - x1) + (center.y - y1)*(center.y - y1));

	return;
}

/***************************************************************************
* 函数名称：   GetDistance
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const cv::Point2f p1
* 参　　数：   [in]  const cv::Point2f p2
* 返回值　：   DLLS_PORT double
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/22      饶智博        添加
***************************************************************************/
DLLS_PORT double GetDistance(const cv::Point2f p1, const cv::Point2f p2)
{
	return sqrtf(powf((p1.x - p2.x), 2) + powf((p1.y - p2.y), 2));
}

/***************************************************************************
* 函数名称：   IsInCircle
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const cv::Point2f point
* 参　　数：   [in]  const ST_CENTER center
* 参　　数：   [in]  const double dbPrecision
* 返回值　：   DLLS_PORT BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/22      饶智博        添加
***************************************************************************/
DLLS_PORT BOOL IsInCircle(const cv::Point point, const ST_CENTER center, const double dbPrecision)
{
	if (abs(GetDistance(point, center.location) - center.radius) <= dbPrecision)
		return TRUE;
	else
		return FALSE;
}


/***************************************************************************
* 函数名称：   CalculateGradientDrectAngle
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const cv::Point2f grad
* 参　　数：   [in]  const cv::Point2f twoPointVector
* 返回值　：   DLLS_PORT BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/23      饶智博        添加
***************************************************************************/
DLLS_PORT double CalculateGradientDrectAngle(const cv::Point2f grad, const cv::Point2f twoPointVector)
{
	return (DotProduct(grad, twoPointVector) / CalculateVectorDistance(twoPointVector));
}

/***************************************************************************
* 函数名称：   CalculateVectorDistance
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const cv::Point2f twoPointVector
* 返回值　：   DLLS_PORT double
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/23      饶智博        添加
***************************************************************************/
DLLS_PORT double CalculateVectorDistance(const cv::Point2f twoPointVector)
{
	return sqrtf(powf((twoPointVector.x), 2) + powf((twoPointVector.y), 2));
}

/***************************************************************************
* 函数名称：   DotProduct
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const cv::Point2f p1
* 参　　数：   [in]  const cv::Point2f p2
* 返回值　：   DLLS_PORT double
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/23      饶智博        添加
***************************************************************************/
DLLS_PORT double DotProduct(const cv::Point2f p1, const cv::Point2f p2)
{
	return (p1.x*p2.x + p1.y*p2.y);
}

/***************************************************************************
* 函数名称：   NormalizerPoint
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const cv::Point2f p1
* 参　　数：   [out] cv::Point2f & p2
* 返回值　：   DLLS_PORT BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/23      饶智博        添加
***************************************************************************/
DLLS_PORT BOOL NormalizerPoint(const cv::Point2f p1, cv::Point2f& p2)
{
	const double distance = CalculateVectorDistance(p1);
	if (distance == 0)
		return FALSE;

	p2.x = (float)(p1.x / distance);
	p2.y = (float)(p1.y / distance);

	return TRUE;
}

/***************************************************************************
* 函数名称：   DetectIsoscelesTriangles
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const cv::Mat src
* 参　　数：   [in]  const cv::Point2f p1
* 参　　数：   [in]  const cv::Point2f & p2
* 返回值　：   DLLS_PORT BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/23      饶智博        添加
*2016/04/23      饶智博        被废弃，请使用另一个函数重载
***************************************************************************/
DLLS_PORT BOOL DetectIsoscelesTriangles(const cv::Mat src, const cv::Point2f p1, const cv::Point2f& p2)
{
	cv::Mat cvMatTemImg = src.clone();                       // 复制图片以免出现污染
	cv::Mat grad_x, grad_y;                                  // 梯度数字
	cv::Point2f gradPoint;                                   // 准备做为去除某点的坐标点

	GradientEstimation(cvMatTemImg, grad_x, grad_y, false);   // 计算梯度

	cv::Point2f grad_p1, grad_p2;

	// 对梯度数据
	grad_p1.x = (float)grad_x.at<unsigned char>((int)p1.x, (int)p1.y);
	grad_p1.y = (float)grad_y.at<unsigned char>((int)p1.x, (int)p1.y);
	NormalizerPoint(grad_p1, grad_p1);

	grad_p2.x = (float)grad_x.at<unsigned char>((int)p2.x, (int)p2.y);
	grad_p2.y = (float)grad_y.at<unsigned char>((int)p2.x, (int)p2.y);
	NormalizerPoint(grad_p2, grad_p2);

	cv::Point2f p1_p2, p2_p1;
	p1_p2 = p2 - p1;
	p2_p1 = p1 - p2;

	double dbResult1, dbResult2;
	dbResult1 = DotProduct(p1_p2, grad_p1) / CalculateVectorDistance(p1_p2);
	dbResult2 = DotProduct(p2_p1, grad_p2) / CalculateVectorDistance(p2_p1);

	/*
	for (int i = 0; i < cvMatTemImg.rows;i++)
	{
	for (int j = 0; j < cvMatTemImg.cols; j++)
	{
	unsigned char x = grad_x.at<unsigned char>(i, j);
	std::cout << (int)x << " ";
	}
	std::cout << std::endl;
	}
	*/


	return TRUE;
}

/***************************************************************************
* 函数名称：   DetectIsoscelesTriangles
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const cv::Mat grad_x
* 参　　数：   [in]  const cv::Mat grad_y
* 参　　数：   [in]  const cv::Point2f p1
* 参　　数：   [in]  const cv::Point2f p2
* 参　　数：   [in]  const double dbDifPrecision
* 参　　数：   [in]  const double dbDotPrecision
* 返回值　：   DLLS_PORT BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/05/06      饶智博        添加
***************************************************************************/
DLLS_PORT BOOL DetectIsoscelesTriangles(const cv::Mat grad_x, const cv::Mat grad_y,
	const cv::Point2f p1, const cv::Point2f p2,
	const double dbDifPrecision, const double dbDotPrecision)
{
	cv::Point2f grad_p1, grad_p2;
	// 取出对梯度数据
	grad_p1.x = (float)grad_x.at<unsigned char>((int)p1.y, (int)p1.x);
	grad_p1.y = (float)grad_y.at<unsigned char>((int)p1.y, (int)p1.x);
	NormalizerPoint(grad_p1, grad_p1);                                    // 归一化

	grad_p2.x = (float)grad_x.at<unsigned char>((int)p2.y, (int)p2.x);
	grad_p2.y = (float)grad_y.at<unsigned char>((int)p2.y, (int)p2.x);
	NormalizerPoint(grad_p2, grad_p2);                                    // 归一化

	cv::Point2f p1_p2, p2_p1;
	p1_p2 = p2 - p1;
	p2_p1 = p1 - p2;

	double dbResult1, dbResult2;
	dbResult1 = DotProduct(p1_p2, grad_p1) / CalculateVectorDistance(p1_p2);
	dbResult2 = DotProduct(p2_p1, grad_p2) / CalculateVectorDistance(p2_p1);

	if (abs(dbResult2) - abs(dbResult1) < dbDifPrecision && abs(DotProduct(grad_p1, grad_p2)) < dbDotPrecision)
		return TRUE;
	else
		return FALSE;

}


/***************************************************************************
* 函数名称：   AnalysisCluster
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const std::vector<cv::Point> contour
* 参　　数：   [in]  const cv::Mat grad_x
* 参　　数：   [in]  const cv::Mat grad_y
* 参　　数：   [in]  const double dbDifPrecision
* 参　　数：   [in]  const double dbDotPrecision
* 返回值　：   DLLS_PORT BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/25      饶智博        添加
***************************************************************************/
DLLS_PORT BOOL AnalysisCluster(const std::vector<cv::Point> contour, const cv::Mat grad_x,
	const cv::Mat grad_y, const double dbDifPrecision, const double dbDotPrecision)
{
	// 如果小于2个点，那么一定不是圆
	if (contour.size() <= 4)
		return FALSE;

	const int iMaxtestNum = 50;
	const int iMinSuccessNum = 5;
	BOOL bOk = FALSE;
	int iSuccessNum = 0;
	for (int i = 0; i < iMaxtestNum;)
	{
		unsigned int idx1 = rand() % contour.size();
		unsigned int idx2 = rand() % contour.size();

		if (idx1 == idx2) continue;

		// 说明正常实验
		i++;

		if (DetectIsoscelesTriangles(grad_x, grad_y, contour.at(idx1), contour.at(idx2), dbDifPrecision, dbDotPrecision))
		{
			//bOk = TRUE;
			if ((++iSuccessNum) > iMinSuccessNum)
			{
				bOk = TRUE;
				break;
			}
		}
	}
	return bOk;
}
/***************************************************************************
* 函数名称：   IsThreePointInLine
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  cv::Point2f p1
* 参　　数：   [in]  cv::Point2f p2
* 参　　数：   [in]  cv::Point2f p3
* 返回值　：   DLLS_PORT BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/25      饶智博        添加
***************************************************************************/
DLLS_PORT BOOL IsThreePointInLine(const cv::Point2f p1, const cv::Point2f p2, const cv::Point2f p3
	, const double dbDifPrecision)
{
	double area = 0;

	area = (p1.x*(p2.y - p3.y) + p2.x*(p3.y - p1.y) + p3.x*(p1.y - p2.y)) / 2;

	if (area < dbDifPrecision)
		return TRUE;
	else
		return FALSE;
}

/***************************************************************************
* 函数名称：   IsFourPointInFace
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Point2f p1
* 参　　数：   [in]  const cv::Point2f p2
* 参　　数：   [in]  const cv::Point2f p3
* 参　　数：   [in]  const cv::Point2f p4
* 参　　数：   [in]  const double dbDifPrecision
* 返回值　：   DLLS_PORT BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/02/25      饶智博        添加
***************************************************************************/
DLLS_PORT BOOL IsFourPointInFace(const cv::Point3f p1, const cv::Point3f p2,
	const cv::Point3f p3, const cv::Point3f p4, const double dbDifPrecision)
{
	double dbVolume = Point3Modulus(Point3Dot(Point3Dif(p1, p4), 
		Point3Cross(Point3Dif(p2, p4), Point3Dif(p3, p4)))) / 6;

	if (dbVolume < dbDifPrecision)
		return TRUE;
	else
		return FALSE;
}



/***************************************************************************
* 函数名称：   Point3Cross
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Point3f p1
* 参　　数：   [in]  const cv::Point3f p2
* 返回值　：   DLLS_PORT cv::Point3f
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/02/25      饶智博        添加
***************************************************************************/
DLLS_PORT cv::Point3f Point3Cross(const cv::Point3f p1, const cv::Point3f p2)
{
	cv::Point3f point;
	point.x = p1.y * p2.z - p1.z * p2.y;
	point.y = p1.z * p2.x - p1.x * p2.z;
	point.z = p1.x * p2.y - p1.y * p2.x;
	return point;
}
/***************************************************************************
* 函数名称：   Point3Dot
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Point3f p1
* 参　　数：   [in]  const cv::Point3f p2
* 返回值　：   DLLS_PORT cv::Point3f
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/02/25      饶智博        添加
***************************************************************************/
DLLS_PORT cv::Point3f Point3Dot(const cv::Point3f p1, const cv::Point3f p2)
{
	cv::Point3f point;
	point.x = p1.x * p2.x;
	point.y = p1.y * p2.y;
	point.z = p1.z * p2.z;
	return point;
}

/***************************************************************************
* 函数名称：   Point3Dis
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Point3f p1
* 参　　数：   [in]  const cv::Point3f p2
* 返回值　：   DLLS_PORT double
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/02/25      饶智博        添加
***************************************************************************/
DLLS_PORT double Point3Dis(const cv::Point3f p1, const cv::Point3f p2)
{
	return Point3Modulus(Point3Dif(p1, p2));
}

/***************************************************************************
* 函数名称：   Point3Dif
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Point3f p1
* 参　　数：   [in]  const cv::Point3f p2
* 返回值　：   DLLS_PORT cv::Point3f
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/02/25      饶智博        添加
***************************************************************************/
DLLS_PORT cv::Point3f Point3Dif(const cv::Point3f p1, const cv::Point3f p2)
{
	cv::Point3f point;
	point.x = p1.x - p2.x;
	point.y = p1.y - p2.y;
	point.z = p1.z - p2.z;
	return point;
}
/***************************************************************************
* 函数名称：   Point3Add
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Point3f p1
* 参　　数：   [in]  const cv::Point3f p2
* 返回值　：   DLLS_PORT cv::Point3f
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/02/25      饶智博        添加
***************************************************************************/
DLLS_PORT cv::Point3f Point3Add(const cv::Point3f p1, const cv::Point3f p2)
{
	cv::Point3f point;
	point.x = p1.x + p2.x;
	point.y = p1.y + p2.y;
	point.z = p1.z + p2.z;
	return point;
}

/***************************************************************************
* 函数名称：   Point3Modulus
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Point3f p1
* 返回值　：   DLLS_PORT double
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/02/25      饶智博        添加
***************************************************************************/
DLLS_PORT double Point3Modulus(const cv::Point3f p1)
{
	return sqrtf(powf(p1.x, 2) + powf(p1.y, 2) + powf(p1.z, 2));
}


/***************************************************************************
* 函数名称：   IsInCircle
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const std::vector<cv::Point> counter
* 参　　数：   [in]  std::vector<ST_CENTER> & center
* 返回值　：   DLLS_PORT BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/25      饶智博        添加
***************************************************************************/
DLLS_PORT BOOL IsInCircle(const std::vector<cv::Point> contour, std::vector<ST_CENTER>& center, const double dbPrecision)
{
	// 设置实验次数
	const int iTestNum = 1000;

	// 整个圆形的数量
	for (int i = 0; i < (int)center.size(); i++)
	{
		int iSuccessNum = 0;
		// 测试其的可信度
		for (int j = 0; j < iTestNum; j++)
		{
			unsigned int idx = rand() % contour.size();
			// 检测是否是一个圆
			if (IsInCircle(contour.at(idx), center.at(i), dbPrecision))
				iSuccessNum++;
		}
		center.at(i).confidence = (double)iSuccessNum / (double)iTestNum;
	}

	return TRUE;
}


/***************************************************************************
* 函数名称：   ConvertClosedContour
* 摘　　要：   被废弃
* 全局影响：   public
* 参　　数：   [in]  std::vector<cv::Point> & contours
* 返回值　：   DLLS_PORT void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/05/17      饶智博        添加
***************************************************************************/
DLLS_PORT void ConvertClosedContour(std::vector<cv::Point> &contours)
{
	// 起始点的边界点
	const int ix_min = contours.at(0).x - 1;
	const int ix_max = contours.at(0).x + 1;
	const int iy_min = contours.at(0).y - 1;
	const int iy_max = contours.at(0).y + 1;

	const int iPoint_x = contours.at(contours.size() - 1).x;
	const int iPoint_y = contours.at(contours.size() - 1).y;

	// 这说明是闭合曲线,就返回
	if (iPoint_x >= ix_min
		&&iPoint_y >= iy_min
		&&iPoint_x <= ix_max
		&&iPoint_y <= iy_max)
		return;

	// 补充成闭合曲线
	// 将末尾的点取出来
	int dx = iPoint_x - contours.at(0).x;
	int dy = iPoint_y - contours.at(0).y;

	if (abs(dx) >= abs(dy))
	{
		float k = ((float)(dy)) / ((float)(dx));
		float b = (float)iPoint_y - k*iPoint_x;
		int x, y;

		if (dx > 0)
		for (int i = 0; i < abs(dx); i++)
		{
			x = iPoint_x + i;
			y = (int)(k*x + b);
			contours.push_back(cv::Point(x, y));
		}
		else
		for (int i = 0; i < abs(dx); i++)
		{
			x = iPoint_x - i;
			y = (int)(k*x + b);
			contours.push_back(cv::Point(x, y));
		}

		return;
	}
	else
	{

		float k = ((float)(dx)) / ((float)(dy));
		float b = (float)iPoint_x - k*iPoint_y;
		int x, y;

		if (dx > 0)
		for (int i = 0; i < abs(dy); i++)
		{
			y = iPoint_y + i;
			x = (int)(k*y + b);
			contours.push_back(cv::Point(x, y));
		}
		else
		for (int i = 0; i < abs(dy); i++)
		{
			y = iPoint_y - i;
			x = (int)(k*y + b);
			contours.push_back(cv::Point(x, y));
		}

		return;
	}
}


/***************************************************************************
* 函数名称：   FindNick
* 摘　　要：   被废弃
* 全局影响：   public
* 参　　数：   [in]  std::vector<cv::Point> & contours
* 返回值　：   DLLS_PORT BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/05/17      饶智博        添加
***************************************************************************/
DLLS_PORT BOOL FindNick(std::vector<cv::Point> &contours)
{
	BOOL bOk = FALSE;

	std::vector<cv::Point> nick;


	for (int i = 0; i < (int)contours.size(); i++)
	{

	}

	return bOk;
}

/***************************************************************************
* 函数名称：   ConvertRgbToYuv
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  BYTE * src
* 参　　数：   [out]  BYTE * dst
* 参　　数：   [in]  const int width
* 参　　数：   [in]  const int height
* 返回值　：   DLLS_PORT BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/05/25      饶智博        添加
***************************************************************************/
DLLS_PORT BOOL ConvertRgbToYuv(BYTE* src, BYTE* dst, const int width, const int height)
{
	// 如果输入和输入的控件都没开辟好，就返回
	if (NULL == src || NULL == dst)
		return FALSE;

	int len = width * height;
	const int rgba_step = 4;    // 每4个字节代表RGBA
	int y, u, v;

	// 一行一行扫过去
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			// 获得当前像素值
			int rgba = src[(i * width + j) * rgba_step];

			int r = (rgba >> 8) & 0xFF;
			int g = (rgba >> 4) & 0xFF;
			int b = (rgba >> 2) & 0xFF;

			y = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
			u = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
			v = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;

			//调整
			y = y < 16 ? 16 : (y > 255 ? 255 : y);
			u = u < 0 ? 0 : (u > 255 ? 255 : u);
			v = v < 0 ? 0 : (v > 255 ? 255 : v);

			// 赋值
			dst[i * width + j] = (BYTE)y;
			dst[len + (i >> 1) * width + (j & ~1) + 0] = (BYTE)u;
			dst[len + +(i >> 1) * width + (j & ~1) + 1] = (BYTE)v;
		}
	}

	return TRUE;
}

/***************************************************************************
* 函数名称：   DrawCircle
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  cv::Mat & src
* 参　　数：   [out]  cv::Mat & keypointsImage
* 参　　数：   [in]  std::vector<ST_CENTER> centers
* 返回值　：   DLLS_PORT BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/27      饶智博        添加
***************************************************************************/
DLLS_PORT BOOL DrawCircle(cv::Mat& src, cv::Mat& keypointsImage, std::vector<ST_CENTER> centers)
{
	cv::Mat srcGray;
	if (CV_8UC1 != src.type())
		cvtColor(src, srcGray, CV_RGB2GRAY);
	else
		srcGray = src.clone();

	cvtColor(srcGray, keypointsImage, CV_GRAY2RGB);

	for (int i = 0; i < (int)centers.size(); i++)
	{
		ST_CENTER center;
		center = centers.at(i);
		cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // 画一个圆 
		cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // 画一个圆 
	}
	//cvtColor(tem, keypointsImage, CV_RGB2GRAY);

	return TRUE;
}

/***************************************************************************
* 函数名称：   Variance
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  std::vector<double> src
* 返回值　：   DLLS_PORT double
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/20      饶智博        添加
***************************************************************************/
DLLS_PORT double Variance(std::vector<double> src)
{
	double dbAverage = 0;
	double dbVariance = 0;

	for (int i = 0; i < (int)src.size(); i++)
		dbAverage = dbAverage + src.at(i);

	dbAverage = dbAverage / (int)src.size();

	for (int i = 0; i < (int)src.size(); i++)
		dbVariance = dbVariance + (src.at(i) - dbAverage)*(src.at(i) - dbAverage);

	dbVariance = dbVariance / (int)src.size();

	return dbVariance;
}

/***************************************************************************
* 函数名称：   thinImage
* 摘　　要：   需要说明的，输入须是二值图像，且白色像素需要为1而不是255
* 全局影响：   public
* 参　　数：   [in]  const cv::Mat & src
* 参　　数：   [in]  const int maxIterations
* 返回值　：   DLLS_PORT cv::Mat
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/21      饶智博        添加
***************************************************************************/
DLLS_PORT cv::Mat thinImage(const cv::Mat & src, const int maxIterations)
{
	assert(src.type() == CV_8UC1);
	cv::Mat dst;
	int width = src.cols;
	int height = src.rows;
	src.copyTo(dst);
	int count = 0;  //记录迭代次数  
	while (true)
	{
		count++;
		if (maxIterations != -1 && count > maxIterations) //限制次数并且迭代次数到达  
			break;
		std::vector<uchar *> mFlag; //用于标记需要删除的点  
		//对点标记  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记  
				//  p9 p2 p3  
				//  p8 p1 p4  
				//  p7 p6 p5  
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);
				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p6 == 0 && p4 * p6 * p8 == 0)
					{
						//标记  
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除  
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
			**i = 0;
	

		//直到没有点满足，算法结束  
		if (mFlag.empty())
			break;
		else
			mFlag.clear();//将mFlag清空


		//对点标记  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记  
				//  p9 p2 p3  
				//  p8 p1 p4  
				//  p7 p6 p5  
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);

				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p8 == 0 && p2 * p6 * p8 == 0)
					{
						//标记  
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除  
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
			**i = 0;
	

		//直到没有点满足，算法结束  
		if (mFlag.empty())
			break;
		else
			mFlag.clear();//将mFlag清空  
	}
	return dst;
}

/***************************************************************************
* 函数名称：   DifferntPoint
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const std::vector<double> src
* 参　　数：   [in]  const double dbDotPrecision
* 返回值　：   DLLS_PORT double
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/22      饶智博        添加
***************************************************************************/
DLLS_PORT double DifferntPoint(const std::vector<double> src, const double dbDotPrecision /* = 1 */)
{
	double dbPossible = 0;
	double dbAverage = 0;

	const int SCALE = 10;

	for (int i = 0; i < (int)src.size(); i++)
		dbAverage = dbAverage + src.at(i);

	dbAverage /= (int)src.size();

	for (int i = 0; i < (int)src.size(); i++){
		dbPossible += abs(src.at(i) - dbAverage);
	}

	dbPossible /= (int)src.size()*dbAverage;

	return dbPossible;
}


/***************************************************************************
* 函数名称：   AutoCanny
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Mat & src
* 参　　数：   [out] cv::Mat & dst
* 返回值　：   DLLS_PORT void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/05      饶智博        添加
***************************************************************************/
DLLS_PORT void AutoCanny(const cv::Mat & src, cv::Mat & dst)
{
	// 新建一个数据缓存区域
	cv::Mat cvMatTemImg = src.clone();  // 复制图片，以免出现问题
	cv::Mat src_gray;                          // 灰度图   

	if (CV_8UC1 != src.type())
		cvtColor(cvMatTemImg, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatTemImg.clone();

	IplImage* pImage = &IplImage(src_gray);
	IplImage* pCannyImage = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 1);

//	blur(cv::Mat(pImage), cv::Mat(pCannyImage), cv::Size(3, 3));
	// 使用Otsu计算出阈值
	int cannyThreshold = Otsu(pImage);
	// 使用canny算子来计算图片,至于为什么*2...我只能说效果好
	cvCanny(pImage, pCannyImage, cannyThreshold*0.8, cannyThreshold*1.2 , 3);
//	cvCanny(pImage, pCannyImage, 40, 100, 3);


	//cvCanny(pImage, pCannyImage, cannyThreshold*0.1, cannyThreshold, 3);

// 	double low, high;
// 	AdaptiveFindThreshold(pImage, &low, &high);
// 	cvCanny(pImage, pCannyImage, low, high, 3);
	//CannyEdgeDetector

	dst = cv::Mat(pCannyImage).clone();

	cvReleaseImage(&pCannyImage);
}

/***************************************************************************
* 函数名称：   AdaptiveFindThreshold
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const CvArr * image
* 参　　数：   [in]  double * low
* 参　　数：   [in]  double * high
* 参　　数：   [in]  int aperture_size
* 返回值　：   DLLS_PORT void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/26      饶智博        添加
***************************************************************************/
DLLS_PORT void AdaptiveFindThreshold(const CvArr* image, double *low, double *high, int aperture_size)
{
	cv::Mat src = cv::cvarrToMat(image);
	const int cn = src.channels();
	cv::Mat dx(src.rows, src.cols, CV_16SC(cn));
	cv::Mat dy(src.rows, src.cols, CV_16SC(cn));

	cv::Sobel(src, dx, CV_16S, 1, 0, aperture_size, 1, 0, cv::BORDER_DEFAULT);
	cv::Sobel(src, dy, CV_16S, 0, 1, aperture_size, 1, 0, cv::BORDER_DEFAULT);

	CvMat _dx = dx, _dy = dy;
	_AdaptiveFindThreshold(&_dx, &_dy, low, high);
}
                               
/***************************************************************************
* 函数名称：   _AdaptiveFindThreshold
* 摘　　要：   // 仿照matlab，自适应求高低两个门限  
* 全局影响：   public 
* 参　　数：   [in]  CvMat * dx
* 参　　数：   [in]  CvMat * dy
* 参　　数：   [in]  double * low
* 参　　数：   [in]  double * high
* 返回值　：   DLLS_PORT void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/26      饶智博        添加
***************************************************************************/
DLLS_PORT void _AdaptiveFindThreshold(CvMat *dx, CvMat *dy, double *low, double *high)
{
	CvSize size;
	IplImage *imge = 0;
	int i, j;
	CvHistogram *hist;
	int hist_size = 255;
	float range_0[] = { 0, 256 };
	float* ranges[] = { range_0 };
	double PercentOfPixelsNotEdges = 0.7;
	size = cvGetSize(dx);
	imge = cvCreateImage(size, IPL_DEPTH_32F, 1);
	// 计算边缘的强度, 并存于图像中                                          
	float maxv = 0;
	for (i = 0; i < size.height; i++)
	{
		const short* _dx = (short*)(dx->data.ptr + dx->step*i);
		const short* _dy = (short*)(dy->data.ptr + dy->step*i);
		float* _image = (float *)(imge->imageData + imge->widthStep*i);
		for (j = 0; j < size.width; j++)
		{
			_image[j] = (float)(abs(_dx[j]) + abs(_dy[j]));
			maxv = maxv < _image[j] ? _image[j] : maxv;

		}
	}
	if (maxv == 0){
		*high = 0;
		*low = 0;
		cvReleaseImage(&imge);
		return;
	}

	// 计算直方图                                                            
	range_0[1] = maxv;
	hist_size = (int)(hist_size > maxv ? maxv : hist_size);
	hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	cvCalcHist(&imge, hist, 0, NULL);
	int total = (int)(size.height * size.width * PercentOfPixelsNotEdges);
	float sum = 0;
	int icount = hist->mat.dim[0].size;

	float *h = (float*)cvPtr1D(hist->bins, 0);
	for (i = 0; i < icount; i++)
	{
		sum += h[i];
		if (sum > total)
			break;
	}
	// 计算高低门限                                                          
	*high = (i + 1) * maxv / hist_size;
	*low = *high * 0.4;
	cvReleaseImage(&imge);
	cvReleaseHist(&hist);
}

/***************************************************************************
* 函数名称：   GetSevenColor
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  int iColorCode
* 返回值　：   DLLS_PORT cv::Scalar
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/13      饶智博        添加
***************************************************************************/
DLLS_PORT cv::Scalar GetSevenColor(int iColorCode)
{
	// return the color(rgb)
	switch (iColorCode)
	{
	case 0:
		return cv::Scalar(255, 0, 0);
	case 1:
		return cv::Scalar(255, 128, 0);
	case 2:
		return cv::Scalar(255, 255, 0);
	case 3:
		return cv::Scalar(0, 255, 0);
	case 4:
		return cv::Scalar(0, 255, 255);
	case 5:
		return cv::Scalar(0, 0, 255);
	case 6:
		return cv::Scalar(128, 0, 255);
	default:
		return cv::Scalar(0, 0, 0);
		break;
	}
}

DLLS_PORT void GetSevenColor(const int iColorCode, uchar& R, uchar& G, uchar& B)
{
	// return the color(rgb)
	switch (iColorCode)
	{
	case 0:
		R = 255;
		G = 0;
		B = 0;
		return;
	case 1:
		R = 255;
		G = 128;
		B = 0;
		return;
	case 2:
		R = 255;
		G = 255;
		B = 0;
		return;
	case 3:
		R = 0;
		G = 255;
		B = 0;
		return;
	case 4:
		R = 0;
		G = 255;
		B = 255;
		return ;
	case 5:
		R = 0;
		G = 0;
		B = 255;
		return;
	case 6:
		R = 128;
		G = 0;
		B = 255;
		return;
	default:
		R = 0;
		G = 0;
		B = 0;
		return;
	}
}
