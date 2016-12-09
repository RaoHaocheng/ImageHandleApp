// 相机标定.cpp文件
#include "CameraCalibration.h"

/***************************************************************************
* 函数名称：   CameraCalibration
* 摘　　要：
* 全局影响：   public
* 返回值　：
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/15      饶智博        添加
***************************************************************************/
CameraCalibration::CameraCalibration()
:m_CalibrationStep(UNSTART),
m_stImageInfo(NULL),
m_stCalibrationResult(NULL),
m_bShowImage(false),
m_vcLImage(0),
m_vcRImage(0),
m_cvSize(NULL),
m_stRecitfyPaprameter(NULL)
{
}

/***************************************************************************
* 函数名称：   ~CameraCalibration
* 摘　　要：
* 全局影响：   virtual public
* 返回值　：
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/15      饶智博        添加
***************************************************************************/
CameraCalibration::~CameraCalibration()
{
	if (m_stImageInfo != NULL){
		delete m_stImageInfo;
		m_stImageInfo = NULL;
	}

	ClearImageCache();
	DeleteCalibrationResult(&m_stCalibrationResult);
}

/***************************************************************************
* 函数名称：   SetCalibrationImageInfo
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const ST_CALIBRATION_IMAGE_INFO & stImageInfo
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/15      饶智博        添加
***************************************************************************/
void CameraCalibration::SetCalibrationImageInfo(const ST_CALIBRATION_IMAGE_INFO& stImageInfo)
{
	// 如果没有开辟新的空间,那么就新建
	if (NULL == m_stImageInfo){
		m_stImageInfo = new ST_CALIBRATION_IMAGE_INFO;
		ZeroMemory(m_stImageInfo, sizeof(ST_CALIBRATION_IMAGE_INFO));
	}

	// 拷贝数据
	memcpy(m_stImageInfo, &stImageInfo, sizeof(ST_CALIBRATION_IMAGE_INFO));

	// 新建新的结果存放空间
	if (NewCalibrationResult()){
		m_CalibrationStep = SET_INFO;   // 设置了这个就说明我们可以开始进行下一步了
		ClearImageCache();              // 清理图片缓存
	}
}

/***************************************************************************
* 函数名称：   DeleteCalibrationResult
* 摘　　要：
* 全局影响：   private
* 参　　数：   [in]  ST_CALIBRATION_RESULT * stCalibrationResult
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/15      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::DeleteCalibrationResult(ST_CALIBRATION_RESULT** stCalibrationResult)
{
	if (NULL == (*stCalibrationResult))
		return FALSE;

	// 释放掉内存
	cvReleaseMat(&(*stCalibrationResult)->cvMatLdistortionCoeff);
	cvReleaseMat(&(*stCalibrationResult)->cvMatLeftPoint);
	cvReleaseMat(&(*stCalibrationResult)->cvMatLeftPointCount);
	cvReleaseMat(&(*stCalibrationResult)->cvMatLintrinsicMatrix);
	cvReleaseMat(&(*stCalibrationResult)->cvMatLRotationVct);
	cvReleaseMat(&(*stCalibrationResult)->cvMatLTranslaVct);
	cvReleaseMat(&(*stCalibrationResult)->cvMatObjectPoint);
	cvReleaseMat(&(*stCalibrationResult)->cvMatRdistortionCoeff);
	cvReleaseMat(&(*stCalibrationResult)->cvMatRightPoint);
	cvReleaseMat(&(*stCalibrationResult)->cvMatRightPointCount);
	cvReleaseMat(&(*stCalibrationResult)->cvMatRintrinsicMatrix);
	cvReleaseMat(&(*stCalibrationResult)->cvMatRotationStereo);
	cvReleaseMat(&(*stCalibrationResult)->cvMatRRotationVct);
	cvReleaseMat(&(*stCalibrationResult)->cvMatRTranslaVct);
	cvReleaseMat(&(*stCalibrationResult)->cvMatTranslationStereo);
	cvReleaseMat(&(*stCalibrationResult)->cvMatFundamentalMat);
	cvReleaseMat(&(*stCalibrationResult)->cvEssentialValueMat);

	// 删掉结构体
	delete (*stCalibrationResult);
	(*stCalibrationResult) = NULL;

	return TRUE;
}


/***************************************************************************
* 函数名称：   NewCalibrationResult
* 摘　　要：
* 全局影响：   private
* 参　　数：   [in]  ST_CALIBRATION_RESULT ** stCalibrationResult
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/15      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::NewCalibrationResult(ST_CALIBRATION_RESULT** stCalibrationResult,
	const ST_CALIBRATION_IMAGE_INFO& stImageInfo)
{
	if (NULL != (*stCalibrationResult))
		DeleteCalibrationResult(stCalibrationResult);

	if (NULL != (*stCalibrationResult))
		return FALSE;

	// 新建一个存放内存
	bool bNewResult = true;
	const int TOTAL_COTNOR_NUM = stImageInfo.iImageHeight*stImageInfo.iImageWidth; // 总角点数
	(*stCalibrationResult) = new ST_CALIBRATION_RESULT;
	ZeroMemory((*stCalibrationResult), sizeof(ST_CALIBRATION_RESULT));

	(*stCalibrationResult)->cvMatLdistortionCoeff
		= cvCreateMat(DISTORTION_COEFFS_ROWS, DISTORTION_COEFFS_COLS, CV_32FC1);
	if (NULL == (*stCalibrationResult)->cvMatLdistortionCoeff)
		bNewResult = false;

	(*stCalibrationResult)->cvMatLeftPoint
		= cvCreateMat(TOTAL_COTNOR_NUM*stImageInfo.iImgCounts, SYSTEM_XY, CV_32FC1);
	if (NULL == (*stCalibrationResult)->cvMatLeftPoint)
		bNewResult = false;

	(*stCalibrationResult)->cvMatLeftPointCount
		= cvCreateMat(stImageInfo.iImgCounts, SYSTEM_X, CV_32SC1);
	if (NULL == (*stCalibrationResult)->cvMatLeftPointCount)
		bNewResult = false;

	(*stCalibrationResult)->cvMatLintrinsicMatrix
		= cvCreateMat(INTRINSIC_MATRIX_ROWS, INTRINSIC_MATRIX_COLS, CV_32FC1);
	if (NULL == (*stCalibrationResult)->cvMatLintrinsicMatrix)
		bNewResult = false;

	(*stCalibrationResult)->cvMatLRotationVct
		= cvCreateMat(stImageInfo.iImgCounts, ROTATION_VCT, CV_64FC1);
	if (NULL == (*stCalibrationResult)->cvMatLRotationVct)
		bNewResult = false;

	(*stCalibrationResult)->cvMatLTranslaVct
		= cvCreateMat(stImageInfo.iImgCounts, TRANSLA_VCT, CV_64FC1);
	if (NULL == (*stCalibrationResult)->cvMatLTranslaVct)
		bNewResult = false;

	(*stCalibrationResult)->cvMatObjectPoint
		= cvCreateMat(TOTAL_COTNOR_NUM*stImageInfo.iImgCounts, OBJECT_VET_COLS, CV_32FC1);
	if (NULL == (*stCalibrationResult)->cvMatObjectPoint)
		bNewResult = false;

	(*stCalibrationResult)->cvMatRdistortionCoeff
		= cvCreateMat(DISTORTION_COEFFS_ROWS, DISTORTION_COEFFS_COLS, CV_32FC1);
	if (NULL == (*stCalibrationResult)->cvMatRdistortionCoeff)
		bNewResult = false;

	(*stCalibrationResult)->cvMatRightPoint
		= cvCreateMat(TOTAL_COTNOR_NUM*stImageInfo.iImgCounts, SYSTEM_XY, CV_32FC1);
	if (NULL == (*stCalibrationResult)->cvMatRightPoint)
		bNewResult = false;

	(*stCalibrationResult)->cvMatRightPointCount
		= cvCreateMat(stImageInfo.iImgCounts, SYSTEM_X, CV_32SC1);
	if (NULL == (*stCalibrationResult)->cvMatRightPointCount)
		bNewResult = false;

	(*stCalibrationResult)->cvMatRintrinsicMatrix
		= cvCreateMat(INTRINSIC_MATRIX_ROWS, INTRINSIC_MATRIX_COLS, CV_32FC1);
	if (NULL == (*stCalibrationResult)->cvMatRintrinsicMatrix)
		bNewResult = false;

	(*stCalibrationResult)->cvMatRotationStereo
		= cvCreateMat(ROTATION_VCT, ROTATION_VCT, CV_64FC1);
	if (NULL == (*stCalibrationResult)->cvMatRotationStereo)
		bNewResult = false;

	(*stCalibrationResult)->cvMatRRotationVct
		= cvCreateMat(stImageInfo.iImgCounts, ROTATION_VCT, CV_64FC1);
	if (NULL == (*stCalibrationResult)->cvMatRRotationVct)
		bNewResult = false;

	(*stCalibrationResult)->cvMatRTranslaVct
		= cvCreateMat(stImageInfo.iImgCounts, TRANSLA_VCT, CV_64FC1);
	if (NULL == (*stCalibrationResult)->cvMatRTranslaVct)
		bNewResult = false;

	(*stCalibrationResult)->cvMatTranslationStereo
		= cvCreateMat(TRANSLA_VCT, SYSTEM_X, CV_64FC1);
	if (NULL == (*stCalibrationResult)->cvMatTranslationStereo)
		bNewResult = false;

	(*stCalibrationResult)->cvMatFundamentalMat
		= cvCreateMat(FUNDAMENTAL_MAT, FUNDAMENTAL_MAT, CV_64FC1);
	if (NULL == (*stCalibrationResult)->cvMatFundamentalMat)
		bNewResult = false;

	(*stCalibrationResult)->cvEssentialValueMat
		= cvCreateMat(ESSENTIALVALUE_MAT, ESSENTIALVALUE_MAT, CV_64FC1);
	if (NULL == (*stCalibrationResult)->cvEssentialValueMat)
		bNewResult = false;

	// 如果有内存开辟失败，就返回失败
	if (!bNewResult){
		DeleteCalibrationResult(stCalibrationResult);
		return FALSE;
	}

	return TRUE;
}

/***************************************************************************
* 函数名称：   NewCalibrationResult
* 摘　　要：
* 全局影响：   private
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/15      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::NewCalibrationResult()
{
	// 在我们调用的内部，图片信息不能为空
	if (NULL == m_stImageInfo)
		return FALSE;

	return NewCalibrationResult(&m_stCalibrationResult, *m_stImageInfo);
}

/***************************************************************************
* 函数名称：   DeleteCalibrationResult
* 摘　　要：
* 全局影响：   private
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/15      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::DeleteCalibrationResult()
{
	return DeleteCalibrationResult(&m_stCalibrationResult);
}

/***************************************************************************
* 函数名称：   OpenCalibrationImage
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  const char * pcFileName
* 参　　数：   [in]  int flags
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/15      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::OpenCalibrationImage(const char* pcFileName, int flags)
{
	// 只有在设置好了数据后才能完成下面的步骤
	if (SET_INFO != m_CalibrationStep)
		return FALSE;

	BOOL bOK = FALSE;
	switch (flags)
	{
	case LEFT_CAMERA_IMAGE:
		SaveLeftImageCache(pcFileName);
		bOK = TRUE;
		break;
	case RIGHT_CAMERA_IMAGE:
		SaveRightImageCache(pcFileName);
		bOK = TRUE;
	default:
		bOK = FALSE;
		break;
	}

	// 说明所有的图片都已经完成
	if ((m_vcLImage.size() == m_stImageInfo->iImgCounts)
		&& (m_vcRImage.size() == m_stImageInfo->iImgCounts))
		m_CalibrationStep = OPEN_IMAGE;

	return bOK;
}
BOOL CameraCalibration::Calibration(ST_CALIBRATION_RESULT& stResult)
{
	return Calibration(stResult, NULL);
}
/***************************************************************************
* 函数名称：   Calibration
* 摘　　要：
* 全局影响：   public
* 参　　数：   [in]  ST_CALIBRATION_RESULT & stResult
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/15      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::Calibration(ST_CALIBRATION_RESULT& stResult, const char* pcFileName)
{
	// 说明已经完成打开所有的图片，否则不做任何处理
	if (OPEN_IMAGE != m_CalibrationStep && CALIBRATION != m_CalibrationStep)
		return FALSE;

	// 找角点
	if (!FindBoardCorner())
		return FALSE;

	// 计算内外参数
	if (!ComputerParameter(stResult))
		return FALSE;

	// 校正图片
	if (!RectifyImg(pcFileName))
		return FALSE;

	memcpy(&stResult, m_stCalibrationResult, sizeof(ST_CALIBRATION_RESULT));
	return TRUE;
}

/***************************************************************************
* 函数名称：   SaveLeftImageCache
* 摘　　要：
* 全局影响：   private
* 参　　数：   [in]  const char * pcFileName
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/15      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::SaveLeftImageCache(const char* pcFileName)
{
	// 如果已经大于输入的照片的话就取消掉
	if (((int)m_vcLImage.size()) >= m_stImageInfo->iImgCounts)
		return FALSE;

	// 读取图片
	IplImage* pImage = cvLoadImage(pcFileName, CV_LOAD_IMAGE_ANYCOLOR);
	if (NULL == pImage)
		return FALSE;

	if (!CheckImgSize(pImage))
		return FALSE;

	m_vcLImage.push_back(pImage);   // 添加到容器中来

	// 显示图片
	if (m_bShowImage)
	{
		cvNamedWindow("LeftCameraImage");
		cvShowImage("LeftCameraImage", pImage);
		cvWaitKey(0);                           // 等待按键
	}

	return TRUE;
}


/***************************************************************************
* 函数名称：   SaveRightImageCache
* 摘　　要：
* 全局影响：   private
* 参　　数：   [in]  const char * pcFileName
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/15      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::SaveRightImageCache(const char* pcFileName)
{

	// 如果已经大于输入的照片的话就取消掉
	if (((int)m_vcRImage.size()) >= m_stImageInfo->iImgCounts)
		return FALSE;

	// 读取图片
	IplImage* pImage = cvLoadImage(pcFileName, CV_LOAD_IMAGE_ANYCOLOR);
	if (NULL == pImage)
		return FALSE;

	if (!CheckImgSize(pImage))
		return FALSE;

	m_vcRImage.push_back(pImage);   // 添加到容器中来

	// 显示图片
	if (m_bShowImage)
	{
		cvNamedWindow("RightCameraImage");
		cvShowImage("RightCameraImage", pImage);
		cvWaitKey(0);                           // 等待按键
	}

	return TRUE;
}

/***************************************************************************
* 函数名称：   ClearImageCache
* 摘　　要：
* 全局影响：   protected
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/15      饶智博        添加
***************************************************************************/
void CameraCalibration::ClearImageCache()
{
	ClearImageCache(LEFT_CAMERA_IMAGE);
	ClearImageCache(RIGHT_CAMERA_IMAGE);
}

/***************************************************************************
* 函数名称：   FindBoardCorner
* 摘　　要：
* 全局影响：   private
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/15      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::FindBoardCorner()
{
	// 说明已经打开了图片
	if (OPEN_IMAGE != m_CalibrationStep && CALIBRATION != m_CalibrationStep)
		return FALSE;

	int iSuccessNum = 0;                                // 找到角点的数量是否都正确
	int iLFindCornorNum = 0, iRFindCornorNum = 0;       // 找到角点的数量
	const int TOTAL_CORNOR_NUM = m_stImageInfo->iImageWidth*m_stImageInfo->iImageHeight; //总角点数
	CvPoint2D32f* cvLImagePointBuf
		= new CvPoint2D32f[TOTAL_CORNOR_NUM]; // 图像坐标点的缓存
	CvPoint2D32f* cvRImagePointBuf
		= new CvPoint2D32f[TOTAL_CORNOR_NUM]; // 图像坐标点的缓存

	for (int i = 0; i != m_stImageInfo->iImgCounts; i++)
	{
		// 找左相机的角点和柚木相机的角点
		if (FindBoardCorner(m_vcLImage.at(i), cvLImagePointBuf, iLFindCornorNum)
			&& FindBoardCorner(m_vcRImage.at(i), cvRImagePointBuf, iRFindCornorNum))
		{
			// 看是否所有的角点都被找到
			if (TOTAL_CORNOR_NUM == iLFindCornorNum && TOTAL_CORNOR_NUM == iRFindCornorNum)
			{
				int iStep = iSuccessNum*TOTAL_CORNOR_NUM;
				for (int j = 0; j < TOTAL_CORNOR_NUM; j++, iStep++)
				{
					// 对左目结果进行赋值
					CV_MAT_ELEM(*(m_stCalibrationResult->cvMatLeftPoint),
						float, iStep, POINTS_X) = cvLImagePointBuf[j].x;
					CV_MAT_ELEM(*(m_stCalibrationResult->cvMatLeftPoint),
						float, iStep, POINTS_Y) = cvLImagePointBuf[j].y;

					// 对右目结果进行赋值
					CV_MAT_ELEM(*(m_stCalibrationResult->cvMatRightPoint),
						float, iStep, POINTS_X) = cvRImagePointBuf[j].x;
					CV_MAT_ELEM(*(m_stCalibrationResult->cvMatRightPoint),
						float, iStep, POINTS_Y) = cvRImagePointBuf[j].y;

					// 写入物理坐标数据
					CV_MAT_ELEM(*(m_stCalibrationResult->cvMatObjectPoint),
						float, iStep, POINTS_X) = (float)(j / m_stImageInfo->iImageWidth*CHESS_BOARD_WIDTH);
					CV_MAT_ELEM(*(m_stCalibrationResult->cvMatObjectPoint),
						float, iStep, POINTS_Y) = (float)(j % m_stImageInfo->iImageWidth*CHESS_BOARD_WIDTH);
					CV_MAT_ELEM(*(m_stCalibrationResult->cvMatObjectPoint),
						float, iStep, POINTS_Z) = 0.0f;
				}

				// 标定的角点数
				CV_MAT_ELEM(*(m_stCalibrationResult->cvMatLeftPointCount), int, iSuccessNum, POINTS_X) = TOTAL_CORNOR_NUM;
				CV_MAT_ELEM(*(m_stCalibrationResult->cvMatRightPointCount), int, iSuccessNum, POINTS_X) = TOTAL_CORNOR_NUM;

				// 说明该图片上的标记点都已经全部找到
				iSuccessNum++;
			}
			else
				break;
		}
		else
			break;

	}

	// 把开辟的内存还回去
	delete[] cvLImagePointBuf;
	delete[] cvRImagePointBuf;

	// 说明所有的角点都查找成功了
	if (iSuccessNum == m_stImageInfo->iImgCounts){
		m_CalibrationStep = FIND_CORNER;  //说明完成了找角点的工作
		return TRUE;
	}
	else{
		// 清空一下结果，错误的结果没有意义
		NewCalibrationResult();
		return FALSE;
	}
}


/***************************************************************************
* 函数名称：   FindBoardCorner
* 摘　　要：
* 全局影响：   private
* 参　　数：   [in]  IplImage * pImg
* 参　　数：   [out] CvPoint2D32f * pImgCornorPoint
* 参　　数：   [out] int iCornorPoints
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/15      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::FindBoardCorner(IplImage* pImg, CvPoint2D32f* pImgCornorPoint, int& iCornorPoints)
{
	if (NULL == pImg || NULL == pImgCornorPoint)
		return FALSE;

	int iFoundCornor = 0;
	iFoundCornor = cvFindChessboardCorners(pImg, cvSize((m_stImageInfo->iImageWidth), m_stImageInfo->iImageHeight),
		pImgCornorPoint, &iCornorPoints,
		CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);

	// 查找失败
	if (ERROR == iFoundCornor)
	{
		if (m_bShowImage)
		{
			cvNamedWindow("FindBoardCornor Failed!");
			cvShowImage("FindBoardCornor", pImg);
			cvWaitKey();
		}
		return FALSE;
	}
	else{// 查找成功
		// 先复制图片
		IplImage* pGrayImg = cvCloneImage(pImg);

		// 查找亚像素
		cvFindCornerSubPix(pGrayImg, pImgCornorPoint, iCornorPoints, WIN_CORNOR, ZERO_CORNOR,
			cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, TERMCRITERIA_MAX_ITER, TERMCRITERIA_EPSILON));

		if (m_bShowImage)
		{
			// 在图片上绘制角点
			cvDrawChessboardCorners(pGrayImg, cvSize((m_stImageInfo->iImageWidth), m_stImageInfo->iImageHeight),
				pImgCornorPoint, iCornorPoints, iFoundCornor);

			cvNamedWindow("CornorImage");
			cvShowImage("CornorImage", pGrayImg);
			cvWaitKey();
		}
	}

	return TRUE;
}

/***************************************************************************
* 函数名称：   ComputerParameter
* 摘　　要：
* 全局影响：   protected
* 参　　数：   [in]  ST_CALIBRATION_RESULT & stResult
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/16      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::ComputerParameter(ST_CALIBRATION_RESULT& stResult)
{
	// 如果没有完成找寻角点的工作就删除掉
	if (FIND_CORNER != m_CalibrationStep || NULL == m_cvSize)
		return FALSE;

	// 对左目相机标定
	cvCalibrateCamera2(m_stCalibrationResult->cvMatObjectPoint,
		m_stCalibrationResult->cvMatLeftPoint, m_stCalibrationResult->cvMatLeftPointCount, *m_cvSize,
		m_stCalibrationResult->cvMatLintrinsicMatrix, m_stCalibrationResult->cvMatLdistortionCoeff,
		m_stCalibrationResult->cvMatLRotationVct, m_stCalibrationResult->cvMatLTranslaVct, 0);///左目相机标定

	// 对右目相机标定
	cvCalibrateCamera2(m_stCalibrationResult->cvMatObjectPoint,
		m_stCalibrationResult->cvMatRightPoint, m_stCalibrationResult->cvMatRightPointCount, *m_cvSize,
		m_stCalibrationResult->cvMatRintrinsicMatrix, m_stCalibrationResult->cvMatRdistortionCoeff,
		m_stCalibrationResult->cvMatRRotationVct, m_stCalibrationResult->cvMatRTranslaVct, 0);///右目相机标定

	// 对双目进行标定
	cvStereoCalibrate(m_stCalibrationResult->cvMatObjectPoint, m_stCalibrationResult->cvMatLeftPoint,
		m_stCalibrationResult->cvMatRightPoint, m_stCalibrationResult->cvMatLeftPointCount, m_stCalibrationResult->cvMatLintrinsicMatrix,
		m_stCalibrationResult->cvMatLdistortionCoeff, m_stCalibrationResult->cvMatRintrinsicMatrix,
		m_stCalibrationResult->cvMatRdistortionCoeff, *m_cvSize,
		m_stCalibrationResult->cvMatRotationStereo, m_stCalibrationResult->cvMatTranslationStereo,
		m_stCalibrationResult->cvEssentialValueMat, m_stCalibrationResult->cvMatFundamentalMat,
		cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, CALIBRATION_MAX_ITER, CALIBRATION_EPSILON),
		CV_CALIB_USE_INTRINSIC_GUESS);///立体标定

	// 说明已经完成计算参数
	m_CalibrationStep = COMPUTER_PARAMETER;

	return TRUE;
}

/***************************************************************************
* 函数名称：   StartCalibration
* 摘　　要：
* 全局影响：   protected
* 参　　数：   [in]  ST_CALIBRATION_RESULT & stResult
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/16      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::RectifyImg(const char* pcFileName)
{
	if (COMPUTER_PARAMETER != m_CalibrationStep)
		return FALSE;

	if (NULL == pcFileName)
		return TRUE;

	CvMat * pcvProjectionMatLeft = cvCreateMat(PROJECTION_MAT_ROWS, PROJECTION_MAT_COLS, CV_64F);  //左投影矩阵
	CvMat * pcvProjectionMatRight = cvCreateMat(PROJECTION_MAT_ROWS, PROJECTION_MAT_COLS, CV_64F); //右投影矩阵	

	CvMat * pcvLeftParallelMat = cvCreateMat(PARALLEL_MAT_ROWS, PARALLEL_MAT_COLS, CV_64F);    //左目相机行对准矩阵
	CvMat * pcvRightParallelMat = cvCreateMat(PARALLEL_MAT_ROWS, PARALLEL_MAT_COLS, CV_64F);   //右目相机行对准矩阵

	CvMat * pcvReprojectionMat = cvCreateMat(REPROJECTION_MAT_ROWS, REPROJECTION_MAT_COLS, CV_64F);  //重投影矩阵

	CvMat* pcvMapXLeft = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F);  //左目相机x方向校正映射表，即mapx1
	CvMat* pcvMapYLeft = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F);  //左目相机y方向校正映射表，即mapy1
	CvMat* pcvMapXRight = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F); // 右目相机x方向校正映射表，即mapx1
	CvMat* pcvMapYRight = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F); // 右目相机y方向校正映射表，即mapy1 correspondences

	cvStereoRectify(m_stCalibrationResult->cvMatLintrinsicMatrix, m_stCalibrationResult->cvMatRintrinsicMatrix,
		m_stCalibrationResult->cvMatLdistortionCoeff, m_stCalibrationResult->cvMatRdistortionCoeff,
		*m_cvSize, m_stCalibrationResult->cvMatRotationStereo, m_stCalibrationResult->cvMatTranslationStereo,
		pcvLeftParallelMat, pcvRightParallelMat, pcvProjectionMatLeft, pcvProjectionMatRight,
		pcvReprojectionMat, CV_CALIB_ZERO_DISPARITY);
	cvInitUndistortRectifyMap(m_stCalibrationResult->cvMatLintrinsicMatrix, m_stCalibrationResult->cvMatLdistortionCoeff,
		pcvLeftParallelMat, pcvProjectionMatLeft, pcvMapXLeft, pcvMapYLeft);
	cvInitUndistortRectifyMap(m_stCalibrationResult->cvMatRintrinsicMatrix,
		m_stCalibrationResult->cvMatRdistortionCoeff, pcvRightParallelMat, pcvProjectionMatRight,
		pcvMapXRight, pcvMapYRight);

	char*  pcFileNameBuf = new char[BUFFER_SIZE]; //新建名字缓存
	IplImage* pRectifyImg = cvCreateImage(*m_cvSize, IPL_DEPTH_8U, PIXEL_SINGLE_CHANNELS);     // 校正之后的相机图像
	CvMat* pcvMatPairImg = cvCreateMat(m_cvSize->height, m_cvSize->width * 2, CV_8UC3);        // 需要放置图片
	CvMat cvSubMat;

	for (int i = 0; i < m_stImageInfo->iImgCounts; i++)
	{
		// 将修正够的图片防止pRectifyImg指针中
		cvRemap(m_vcLImage.at(i), pRectifyImg, pcvMapXLeft, pcvMapYLeft);

		// 显示出来
		if (m_bShowImage){
			cvNamedWindow("RectifyLeftImage");
			cvShowImage("RectifyLeftImage", pRectifyImg);
		}

		// 先填左图
		cvGetCols(pcvMatPairImg, &cvSubMat, START_ROWS, m_cvSize->width);
		cvCvtColor(pRectifyImg, &cvSubMat, CV_GRAY2BGR);

		// 保存图片
		sprintf_s(pcFileNameBuf, BUFFER_SIZE, "");                                          // 清空名字
		sprintf_s(pcFileNameBuf, BUFFER_SIZE, "%s_LeftCremeraImg_%d.bmp", pcFileName, i);   // 写入名字
		cvSaveImage(pcFileNameBuf, pRectifyImg);                                            // 保存图片

		// 修正二张图片
		cvRemap(m_vcRImage.at(i), pRectifyImg, pcvMapXRight, pcvMapYRight);

		// 显示出来
		if (m_bShowImage){
			cvNamedWindow("RectifyRightImage");
			cvShowImage("RectifyRightImage", pRectifyImg);
		}

		// 拼接右图
		cvGetCols(pcvMatPairImg, &cvSubMat, m_cvSize->width, m_cvSize->width * 2);
		cvCvtColor(pRectifyImg, &cvSubMat, CV_GRAY2BGR);

		// 保存图片
		sprintf_s(pcFileNameBuf, BUFFER_SIZE, "");
		sprintf_s(pcFileNameBuf, BUFFER_SIZE, "%s_RightCremeraImg_%d.bmp", pcFileName, i);
		cvSaveImage(pcFileNameBuf, pRectifyImg);

		/// 绘制线条，以便查看两幅图像是否行对准
		for (int j = 0; j < m_cvSize->height; j += INTERVAL_PIXEL)
			cvLine(pcvMatPairImg, cvPoint(START_ROWS, j), cvPoint(m_cvSize->width * 2, j), CV_RGB(0, 255, 0));

		sprintf_s(pcFileNameBuf, BUFFER_SIZE, "");
		sprintf_s(pcFileNameBuf, BUFFER_SIZE, "%s_MergeLeftAndRightImg_%d.bmp", pcFileName, i);
		cvSaveImage(pcFileNameBuf, pcvMatPairImg);
	}

	// 释放这些空间
	delete[] pcFileNameBuf;
	cvReleaseMat(&pcvProjectionMatLeft);
	cvReleaseMat(&pcvProjectionMatRight);
	cvReleaseMat(&pcvLeftParallelMat);
	cvReleaseMat(&pcvRightParallelMat);
	cvReleaseMat(&pcvReprojectionMat);
	cvReleaseMat(&pcvMapXLeft);
	cvReleaseMat(&pcvMapYLeft);
	cvReleaseMat(&pcvMapXRight);
	cvReleaseMat(&pcvMapYRight);
	cvReleaseMat(&pcvMatPairImg);
	cvReleaseImage(&pRectifyImg);

	// 说明已经完成了标定
	m_CalibrationStep = CALIBRATION;

	return TRUE;
}

/***************************************************************************
* 函数名称：   GetImgSize
* 摘　　要：
* 全局影响：   public
* 返回值　：   CvSize
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/16      饶智博        添加
***************************************************************************/
CvSize CameraCalibration::GetImgSize() const
{
	if (NULL == m_cvSize){
		CvSize cvImgSize;
		cvImgSize.width = 0;
		cvImgSize.height = 0;
		return cvImgSize;
	}

	return (*m_cvSize);
}

/***************************************************************************
* 函数名称：   ClearImageCache
* 摘　　要：
* 全局影响：   protected
* 参　　数：   [in]  const int flags
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/17      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::ClearImageCache(const int flags)
{
	if ((LEFT_CAMERA_IMAGE != flags) ||
		(RIGHT_CAMERA_IMAGE != flags))
		return FALSE;

	if (LEFT_CAMERA_IMAGE == flags)// 说明有缓存
	{
		for (int i = 0; i != m_vcLImage.size(); i++)
			cvReleaseImage(&(m_vcLImage.at(i)));
		m_vcLImage.clear();
	}
	if (RIGHT_CAMERA_IMAGE == flags)
	{
		for (int i = 0; i != m_vcRImage.size(); i++)
			cvReleaseImage(&(m_vcRImage.at(i)));
		m_vcRImage.clear();
	}

	// 将状态设置好
	if ((m_CalibrationStep != UNSTART) || (m_CalibrationStep != CALIBRATION_ERROR))
		m_CalibrationStep = SET_INFO;

	return TRUE;
}

/***************************************************************************
* 函数名称：   CheckImgSize
* 摘　　要：
* 全局影响：   private
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/17      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::CheckImgSize(IplImage* pImg)
{
	// 如果万一没有新建指针我就
	if (NULL == m_cvSize){
		m_cvSize = new CvSize();
		(*m_cvSize) = cvGetSize(pImg);
		return TRUE;
	}

	CvSize cvImgSize = cvGetSize(pImg);
	if (m_cvSize->width != cvImgSize.width || m_cvSize->height != cvImgSize.height){
		delete m_cvSize;
		m_cvSize = NULL;
		return FALSE;
	}

	return TRUE;
}

/***************************************************************************
* 函数名称：   RectifyImg
* 摘　　要：   
* 全局影响：   protected 
* 参　　数：   [in]  IplImage * pImg
* 参　　数：   [in]  int flags
* 返回值　：   BOOL
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/03/21      饶智博        添加
***************************************************************************/
BOOL CameraCalibration::RectifyImg(IplImage* pImg, int flags)
{
	if (nullptr == pImg || nullptr == m_stCalibrationResult)
		return FALSE;

	CvMat* pcvProjectionMatLeft = cvCreateMat(PROJECTION_MAT_ROWS, PROJECTION_MAT_COLS, CV_64F);  //左投影矩阵
	CvMat* pcvProjectionMatRight = cvCreateMat(PROJECTION_MAT_ROWS, PROJECTION_MAT_COLS, CV_64F); //右投影矩阵	
	CvMat* pcvLeftParallelMat = cvCreateMat(PARALLEL_MAT_ROWS, PARALLEL_MAT_COLS, CV_64F);    //左目相机行对准矩阵
	CvMat* pcvRightParallelMat = cvCreateMat(PARALLEL_MAT_ROWS, PARALLEL_MAT_COLS, CV_64F);   //右目相机行对准矩阵
	CvMat* pcvReprojectionMat = cvCreateMat(REPROJECTION_MAT_ROWS, REPROJECTION_MAT_COLS, CV_64F);  //重投影矩阵
	CvMat* pcvMapXLeft = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F);  //左目相机x方向校正映射表，即mapx1
	CvMat* pcvMapYLeft = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F);  //左目相机y方向校正映射表，即mapy1
	CvMat* pcvMapXRight = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F); // 右目相机x方向校正映射表，即mapx1
	CvMat* pcvMapYRight = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F); // 右目相机y方向校正映射表，即mapy1 correspondences

	cvStereoRectify(m_stCalibrationResult->cvMatLintrinsicMatrix, m_stCalibrationResult->cvMatRintrinsicMatrix,
		m_stCalibrationResult->cvMatLdistortionCoeff, m_stCalibrationResult->cvMatRdistortionCoeff,
		*m_cvSize, m_stCalibrationResult->cvMatRotationStereo, m_stCalibrationResult->cvMatTranslationStereo,
		pcvLeftParallelMat, pcvRightParallelMat, pcvProjectionMatLeft, pcvProjectionMatRight,
		pcvReprojectionMat, CV_CALIB_ZERO_DISPARITY);
	cvInitUndistortRectifyMap(m_stCalibrationResult->cvMatLintrinsicMatrix, m_stCalibrationResult->cvMatLdistortionCoeff,
		pcvLeftParallelMat, pcvProjectionMatLeft, pcvMapXLeft, pcvMapYLeft);
	cvInitUndistortRectifyMap(m_stCalibrationResult->cvMatRintrinsicMatrix,
		m_stCalibrationResult->cvMatRdistortionCoeff, pcvRightParallelMat, pcvProjectionMatRight,
		pcvMapXRight, pcvMapYRight);

	BOOL bOk = TRUE;
	IplImage* pCopyImg = cvCreateImage(cvGetSize(pImg), pImg->depth, pImg->nChannels);
	cvCopy(pImg, pCopyImg, NULL);

	// 将修正够的图片防止pRectifyImg指针中
	switch (flags)
	{
	case LEFT_CAMERA_IMAGE:
		cvRemap(pCopyImg, pImg, pcvMapXLeft, pcvMapYLeft);
		break;
	case RIGHT_CAMERA_IMAGE:
		cvRemap(pCopyImg, pImg, pcvMapXRight, pcvMapYRight);
		break;
	default:
		bOk = FALSE;
		break;
	}

	cvReleaseMat(&pcvProjectionMatLeft);
	cvReleaseMat(&pcvProjectionMatRight);
	cvReleaseMat(&pcvLeftParallelMat);
	cvReleaseMat(&pcvRightParallelMat);
	cvReleaseMat(&pcvReprojectionMat);
	cvReleaseMat(&pcvMapXLeft);
	cvReleaseMat(&pcvMapYLeft);
	cvReleaseMat(&pcvMapXRight);
	cvReleaseMat(&pcvMapYRight);
	cvReleaseImage(&pCopyImg);

	return bOk;
}

BOOL CameraCalibration::Test()
{
	int x = 0;
	//x = 1 / x;
	std::cout << "hello world!" << std::endl;
	std::cout << "hello world!" << std::endl;
	std::cout << "hello world!" << std::endl;
	std::cout << "hello world!" << std::endl;
	std::cout << "hello world!" << std::endl; 
	std::cout << "hello world!" << std::endl;
	system("pause");
	return TRUE;
	
}