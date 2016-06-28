#include "BaseImageHandleFunction.h"

/***************************************************************************
* �������ƣ�   Otsu
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  IplImage * src
* ����ֵ����   int
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/14      ���ǲ�        ���
***************************************************************************/
DLLS_PORT int  Otsu(IplImage* src)
{
	int iHeight = src->height;			 // ͼƬ�߶�
	int iWidth = src->width;             // ͼƬ���
	long lSize = iHeight * iWidth;       // ͼƬ��С

	float fHistogram[GRAY_SCALE_VALUE] = { 0 }; 
	for (int i = 0; i < iHeight; i++){
		unsigned char* p = (unsigned char*)src->imageData + src->widthStep * i;
		for (int j = 0; j < iWidth; j++)
			fHistogram[int(*p++)]++;
	}

	int    iThreshold;                         // ���ص���ֵ
	long   lFrontgroundTotalGrayValue = 0,
		   lBackgroundTotalGrayValue = 0;      // �洢ǰ���ĻҶ��ܺͺͱ����Ҷ��ܺ�  
	long   lFrontgroundTotalNum = 0, 
		   lBackgroundTotalNum = 0;            // ǰ�����ܸ����ͱ������ܸ���  
	double dbFrontgroundRation = 0, 
		   dbBackgroundRation = 0;             // ǰ���ͱ�����ռ����ͼ��ı���  
	double dbFrontgroundAveGrayValue = 0, 
		   dbBackgroundAveGrayValue = 0;       // ǰ���ͱ�����ƽ���Ҷ�  
	double dbVariance = 0;                     // �����䷽��  
	double dbAveGrayValue = 0;
	double dbMaxVariance = 0;

	for (int i = 1; i < GRAY_SCALE_VALUE; i++) //һ�α���ÿ������  
	{
		// ���³�ʼ��
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
			+ dbBackgroundAveGrayValue * dbBackgroundRation; //ͼ���ƽ���Ҷ�  
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
* �������ƣ�   Get3DPos
* ժ����Ҫ��   ///��ì�ܷ�����
* ȫ��Ӱ�죺   public 
* �Ρ�������   [inout]  double * pdbQ
* �Ρ�������   [inout]  double * pdb2D
* �Ρ�������   [inout]  double * pdbSolution
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/21      ���ǲ�        ���
***************************************************************************/
DLLS_PORT void  Get3DPos(double* pdbQ, double* pdb2D, double *pdbSolution)
{
	// ����XYZW4�������Ľ�
	double* pdb4D = new double[PARAMETER_XYZW_NUM];
	for (int i = 0; i < PARAMETER_XYZW_NUM; i++){
		pdb4D[i] = 0;
		for (int j = 0; j < PARAMETER_XYZW_NUM; j++){
			pdb4D[i] += *(pdbQ + i * PARAMETER_XYZW_NUM + j) * *(pdb2D + j);
		}
	}

	// ����XYZ3�������Ľ�
	for (int i = 0; i < PARAMETER_XYZ_NUM; i++)
		*(pdbSolution + i) = (*(pdb4D + i) / *(pdb4D + PARAMETER_XYZ_NUM));

	delete[] pdb4D;
	pdb4D = NULL;
}

/***************************************************************************
* �������ƣ�   AddGaussianNoise
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  IplImage * src
* �Ρ�������   [in]  double dbVariance
* ����ֵ����   DLLS_PORT IplImage*
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/22      ���ǲ�        ���
***************************************************************************/
DLLS_PORT IplImage*  AddGaussianNoise(IplImage* src, double dbVariance)
{
	if (!src)
		return FALSE;

	return AddGaussianNoise(src, GAUSSIAN_MEAN, dbVariance);
}

/***************************************************************************
* �������ƣ�   AddGaussianNoise
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  IplImage * src
* �Ρ�������   [in]  double dbMean
* �Ρ�������   [in]  double dbVariance
* ����ֵ����   DLLS_PORT IplImage*
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/22      ���ǲ�        ���
***************************************************************************/
DLLS_PORT IplImage*  AddGaussianNoise(IplImage* src, double dbMean, double dbVariance /* = GAUSSIAN_VARIANCE */)
{
	if (!src)
		return FALSE;

	// ����ͼƬ
	IplImage* temImg = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	cvCopy(src, temImg);

	// ��ʼ������,��Щ��������ʱ��Ҫʹ�õ�
	cv::Mat cvMatImg(temImg);                    // ת����Ϊһ������
	cv::Mat cvMatDst(temImg, true);
	cv::Mat cvMatNoise(cvMatImg.size(), CV_64F);
	
	// ͼƬ��һ������
	normalize(cvMatImg, cvMatDst, GAUSSIAN_MEAN, GAUSSIAN_VARIANCE, CV_MINMAX, CV_64F);
	// ��������
	cv::randn(cvMatNoise, dbMean, dbVariance);
	// ��������
	cvMatDst = cvMatDst + cvMatNoise;
	// ��һ������
	normalize(cvMatDst, cvMatDst, GAUSSIAN_MEAN, GAUSSIAN_VARIANCE, CV_MINMAX, CV_64F);

	// ����������ҪͼƬ����
	IplImage temRelsutImg = (IplImage)(cvMatDst);
	IplImage* relsutImg = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);

	// ���ڼ��㶼��8λ�ĻҶ�ͼ��������Ҫ��ԭ��64Ϊ������ת����8δ������Ϊ255
	cvConvertScaleAbs(&temRelsutImg, relsutImg, CONVERT_SCALE);

	// ɾ����ʱ�ı���,�����ڴ�й¶
	cvReleaseImage(&temImg);

	return relsutImg;
}

/***************************************************************************
* �������ƣ�   GradientEstimation
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  IplImage * src
* �Ρ�������   [in]  cv::Mat & grad_x
* �Ρ�������   [in]  cv::Mat & grad_y
* �Ρ�������   [in]  BOOL bShow
* ����ֵ����   DLLS_PORT void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/05/09      ���ǲ�        ���
***************************************************************************/
DLLS_PORT void GradientEstimation(IplImage* src, cv::Mat &grad_x, cv::Mat &grad_y, BOOL bShow /* = false */)
{
	cv::Mat cvMatImg(src);
	return GradientEstimation(cvMatImg, grad_x, grad_y, bShow);
}

/***************************************************************************
* �������ƣ�   GradientEstimation
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  cv::Mat src
* �Ρ�������   [in]  cv::Mat & grad_x
* �Ρ�������   [in]  cv::Mat & grad_y
* �Ρ�������   [in]  BOOL bShow
* ����ֵ����   DLLS_PORT void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/05/09      ���ǲ�        ���
***************************************************************************/
DLLS_PORT void GradientEstimation(cv::Mat src, cv::Mat &grad_x, cv::Mat &grad_y, BOOL bShow)
{
	// ʹ����ȿ����������������
	cv::Mat src_gray = src.clone();
	int scale = 1;
	int delta = 1;
	int ddepth = CV_16S;

	// ���ͼƬû�����ݾͷ���
	if (!src.data)
		return ;
	
	// ��ͼƬ�����˹�˺���
	GaussianBlur(src_gray, src_gray, cv::Size(5, 5), 0, 1.28, cv::BORDER_DEFAULT);

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

	if (bShow)
	{	// ��ʾ���ڵ�����
		char* window_name = "GradientEstimation";
		cv::Mat grad;
		addWeighted(grad_x, 0.5, grad_y, 0.5, 0, grad);
		imshow(window_name, grad);
		cvWaitKey(0);
	}

}

/***************************************************************************
* �������ƣ�   GradientEstimation
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  cv::Mat src
* �Ρ�������   [in]  cv::Mat & dst
* ����ֵ����   DLLS_PORT void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/05/09      ���ǲ�        ���
***************************************************************************/
DLLS_PORT void GradientEstimation(cv::Mat src, cv::Mat &dst)
{
	// ʹ����ȿ����������������
	cv::Mat src_gray = src.clone();
	int scale = 1;
	int delta = 1;
	int ddepth = CV_16S;

	// ���ͼƬû�����ݾͷ���
	if (!src.data)
		return;

	// ��ͼƬ�����˹�˺���
	GaussianBlur(src_gray, src_gray, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);

	// ��������ͼƬת�ɻҶ�ͼ
	//	cvtColor(src, src_gray, CV_BGR2GRAY);

	cv::Mat tem_grad_x, tem_grad_y;
	cv::Mat grad_x, grad_y;

	// �����ݶ�
	/// Gradient X
	Sobel(src_gray, tem_grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
	/// Gradient Y
	Sobel(src_gray, tem_grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);

	// ת����8λ�õ�ͼƬ
	convertScaleAbs(tem_grad_x, grad_x);
	convertScaleAbs(tem_grad_y, grad_y);

	// �����ݶ�ͼ
	cv::Mat grad;
	addWeighted(grad_x, 0.5, grad_y, 0.5, 0, grad);

	dst = grad.clone();
}

/***************************************************************************
* �������ƣ�   GetCircle
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Point2f & p1
* �Ρ�������   [in]  const cv::Point2f & p2
* �Ρ�������   [in]  const cv::Point2f & p3
* �Ρ�������   [out]  cv::Point2f & center
* �Ρ�������   [out]  float & radius
* ����ֵ����   DLLS_PORT void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/22      ���ǲ�        ���
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
	// ����Բ������
	center.x = (x1*x1 + y1*y1)*(y2 - y3) + (x2*x2 + y2*y2)*(y3 - y1) + (x3*x3 + y3*y3)*(y1 - y2);
	center.x /= (2 * (x1*(y2 - y3) - y1*(x2 - x3) + x2*y3 - x3*y2));

	center.y = (x1*x1 + y1*y1)*(x3 - x2) + (x2*x2 + y2*y2)*(x1 - x3) + (x3*x3 + y3*y3)*(x2 - x1);
	center.y /= (2 * (x1*(y2 - y3) - y1*(x2 - x3) + x2*y3 - x3*y2));

	// ����Բ�ΰ뾶
	radius = sqrt((center.x - x1)*(center.x - x1) + (center.y - y1)*(center.y - y1));

	return;
}

/***************************************************************************
* �������ƣ�   GetDistance
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Point2f p1
* �Ρ�������   [in]  const cv::Point2f p2
* ����ֵ����   DLLS_PORT double
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/22      ���ǲ�        ���
***************************************************************************/
DLLS_PORT double GetDistance(const cv::Point2f p1, const cv::Point2f p2)
{ 
	return sqrtf(powf((p1.x - p2.x), 2) + powf((p1.y - p2.y), 2));
}

/***************************************************************************
* �������ƣ�   IsInCircle
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Point2f point
* �Ρ�������   [in]  const ST_CENTER center
* �Ρ�������   [in]  const double dbPrecision
* ����ֵ����   DLLS_PORT BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/22      ���ǲ�        ���
***************************************************************************/
DLLS_PORT BOOL IsInCircle(const cv::Point point, const ST_CENTER center, const double dbPrecision)
{
	if (abs(GetDistance(point, center.location)-center.radius )<= dbPrecision)
		return TRUE;
	else
		return FALSE;
}


/***************************************************************************
* �������ƣ�   CalculateGradientDrectAngle
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Point2f grad
* �Ρ�������   [in]  const cv::Point2f twoPointVector
* ����ֵ����   DLLS_PORT BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/23      ���ǲ�        ���
***************************************************************************/
DLLS_PORT double CalculateGradientDrectAngle(const cv::Point2f grad, const cv::Point2f twoPointVector)
{
	return (DotProduct(grad, twoPointVector) / CalculateVectorDistance(twoPointVector));
}

/***************************************************************************
* �������ƣ�   CalculateVectorDistance
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Point2f twoPointVector
* ����ֵ����   DLLS_PORT double
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/23      ���ǲ�        ���
***************************************************************************/
DLLS_PORT double CalculateVectorDistance(const cv::Point2f twoPointVector)
{
	return sqrtf(powf((twoPointVector.x), 2) + powf((twoPointVector.y), 2));
}

/***************************************************************************
* �������ƣ�   DotProduct
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Point2f p1
* �Ρ�������   [in]  const cv::Point2f p2
* ����ֵ����   DLLS_PORT double
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/23      ���ǲ�        ���
***************************************************************************/
DLLS_PORT double DotProduct(const cv::Point2f p1, const cv::Point2f p2)
{
	return (p1.x*p2.x + p1.y*p2.y);
}

/***************************************************************************
* �������ƣ�   NormalizerPoint
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Point2f p1
* �Ρ�������   [out] cv::Point2f & p2
* ����ֵ����   DLLS_PORT BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/23      ���ǲ�        ���
***************************************************************************/
DLLS_PORT BOOL NormalizerPoint(const cv::Point2f p1, cv::Point2f& p2)
{
	const double distance = CalculateVectorDistance(p1);	
	if (distance == 0)
		return FALSE;

	p2.x =(float)( p1.x / distance);
	p2.y =(float)( p1.y / distance);

	return TRUE;
}

/***************************************************************************
* �������ƣ�   DetectIsoscelesTriangles
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Mat src
* �Ρ�������   [in]  const cv::Point2f p1
* �Ρ�������   [in]  const cv::Point2f & p2
* ����ֵ����   DLLS_PORT BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/23      ���ǲ�        ���
*2016/04/23      ���ǲ�        ����������ʹ����һ����������
***************************************************************************/
DLLS_PORT BOOL DetectIsoscelesTriangles(const cv::Mat src, const cv::Point2f p1, const cv::Point2f& p2)
{
	cv::Mat cvMatTemImg = src.clone();                       // ����ͼƬ���������Ⱦ
	cv::Mat grad_x, grad_y;                                  // �ݶ�����
	cv::Point2f gradPoint;                                   // ׼����Ϊȥ��ĳ��������

	GradientEstimation(cvMatTemImg, grad_x, grad_y,false);   // �����ݶ�

	cv::Point2f grad_p1,grad_p2;

	// ���ݶ�����
	grad_p1.x = (float)grad_x.at<unsigned char>((int)p1.x,(int)p1.y);
	grad_p1.y = (float)grad_y.at<unsigned char>((int)p1.x, (int)p1.y);
	NormalizerPoint(grad_p1, grad_p1);
	
	grad_p2.x = (float)grad_x.at<unsigned char>((int)p2.x, (int)p2.y);
	grad_p2.y = (float)grad_y.at<unsigned char>((int)p2.x, (int)p2.y);
	NormalizerPoint(grad_p2, grad_p2);
	
	cv::Point2f p1_p2,p2_p1;
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
* �������ƣ�   DetectIsoscelesTriangles
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Mat grad_x
* �Ρ�������   [in]  const cv::Mat grad_y
* �Ρ�������   [in]  const cv::Point2f p1
* �Ρ�������   [in]  const cv::Point2f p2
* �Ρ�������   [in]  const double dbDifPrecision
* �Ρ�������   [in]  const double dbDotPrecision
* ����ֵ����   DLLS_PORT BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/05/06      ���ǲ�        ���
***************************************************************************/
DLLS_PORT BOOL DetectIsoscelesTriangles(const cv::Mat grad_x, const cv::Mat grad_y,
	const cv::Point2f p1, const cv::Point2f p2,
	const double dbDifPrecision , const double dbDotPrecision)
{
	cv::Point2f grad_p1, grad_p2;
	// ȡ�����ݶ�����
	grad_p1.x = (float)grad_x.at<unsigned char>((int)p1.y ,(int)p1.x);
	grad_p1.y = (float)grad_y.at<unsigned char>((int)p1.y, (int)p1.x);
	NormalizerPoint(grad_p1, grad_p1);                                    // ��һ��

	grad_p2.x = (float)grad_x.at<unsigned char>((int)p2.y, (int)p2.x) ;
	grad_p2.y = (float)grad_y.at<unsigned char>((int)p2.y, (int)p2.x);
	NormalizerPoint(grad_p2, grad_p2);                                    // ��һ��

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
* �������ƣ�   AnalysisCluster
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const std::vector<cv::Point> contour
* �Ρ�������   [in]  const cv::Mat grad_x
* �Ρ�������   [in]  const cv::Mat grad_y
* �Ρ�������   [in]  const double dbDifPrecision
* �Ρ�������   [in]  const double dbDotPrecision
* ����ֵ����   DLLS_PORT BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/25      ���ǲ�        ���
***************************************************************************/
DLLS_PORT BOOL AnalysisCluster(const std::vector<cv::Point> contour, const cv::Mat grad_x,
	const cv::Mat grad_y, const double dbDifPrecision, const double dbDotPrecision)
{
	// ���С��2���㣬��ôһ������Բ
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

		// ˵������ʵ��
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
* �������ƣ�   IsThreePointInLine
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  cv::Point2f p1
* �Ρ�������   [in]  cv::Point2f p2
* �Ρ�������   [in]  cv::Point2f p3
* ����ֵ����   DLLS_PORT BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/25      ���ǲ�        ���
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
* �������ƣ�   IsInCircle
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const std::vector<cv::Point> counter
* �Ρ�������   [in]  std::vector<ST_CENTER> & center
* ����ֵ����   DLLS_PORT BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/25      ���ǲ�        ���
***************************************************************************/
DLLS_PORT BOOL IsInCircle(const std::vector<cv::Point> contour, std::vector<ST_CENTER>& center,const double dbPrecision)
{
	// ����ʵ�����
	const int iTestNum = 1000;

	// ����Բ�ε�����
	for (int i = 0; i < (int)center.size(); i++)
	{
		int iSuccessNum = 0;
		// ������Ŀ��Ŷ�
		for (int j = 0; j < iTestNum; j++)
		{
			unsigned int idx = rand() % contour.size();
			// ����Ƿ���һ��Բ
			if (IsInCircle(contour.at(idx), center.at(i), dbPrecision))
				iSuccessNum++;
		}
		center.at(i).confidence = (double)iSuccessNum / (double)iTestNum;
	}

	return TRUE;
}


/***************************************************************************
* �������ƣ�   ConvertClosedContour
* ժ����Ҫ��   ������
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  std::vector<cv::Point> & contours
* ����ֵ����   DLLS_PORT void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/05/17      ���ǲ�        ���
***************************************************************************/
DLLS_PORT void ConvertClosedContour(std::vector<cv::Point> &contours)
{
	// ��ʼ��ı߽��
	const int ix_min = contours.at(0).x - 1;
	const int ix_max = contours.at(0).x + 1;
	const int iy_min = contours.at(0).y - 1;
	const int iy_max = contours.at(0).y + 1;

	const int iPoint_x = contours.at(contours.size() - 1).x;
	const int iPoint_y = contours.at(contours.size() - 1).y;

	// ��˵���Ǳպ�����,�ͷ���
	if (iPoint_x >= ix_min
		&&iPoint_y >= iy_min
		&&iPoint_x <= ix_max
		&&iPoint_y <= iy_max)
		return;

	// ����ɱպ�����
	// ��ĩβ�ĵ�ȡ����
	int dx = iPoint_x - contours.at(0).x;
	int dy = iPoint_y - contours.at(0).y;

	if (abs(dx) >= abs(dy))
	{
		float k = ((float)(dy)) / ((float)(dx));
		float b = (float)iPoint_y - k*iPoint_x;
		int x, y;

		if (dx > 0)
		for (int i = 0; i < abs(dx) ; i++)
		{
			x = iPoint_x + i;
			y = (int)(k*x + b);
			contours.push_back(cv::Point(x, y));
		}
		else
		for (int i = 0; i < abs(dx) ; i++)
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
		for (int i = 0; i < abs(dy) ; i++)
		{
			y = iPoint_y + i;
			x = (int)(k*y + b);
			contours.push_back(cv::Point(x, y));
		}
		else
		for (int i = 0; i < abs(dy) ; i++)
		{
			y = iPoint_y - i;
			x = (int)(k*y + b);
			contours.push_back(cv::Point(x, y));
		}

		return;
	}
}


/***************************************************************************
* �������ƣ�   FindNick
* ժ����Ҫ��   ������
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  std::vector<cv::Point> & contours
* ����ֵ����   DLLS_PORT BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/05/17      ���ǲ�        ���
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
* �������ƣ�   ConvertRgbToYuv
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  BYTE * src
* �Ρ�������   [out]  BYTE * dst
* �Ρ�������   [in]  const int width
* �Ρ�������   [in]  const int height
* ����ֵ����   DLLS_PORT BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/05/25      ���ǲ�        ���
***************************************************************************/
DLLS_PORT BOOL ConvertRgbToYuv(BYTE* src, BYTE* dst, const int width, const int height)
{
	// ������������Ŀؼ���û���ٺã��ͷ���
	if (NULL == src || NULL == dst)
		return FALSE;

	int len = width * height;
	const int rgba_step = 4;    // ÿ4���ֽڴ���RGBA
	int y, u, v;

	// һ��һ��ɨ��ȥ
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			// ��õ�ǰ����ֵ
			int rgba = src[(i * width + j) * rgba_step];

			int r = (rgba >> 8) & 0xFF;
			int g = (rgba >> 4) & 0xFF;
			int b = (rgba >> 2) & 0xFF;

			y = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
			u = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
			v = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;

			//����
			y = y < 16 ? 16 : (y > 255 ? 255 : y);
			u = u < 0 ? 0 : (u > 255 ? 255 : u);
			v = v < 0 ? 0 : (v > 255 ? 255 : v);

			// ��ֵ
			dst[i * width + j] = (BYTE)y;
			dst[len + (i >> 1) * width + (j & ~1) + 0] = (BYTE)u;
			dst[len + +(i >> 1) * width + (j & ~1) + 1] = (BYTE)v;
		}
	}

	return TRUE;
}

/***************************************************************************
* �������ƣ�   DrawCircle
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  cv::Mat & src
* �Ρ�������   [out]  cv::Mat & keypointsImage
* �Ρ�������   [in]  std::vector<ST_CENTER> centers
* ����ֵ����   DLLS_PORT BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/27      ���ǲ�        ���
***************************************************************************/
DLLS_PORT BOOL DrawCircle(cv::Mat& src, cv::Mat& keypointsImage, std::vector<ST_CENTER> centers)
{
	if (CV_8UC1 != src.type())
		return FALSE;

	cvtColor(src, keypointsImage, CV_GRAY2RGB);
	for (int i = 0; i < (int)centers.size(); i++)
	{
		ST_CENTER center;
		center = centers.at(i);
		cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // ��һ��Բ 
		cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // ��һ��Բ 
	}

	return TRUE;
}