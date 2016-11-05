#include "FindCircularMarker.h"
#include "FindCountersMethod.h"

int FindCircularMarker::m_siGobalThreHold = 0;

/***************************************************************************
* 函数名称：   FindCircularMarker
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/21      饶智博        添加
***************************************************************************/
FindCircularMarker::FindCircularMarker():
	m_fileName(NULL)
{
}

/***************************************************************************
* 函数名称：   ~FindCircularMarker
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/21      饶智博        添加
***************************************************************************/
FindCircularMarker::~FindCircularMarker()
{
	if (NULL != m_fileName)
		delete m_fileName;
}

/***************************************************************************
* 函数名称：   FindCircleCenter
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  IplImage * pImg
* 参　　数：   [in]  cv::SimpleBlobDetector::Params params
* 参　　数：   [out] std::vector<cv::KeyPoint> & Vct_ponits
* 参　　数：   [in]  BOOL bShow
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/21      饶智博        添加
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

// 被弃用，原因是不需要被使用
//	int thresh = 10;
//	int max_thresh = 255;
//	cv::createTrackbar("thresh:", "SimpleBlobDetector", &thresh, max_thresh, FindCircularMarker::Trackbar_callback, &cvBinaryImage);
//	cvWaitKey(0);
	delete cvDetector;
	return TRUE;
}

/***************************************************************************
* 函数名称：   Trackbar_callback
* 摘　　要：   
* 全局影响：   protected static 
* 参　　数：   [in]  int Pos
* 参　　数：   [in]  void * Usrdata
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/21      饶智博        添加
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
* 函数名称：   CalCircleCentrePoint
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  IplImage * pImg
* 参　　数：   [in]  std::vector<cv::KeyPoint> & Vct_ponits
* 参　　数：   [in]  int ipointscounts
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/21      饶智博        添加
***************************************************************************/
void FindCircularMarker::CalCircleCentrePoint(IplImage* pImg, std::vector<cv::KeyPoint>&Vct_ponits, int ipointscounts)
{
	for (int iCirclenCounts = 0; iCirclenCounts < (int)Vct_ponits.size(); iCirclenCounts++)
	{
		/// 粗略的圆心坐标
		int iCentreX = (int)Vct_ponits.at(iCirclenCounts).pt.x;
		int iCentreY = (int)Vct_ponits.at(iCirclenCounts).pt.y;

		///扫描到的边缘点集
		int aiPos[CAL_PONIT_NUM][SYSTEM_XY];

		memset(aiPos, 0, sizeof(int)* CAL_PONIT_NUM * SYSTEM_XY);

		/// 以0°（水平向右）为起点，沿逆时针方向每隔5°在圆周上取一点，并向圆心方向搜索，若某一点的像素值为255时，则为边缘点。
		for (int iAngle = 0; iAngle < 360; iAngle += 5)
		{
			for (int iR = 1; iR < 25; iR++)
			{
				int iX = (int)(iCentreX + (iR * cos((double)iAngle * PI / 180) + (((iAngle >= 0 && iAngle <= 90) || (iAngle >= 270 && iAngle < 360)) ? 0.5 : -0.5)));		// 待判断点的X坐标，式中的0.5为修正值
				int iY = (int)(iCentreY - (iR * sin((double)iAngle * PI / 180) + ((iAngle >= 0 && iAngle < 180) ? 0.5 : -0.5)));		// 待判断点的Y坐标，式中的0.5为修正值

				/// 判断是否超出图像的范围
				if (iX < 0 || iX >= pImg->width || iY < 0 || iY >= pImg->height)
				{
					aiPos[iAngle / 5][POINTS_X] = iX - iCentreX;					// 扫描到的边缘点集（iPos）是以（iCentreX，iCentreY）为坐标中心的坐标
					aiPos[iAngle / 5][POINTS_Y] = iY - iCentreY;
					break;
				}

				uchar ucVal = (uchar)*(pImg->imageData + iY * pImg->widthStep + iX * pImg->nChannels + 0);
				if (255 == ucVal)
				{
					aiPos[iAngle / 5][POINTS_X] = iX - iCentreX;					// 扫描到的边缘点集（iPos）是以（iCentreX，iCentreY）为坐标中心的坐标
					aiPos[iAngle / 5][POINTS_Y] = iY - iCentreY;
					break;
				}
			}
		}

		/// 采用交换排序法对扫描到的边缘点集按其到圆心的距离由近到远进行排序
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

		/// 取位于iPos中间的12个点，用于拟合边缘圆。
		int iFitPos[36][2];				// 拟合点数组
		memset(iFitPos, 0, sizeof(int)* 36 * 2);
		for (int i = 0; i < 36; i++)
		{
			iFitPos[i][0] = aiPos[24 + i][0];
			iFitPos[i][1] = aiPos[24 + i][1];
		}

		/// 以下是最小二乘拟合求解圆心坐标及半径
		/// 矩阵A
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

		/// 矩阵B
		CvMat* matB = cvCreateMat(3, 1, CV_64FC1);
		cvZero(matB);
		for (int i = 0; i < 36; i++)
		{
			*(matB->data.db + 0) -= iFitPos[i][0] * iFitPos[i][0] * iFitPos[i][0] + iFitPos[i][1] * iFitPos[i][1] * iFitPos[i][0];
			*(matB->data.db + 1) -= iFitPos[i][0] * iFitPos[i][0] * iFitPos[i][1] + iFitPos[i][1] * iFitPos[i][1] * iFitPos[i][1];
			*(matB->data.db + 2) -= iFitPos[i][0] * iFitPos[i][0] + iFitPos[i][1] * iFitPos[i][1];
		}

		/// 求A的逆矩阵
		CvMat* matA_Invert = cvCreateMat(3, 3, CV_64FC1);
		cvZero(matA_Invert);
		cvInvert(matA, matA_Invert);

		/// 求解圆心坐标及半径
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
* 函数名称：   FindCircle
* 摘　　要：   在调用该函数，请确保图片的格式为8位的
* 全局影响：   public 
* 参　　数：   [in]  IplImage * pImg
* 参　　数：   [in]  cv::SimpleBlobDetector::Params params
* 参　　数：   [in]  std::vector<ST_CENTER> & vct_ponits
* 参　　数：   [in]  BOOL bShow
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/18      饶智博        添加
***************************************************************************/
BOOL FindCircularMarker::FindCircle(IplImage* pImg, cv::SimpleBlobDetector::Params params, std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
	// 初始化要输出的数据
	cv::Mat cvMatimage(pImg); // 把图片转换成矩阵
	return FindCircle(cvMatimage,params,vct_ponits,bShow);
}

/***************************************************************************
* 函数名称：   FindCircle
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  cv::Mat & cvMatimage
* 参　　数：   [in]  cv::SimpleBlobDetector::Params params
* 参　　数：   [out] std::vector<ST_CENTER> & vct_ponits
* 参　　数：   [in]  BOOL bShow
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/18      饶智博        添加
***************************************************************************/
BOOL FindCircularMarker::FindCircle(cv::Mat& cvMatimage, cv::SimpleBlobDetector::Params params, std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{

#ifdef TIME_TEST
	double t = (double)cv::getTickCount();
#endif // TIME_TEST

	// 初始化需要使用的变量
	vct_ponits.clear();
	std::vector < std::vector<cv::Point> > contours; // 轮廓标记点存储的位置
	cv::Mat keypointsImage;
	cv::Mat src_gray;                          // 灰度图   

	if (CV_8UC1 != cvMatimage.type())
		cvtColor(cvMatimage, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatimage.clone();

	cvtColor(src_gray, keypointsImage, CV_GRAY2RGB);

	/************************************************************************/
	/* 计算轮廓                                                                */
	/************************************************************************/
// 每一步所用时间进行测试
#ifdef STEP_TIME_TEST
	double dbStepTime = (double)cv::getTickCount();
#endif // STEP_TIME_TEST

//	std::vector<cv::Vec4i> hierarchy;
	//CvMemStorage * storage = cvCreateMemStorage(0);
	// 找轮廓， 输入的Mat必须是二值化后的图片，否则会出现问题
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
		// 保存图片
		if (NULL != m_fileName)
			cvSaveImage(m_fileName, &IplImage(contoursImage));
 		//cvWaitKey(0);
	}

// 每一步所用时间进行测试
#ifdef STEP_TIME_TEST
	dbStepTime = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Find contours in seconds:" << t << std::endl;
#endif // TIME_TEST


	for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++)
	{

		// 用于判定某个轮廓使用
//   		if ((int)contourIdx==420)
//   		{
//   			int x=0;
//   			x++;
 			//std::vector < std::vector<cv::Point> > contour;
 			//contour.push_back(contours[contourIdx]);
 			//drawContours(contoursImage, contour, -1, cv::Scalar(0, 0, 255));
 			//imshow("ContourImg", contoursImage);
  		//}
		// 因为其会找两遍，因此，把第二遍的给虑除
		//if (contourIdx % 2)
			//continue;
		// 这段代码用于判断轮廓的方向
//  		cv::circle(keypointsImage, contours[contourIdx].at(0), (int)1, cv::Scalar(0, 0, 255)); // 画一个圆 
//  		imshow("FindCircle", keypointsImage);
//  		cvWaitKey(0);
//  		cv::circle(keypointsImage, contours[contourIdx].at(contours[contourIdx].size() / 3), (int)1, cv::Scalar(0, 0, 255)); // 画一个圆 
//  		imshow("FindCircle", keypointsImage);
//  		cvWaitKey(0);
//  		cv::circle(keypointsImage, contours[contourIdx].at(contours[contourIdx].size() /2), (int)1, cv::Scalar(0, 0, 255)); // 画一个圆 
//  		imshow("FindCircle", keypointsImage);
//  		cvWaitKey(0);
//  		cv::circle(keypointsImage, contours[contourIdx].at(contours[contourIdx].size()-1), (int)1, cv::Scalar(0, 0, 255)); // 画一个圆 
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
		// 这个轮廓是不闭合的，导致会出现问题
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
			// 使用拟合多边形来代替原有的轮廓
			area = contourArea(cv::Mat(hull));
			// 半径需要减半
			perimeter = perimeter / 2;
			moms = cv::moments(cv::Mat(hull));
		}

/************************************************************************/
/* 通过大小来排除信息                                                     */
/************************************************************************/
		// 每一步所用时间进行测试
#ifdef STEP_TIME_TEST
		dbStepTime = (double)cv::getTickCount();
#endif // STEP_TIME_TEST
		// 看是否需要通过大小来虑除信息
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
/* 通过圆度来排除信息                                                     */
/************************************************************************/
		// 每一步所用时间进行测试
#ifdef STEP_TIME_TEST
		double dbStepTime = (double)cv::getTickCount();
#endif // STEP_TIME_TEST
		// 通过圆度来虑除信息
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

		// 每一步所用时间进行测试
#ifdef STEP_TIME_TEST
		dbStepTime = (double)cv::getTickCount();
#endif // STEP_TIME_TEST
		// 通过惯性率来排除信息
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


		// 每一步所用时间进行测试
#ifdef STEP_TIME_TEST
		dbStepTime = (double)cv::getTickCount();
#endif // STEP_TIME_TEST
		// 通过凹凸度来虑除信息
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

		// 计算出圆心
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
		// 重新排列数据
		std::sort(dists.begin(), dists.end());
		
//    		if (!CheckReliability(dists,0.65,0.55))
//    			continue;

		// 计算半径
		center.radius = (dists[(dists.size() - 1) / 2] + dists[dists.size() / 2]) / 2.;

		// 与上一个进行对比
		BOOL bIsNew = TRUE;
		if (vct_ponits.size()){
			double dist = norm(vct_ponits.at(vct_ponits.size() - 1).location - center.location);
			double disr = vct_ponits.at(vct_ponits.size() - 1).radius - center.radius;
			bIsNew = dist >= params.minDistBetweenBlobs || vct_ponits.at(vct_ponits.size() - 1).radius - center.radius >= params.minDistBetweenBlobs;
		}

		if (bIsNew)
			vct_ponits.push_back(center);                    // 加入到容器中去
		else
			continue;

//  		if (center.location.x > 800 && center.location.x<920 && center.location.y>400 && center.location.y < 500)
//  		{
//  			int x = 0;
//  			x++;
//  		}


		// 显示图片
		if (bShow)
		{
			cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // 画一个圆 
			cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // 画一个圆 
		}
	}

#ifdef TIME_TEST
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Find Circle by our function in seconds: " << t << std::endl;
#endif // TIME_TEST

	if (bShow)
	{
		// 显示圆出来
		//cvNamedWindow("FindCircle", CV_WINDOW_NORMAL);
		//imshow("FindCircle", keypointsImage);
		//cvSaveImage("1.bmp", &IplImage(keypointsImage));
		//cvWaitKey(0);
	}

	return TRUE;
}

/***************************************************************************
* 函数名称：   SamplingTrianglesFindCircle
* 摘　　要：   对比别人论文中使用别人的方法
* 全局影响：   public 
* 参　　数：   [in]  IplImage * pImg
* 参　　数：   [out] std::vector<ST_CENTER> & vct_ponits
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/18      饶智博        添加
***************************************************************************/
void FindCircularMarker::FindCircleBySamplingTriangles(IplImage* pImg, std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
	// 初始化要输出的数据
	cv::Mat cvMatimage(pImg); // 把图片转换成矩阵
	return FindCircleBySamplingTriangles(cvMatimage, vct_ponits, bShow);
}

/***************************************************************************
* 函数名称：   FindCircleBySamplingTriangles
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  cv::Mat & cvMatimage
* 参　　数：   [in]  std::vector<ST_CENTER> & vct_ponits
* 参　　数：   [in]  BOOL bShow
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/19      饶智博        添加
***************************************************************************/
void FindCircularMarker::FindCircleBySamplingTriangles(const cv::Mat& cvMatimage,
	std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
#ifdef TIME_TEST
	double t = (double)cv::getTickCount();
#endif // TIME_TEST
	// 初始化需要使用的变量
	vct_ponits.clear();                                  // 清空结果数据
	cv::Mat cvMatTemImg = cvMatimage.clone();            // 复制图片出来操作，以免源图被污染
	std::vector < std::vector<cv::Point> > contours;     // 轮廓标记点存储的位置
	cv::Mat keypointsImage;                              // 在这阵图片中做显示使用
	cv::Mat grad_x, grad_y;                              // 用于计算图片梯度

	// 计算图片的梯度
	GradientEstimation(cvMatTemImg, grad_x, grad_y, bShow); // 将梯度信息都保存在grad_x和grad_y中

	// !!!FIXME：查找轮廓的代码出现问题，原因是如果不设置阈值二值化，轮廓找不到,欢迎提供更好的方法找轮廓
	// 找轮廓， 输入的Mat必须是二值化后的图片，否则会出现问题,
	//addWeighted(grad_x, 0.5, grad_y, 0.5, 0, grad);
//	int iThreshold = Otsu(&IplImage(cvMatTemImg));
	//cv::threshold(cvMatTemImg, grad, iThreshold, 255, cv::THRESH_BINARY);

	// 使用Otsu计算出阈值
	int cannyThreshold = Otsu(&IplImage(cvMatTemImg));
	IplImage* pCannyImage = cvCreateImage(cvGetSize(&IplImage(cvMatTemImg)), IPL_DEPTH_8U, 1);
	// 使用canny算子来计算图片,至于为什么*2...我只能说效果好
	cvCanny(&IplImage(cvMatTemImg), pCannyImage, cannyThreshold, cannyThreshold * 2, 3);
	cv::Mat grad(pCannyImage);
	//imshow("ContourImg", grad);
	//cvWaitKey(0);
	cvtColor(cv::Mat(pCannyImage), keypointsImage, CV_GRAY2RGB);  // 把灰度图片转换成RGB图片
	//cv::Mat cvMatContourImg = cv::Mat::zeros(cvMatimage.rows, cvMatimage.cols, CV_8UC3);

	findContours(grad, contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	// 将轮廓画出来
	if (bShow)
	{
		cv::Mat contoursImage;
		cvtColor(cv::Mat(pCannyImage), contoursImage, CV_GRAY2RGB);
		drawContours(contoursImage, contours, -1, cv::Scalar(0, 0, 255));
		imshow("ContourImg", contoursImage);
		cvWaitKey(0);
	}
	//int three=0;

	// 开始利用轮廓判断是否圆
	for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++)
	{
		// 因为其会找两遍，因此，把第二遍的给虑除
		//if (contourIdx % 2)
		//	continue;

		// 圆的信息存储位置
		ST_CENTER center;
		center.confidence = 1;

		cv::Moments moms = cv::moments(cv::Mat(contours[contourIdx]));
		double area = moms.m00;
		if (area < 30 || area >= 1000000)
			continue;

		// 是否符合三点早圆法，目前使用的精度是5次试验，只要有2次满足精度小于2个像素，就认为其是圆
		if (!FindCircleByThreePoint(contours.at(contourIdx), center, 3, 85, 100))
			continue;

		//std::cout << "Three Point find:" << ++three << std::endl;
		// FIXME： 其实在这里只需要计算一次就好，但是代码还没有被修改
		//int x = rand() % contours.at(contourIdx).size();
		//int y = rand() % contours.at(contourIdx).size();
		//DetectIsoscelesTriangles(cvMatTemImg, contours.at(contourIdx).at(x), contours.at(contourIdx).at(y));

		if (!AnalysisCluster(contours.at(contourIdx),grad_x,grad_y))
			continue;

		// 加入到结果中去
		
		// 与上一个进行对比
		BOOL bIsNew = TRUE;
		if (vct_ponits.size()){
			double dist = norm(vct_ponits.at(vct_ponits.size() - 1).location - center.location);
			double disr = vct_ponits.at(vct_ponits.size() - 1).radius - center.radius;
			//bIsNew = dist >= params.minDistBetweenBlobs &&  dist >= vct_ponits.at(vct_ponits.size() - 1).radius && dist >= center.radius;
			//bIsNew = dist >= params.minDistBetweenBlobs &&  vct_ponits.at(vct_ponits.size() - 1).radius - center.radius <= params.minDistBetweenBlobs;
			bIsNew = dist >= 3 || vct_ponits.at(vct_ponits.size() - 1).radius - center.radius >= 3;
		}

		if (bIsNew)
			vct_ponits.push_back(center);                    // 加入到容器中去
		else
			continue;
		//vct_ponits.push_back(center);

		// 是否显示图片
		if (bShow)
			cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // 画一个圆 
	}

#ifdef TIME_TEST
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Find Circle by  SamplingTriangles ：" << t << std::endl;
#endif // TIME_TEST

	if (bShow)
	{
		// 显示圆出来
		imshow("FindCircle", keypointsImage);
		cvWaitKey(0);
	}

	// 删除临时使用的变量
	cvReleaseImage(&pCannyImage);

	return;
}


/***************************************************************************
* 函数名称：   FindCircleByThreePoint
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const std::vector<cv::Point> counter
* 参　　数：   [out] ST_CENTER & center
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/23      饶智博        添加
***************************************************************************/
BOOL FindCircularMarker::FindCircleByThreePoint(const std::vector<cv::Point> counter,  ST_CENTER& center,
	const double dbPrecision,  const UINT iMinSuccessNum /* = 1  */, const UINT iMaxTestNum /* = 10 */)
{
	// 最大的成功次数比最要实验的次数好多就认为是假的
	if (iMaxTestNum < iMinSuccessNum)
		return FALSE;

	// 如果整个轮廓小于4点，显然不能使用这个方法
	if (counter.size() <= 4)
		return FALSE;

	// 初始化数据
	ST_CENTER temCenter; // 建立一个临时的
	std::vector<ST_CENTER> vcCenters;

	// 清空结果数据
	memset(&center, 0, sizeof(ST_CENTER));
	vcCenters.clear();

	// 概率
	const float confidence = iMinSuccessNum / (float)iMaxTestNum;
	// 实际上实验次数
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

		// 说明正常实验一次；
		i++;

		// 三点共线，显然不是一个圆
		if (IsThreePointInLine(counter.at(idx1), counter.at(idx2), counter.at(idx3)))
			continue;

		// 检验的点也不能在这三点上
		if (idx4 == idx1 || idx4 == idx2 || idx4 == idx3)
			continue;


		testNum++;
		// 得到一个圆
		GetCircle(counter.at(idx1), counter.at(idx2), counter.at(idx3), temCenter.location, temCenter.radius);

		if (IsInCircle(counter.at(idx4), temCenter, dbPrecision))
			vcCenters.push_back(temCenter);
	}

	// 计算出真实的图片
	if (confidence < vcCenters.size() / (float)testNum)
	{
		double x = 0;
		double y = 0;
		double r = 0;
		// 求解圆形的平均值
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
* 函数名称：   FindCircleByThreePoint
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  IplImage * pImg
* 参　　数：   [in]  std::vector<ST_CENTER> & vct_ponits
* 参　　数：   [in]  BOOL bShow
* 参　　数：   [in]  const double dbPrecision
* 参　　数：   [in]  const UINT iMinSuccessNum
* 参　　数：   [in]  const UINT iMaxTestNum
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/25      饶智博        添加
***************************************************************************/
BOOL FindCircularMarker::FindCircleByThreePoint(IplImage*pImg, std::vector<ST_CENTER>& vct_ponits, 
	BOOL bShow /* = FALSE  */, const double dbPrecision /* = 0.1 */, 
	const UINT iMinSuccessNum /* = 1 */, const UINT iMaxTestNum /* = 10 */)
{
	cv::Mat cvMatImg(pImg);
	return FindCircleByThreePoint(cvMatImg, vct_ponits, bShow, dbPrecision, iMinSuccessNum, iMaxTestNum);
}

/***************************************************************************
* 函数名称：   FindCircleByThreePoint
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Mat & cvMatimage
* 参　　数：   [in]  std::vector<ST_CENTER> & vct_ponits
* 参　　数：   [in]  BOOL bShow
* 参　　数：   [in]  const double dbPrecision
* 参　　数：   [in]  const UINT iMinSuccessNum
* 参　　数：   [in]  const UINT iMaxTestNum
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/25      饶智博        添加
***************************************************************************/
BOOL FindCircularMarker::FindCircleByThreePoint(const cv::Mat& cvMatimage, std::vector<ST_CENTER>& vct_ponits,
	BOOL bShow /* = FALSE */, const double dbPrecision /* = 0.1 */, 
	const UINT iMinSuccessNum /* = 1 */, const UINT iMaxTestNum /* = 10 */)
{
	// 先拷贝数据下来
	cv::Mat cvMatTemImg = cvMatimage.clone();


	return TRUE;
}

/***************************************************************************
* 函数名称：   FindCircleByThreePoint
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const std::vector<cv::Point> counter
* 参　　数：   [out]  std::vector<ST_CENTER> & center
* 参　　数：   [in]  const double dbPrecision
* 参　　数：   [in]  const double dbMinConfidence
* 参　　数：   [in]  const UINT iMinSuccessNum
* 参　　数：   [in]  const UINT iMaxTestNum
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/25      饶智博        添加
***************************************************************************/
BOOL FindCircularMarker::FindCircleByThreePoint(const std::vector<cv::Point> counter, 
				std::vector<ST_CENTER>& centers, const double dbPrecision /* = 0.1 */, 
				const double dbMinConfidence /* =0.4 */, const UINT iMinSuccessNum /* = 1 */, 
				const UINT iMaxTestNum /* = 10 */)
{

	// 最大的成功次数比最要实验的次数好多就认为是假的
	if (iMaxTestNum < iMinSuccessNum)
		return FALSE;

	// 如果整个轮廓小于4点，显然不能使用这个方法
	if (counter.size() <= 4)
		return FALSE;

	// 初始化数据
	ST_CENTER temCenter; // 建立一个临时的
	std::vector<ST_CENTER> vcCenters;
	vcCenters.clear();

	// 粗略的计算圆形
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

		// 三点共线，显然不是一个圆
		if (IsThreePointInLine(counter.at(idx1), counter.at(idx2), counter.at(idx3)))
			continue;

		// 说明成功进行试验
		i++;

		// 得到一个圆
		GetCircle(counter.at(idx1), counter.at(idx2), counter.at(idx3), temCenter.location, temCenter.radius);
		
		// 需要检查其的精度是否正确
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

	// 判决是否是圆
	centers.clear();
	IsInCircle(counter, vcCenters, dbPrecision);
	for (int i = 0; i < (int)vcCenters.size(); i++)
	{
		if (vcCenters.at(i).confidence > dbMinConfidence)
			centers.push_back(vcCenters.at(i));
	}

	// 返回是都找到了圆
	if ((int)centers.size() > 0)
		return TRUE;
	else
		return FALSE;
}

/***************************************************************************
* 函数名称：   FindCircleBySamplingTrianglesImproved
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Mat & cvMatimage
* 参　　数：   [in]  std::vector<ST_CENTER> & vct_ponits
* 参　　数：   [in]  BOOL bShow
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/25      饶智博        添加
***************************************************************************/
void FindCircularMarker::FindCircleBySamplingTrianglesImproved(const cv::Mat& cvMatimage,
	std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
#ifdef TIME_TEST
	double t = (double)cv::getTickCount();
#endif // TIME_TEST
	// 初始化需要使用的变量
	vct_ponits.clear();                                  // 清空结果数据
	cv::Mat cvMatTemImg = cvMatimage.clone();            // 复制图片出来操作，以免源图被污染

	std::vector < std::vector<cv::Point> > contours;     // 轮廓标记点存储的位置
	cv::Mat keypointsImage;                              // 在这阵图片中做显示使用
	cv::Mat grad_x, grad_y;                              // 用于计算图片梯度
	cvtColor(cvMatTemImg, keypointsImage, CV_GRAY2RGB);  // 把灰度图片转换成RGB图片
	cv::Mat cvMatContourImg = cv::Mat::zeros(cvMatimage.rows, cvMatimage.cols, CV_8UC3);

	// 计算图片的梯度
	GradientEstimation(cvMatTemImg, grad_x, grad_y, bShow); // 将梯度信息都保存在grad_x和grad_y中

	// !!!FIXME：查找轮廓的代码出现问题，原因是如果不设置阈值二值化，轮廓找不到,欢迎提供更好的方法找轮廓
	// 找轮廓， 输入的Mat必须是二值化后的图片，否则会出现问题,
	cv::Mat grad;
	//	addWeighted(grad_x, 0.5, grad_y, 0.5, 0, grad);
	cv::threshold(cvMatTemImg, grad, 145, 255, cv::THRESH_BINARY);
	findContours(grad, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	// 将轮廓画出来
	if (bShow)
	{
		cv::Mat contoursImage;
		cvtColor(cvMatimage, contoursImage, CV_GRAY2RGB);
		drawContours(contoursImage, contours, -1, cv::Scalar(0, 0, 255));
		imshow("ContourImg", contoursImage);
		cvWaitKey(0);
	}
	//int three=0;

	// 开始利用轮廓判断是否圆
	for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++)
	{
		// 圆的信息存储位置
		std::vector<ST_CENTER> centers;
		centers.clear();

		// 精确到两个像素
		FindCircleByThreePoint(contours.at(contourIdx), centers, 2);
		if (!centers.size())               
			continue;

		// 加入到结果中去
		for (int i = 0; i < (int)centers.size();i++)
			vct_ponits.push_back(centers.at(i));

		// 是否显示图片
		if (bShow)
		{
			for (int i = 0; i < (int)centers.size(); i++)
			{
				cv::circle(keypointsImage, centers.at(i).location, (int)1, cv::Scalar(255, 0, 0)); // 画一个圆
				cv::circle(keypointsImage, centers.at(i).location, (int)centers.at(i).radius, cv::Scalar(0, 255, 0)); // 画一个圆
			}
		} 
	}

#ifdef TIME_TEST
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Find Circle by  SamplingTriangles ：" << t << std::endl;
#endif // TIME_TEST

	if (bShow)
	{
		// 显示圆出来
		imshow("FindCircle", keypointsImage);
		cvWaitKey(0);
	}

	return;
}

/***************************************************************************
* 函数名称：   FindCircleBySamplingTriangles
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  IplImage * pImg
* 参　　数：   [in]  std::vector<ST_CENTER> & vct_ponits
* 参　　数：   [in]  BOOL bShow
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/04/25      饶智博        添加
***************************************************************************/
void FindCircularMarker::FindCircleBySamplingTrianglesImproved(IplImage* pImg, std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
	// 初始化要输出的数据
	cv::Mat cvMatimage(pImg); // 把图片转换成矩阵
	return FindCircleBySamplingTrianglesImproved(cvMatimage, vct_ponits, bShow);
}

/***************************************************************************
* 函数名称：   FindCircleByCICImproved
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Mat & cvMatimage
* 参　　数：   [in]  cv::SimpleBlobDetector::Params params
* 参　　数：   [in]  std::vector<ST_CENTER> & vct_ponits
* 参　　数：   [in]  BOOL bShow
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/05/10      饶智博        添加
***************************************************************************/
void FindCircularMarker::FindCircleByCICImproved(const cv::Mat& cvMatImage,
	cv::SimpleBlobDetector::Params params, std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
	vct_ponits.clear();
	// 新建一个数据缓存区域
	cv::Mat cvMatTemImg = cvMatImage.clone();  // 复制图片，以免出现问题
	cv::Mat src_gray;                          // 灰度图   

	if (CV_8UC1 != cvMatImage.type())
		cvtColor(cvMatTemImg, src_gray, CV_BGR2GRAY);
	else
		src_gray=cvMatTemImg.clone();


	// 转一下图片
	IplImage* pImage = &IplImage(src_gray);
	IplImage* pCannyImage = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 1);

	// 下面是算法真正使用的时间
#ifdef TIME_TEST
	double t = (double)cv::getTickCount();
#endif // TIME_TEST

	// 使用Otsu计算出阈值
	int cannyThreshold = Otsu(pImage);
	// 使用canny算子来计算图片,至于为什么*2...我只能说效果好
	cvCanny(pImage, pCannyImage, cannyThreshold*0.8, cannyThreshold , 3);
//  	double low,high;
//  	AdaptiveFindThreshold(pImage, &low, &high);
// 	cvCanny(pImage, pCannyImage, low, high, 3);

	// 看是否要显示出图片
	if (bShow){
		const char* windows_name = "canny";
		cvNamedWindow(windows_name, CV_WINDOW_NORMAL);
		imshow(windows_name, cv::Mat(pCannyImage));
		cvWaitKey(0);
	}

	FindCircle(pCannyImage, params, vct_ponits, bShow);

#ifdef TIME_TEST
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Find Circle by CICImproved：" << t << std::endl;
#endif // TIME_TEST

	cvReleaseImage(&pCannyImage);
}


/***************************************************************************
* 函数名称：   FindCircleImproved
* 摘　　要：   被废弃，理由是无法实现想要的功能
* 全局影响：   protected 
* 参　　数：   [in]  cv::Mat & cvMatimage
* 参　　数：   [in]  cv::SimpleBlobDetector::Params params
* 参　　数：   [in]  std::vector<ST_CENTER> & vct_ponits
* 参　　数：   [in]  BOOL bShow
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/05/17      饶智博        添加
***************************************************************************/
BOOL FindCircularMarker::FindCircleImproved(cv::Mat& cvMatimage, 
											cv::SimpleBlobDetector::Params params, 
											std::vector<ST_CENTER>& vct_ponits, 
											BOOL bShow /* = FALSE */)
{
	// 初始化需要使用的变量
	vct_ponits.clear();
	std::vector < std::vector<cv::Point> > contours; // 轮廓标记点存储的位置
	cv::Mat keypointsImage;
	cv::Mat src_gray;                               // 灰度图   

	// 我们只能做灰度图，所以把不是灰度图的转换成灰度图
	if (CV_8UC1 != cvMatimage.type())
		cvtColor(cvMatimage, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatimage.clone();
	cvtColor(src_gray, keypointsImage, CV_GRAY2RGB);

	// 查找轮廓
	findContours(src_gray, contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

	// 初始化结果的空间
	ST_CENTER center;
	center.confidence = 1;

	// 对圆进行识别和定位
	for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++)
	{
		// 清理结果空间
		ZeroMemory(&center, sizeof(ST_CENTER));
		
		// 计算当前的图形的集合矩
		cv::Moments moms = cv::moments(cv::Mat(contours[contourIdx]), true);

		// 通过面积虑除不要的面积的轮廓，一般来说，我们是不设定该数据的最大值，因为会影响圆定位的精度
		if (params.filterByArea){
			double area = moms.m00;
			if (area < params.minArea || area >= params.maxArea)
				continue;
		}

		// 圆度计算
		if (params.filterByCircularity){
			double ratio = moms.m00 * moms.m00 / (2 * CV_PI*(moms.mu02 + moms.mu20));
			if (ratio < params.minCircularity || ratio >= params.maxCircularity)
				continue;
		}

		// 惯性率计算
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

		// 计算圆形坐标和半径
		center.location = cv::Point2d(moms.m10 / moms.m00, moms.m01 / moms.m00);

		// 通过颜色去排除非圆轮廓，但是一般并不使用
		if (params.filterByColor)
		{
			if (src_gray.at<uchar>(cvRound(center.location.y), cvRound(center.location.x)) != params.blobColor)
				continue;
		}

		// 计算半径，使用的方法是求取中位数的方式
		std::vector<double> dists;
		for (size_t pointIdx = 0; pointIdx < contours[contourIdx].size(); pointIdx++)
		{
			cv::Point2d pt = contours[contourIdx][pointIdx];
			dists.push_back(norm(center.location - pt));
		}
		// 重新排列数据
		std::sort(dists.begin(), dists.end());
		center.radius = (dists[(dists.size() - 1) / 2] + dists[dists.size() / 2]) / 2.;

		// 看一下是否是新的内容
		BOOL bIsNew = TRUE;
		if (vct_ponits.size()){
			double dist = norm(vct_ponits.at(vct_ponits.size() - 1).location - center.location);
			double disr = vct_ponits.at(vct_ponits.size() - 1).radius - center.radius;
			bIsNew = dist >= params.minDistBetweenBlobs || vct_ponits.at(vct_ponits.size() - 1).radius - center.radius >= params.minDistBetweenBlobs;
		}

		// 是的话就加入进去
		if (bIsNew)
			vct_ponits.push_back(center);                    // 加入到容器中去
		else
			continue;

		// 显示图片
		if (bShow)
		{
			cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // 画一个圆 
			cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // 画一个圆 
		}
	}

	return TRUE;
// 	vct_ponits.clear();
// 	cv::Mat cvMatTemImg = cvMatimage.clone();
// 	cv::Mat keypointsImage;
// 	cvtColor(cvMatimage, keypointsImage, CV_GRAY2RGB);
// 
// 	// 存储内容
// 	CvMemStorage* contoursStorge = cvCreateMemStorage(0);
// 	CvMemStorage* approxPolyStorge = cvCreateMemStorage(0);
// 
// 	CvSeq* contours = 0;
// 	CvSeq* approxPloy = NULL;
// 
// 	// 原因是后面的信息都是在上面使用
// 	IplImage* temImg = &IplImage(cvMatTemImg);
// 	IplImage* contoursImg = cvCreateImage(cvGetSize(temImg), temImg->depth, temImg->nChannels);
// 	IplImage* approxPloyImg = cvCreateImage(cvGetSize(temImg), temImg->depth, temImg->nChannels);
// 	cvZero(contoursImg);
// 	cvZero(approxPloyImg);
// 	
// 	// 找轮廓
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
// 		//多边形逼近  
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
// 	// 删除内容荣
// 	cvReleaseMemStorage(&contoursStorge);
// 	cvReleaseMemStorage(&approxPolyStorge);
// 	cvReleaseImage(&contoursImg);
// 	cvReleaseImage(&approxPloyImg);

	// 使用改进型的算法，以便提高算法的执行速度与检测效率
}


/***************************************************************************
* 函数名称：   SetFileName
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const char * fileName
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/05/22      饶智博        添加
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
* 函数名称：   DrawCircle
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  IplImage * src
* 参　　数：   [out] cv::Mat dst
* 参　　数：   [in]  std::vector<ST_CENTER> & vct_ponits
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/05/22      饶智博        添加
***************************************************************************/
void FindCircularMarker::DrawCircle(IplImage* src, cv::Mat dst, std::vector<ST_CENTER>& vct_ponits, const char* saveFileName /* = NULL */)
{
	cv::Mat keypointsImage;

	cvtColor(cv::Mat(src), keypointsImage, CV_GRAY2RGB);
	for (int j = 0; j < (int)vct_ponits.size(); j++)
	{
		ST_CENTER center;
		center = vct_ponits.at(j);
		cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // 画一个圆 
		cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // 画一个圆 
	}

	if (NULL != saveFileName)
		cvSaveImage(saveFileName, &IplImage(keypointsImage));

	dst = keypointsImage.clone();
}


/***************************************************************************
* 函数名称：   FindCircleByCICImproved
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const cv::Mat & cvMatimage
* 参　　数：   [in]  cv::SimpleBlobDetector::Params params
* 参　　数：   [in]  std::vector<ST_CENTER> & vct_ponits
* 参　　数：   [in]  BOOL bShow
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/05/10      饶智博        添加
***************************************************************************/
void FindCircularMarker::FindCircleByThreadshold(const cv::Mat& cvMatImage,
	cv::SimpleBlobDetector::Params params, std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
	vct_ponits.clear();
	// 新建一个数据缓存区域
	cv::Mat cvMatTemImg = cvMatImage.clone();  // 复制图片，以免出现问题
	cv::Mat src_gray;                          // 灰度图   

	if (CV_8UC1 != cvMatImage.type())
		cvtColor(cvMatTemImg, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatTemImg.clone();

	// 转一下图片
	IplImage* pImage = &IplImage(src_gray);
	cv::Mat threadsholdImg;

	int ithreadshold = Otsu(pImage);
	cv::threshold(cv::Mat(pImage), threadsholdImg, ithreadshold-5, 255, cv::THRESH_BINARY);

	// 下面是算法真正使用的时间
#ifdef TIME_TEST
	double t = (double)cv::getTickCount();
#endif // TIME_TEST


	FindCircle(threadsholdImg, params, vct_ponits, bShow);

#ifdef TIME_TEST
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Find Circle by CICImproved：" << t << std::endl;
#endif // TIME_TEST
}

/***************************************************************************
* 函数名称：   CheckReliability
* 摘　　要：   
* 全局影响：   private 
* 参　　数：   [in]  const std::vector<double> src
* 参　　数：   [in]  const double dbVarPrecision
* 参　　数：   [in]  const double dbRaPrecision
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/11/04      饶智博        添加
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
* 函数名称：   FindCircleByThreshold
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Mat & cvMatImage
* 参　　数：   [in]  cv::SimpleBlobDetector::Params params
* 参　　数：   [in]  std::vector<ST_CENTER> & vct_ponits
* 参　　数：   [in]  BOOL bShow
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/26      饶智博        添加
***************************************************************************/
void FindCircularMarker::FindCircleByWaterThed(const cv::Mat& cvMatImage,
	cv::SimpleBlobDetector::Params params, std::vector<ST_CENTER>& vct_ponits, BOOL bShow /* = FALSE */)
{
	// 新建一个数据缓存区域
	cv::Mat cvMatTemImg = cvMatImage.clone();  // 复制图片，以免出现问题
	cv::Mat src_gray;                          // 灰度图   

	// 如果不是灰度图就转成灰度图
	//	if (IPL_DEPTH_8U != cvMatTemImg.depth())
	//		cvtColor(cvMatTemImg, src_gray, CV_BGR2GRAY);

	// 转一下图片

	cv::Mat keypointsImage;
	cvtColor(cv::Mat(cvMatTemImg), keypointsImage, CV_GRAY2RGB);
	IplImage* pImage = &IplImage(keypointsImage);
	IplImage* pCannyImage = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_32S, 1);

	// 下面是算法真正使用的时间
#ifdef TIME_TEST
	double t = (double)cv::getTickCount();
#endif // TIME_TEST

	// 使用Otsu计算出阈值
	//int cannyThreshold = Otsu(pImage);
	// 使用canny算子来计算图片,至于为什么*2...我只能说效果好
	//cvCanny(pImage, pCannyImage, cannyThreshold, cannyThreshold * 2, 3);
	cv::watershed(cv::Mat(pImage), cv::Mat(pCannyImage));

	// 看是否要显示出图片
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
	std::cout << "Find Circle by CICImproved：" << t << std::endl;
#endif // TIME_TEST

	cvReleaseImage(&pCannyImage);
}


/***************************************************************************
* 函数名称：   FindCircleByCICImproved
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Mat & cvMatImage
* 参　　数：   [in]  cv::SimpleBlobDetector::Params params
* 参　　数：   [in]  std::vector<ST_CENTER> & vct_ponits
* 参　　数：   [in]  double smoothingSigma
* 参　　数：   [in]  BOOL bShow
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/10/12      饶智博        添加
***************************************************************************/
void FindCircularMarker::FindCircleByCICImproved(const cv::Mat& cvMatImage,
	cv::SimpleBlobDetector::Params params, std::vector<ST_CENTER>& vct_ponits, double smoothingSigma , BOOL bShow /* = FALSE */)
{
	vct_ponits.clear();
	// 新建一个数据缓存区域
	cv::Mat cvMatTemImg = cvMatImage.clone();  // 复制图片，以免出现问题
	cv::Mat src_gray;                          // 灰度图   

	if (CV_8UC1 != cvMatImage.type())
		cvtColor(cvMatTemImg, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatTemImg.clone();

	// 使用EDPF找边界
	FindContoursMethod f;
	cv::Mat dst;
	f.findContoursByEDPF(src_gray, dst, smoothingSigma);
	f.findContoursByEDPF(dst, dst, smoothingSigma);

	// 看是否要显示出图片
	if (bShow){
		const char* windows_name = "canny";
		cvNamedWindow(windows_name, CV_WINDOW_NORMAL);
		imshow(windows_name, dst);
		cvWaitKey(0);
	}

	FindCircle(dst, params, vct_ponits, bShow);
}


/***************************************************************************
* 函数名称：   FindCircleByCIC
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const cv::Mat & cvMatImage
* 参　　数：   [in]  cv::SimpleBlobDetector::Params params
* 参　　数：   [in]  std::vector<ST_CENTER> & vct_ponits
* 参　　数：   [in]  double smoothingSigma
* 参　　数：   [in]  BOOL bShow
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/11/04      饶智博        添加
***************************************************************************/
void FindCircularMarker::FindCircleByCIC(const cv::Mat& cvMatImage,
	cv::SimpleBlobDetector::Params params,
	std::vector<ST_CENTER>& vct_ponits,
	double smoothingSigma /* = 1.0 */,
	BOOL bShow /* = FALSE */)
{
	vct_ponits.clear();
	// 新建一个数据缓存区域
	cv::Mat cvMatTemImg = cvMatImage.clone();  // 复制图片，以免出现问题
	cv::Mat src_gray;                          // 灰度图   

	if (CV_8UC1 != cvMatImage.type())
		cvtColor(cvMatTemImg, src_gray, CV_BGR2GRAY);
	else
		src_gray = cvMatTemImg.clone();

	// 使用EDPF找边界
	FindContoursMethod f;
	cv::Mat dst;
	f.findContoursByEDPF(src_gray, dst, smoothingSigma);
	//f.findContoursByEDPF(dst, dst, smoothingSigma);

	// 看是否要显示出图片
	if (bShow){
		const char* windows_name = "EDPF";
		cvNamedWindow(windows_name, CV_WINDOW_NORMAL);
		imshow(windows_name, dst);
		cvWaitKey(0);
	}

	// 调用改进型算法完成该内容
	//FindCircleImproved(dst, params, vct_ponits, bShow);
	FindCircle(dst, params, vct_ponits, bShow);
}