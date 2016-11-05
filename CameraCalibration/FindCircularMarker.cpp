#include "FindCircularMarker.h"
#include "FindCountersMethod.h"

int FindCircularMarker::m_siGobalThreHold = 0;

/***************************************************************************
* �������ƣ�   FindCircularMarker
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/21      ���ǲ�        ���
***************************************************************************/
FindCircularMarker::FindCircularMarker():
	m_fileName(NULL)
{
}

/***************************************************************************
* �������ƣ�   ~FindCircularMarker
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/21      ���ǲ�        ���
***************************************************************************/
FindCircularMarker::~FindCircularMarker()
{
	if (NULL != m_fileName)
		delete m_fileName;
}

/***************************************************************************
* �������ƣ�   FindCircleCenter
* ժ����Ҫ��
* ȫ��Ӱ�죺   public
* �Ρ�������   [in]  IplImage * pImg
* �Ρ�������   [in]  cv::SimpleBlobDetector::Params params
* �Ρ�������   [out] std::vector<cv::KeyPoint> & Vct_ponits
* �Ρ�������   [in]  BOOL bShow
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/21      ���ǲ�        ���
***************************************************************************/
BOOL FindCircularMarker::FindCircleCenter(IplImage* pImg,
	cv::SimpleBlobDetector::Params params,
	std::vector<cv::KeyPoint>& Vct_ponits,
	BOOL bShow)
{
	cv::Mat cvImgMat(pImg);
	CvSize cvImgSize = cvGetSize(pImg);
	cv::Mat cvBinaryImage(pImg);
	cv::Mat cvMask;

	cv::SimpleBlobDetector* cvDetector = new cv::SimpleBlobDetector(params);

#ifdef TIME_TEST
	double t = (double)cv::getTickCount();
#endif // TIME_TEST

	cvDetector->detect(cvImgMat, Vct_ponits,cvMask);

#ifdef TIME_TEST
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Find Circle by  opencv function in seconds: " << t << std::endl;
#endif
	
	if (bShow){
		drawKeypoints(cvImgMat, Vct_ponits, cvImgMat, cv::Scalar(255, 0, 0), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		cv::namedWindow("SimpleBlobDetector",CV_WINDOW_NORMAL);
		imshow("SimpleBlobDetector", cvImgMat);
		cvWaitKey(0);
	}

// �����ã�ԭ���ǲ���Ҫ��ʹ��
//	int thresh = 10;
//	int max_thresh = 255;
//	cv::createTrackbar("thresh:", "SimpleBlobDetector", &thresh, max_thresh, FindCircularMarker::Trackbar_callback, &cvBinaryImage);
//	cvWaitKey(0);
	delete cvDetector;
	return TRUE;
}

/***************************************************************************
* �������ƣ�   Trackbar_callback
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected static 
* �Ρ�������   [in]  int Pos
* �Ρ�������   [in]  void * Usrdata
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/21      ���ǲ�        ���
***************************************************************************/
void FindCircularMarker::Trackbar_callback(int Pos, void* Usrdata)
{
	cv::Mat cvMatBinarizedImage;
	cv::Mat cvMatDst;
	cvMatBinarizedImage = *(cv::Mat*)(Usrdata);
	Canny(cvMatBinarizedImage, cvMatDst, Pos, Pos * 2);

	//threshold(binarizedImage, dst, Pos, 255, THRESH_BINARY);
	m_siGobalThreHold = cv::getTrackbarPos("thresh:", "SimpleBlobDetector");
	imshow("SimpleBlobDetector", cvMatDst);
}


/***************************************************************************
* �������ƣ�   CalCircleCentrePoint
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  IplImage * pImg
* �Ρ�������   [in]  std::vector<cv::KeyPoint> & Vct_ponits
* �Ρ�������   [in]  int ipointscounts
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/21      ���ǲ�        ���
***************************************************************************/
void FindCircularMarker::CalCircleCentrePoint(IplImage* pImg, std::vector<cv::KeyPoint>&Vct_ponits, int ipointscounts)
{
	for (int iCirclenCounts = 0; iCirclenCounts < (int)Vct_ponits.size(); iCirclenCounts++)
	{
		/// ���Ե�Բ������
		int iCentreX = (int)Vct_ponits.at(iCirclenCounts).pt.x;
		int iCentreY = (int)Vct_ponits.at(iCirclenCounts).pt.y;

		///ɨ�赽�ı�Ե�㼯
		int aiPos[CAL_PONIT_NUM][SYSTEM_XY];

		memset(aiPos, 0, sizeof(int)* CAL_PONIT_NUM * SYSTEM_XY);

		/// ��0�㣨ˮƽ���ң�Ϊ��㣬����ʱ�뷽��ÿ��5����Բ����ȡһ�㣬����Բ�ķ�����������ĳһ�������ֵΪ255ʱ����Ϊ��Ե�㡣
		for (int iAngle = 0; iAngle < 360; iAngle += 5)
		{
			for (int iR = 1; iR < 25; iR++)
			{
				int iX = (int)(iCentreX + (iR * cos((double)iAngle * PI / 180) + (((iAngle >= 0 && iAngle <= 90) || (iAngle >= 270 && iAngle < 360)) ? 0.5 : -0.5)));		// ���жϵ��X���꣬ʽ�е�0.5Ϊ����ֵ
				int iY = (int)(iCentreY - (iR * sin((double)iAngle * PI / 180) + ((iAngle >= 0 && iAngle < 180) ? 0.5 : -0.5)));		// ���жϵ��Y���꣬ʽ�е�0.5Ϊ����ֵ

				/// �ж��Ƿ񳬳�ͼ��ķ�Χ
				if (iX < 0 || iX >= pImg->width || iY < 0 || iY >= pImg->height)
				{
					aiPos[iAngle / 5][POINTS_X] = iX - iCentreX;					// ɨ�赽�ı�Ե�㼯��iPos�����ԣ�iCentreX��iCentreY��Ϊ�������ĵ�����
					aiPos[iAngle / 5][POINTS_Y] = iY - iCentreY;
					break;
				}

				uchar ucVal = (uchar)*(pImg->imageData + iY * pImg->widthStep + iX * pImg->nChannels + 0);
				if (255 == ucVal)
				{
					aiPos[iAngle / 5][POINTS_X] = iX - iCentreX;					// ɨ�赽�ı�Ե�㼯��iPos�����ԣ�iCentreX��iCentreY��Ϊ�������ĵ�����
					aiPos[iAngle / 5][POINTS_Y] = iY - iCentreY;
					break;
				}
			}
		}

		/// ���ý������򷨶�ɨ�赽�ı�Ե�㼯���䵽Բ�ĵľ����ɽ���Զ��������
		for (int i = 0; i < CAL_PONIT_NUM; i++)
		{
			double iDistance = sqrt((double)(aiPos[i][0] * aiPos[i][0] + aiPos[i][1]/* -iCentreY*/ * aiPos[i][1]));
			for (int j = i + 1; j < CAL_PONIT_NUM; j++)
			{
				double iTemp = sqrt((double)(aiPos[j][0] * aiPos[j][0] + aiPos[j][1]/* -iCentreY*/ * aiPos[j][1]));
				if (iTemp - iDistance < 0)
				{
					int x = aiPos[i][0];
					int y = aiPos[i][1];
					aiPos[i][0] = aiPos[j][0];
					aiPos[i][1] = aiPos[j][1];
					aiPos[j][0] = x;
					aiPos[j][1] = y;

					iDistance = sqrt((double)(aiPos[i][0] * aiPos[i][0] + aiPos[i][1]/* -iCentreY*/ * aiPos[i][1]));
				}
			}
		}

		/// ȡλ��iPos�м��12���㣬������ϱ�ԵԲ��
		int iFitPos[36][2];				// ��ϵ�����
		memset(iFitPos, 0, sizeof(int)* 36 * 2);
		for (int i = 0; i < 36; i++)
		{
			iFitPos[i][0] = aiPos[24 + i][0];
			iFitPos[i][1] = aiPos[24 + i][1];
		}

		/// ��������С����������Բ�����꼰�뾶
		/// ����A
		CvMat* matA = cvCreateMat(3, 3, CV_64FC1);
		cvZero(matA);
		for (int i = 0; i < 36; i++)
		{
			*(matA->data.db + 0) += iFitPos[i][0] * iFitPos[i][0];
			*(matA->data.db + 1) += iFitPos[i][0] * iFitPos[i][1];
			*(matA->data.db + 2) += iFitPos[i][0];
			*(matA->data.db + 3) += iFitPos[i][0] * iFitPos[i][1];
			*(matA->data.db + 4) += iFitPos[i][0] * iFitPos[i][0];
			*(matA->data.db + 5) += iFitPos[i][1];
			*(matA->data.db + 6) += iFitPos[i][0];
			*(matA->data.db + 7) += iFitPos[i][1];
		}
		*(matA->data.db + 8) = 36;

		/// ����B
		CvMat* matB = cvCreateMat(3, 1, CV_64FC1);
		cvZero(matB);
		for (int i = 0; i < 36; i++)
		{
			*(matB->data.db + 0) -= iFitPos[i][0] * iFitPos[i][0] * iFitPos[i][0] + iFitPos[i][1] * iFitPos[i][1] * iFitPos[i][0];
			*(matB->data.db + 1) -= iFitPos[i][0] * iFitPos[i][0] * iFitPos[i][1] + iFitPos[i][1] * iFitPos[i][1] * iFitPos[i][1];
			*(matB->data.db + 2) -= iFitPos[i][0] * iFitPos[i][0] + iFitPos[i][1] * iFitPos[i][1];
		}

		/// ��A�������
		CvMat* matA_Invert = cvCreateMat(3, 3, CV_64FC1);
		cvZero(matA_Invert);
		cvInvert(matA, matA_Invert);

		/// ���Բ�����꼰�뾶
		CvMat* matResult = cvCreateMat(3, 1, CV_64FC1);
		cvMatMul(matA_Invert, matB, matResult);

		double dbResultX = -matResult->data.db[0] / 2;
		double dbResultY = -matResult->data.db[1] / 2;
		double dbResultR = sqrt(dbResultX * dbResultX + dbResultY * dbResultY - matResult->data.db[2]);

		Vct_ponits.at(iCirclenCounts).pt.x = (float)(dbResultX + iCentreX);
		Vct_ponits.at(iCirclenCounts).pt.y = (float)(dbResultY + iCentreY);

		if (dbResultR != 0)
		{
			Vct_ponits.at(iCirclenCounts).size = (float)(dbResultR*dbResultR);
		}
		else
			Vct_ponits.at(iCirclenCounts).size = Vct_ponits.at(iCirclenCounts).size*Vct_ponits.at(iCirclenCounts).size;


		cvReleaseMat(&matA);
		cvReleaseMat(&matB); 
		cvReleaseMat(&matA_Invert);
		cvReleaseMat(&matResult);
	}
}


/***************************************************************************
* �������ƣ�   FindCircle
* ժ����Ҫ��   �ڵ��øú�������ȷ��ͼƬ�ĸ�ʽΪ8λ��
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  IplImage * pImg
* �Ρ�������   [in]  cv::SimpleBlobDetector::Params params
* �Ρ�������   [in]  std::vector<ST_CENTER> & vct_ponits
* �Ρ�������   [in]  BOOL bShow
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/18      ���ǲ�        ���
***************************************************************************/
BOOL FindCircularMarker::FindCircle(IplImage* pImg, cv::SimpleBlobDetector::Params params, std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
	// ��ʼ��Ҫ���������
	cv::Mat cvMatimage(pImg); // ��ͼƬת���ɾ���
	return FindCircle(cvMatimage,params,vct_ponits,bShow);
}

/***************************************************************************
* �������ƣ�   FindCircle
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  cv::Mat & cvMatimage
* �Ρ�������   [in]  cv::SimpleBlobDetector::Params params
* �Ρ�������   [out] std::vector<ST_CENTER> & vct_ponits
* �Ρ�������   [in]  BOOL bShow
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/18      ���ǲ�        ���
***************************************************************************/
BOOL FindCircularMarker::FindCircle(cv::Mat& cvMatimage, cv::SimpleBlobDetector::Params params, std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{

#ifdef TIME_TEST
	double t = (double)cv::getTickCount();
#endif // TIME_TEST

	// ��ʼ����Ҫʹ�õı���
	vct_ponits.clear();
	std::vector < std::vector<cv::Point> > contours; // ������ǵ�洢��λ��
	cv::Mat keypointsImage;
	cv::Mat src_gray;                          // �Ҷ�ͼ   

	if (CV_8UC1 != cvMatimage.type())
		cvtColor(cvMatimage, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatimage.clone();

	cvtColor(src_gray, keypointsImage, CV_GRAY2RGB);

	/************************************************************************/
	/* ��������                                                                */
	/************************************************************************/
// ÿһ������ʱ����в���
#ifdef STEP_TIME_TEST
	double dbStepTime = (double)cv::getTickCount();
#endif // STEP_TIME_TEST

//	std::vector<cv::Vec4i> hierarchy;
	//CvMemStorage * storage = cvCreateMemStorage(0);
	// �������� �����Mat�����Ƕ�ֵ�����ͼƬ��������������
	findContours(src_gray, contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	//findContours(cvMatimage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	//canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0)

	

	if (bShow)
	{
		cv::Mat contoursImage;
		cvtColor(cvMatimage, contoursImage, CV_GRAY2RGB);
// 		for (int i = 0; i < (int)contours.size(); i++)
// 		{
// 			std::vector < std::vector<cv::Point> > contour;
// 			contour.push_back(contours.at(i));

// 			int curColor = i % 6;
//			if (0 == curColor || 1 == curColor)
 			//if (0==i)
//				drawContours(contoursImage, contour, -1, cv::Scalar(255,0 , 0));
// 			else if (2 == curColor || 3 == curColor)
			//if (1 == i)
 //				drawContours(contoursImage, contour, -1, cv::Scalar(0, 255, 0));
// 			else if (5 == curColor || 4 == curColor)
			//if (2 == i)
//				drawContours(contoursImage, contour, -1, cv::Scalar(0, 255, 255));
			//if (3 == i)
				//drawContours(contoursImage, contour, -1, cv::Scalar(255, 255, 255));
			//if (4 == i)
				//drawContours(contoursImage, contour, -1, cv::Scalar(255, 0, 255));
			//if (5 == i)
				//drawContours(contoursImage, contour, -1, cv::Scalar(255, 255, 0));
			
			//if (6<=i)
				//break;		
			
//		}
		drawContours(contoursImage, contours, -1, cv::Scalar(0, 255, 255));
		//imshow("ContourImg", contoursImage);
		// ����ͼƬ
		if (NULL != m_fileName)
			cvSaveImage(m_fileName, &IplImage(contoursImage));
 		//cvWaitKey(0);
	}

// ÿһ������ʱ����в���
#ifdef STEP_TIME_TEST
	dbStepTime = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Find contours in seconds:" << t << std::endl;
#endif // TIME_TEST


	for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++)
	{

		// �����ж�ĳ������ʹ��
//   		if ((int)contourIdx==420)
//   		{
//   			int x=0;
//   			x++;
 			//std::vector < std::vector<cv::Point> > contour;
 			//contour.push_back(contours[contourIdx]);
 			//drawContours(contoursImage, contour, -1, cv::Scalar(0, 0, 255));
 			//imshow("ContourImg", contoursImage);
  		//}
		// ��Ϊ��������飬��ˣ��ѵڶ���ĸ��ǳ�
		//if (contourIdx % 2)
			//continue;
		// ��δ��������ж������ķ���
//  		cv::circle(keypointsImage, contours[contourIdx].at(0), (int)1, cv::Scalar(0, 0, 255)); // ��һ��Բ 
//  		imshow("FindCircle", keypointsImage);
//  		cvWaitKey(0);
//  		cv::circle(keypointsImage, contours[contourIdx].at(contours[contourIdx].size() / 3), (int)1, cv::Scalar(0, 0, 255)); // ��һ��Բ 
//  		imshow("FindCircle", keypointsImage);
//  		cvWaitKey(0);
//  		cv::circle(keypointsImage, contours[contourIdx].at(contours[contourIdx].size() /2), (int)1, cv::Scalar(0, 0, 255)); // ��һ��Բ 
//  		imshow("FindCircle", keypointsImage);
//  		cvWaitKey(0);
//  		cv::circle(keypointsImage, contours[contourIdx].at(contours[contourIdx].size()-1), (int)1, cv::Scalar(0, 0, 255)); // ��һ��Բ 
//  		imshow("FindCircle", keypointsImage);
//  		cvWaitKey(0);



		ST_CENTER center;
		center.confidence = 1;

// 		if (!FindCircleByThreePoint(contours.at(contourIdx), center, 0.8, 6, 10))
// 			continue;

		cv::Moments moms = cv::moments(cv::Mat(contours[contourIdx]),true);
		double area = moms.m00;
		double perimeter = arcLength(cv::Mat(contours[contourIdx]), true);
//		ConvertClosedContour(contours[contourIdx]);
		// ��������ǲ��պϵģ����»��������
		if (area <= contours[contourIdx].size() && area > 0)
		{
		//	cv::Mat contoursImage;
		//	cvtColor(cvMatimage, contoursImage, CV_GRAY2RGB);

		//	std::vector < std::vector<cv::Point> > contour;
		//	contour.push_back(contours[contourIdx]);
		//	drawContours(contoursImage, contour, -1, cv::Scalar(0, 0, 255));
		//	imshow("ContourImg", contoursImage);

		//	cvWaitKey(0);
		//	CvMat  tmp = cv::Mat(contours[contourIdx]);
		//	area = cvContourArea(&tmp, CV_WHOLE_SEQ);
			std::vector < cv::Point > hull;
			convexHull(cv::Mat(contours[contourIdx]), hull);
			//double area = contourArea(cv::Mat(contours[contourIdx]));
			// ʹ����϶����������ԭ�е�����
			area = contourArea(cv::Mat(hull));
			// �뾶��Ҫ����
			perimeter = perimeter / 2;
			moms = cv::moments(cv::Mat(hull));
		}

/************************************************************************/
/* ͨ����С���ų���Ϣ                                                     */
/************************************************************************/
		// ÿһ������ʱ����в���
#ifdef STEP_TIME_TEST
		dbStepTime = (double)cv::getTickCount();
#endif // STEP_TIME_TEST
		// ���Ƿ���Ҫͨ����С���ǳ���Ϣ
		if (params.filterByArea)
		{
			//double area = moms.m00;
			if (area < params.minArea || area >= params.maxArea)
				continue;
		}
#ifdef STEP_TIME_TEST
		dbStepTime = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		std::cout << "Exclude Infomation by Area in seconds:" << t << std::endl;
#endif // TIME_TEST


/************************************************************************/
/* ͨ��Բ�����ų���Ϣ                                                     */
/************************************************************************/
		// ÿһ������ʱ����в���
#ifdef STEP_TIME_TEST
		double dbStepTime = (double)cv::getTickCount();
#endif // STEP_TIME_TEST
		// ͨ��Բ�����ǳ���Ϣ
		if (params.filterByCircularity)
		{
		//	std::vector < cv::Point > hull;
		//	convexHull(cv::Mat(contours[contourIdx]), hull);
		//	double area = contourArea(cv::Mat(hull));
		
			double ratio = 4 * CV_PI * area / (perimeter * perimeter);
			if (ratio < params.minCircularity || ratio >= params.maxCircularity)
				continue;
		}
#ifdef STEP_TIME_TEST
		dbStepTime = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		std::cout << "Exclude Infomation by Circularity in seconds:" << t << std::endl;
#endif // TIME_TEST

		// ÿһ������ʱ����в���
#ifdef STEP_TIME_TEST
		dbStepTime = (double)cv::getTickCount();
#endif // STEP_TIME_TEST
		// ͨ�����������ų���Ϣ
		if (params.filterByInertia)
		{
			double denominator = sqrt(pow(2 * moms.mu11, 2) + pow(moms.mu20 - moms.mu02, 2));
			const double eps = 1e-2;
			double ratio;
			if (denominator > eps)
			{
				double cosmin = (moms.mu20 - moms.mu02) / denominator;
				double sinmin = 2 * moms.mu11 / denominator;
				double cosmax = -cosmin;
				double sinmax = -sinmin;

				double imin = 0.5 * (moms.mu20 + moms.mu02) - 0.5 * (moms.mu20 - moms.mu02) * cosmin - moms.mu11 * sinmin;
				double imax = 0.5 * (moms.mu20 + moms.mu02) - 0.5 * (moms.mu20 - moms.mu02) * cosmax - moms.mu11 * sinmax;
				ratio = imin / imax;
			}
			else
			{
				ratio = 1;
			}

			if (ratio < params.minInertiaRatio || ratio >= params.maxInertiaRatio)
				continue;

			center.confidence = ratio * ratio;
		}
#ifdef STEP_TIME_TEST
		dbStepTime = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		std::cout << "Exclude Infomation by Inertia in seconds:" << t << std::endl;
#endif // TIME_TEST


		// ÿһ������ʱ����в���
#ifdef STEP_TIME_TEST
		dbStepTime = (double)cv::getTickCount();
#endif // STEP_TIME_TEST
		// ͨ����͹�����ǳ���Ϣ
		if (params.filterByConvexity)
		{
			std::vector < cv::Point > hull;
			convexHull(cv::Mat(contours[contourIdx]), hull);
			//double area = contourArea(cv::Mat(contours[contourIdx]));
			double hullArea = contourArea(cv::Mat(hull));
			double ratio = area / hullArea;
			if (ratio < params.minConvexity || ratio >= params.maxConvexity)
				continue;
		}

#ifdef STEP_TIME_TEST
		dbStepTime = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		std::cout << "Exclude Infomation by Convexity in seconds:" << t << std::endl;
#endif // TIME_TEST

		// �����Բ��
		center.location = cv::Point2d(moms.m10 / moms.m00, moms.m01 / moms.m00);

		if (params.filterByColor)
		{
			if (src_gray.at<uchar>(cvRound(center.location.y), cvRound(center.location.x)) != params.blobColor)
				continue;
		}

		//compute blob radius
		std::vector<double> dists;
		for (size_t pointIdx = 0; pointIdx < contours[contourIdx].size(); pointIdx++)
		{
			cv::Point2d pt = contours[contourIdx][pointIdx];
			dists.push_back(norm(center.location - pt));
		}
		// ������������
		std::sort(dists.begin(), dists.end());
		
//    		if (!CheckReliability(dists,0.65,0.55))
//    			continue;

		// ����뾶
		center.radius = (dists[(dists.size() - 1) / 2] + dists[dists.size() / 2]) / 2.;

		// ����һ�����жԱ�
		BOOL bIsNew = TRUE;
		if (vct_ponits.size()){
			double dist = norm(vct_ponits.at(vct_ponits.size() - 1).location - center.location);
			double disr = vct_ponits.at(vct_ponits.size() - 1).radius - center.radius;
			bIsNew = dist >= params.minDistBetweenBlobs || vct_ponits.at(vct_ponits.size() - 1).radius - center.radius >= params.minDistBetweenBlobs;
		}

		if (bIsNew)
			vct_ponits.push_back(center);                    // ���뵽������ȥ
		else
			continue;

//  		if (center.location.x > 800 && center.location.x<920 && center.location.y>400 && center.location.y < 500)
//  		{
//  			int x = 0;
//  			x++;
//  		}


		// ��ʾͼƬ
		if (bShow)
		{
			cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // ��һ��Բ 
			cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // ��һ��Բ 
		}
	}

#ifdef TIME_TEST
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Find Circle by our function in seconds: " << t << std::endl;
#endif // TIME_TEST

	if (bShow)
	{
		// ��ʾԲ����
		//cvNamedWindow("FindCircle", CV_WINDOW_NORMAL);
		//imshow("FindCircle", keypointsImage);
		//cvSaveImage("1.bmp", &IplImage(keypointsImage));
		//cvWaitKey(0);
	}

	return TRUE;
}

/***************************************************************************
* �������ƣ�   SamplingTrianglesFindCircle
* ժ����Ҫ��   �Աȱ���������ʹ�ñ��˵ķ���
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  IplImage * pImg
* �Ρ�������   [out] std::vector<ST_CENTER> & vct_ponits
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/18      ���ǲ�        ���
***************************************************************************/
void FindCircularMarker::FindCircleBySamplingTriangles(IplImage* pImg, std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
	// ��ʼ��Ҫ���������
	cv::Mat cvMatimage(pImg); // ��ͼƬת���ɾ���
	return FindCircleBySamplingTriangles(cvMatimage, vct_ponits, bShow);
}

/***************************************************************************
* �������ƣ�   FindCircleBySamplingTriangles
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  cv::Mat & cvMatimage
* �Ρ�������   [in]  std::vector<ST_CENTER> & vct_ponits
* �Ρ�������   [in]  BOOL bShow
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/19      ���ǲ�        ���
***************************************************************************/
void FindCircularMarker::FindCircleBySamplingTriangles(const cv::Mat& cvMatimage,
	std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
#ifdef TIME_TEST
	double t = (double)cv::getTickCount();
#endif // TIME_TEST
	// ��ʼ����Ҫʹ�õı���
	vct_ponits.clear();                                  // ��ս������
	cv::Mat cvMatTemImg = cvMatimage.clone();            // ����ͼƬ��������������Դͼ����Ⱦ
	std::vector < std::vector<cv::Point> > contours;     // ������ǵ�洢��λ��
	cv::Mat keypointsImage;                              // ������ͼƬ������ʾʹ��
	cv::Mat grad_x, grad_y;                              // ���ڼ���ͼƬ�ݶ�

	// ����ͼƬ���ݶ�
	GradientEstimation(cvMatTemImg, grad_x, grad_y, bShow); // ���ݶ���Ϣ��������grad_x��grad_y��

	// !!!FIXME�����������Ĵ���������⣬ԭ���������������ֵ��ֵ���������Ҳ���,��ӭ�ṩ���õķ���������
	// �������� �����Mat�����Ƕ�ֵ�����ͼƬ��������������,
	//addWeighted(grad_x, 0.5, grad_y, 0.5, 0, grad);
//	int iThreshold = Otsu(&IplImage(cvMatTemImg));
	//cv::threshold(cvMatTemImg, grad, iThreshold, 255, cv::THRESH_BINARY);

	// ʹ��Otsu�������ֵ
	int cannyThreshold = Otsu(&IplImage(cvMatTemImg));
	IplImage* pCannyImage = cvCreateImage(cvGetSize(&IplImage(cvMatTemImg)), IPL_DEPTH_8U, 1);
	// ʹ��canny����������ͼƬ,����Ϊʲô*2...��ֻ��˵Ч����
	cvCanny(&IplImage(cvMatTemImg), pCannyImage, cannyThreshold, cannyThreshold * 2, 3);
	cv::Mat grad(pCannyImage);
	//imshow("ContourImg", grad);
	//cvWaitKey(0);
	cvtColor(cv::Mat(pCannyImage), keypointsImage, CV_GRAY2RGB);  // �ѻҶ�ͼƬת����RGBͼƬ
	//cv::Mat cvMatContourImg = cv::Mat::zeros(cvMatimage.rows, cvMatimage.cols, CV_8UC3);

	findContours(grad, contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	// ������������
	if (bShow)
	{
		cv::Mat contoursImage;
		cvtColor(cv::Mat(pCannyImage), contoursImage, CV_GRAY2RGB);
		drawContours(contoursImage, contours, -1, cv::Scalar(0, 0, 255));
		imshow("ContourImg", contoursImage);
		cvWaitKey(0);
	}
	//int three=0;

	// ��ʼ���������ж��Ƿ�Բ
	for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++)
	{
		// ��Ϊ��������飬��ˣ��ѵڶ���ĸ��ǳ�
		//if (contourIdx % 2)
		//	continue;

		// Բ����Ϣ�洢λ��
		ST_CENTER center;
		center.confidence = 1;

		cv::Moments moms = cv::moments(cv::Mat(contours[contourIdx]));
		double area = moms.m00;
		if (area < 30 || area >= 1000000)
			continue;

		// �Ƿ����������Բ����Ŀǰʹ�õľ�����5�����飬ֻҪ��2�����㾫��С��2�����أ�����Ϊ����Բ
		if (!FindCircleByThreePoint(contours.at(contourIdx), center, 3, 85, 100))
			continue;

		//std::cout << "Three Point find:" << ++three << std::endl;
		// FIXME�� ��ʵ������ֻ��Ҫ����һ�ξͺã����Ǵ��뻹û�б��޸�
		//int x = rand() % contours.at(contourIdx).size();
		//int y = rand() % contours.at(contourIdx).size();
		//DetectIsoscelesTriangles(cvMatTemImg, contours.at(contourIdx).at(x), contours.at(contourIdx).at(y));

		if (!AnalysisCluster(contours.at(contourIdx),grad_x,grad_y))
			continue;

		// ���뵽�����ȥ
		
		// ����һ�����жԱ�
		BOOL bIsNew = TRUE;
		if (vct_ponits.size()){
			double dist = norm(vct_ponits.at(vct_ponits.size() - 1).location - center.location);
			double disr = vct_ponits.at(vct_ponits.size() - 1).radius - center.radius;
			//bIsNew = dist >= params.minDistBetweenBlobs &&  dist >= vct_ponits.at(vct_ponits.size() - 1).radius && dist >= center.radius;
			//bIsNew = dist >= params.minDistBetweenBlobs &&  vct_ponits.at(vct_ponits.size() - 1).radius - center.radius <= params.minDistBetweenBlobs;
			bIsNew = dist >= 3 || vct_ponits.at(vct_ponits.size() - 1).radius - center.radius >= 3;
		}

		if (bIsNew)
			vct_ponits.push_back(center);                    // ���뵽������ȥ
		else
			continue;
		//vct_ponits.push_back(center);

		// �Ƿ���ʾͼƬ
		if (bShow)
			cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // ��һ��Բ 
	}

#ifdef TIME_TEST
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Find Circle by  SamplingTriangles ��" << t << std::endl;
#endif // TIME_TEST

	if (bShow)
	{
		// ��ʾԲ����
		imshow("FindCircle", keypointsImage);
		cvWaitKey(0);
	}

	// ɾ����ʱʹ�õı���
	cvReleaseImage(&pCannyImage);

	return;
}


/***************************************************************************
* �������ƣ�   FindCircleByThreePoint
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const std::vector<cv::Point> counter
* �Ρ�������   [out] ST_CENTER & center
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/23      ���ǲ�        ���
***************************************************************************/
BOOL FindCircularMarker::FindCircleByThreePoint(const std::vector<cv::Point> counter,  ST_CENTER& center,
	const double dbPrecision,  const UINT iMinSuccessNum /* = 1  */, const UINT iMaxTestNum /* = 10 */)
{
	// ���ĳɹ���������Ҫʵ��Ĵ����ö����Ϊ�Ǽٵ�
	if (iMaxTestNum < iMinSuccessNum)
		return FALSE;

	// �����������С��4�㣬��Ȼ����ʹ���������
	if (counter.size() <= 4)
		return FALSE;

	// ��ʼ������
	ST_CENTER temCenter; // ����һ����ʱ��
	std::vector<ST_CENTER> vcCenters;

	// ��ս������
	memset(&center, 0, sizeof(ST_CENTER));
	vcCenters.clear();

	// ����
	const float confidence = iMinSuccessNum / (float)iMaxTestNum;
	// ʵ����ʵ�����
	int testNum = 0;

	for (int i = 0; (UINT)i < iMaxTestNum;)
	{
		memset(&temCenter, 0, sizeof(ST_CENTER));
		//RANSAC: randomly choose 3 point and create a circle:
		//TODO: choose randomly but more intelligent, 
		//so that it is more likely to choose three points of a circle.  
		unsigned int idx1 = rand() % counter.size();
		unsigned int idx2 = rand() % counter.size();
		unsigned int idx3 = rand() % counter.size();
		unsigned int idx4 = rand() % counter.size();

		// we need 3 different samples:
		if (idx1 == idx2) continue;
		if (idx1 == idx3) continue;
		if (idx3 == idx2) continue;

		// ˵������ʵ��һ�Σ�
		i++;

		// ���㹲�ߣ���Ȼ����һ��Բ
		if (IsThreePointInLine(counter.at(idx1), counter.at(idx2), counter.at(idx3)))
			continue;

		// ����ĵ�Ҳ��������������
		if (idx4 == idx1 || idx4 == idx2 || idx4 == idx3)
			continue;


		testNum++;
		// �õ�һ��Բ
		GetCircle(counter.at(idx1), counter.at(idx2), counter.at(idx3), temCenter.location, temCenter.radius);

		if (IsInCircle(counter.at(idx4), temCenter, dbPrecision))
			vcCenters.push_back(temCenter);
	}

	// �������ʵ��ͼƬ
	if (confidence < vcCenters.size() / (float)testNum)
	{
		double x = 0;
		double y = 0;
		double r = 0;
		// ���Բ�ε�ƽ��ֵ
		for (int i = 0; i < (int)vcCenters.size(); i++)
		{
			x = x + vcCenters.at(i).location.x;
			y = y + vcCenters.at(i).location.y;
			r = r + vcCenters.at(i).radius;
		}

		x = x / (int)vcCenters.size();
		y = y / (int)vcCenters.size();
		r = r / (int)vcCenters.size();

		center.location.x = x;
		center.location.y = y;
		center.radius = r;
		return TRUE;
	}
	else
		return FALSE;
}

/***************************************************************************
* �������ƣ�   FindCircleByThreePoint
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  IplImage * pImg
* �Ρ�������   [in]  std::vector<ST_CENTER> & vct_ponits
* �Ρ�������   [in]  BOOL bShow
* �Ρ�������   [in]  const double dbPrecision
* �Ρ�������   [in]  const UINT iMinSuccessNum
* �Ρ�������   [in]  const UINT iMaxTestNum
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/25      ���ǲ�        ���
***************************************************************************/
BOOL FindCircularMarker::FindCircleByThreePoint(IplImage*pImg, std::vector<ST_CENTER>& vct_ponits, 
	BOOL bShow /* = FALSE  */, const double dbPrecision /* = 0.1 */, 
	const UINT iMinSuccessNum /* = 1 */, const UINT iMaxTestNum /* = 10 */)
{
	cv::Mat cvMatImg(pImg);
	return FindCircleByThreePoint(cvMatImg, vct_ponits, bShow, dbPrecision, iMinSuccessNum, iMaxTestNum);
}

/***************************************************************************
* �������ƣ�   FindCircleByThreePoint
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Mat & cvMatimage
* �Ρ�������   [in]  std::vector<ST_CENTER> & vct_ponits
* �Ρ�������   [in]  BOOL bShow
* �Ρ�������   [in]  const double dbPrecision
* �Ρ�������   [in]  const UINT iMinSuccessNum
* �Ρ�������   [in]  const UINT iMaxTestNum
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/25      ���ǲ�        ���
***************************************************************************/
BOOL FindCircularMarker::FindCircleByThreePoint(const cv::Mat& cvMatimage, std::vector<ST_CENTER>& vct_ponits,
	BOOL bShow /* = FALSE */, const double dbPrecision /* = 0.1 */, 
	const UINT iMinSuccessNum /* = 1 */, const UINT iMaxTestNum /* = 10 */)
{
	// �ȿ�����������
	cv::Mat cvMatTemImg = cvMatimage.clone();


	return TRUE;
}

/***************************************************************************
* �������ƣ�   FindCircleByThreePoint
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const std::vector<cv::Point> counter
* �Ρ�������   [out]  std::vector<ST_CENTER> & center
* �Ρ�������   [in]  const double dbPrecision
* �Ρ�������   [in]  const double dbMinConfidence
* �Ρ�������   [in]  const UINT iMinSuccessNum
* �Ρ�������   [in]  const UINT iMaxTestNum
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/25      ���ǲ�        ���
***************************************************************************/
BOOL FindCircularMarker::FindCircleByThreePoint(const std::vector<cv::Point> counter, 
				std::vector<ST_CENTER>& centers, const double dbPrecision /* = 0.1 */, 
				const double dbMinConfidence /* =0.4 */, const UINT iMinSuccessNum /* = 1 */, 
				const UINT iMaxTestNum /* = 10 */)
{

	// ���ĳɹ���������Ҫʵ��Ĵ����ö����Ϊ�Ǽٵ�
	if (iMaxTestNum < iMinSuccessNum)
		return FALSE;

	// �����������С��4�㣬��Ȼ����ʹ���������
	if (counter.size() <= 4)
		return FALSE;

	// ��ʼ������
	ST_CENTER temCenter; // ����һ����ʱ��
	std::vector<ST_CENTER> vcCenters;
	vcCenters.clear();

	// ���Եļ���Բ��
	for (int i = 0; (UINT)i < iMaxTestNum;)
	{
		memset(&temCenter, 0, sizeof(ST_CENTER));
		//RANSAC: randomly choose 3 point and create a circle:
		//TODO: choose randomly but more intelligent, 
		//so that it is more likely to choose three points of a circle.  
		unsigned int idx1 = rand() % counter.size();
		unsigned int idx2 = rand() % counter.size();
		unsigned int idx3 = rand() % counter.size();

		// we need 3 different samples:
		if (idx1 == idx2) continue;
		if (idx1 == idx3) continue;
		if (idx3 == idx2) continue;

		// ���㹲�ߣ���Ȼ����һ��Բ
		if (IsThreePointInLine(counter.at(idx1), counter.at(idx2), counter.at(idx3)))
			continue;

		// ˵���ɹ���������
		i++;

		// �õ�һ��Բ
		GetCircle(counter.at(idx1), counter.at(idx2), counter.at(idx3), temCenter.location, temCenter.radius);
		
		// ��Ҫ�����ľ����Ƿ���ȷ
		if (!vcCenters.size())
			vcCenters.push_back(temCenter);
		else
		{
			BOOL bOK = TRUE;
			for (int j = 0; j < (int)vcCenters.size(); j++)
			{
				if (GetDistance(vcCenters.at(j).location, temCenter.location) < 15)
				{
					bOK = FALSE;
					break;
				}
			}
			if (bOK)
				vcCenters.push_back(temCenter);
		}
	}

	// �о��Ƿ���Բ
	centers.clear();
	IsInCircle(counter, vcCenters, dbPrecision);
	for (int i = 0; i < (int)vcCenters.size(); i++)
	{
		if (vcCenters.at(i).confidence > dbMinConfidence)
			centers.push_back(vcCenters.at(i));
	}

	// �����Ƕ��ҵ���Բ
	if ((int)centers.size() > 0)
		return TRUE;
	else
		return FALSE;
}

/***************************************************************************
* �������ƣ�   FindCircleBySamplingTrianglesImproved
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Mat & cvMatimage
* �Ρ�������   [in]  std::vector<ST_CENTER> & vct_ponits
* �Ρ�������   [in]  BOOL bShow
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/25      ���ǲ�        ���
***************************************************************************/
void FindCircularMarker::FindCircleBySamplingTrianglesImproved(const cv::Mat& cvMatimage,
	std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
#ifdef TIME_TEST
	double t = (double)cv::getTickCount();
#endif // TIME_TEST
	// ��ʼ����Ҫʹ�õı���
	vct_ponits.clear();                                  // ��ս������
	cv::Mat cvMatTemImg = cvMatimage.clone();            // ����ͼƬ��������������Դͼ����Ⱦ

	std::vector < std::vector<cv::Point> > contours;     // ������ǵ�洢��λ��
	cv::Mat keypointsImage;                              // ������ͼƬ������ʾʹ��
	cv::Mat grad_x, grad_y;                              // ���ڼ���ͼƬ�ݶ�
	cvtColor(cvMatTemImg, keypointsImage, CV_GRAY2RGB);  // �ѻҶ�ͼƬת����RGBͼƬ
	cv::Mat cvMatContourImg = cv::Mat::zeros(cvMatimage.rows, cvMatimage.cols, CV_8UC3);

	// ����ͼƬ���ݶ�
	GradientEstimation(cvMatTemImg, grad_x, grad_y, bShow); // ���ݶ���Ϣ��������grad_x��grad_y��

	// !!!FIXME�����������Ĵ���������⣬ԭ���������������ֵ��ֵ���������Ҳ���,��ӭ�ṩ���õķ���������
	// �������� �����Mat�����Ƕ�ֵ�����ͼƬ��������������,
	cv::Mat grad;
	//	addWeighted(grad_x, 0.5, grad_y, 0.5, 0, grad);
	cv::threshold(cvMatTemImg, grad, 145, 255, cv::THRESH_BINARY);
	findContours(grad, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	// ������������
	if (bShow)
	{
		cv::Mat contoursImage;
		cvtColor(cvMatimage, contoursImage, CV_GRAY2RGB);
		drawContours(contoursImage, contours, -1, cv::Scalar(0, 0, 255));
		imshow("ContourImg", contoursImage);
		cvWaitKey(0);
	}
	//int three=0;

	// ��ʼ���������ж��Ƿ�Բ
	for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++)
	{
		// Բ����Ϣ�洢λ��
		std::vector<ST_CENTER> centers;
		centers.clear();

		// ��ȷ����������
		FindCircleByThreePoint(contours.at(contourIdx), centers, 2);
		if (!centers.size())               
			continue;

		// ���뵽�����ȥ
		for (int i = 0; i < (int)centers.size();i++)
			vct_ponits.push_back(centers.at(i));

		// �Ƿ���ʾͼƬ
		if (bShow)
		{
			for (int i = 0; i < (int)centers.size(); i++)
			{
				cv::circle(keypointsImage, centers.at(i).location, (int)1, cv::Scalar(255, 0, 0)); // ��һ��Բ
				cv::circle(keypointsImage, centers.at(i).location, (int)centers.at(i).radius, cv::Scalar(0, 255, 0)); // ��һ��Բ
			}
		} 
	}

#ifdef TIME_TEST
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Find Circle by  SamplingTriangles ��" << t << std::endl;
#endif // TIME_TEST

	if (bShow)
	{
		// ��ʾԲ����
		imshow("FindCircle", keypointsImage);
		cvWaitKey(0);
	}

	return;
}

/***************************************************************************
* �������ƣ�   FindCircleBySamplingTriangles
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  IplImage * pImg
* �Ρ�������   [in]  std::vector<ST_CENTER> & vct_ponits
* �Ρ�������   [in]  BOOL bShow
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/04/25      ���ǲ�        ���
***************************************************************************/
void FindCircularMarker::FindCircleBySamplingTrianglesImproved(IplImage* pImg, std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
	// ��ʼ��Ҫ���������
	cv::Mat cvMatimage(pImg); // ��ͼƬת���ɾ���
	return FindCircleBySamplingTrianglesImproved(cvMatimage, vct_ponits, bShow);
}

/***************************************************************************
* �������ƣ�   FindCircleByCICImproved
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Mat & cvMatimage
* �Ρ�������   [in]  cv::SimpleBlobDetector::Params params
* �Ρ�������   [in]  std::vector<ST_CENTER> & vct_ponits
* �Ρ�������   [in]  BOOL bShow
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/05/10      ���ǲ�        ���
***************************************************************************/
void FindCircularMarker::FindCircleByCICImproved(const cv::Mat& cvMatImage,
	cv::SimpleBlobDetector::Params params, std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
	vct_ponits.clear();
	// �½�һ�����ݻ�������
	cv::Mat cvMatTemImg = cvMatImage.clone();  // ����ͼƬ�������������
	cv::Mat src_gray;                          // �Ҷ�ͼ   

	if (CV_8UC1 != cvMatImage.type())
		cvtColor(cvMatTemImg, src_gray, CV_BGR2GRAY);
	else
		src_gray=cvMatTemImg.clone();


	// תһ��ͼƬ
	IplImage* pImage = &IplImage(src_gray);
	IplImage* pCannyImage = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 1);

	// �������㷨����ʹ�õ�ʱ��
#ifdef TIME_TEST
	double t = (double)cv::getTickCount();
#endif // TIME_TEST

	// ʹ��Otsu�������ֵ
	int cannyThreshold = Otsu(pImage);
	// ʹ��canny����������ͼƬ,����Ϊʲô*2...��ֻ��˵Ч����
	cvCanny(pImage, pCannyImage, cannyThreshold*0.8, cannyThreshold , 3);
//  	double low,high;
//  	AdaptiveFindThreshold(pImage, &low, &high);
// 	cvCanny(pImage, pCannyImage, low, high, 3);

	// ���Ƿ�Ҫ��ʾ��ͼƬ
	if (bShow){
		const char* windows_name = "canny";
		cvNamedWindow(windows_name, CV_WINDOW_NORMAL);
		imshow(windows_name, cv::Mat(pCannyImage));
		cvWaitKey(0);
	}

	FindCircle(pCannyImage, params, vct_ponits, bShow);

#ifdef TIME_TEST
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Find Circle by CICImproved��" << t << std::endl;
#endif // TIME_TEST

	cvReleaseImage(&pCannyImage);
}


/***************************************************************************
* �������ƣ�   FindCircleImproved
* ժ����Ҫ��   ���������������޷�ʵ����Ҫ�Ĺ���
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  cv::Mat & cvMatimage
* �Ρ�������   [in]  cv::SimpleBlobDetector::Params params
* �Ρ�������   [in]  std::vector<ST_CENTER> & vct_ponits
* �Ρ�������   [in]  BOOL bShow
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/05/17      ���ǲ�        ���
***************************************************************************/
BOOL FindCircularMarker::FindCircleImproved(cv::Mat& cvMatimage, 
											cv::SimpleBlobDetector::Params params, 
											std::vector<ST_CENTER>& vct_ponits, 
											BOOL bShow /* = FALSE */)
{
	// ��ʼ����Ҫʹ�õı���
	vct_ponits.clear();
	std::vector < std::vector<cv::Point> > contours; // ������ǵ�洢��λ��
	cv::Mat keypointsImage;
	cv::Mat src_gray;                               // �Ҷ�ͼ   

	// ����ֻ�����Ҷ�ͼ�����԰Ѳ��ǻҶ�ͼ��ת���ɻҶ�ͼ
	if (CV_8UC1 != cvMatimage.type())
		cvtColor(cvMatimage, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatimage.clone();
	cvtColor(src_gray, keypointsImage, CV_GRAY2RGB);

	// ��������
	findContours(src_gray, contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

	// ��ʼ������Ŀռ�
	ST_CENTER center;
	center.confidence = 1;

	// ��Բ����ʶ��Ͷ�λ
	for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++)
	{
		// �������ռ�
		ZeroMemory(&center, sizeof(ST_CENTER));
		
		// ���㵱ǰ��ͼ�εļ��Ͼ�
		cv::Moments moms = cv::moments(cv::Mat(contours[contourIdx]), true);

		// ͨ������ǳ���Ҫ�������������һ����˵�������ǲ��趨�����ݵ����ֵ����Ϊ��Ӱ��Բ��λ�ľ���
		if (params.filterByArea){
			double area = moms.m00;
			if (area < params.minArea || area >= params.maxArea)
				continue;
		}

		// Բ�ȼ���
		if (params.filterByCircularity){
			double ratio = moms.m00 * moms.m00 / (2 * CV_PI*(moms.mu02 + moms.mu20));
			if (ratio < params.minCircularity || ratio >= params.maxCircularity)
				continue;
		}

		// �����ʼ���
		if (params.filterByInertia)
		{
			double denominator = sqrt(pow(2 * moms.mu11, 2) + pow(moms.mu20 - moms.mu02, 2));
			const double eps = 1e-2;
			double ratio;
			if (denominator > eps)
			{
				double cosmin = (moms.mu20 - moms.mu02) / denominator;
				double sinmin = 2 * moms.mu11 / denominator;
				double cosmax = -cosmin;
				double sinmax = -sinmin;

				double imin = 0.5 * (moms.mu20 + moms.mu02) - 0.5 * (moms.mu20 - moms.mu02) * cosmin - moms.mu11 * sinmin;
				double imax = 0.5 * (moms.mu20 + moms.mu02) - 0.5 * (moms.mu20 - moms.mu02) * cosmax - moms.mu11 * sinmax;
				ratio = imin / imax;
			}
			else
			{
				ratio = 1;
			}

			if (ratio < params.minInertiaRatio || ratio >= params.maxInertiaRatio)
				continue;

			center.confidence = ratio * ratio;
		}

		// ����Բ������Ͱ뾶
		center.location = cv::Point2d(moms.m10 / moms.m00, moms.m01 / moms.m00);

		// ͨ����ɫȥ�ų���Բ����������һ�㲢��ʹ��
		if (params.filterByColor)
		{
			if (src_gray.at<uchar>(cvRound(center.location.y), cvRound(center.location.x)) != params.blobColor)
				continue;
		}

		// ����뾶��ʹ�õķ�������ȡ��λ���ķ�ʽ
		std::vector<double> dists;
		for (size_t pointIdx = 0; pointIdx < contours[contourIdx].size(); pointIdx++)
		{
			cv::Point2d pt = contours[contourIdx][pointIdx];
			dists.push_back(norm(center.location - pt));
		}
		// ������������
		std::sort(dists.begin(), dists.end());
		center.radius = (dists[(dists.size() - 1) / 2] + dists[dists.size() / 2]) / 2.;

		// ��һ���Ƿ����µ�����
		BOOL bIsNew = TRUE;
		if (vct_ponits.size()){
			double dist = norm(vct_ponits.at(vct_ponits.size() - 1).location - center.location);
			double disr = vct_ponits.at(vct_ponits.size() - 1).radius - center.radius;
			bIsNew = dist >= params.minDistBetweenBlobs || vct_ponits.at(vct_ponits.size() - 1).radius - center.radius >= params.minDistBetweenBlobs;
		}

		// �ǵĻ��ͼ����ȥ
		if (bIsNew)
			vct_ponits.push_back(center);                    // ���뵽������ȥ
		else
			continue;

		// ��ʾͼƬ
		if (bShow)
		{
			cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // ��һ��Բ 
			cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // ��һ��Բ 
		}
	}

	return TRUE;
// 	vct_ponits.clear();
// 	cv::Mat cvMatTemImg = cvMatimage.clone();
// 	cv::Mat keypointsImage;
// 	cvtColor(cvMatimage, keypointsImage, CV_GRAY2RGB);
// 
// 	// �洢����
// 	CvMemStorage* contoursStorge = cvCreateMemStorage(0);
// 	CvMemStorage* approxPolyStorge = cvCreateMemStorage(0);
// 
// 	CvSeq* contours = 0;
// 	CvSeq* approxPloy = NULL;
// 
// 	// ԭ���Ǻ������Ϣ����������ʹ��
// 	IplImage* temImg = &IplImage(cvMatTemImg);
// 	IplImage* contoursImg = cvCreateImage(cvGetSize(temImg), temImg->depth, temImg->nChannels);
// 	IplImage* approxPloyImg = cvCreateImage(cvGetSize(temImg), temImg->depth, temImg->nChannels);
// 	cvZero(contoursImg);
// 	cvZero(approxPloyImg);
// 	
// 	// ������
// 	cvFindContours(temImg, contoursStorge, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
// 
// 	if (bShow)
// 	{
// 		cvDrawContours(contoursImg, contours, CV_RGB(0, 0, 255), CV_RGB(255, 0, 0), 2, 1, 8, cvPoint(0, 0));
// 		cvShowImage("Contours Img", contoursImg);
// 	}
// 
// 	for (CvSeq* contour = contours; contour != 0; contour = contour->h_next)
// 	{
// 		//����αƽ�  
// 		approxPloy = cvApproxPoly(contour, sizeof(CvContour), approxPolyStorge, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.02, 1);
// 		cvDrawContours(approxPloyImg, approxPloy, CV_RGB(255, 0, 0), CV_RGB(0, 0, 100), 1, 2, 8, cvPoint(0, 0));
// 
// 	}
// 
// 	if (bShow)
// 	{
// 		cvShowImage("Contour", approxPloyImg);
// 		cvWaitKey(0);
// 	}
// 	// ɾ��������
// 	cvReleaseMemStorage(&contoursStorge);
// 	cvReleaseMemStorage(&approxPolyStorge);
// 	cvReleaseImage(&contoursImg);
// 	cvReleaseImage(&approxPloyImg);

	// ʹ�øĽ��͵��㷨���Ա�����㷨��ִ���ٶ�����Ч��
}


/***************************************************************************
* �������ƣ�   SetFileName
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const char * fileName
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/05/22      ���ǲ�        ���
***************************************************************************/
void FindCircularMarker::SetFileName(const char* fileName)
{
	if (NULL != m_fileName)
		delete[] m_fileName;

	m_fileName = new char[BUFFER_SIZE];
	ZeroMemory(m_fileName, BUFFER_SIZE);
	
	memcpy(m_fileName, fileName, BUFFER_SIZE);
}


/***************************************************************************
* �������ƣ�   DrawCircle
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  IplImage * src
* �Ρ�������   [out] cv::Mat dst
* �Ρ�������   [in]  std::vector<ST_CENTER> & vct_ponits
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/05/22      ���ǲ�        ���
***************************************************************************/
void FindCircularMarker::DrawCircle(IplImage* src, cv::Mat dst, std::vector<ST_CENTER>& vct_ponits, const char* saveFileName /* = NULL */)
{
	cv::Mat keypointsImage;

	cvtColor(cv::Mat(src), keypointsImage, CV_GRAY2RGB);
	for (int j = 0; j < (int)vct_ponits.size(); j++)
	{
		ST_CENTER center;
		center = vct_ponits.at(j);
		cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // ��һ��Բ 
		cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // ��һ��Բ 
	}

	if (NULL != saveFileName)
		cvSaveImage(saveFileName, &IplImage(keypointsImage));

	dst = keypointsImage.clone();
}


/***************************************************************************
* �������ƣ�   FindCircleByCICImproved
* ժ����Ҫ��
* ȫ��Ӱ�죺   public
* �Ρ�������   [in]  const cv::Mat & cvMatimage
* �Ρ�������   [in]  cv::SimpleBlobDetector::Params params
* �Ρ�������   [in]  std::vector<ST_CENTER> & vct_ponits
* �Ρ�������   [in]  BOOL bShow
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/05/10      ���ǲ�        ���
***************************************************************************/
void FindCircularMarker::FindCircleByThreadshold(const cv::Mat& cvMatImage,
	cv::SimpleBlobDetector::Params params, std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
	vct_ponits.clear();
	// �½�һ�����ݻ�������
	cv::Mat cvMatTemImg = cvMatImage.clone();  // ����ͼƬ�������������
	cv::Mat src_gray;                          // �Ҷ�ͼ   

	if (CV_8UC1 != cvMatImage.type())
		cvtColor(cvMatTemImg, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatTemImg.clone();

	// תһ��ͼƬ
	IplImage* pImage = &IplImage(src_gray);
	cv::Mat threadsholdImg;

	int ithreadshold = Otsu(pImage);
	cv::threshold(cv::Mat(pImage), threadsholdImg, ithreadshold-5, 255, cv::THRESH_BINARY);

	// �������㷨����ʹ�õ�ʱ��
#ifdef TIME_TEST
	double t = (double)cv::getTickCount();
#endif // TIME_TEST


	FindCircle(threadsholdImg, params, vct_ponits, bShow);

#ifdef TIME_TEST
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Find Circle by CICImproved��" << t << std::endl;
#endif // TIME_TEST
}

/***************************************************************************
* �������ƣ�   CheckReliability
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* �Ρ�������   [in]  const std::vector<double> src
* �Ρ�������   [in]  const double dbVarPrecision
* �Ρ�������   [in]  const double dbRaPrecision
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/11/04      ���ǲ�        ���
***************************************************************************/
BOOL FindCircularMarker::CheckReliability(const std::vector<double> src, 
										  const double dbVarPrecision /* = 0.3 */, 
										  const double dbRaPrecision /* = 0.7 */)
{
	double dbAverage = 0;
	double dbVariance = 0;
	double dbPossible = 0;
	double dbTemVar = 0;
	const int SCALE = 10;

	for (int i = 0; i < (int)src.size(); i++)
		dbAverage = dbAverage + src.at(i);

	dbAverage = dbAverage / (int)src.size();

	for (int i = 0; i < (int)src.size(); i++)
	{
		dbTemVar = abs((src.at(i) - dbAverage));
		dbPossible += dbTemVar;
		dbVariance += dbTemVar*dbTemVar;
	}

	dbVariance /= (int)src.size()-1;
	double x = dbVariance;
	dbVariance = sqrt(dbVariance) / dbAverage;
	dbVariance *= SCALE;
	dbPossible /= (int)src.size()*dbAverage;
	dbPossible *= SCALE;

	if ((dbVariance < dbVarPrecision && dbPossible < dbRaPrecision) || x<0.3)
		//(dbPossible > dbRaPrecision)// && 
		return TRUE;
	else
		return FALSE;

}


/***************************************************************************
* �������ƣ�   FindCircleByThreshold
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Mat & cvMatImage
* �Ρ�������   [in]  cv::SimpleBlobDetector::Params params
* �Ρ�������   [in]  std::vector<ST_CENTER> & vct_ponits
* �Ρ�������   [in]  BOOL bShow
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/26      ���ǲ�        ���
***************************************************************************/
void FindCircularMarker::FindCircleByWaterThed(const cv::Mat& cvMatImage,
	cv::SimpleBlobDetector::Params params, std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
	// �½�һ�����ݻ�������
	cv::Mat cvMatTemImg = cvMatImage.clone();  // ����ͼƬ�������������
	cv::Mat src_gray;                          // �Ҷ�ͼ   

	// ������ǻҶ�ͼ��ת�ɻҶ�ͼ
	//	if (IPL_DEPTH_8U != cvMatTemImg.depth())
	//		cvtColor(cvMatTemImg, src_gray, CV_BGR2GRAY);

	// תһ��ͼƬ

	cv::Mat keypointsImage;
	cvtColor(cv::Mat(cvMatTemImg), keypointsImage, CV_GRAY2RGB);
	IplImage* pImage = &IplImage(keypointsImage);
	IplImage* pCannyImage = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_32S, 1);

	// �������㷨����ʹ�õ�ʱ��
#ifdef TIME_TEST
	double t = (double)cv::getTickCount();
#endif // TIME_TEST

	// ʹ��Otsu�������ֵ
	//int cannyThreshold = Otsu(pImage);
	// ʹ��canny����������ͼƬ,����Ϊʲô*2...��ֻ��˵Ч����
	//cvCanny(pImage, pCannyImage, cannyThreshold, cannyThreshold * 2, 3);
	cv::watershed(cv::Mat(pImage), cv::Mat(pCannyImage));

	// ���Ƿ�Ҫ��ʾ��ͼƬ
	if (bShow){
		const char* windows_name = "canny";
		//	cvNamedWindow(windows_name, CV_WINDOW_NORMAL);
		imshow(windows_name, cv::Mat(pCannyImage));
		cvWaitKey(0);
	}

	FindCircle(pCannyImage, params, vct_ponits, bShow);
	//FindCircleImproved(cv::Mat(pCannyImage), params, vct_ponits, bShow);

#ifdef TIME_TEST
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Find Circle by CICImproved��" << t << std::endl;
#endif // TIME_TEST

	cvReleaseImage(&pCannyImage);
}


/***************************************************************************
* �������ƣ�   FindCircleByCICImproved
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Mat & cvMatImage
* �Ρ�������   [in]  cv::SimpleBlobDetector::Params params
* �Ρ�������   [in]  std::vector<ST_CENTER> & vct_ponits
* �Ρ�������   [in]  double smoothingSigma
* �Ρ�������   [in]  BOOL bShow
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/10/12      ���ǲ�        ���
***************************************************************************/
void FindCircularMarker::FindCircleByCICImproved(const cv::Mat& cvMatImage,
	cv::SimpleBlobDetector::Params params, std::vector<ST_CENTER>& vct_ponits, double smoothingSigma , BOOL bShow /* = FALSE */)
{
	vct_ponits.clear();
	// �½�һ�����ݻ�������
	cv::Mat cvMatTemImg = cvMatImage.clone();  // ����ͼƬ�������������
	cv::Mat src_gray;                          // �Ҷ�ͼ   

	if (CV_8UC1 != cvMatImage.type())
		cvtColor(cvMatTemImg, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatTemImg.clone();

	// ʹ��EDPF�ұ߽�
	FindContoursMethod f;
	cv::Mat dst;
	f.findContoursByEDPF(src_gray, dst, smoothingSigma);
	f.findContoursByEDPF(dst, dst, smoothingSigma);

	// ���Ƿ�Ҫ��ʾ��ͼƬ
	if (bShow){
		const char* windows_name = "canny";
		cvNamedWindow(windows_name, CV_WINDOW_NORMAL);
		imshow(windows_name, dst);
		cvWaitKey(0);
	}

	FindCircle(dst, params, vct_ponits, bShow);
}


/***************************************************************************
* �������ƣ�   FindCircleByCIC
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const cv::Mat & cvMatImage
* �Ρ�������   [in]  cv::SimpleBlobDetector::Params params
* �Ρ�������   [in]  std::vector<ST_CENTER> & vct_ponits
* �Ρ�������   [in]  double smoothingSigma
* �Ρ�������   [in]  BOOL bShow
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/11/04      ���ǲ�        ���
***************************************************************************/
void FindCircularMarker::FindCircleByCIC(const cv::Mat& cvMatImage,
	cv::SimpleBlobDetector::Params params,
	std::vector<ST_CENTER>& vct_ponits,
	double smoothingSigma /* = 1.0 */,
	BOOL bShow /* = FALSE */)
{
	vct_ponits.clear();
	// �½�һ�����ݻ�������
	cv::Mat cvMatTemImg = cvMatImage.clone();  // ����ͼƬ�������������
	cv::Mat src_gray;                          // �Ҷ�ͼ   

	if (CV_8UC1 != cvMatImage.type())
		cvtColor(cvMatTemImg, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatTemImg.clone();

	// ʹ��EDPF�ұ߽�
	FindContoursMethod f;
	cv::Mat dst;
	f.findContoursByEDPF(src_gray, dst, smoothingSigma);
	//f.findContoursByEDPF(dst, dst, smoothingSigma);

	// ���Ƿ�Ҫ��ʾ��ͼƬ
	if (bShow){
		const char* windows_name = "EDPF";
		cvNamedWindow(windows_name, CV_WINDOW_NORMAL);
		imshow(windows_name, dst);
		cvWaitKey(0);
	}

	// ���øĽ����㷨��ɸ�����
	//FindCircleImproved(dst, params, vct_ponits, bShow);
	FindCircle(dst, params, vct_ponits, bShow);
}