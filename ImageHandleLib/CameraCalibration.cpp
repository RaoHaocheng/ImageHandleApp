// ����궨.cpp�ļ�
#include "CameraCalibration.h"

/***************************************************************************
* �������ƣ�   CameraCalibration
* ժ����Ҫ��
* ȫ��Ӱ�죺   public
* ����ֵ����
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/15      ���ǲ�        ���
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
* �������ƣ�   ~CameraCalibration
* ժ����Ҫ��
* ȫ��Ӱ�죺   virtual public
* ����ֵ����
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/15      ���ǲ�        ���
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
* �������ƣ�   SetCalibrationImageInfo
* ժ����Ҫ��
* ȫ��Ӱ�죺   public
* �Ρ�������   [in]  const ST_CALIBRATION_IMAGE_INFO & stImageInfo
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/15      ���ǲ�        ���
***************************************************************************/
void CameraCalibration::SetCalibrationImageInfo(const ST_CALIBRATION_IMAGE_INFO& stImageInfo)
{
	// ���û�п����µĿռ�,��ô���½�
	if (NULL == m_stImageInfo){
		m_stImageInfo = new ST_CALIBRATION_IMAGE_INFO;
		ZeroMemory(m_stImageInfo, sizeof(ST_CALIBRATION_IMAGE_INFO));
	}

	// ��������
	memcpy(m_stImageInfo, &stImageInfo, sizeof(ST_CALIBRATION_IMAGE_INFO));

	// �½��µĽ����ſռ�
	if (NewCalibrationResult()){
		m_CalibrationStep = SET_INFO;   // �����������˵�����ǿ��Կ�ʼ������һ����
		ClearImageCache();              // ����ͼƬ����
	}
}

/***************************************************************************
* �������ƣ�   DeleteCalibrationResult
* ժ����Ҫ��
* ȫ��Ӱ�죺   private
* �Ρ�������   [in]  ST_CALIBRATION_RESULT * stCalibrationResult
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/15      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::DeleteCalibrationResult(ST_CALIBRATION_RESULT** stCalibrationResult)
{
	if (NULL == (*stCalibrationResult))
		return FALSE;

	// �ͷŵ��ڴ�
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

	// ɾ���ṹ��
	delete (*stCalibrationResult);
	(*stCalibrationResult) = NULL;

	return TRUE;
}


/***************************************************************************
* �������ƣ�   NewCalibrationResult
* ժ����Ҫ��
* ȫ��Ӱ�죺   private
* �Ρ�������   [in]  ST_CALIBRATION_RESULT ** stCalibrationResult
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/15      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::NewCalibrationResult(ST_CALIBRATION_RESULT** stCalibrationResult,
	const ST_CALIBRATION_IMAGE_INFO& stImageInfo)
{
	if (NULL != (*stCalibrationResult))
		DeleteCalibrationResult(stCalibrationResult);

	if (NULL != (*stCalibrationResult))
		return FALSE;

	// �½�һ������ڴ�
	bool bNewResult = true;
	const int TOTAL_COTNOR_NUM = stImageInfo.iImageHeight*stImageInfo.iImageWidth; // �ܽǵ���
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

	// ������ڴ濪��ʧ�ܣ��ͷ���ʧ��
	if (!bNewResult){
		DeleteCalibrationResult(stCalibrationResult);
		return FALSE;
	}

	return TRUE;
}

/***************************************************************************
* �������ƣ�   NewCalibrationResult
* ժ����Ҫ��
* ȫ��Ӱ�죺   private
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/15      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::NewCalibrationResult()
{
	// �����ǵ��õ��ڲ���ͼƬ��Ϣ����Ϊ��
	if (NULL == m_stImageInfo)
		return FALSE;

	return NewCalibrationResult(&m_stCalibrationResult, *m_stImageInfo);
}

/***************************************************************************
* �������ƣ�   DeleteCalibrationResult
* ժ����Ҫ��
* ȫ��Ӱ�죺   private
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/15      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::DeleteCalibrationResult()
{
	return DeleteCalibrationResult(&m_stCalibrationResult);
}

/***************************************************************************
* �������ƣ�   OpenCalibrationImage
* ժ����Ҫ��
* ȫ��Ӱ�죺   public
* �Ρ�������   [in]  const char * pcFileName
* �Ρ�������   [in]  int flags
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/15      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::OpenCalibrationImage(const char* pcFileName, int flags)
{
	// ֻ�������ú������ݺ�����������Ĳ���
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

	// ˵�����е�ͼƬ���Ѿ����
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
* �������ƣ�   Calibration
* ժ����Ҫ��
* ȫ��Ӱ�죺   public
* �Ρ�������   [in]  ST_CALIBRATION_RESULT & stResult
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/15      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::Calibration(ST_CALIBRATION_RESULT& stResult, const char* pcFileName)
{
	// ˵���Ѿ���ɴ����е�ͼƬ���������κδ���
	if (OPEN_IMAGE != m_CalibrationStep && CALIBRATION != m_CalibrationStep)
		return FALSE;

	// �ҽǵ�
	if (!FindBoardCorner())
		return FALSE;

	// �����������
	if (!ComputerParameter(stResult))
		return FALSE;

	// У��ͼƬ
	if (!RectifyImg(pcFileName))
		return FALSE;

	memcpy(&stResult, m_stCalibrationResult, sizeof(ST_CALIBRATION_RESULT));
	return TRUE;
}

/***************************************************************************
* �������ƣ�   SaveLeftImageCache
* ժ����Ҫ��
* ȫ��Ӱ�죺   private
* �Ρ�������   [in]  const char * pcFileName
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/15      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::SaveLeftImageCache(const char* pcFileName)
{
	// ����Ѿ������������Ƭ�Ļ���ȡ����
	if (((int)m_vcLImage.size()) >= m_stImageInfo->iImgCounts)
		return FALSE;

	// ��ȡͼƬ
	IplImage* pImage = cvLoadImage(pcFileName, CV_LOAD_IMAGE_ANYCOLOR);
	if (NULL == pImage)
		return FALSE;

	if (!CheckImgSize(pImage))
		return FALSE;

	m_vcLImage.push_back(pImage);   // ��ӵ���������

	// ��ʾͼƬ
	if (m_bShowImage)
	{
		cvNamedWindow("LeftCameraImage");
		cvShowImage("LeftCameraImage", pImage);
		cvWaitKey(0);                           // �ȴ�����
	}

	return TRUE;
}


/***************************************************************************
* �������ƣ�   SaveRightImageCache
* ժ����Ҫ��
* ȫ��Ӱ�죺   private
* �Ρ�������   [in]  const char * pcFileName
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/15      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::SaveRightImageCache(const char* pcFileName)
{

	// ����Ѿ������������Ƭ�Ļ���ȡ����
	if (((int)m_vcRImage.size()) >= m_stImageInfo->iImgCounts)
		return FALSE;

	// ��ȡͼƬ
	IplImage* pImage = cvLoadImage(pcFileName, CV_LOAD_IMAGE_ANYCOLOR);
	if (NULL == pImage)
		return FALSE;

	if (!CheckImgSize(pImage))
		return FALSE;

	m_vcRImage.push_back(pImage);   // ��ӵ���������

	// ��ʾͼƬ
	if (m_bShowImage)
	{
		cvNamedWindow("RightCameraImage");
		cvShowImage("RightCameraImage", pImage);
		cvWaitKey(0);                           // �ȴ�����
	}

	return TRUE;
}

/***************************************************************************
* �������ƣ�   ClearImageCache
* ժ����Ҫ��
* ȫ��Ӱ�죺   protected
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/15      ���ǲ�        ���
***************************************************************************/
void CameraCalibration::ClearImageCache()
{
	ClearImageCache(LEFT_CAMERA_IMAGE);
	ClearImageCache(RIGHT_CAMERA_IMAGE);
}

/***************************************************************************
* �������ƣ�   FindBoardCorner
* ժ����Ҫ��
* ȫ��Ӱ�죺   private
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/15      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::FindBoardCorner()
{
	// ˵���Ѿ�����ͼƬ
	if (OPEN_IMAGE != m_CalibrationStep && CALIBRATION != m_CalibrationStep)
		return FALSE;

	int iSuccessNum = 0;                                // �ҵ��ǵ�������Ƿ���ȷ
	int iLFindCornorNum = 0, iRFindCornorNum = 0;       // �ҵ��ǵ������
	const int TOTAL_CORNOR_NUM = m_stImageInfo->iImageWidth*m_stImageInfo->iImageHeight; //�ܽǵ���
	CvPoint2D32f* cvLImagePointBuf
		= new CvPoint2D32f[TOTAL_CORNOR_NUM]; // ͼ�������Ļ���
	CvPoint2D32f* cvRImagePointBuf
		= new CvPoint2D32f[TOTAL_CORNOR_NUM]; // ͼ�������Ļ���

	for (int i = 0; i != m_stImageInfo->iImgCounts; i++)
	{
		// ��������Ľǵ����ľ����Ľǵ�
		if (FindBoardCorner(m_vcLImage.at(i), cvLImagePointBuf, iLFindCornorNum)
			&& FindBoardCorner(m_vcRImage.at(i), cvRImagePointBuf, iRFindCornorNum))
		{
			// ���Ƿ����еĽǵ㶼���ҵ�
			if (TOTAL_CORNOR_NUM == iLFindCornorNum && TOTAL_CORNOR_NUM == iRFindCornorNum)
			{
				int iStep = iSuccessNum*TOTAL_CORNOR_NUM;
				for (int j = 0; j < TOTAL_CORNOR_NUM; j++, iStep++)
				{
					// ����Ŀ������и�ֵ
					CV_MAT_ELEM(*(m_stCalibrationResult->cvMatLeftPoint),
						float, iStep, POINTS_X) = cvLImagePointBuf[j].x;
					CV_MAT_ELEM(*(m_stCalibrationResult->cvMatLeftPoint),
						float, iStep, POINTS_Y) = cvLImagePointBuf[j].y;

					// ����Ŀ������и�ֵ
					CV_MAT_ELEM(*(m_stCalibrationResult->cvMatRightPoint),
						float, iStep, POINTS_X) = cvRImagePointBuf[j].x;
					CV_MAT_ELEM(*(m_stCalibrationResult->cvMatRightPoint),
						float, iStep, POINTS_Y) = cvRImagePointBuf[j].y;

					// д��������������
					CV_MAT_ELEM(*(m_stCalibrationResult->cvMatObjectPoint),
						float, iStep, POINTS_X) = (float)(j / m_stImageInfo->iImageWidth*CHESS_BOARD_WIDTH);
					CV_MAT_ELEM(*(m_stCalibrationResult->cvMatObjectPoint),
						float, iStep, POINTS_Y) = (float)(j % m_stImageInfo->iImageWidth*CHESS_BOARD_WIDTH);
					CV_MAT_ELEM(*(m_stCalibrationResult->cvMatObjectPoint),
						float, iStep, POINTS_Z) = 0.0f;
				}

				// �궨�Ľǵ���
				CV_MAT_ELEM(*(m_stCalibrationResult->cvMatLeftPointCount), int, iSuccessNum, POINTS_X) = TOTAL_CORNOR_NUM;
				CV_MAT_ELEM(*(m_stCalibrationResult->cvMatRightPointCount), int, iSuccessNum, POINTS_X) = TOTAL_CORNOR_NUM;

				// ˵����ͼƬ�ϵı�ǵ㶼�Ѿ�ȫ���ҵ�
				iSuccessNum++;
			}
			else
				break;
		}
		else
			break;

	}

	// �ѿ��ٵ��ڴ滹��ȥ
	delete[] cvLImagePointBuf;
	delete[] cvRImagePointBuf;

	// ˵�����еĽǵ㶼���ҳɹ���
	if (iSuccessNum == m_stImageInfo->iImgCounts){
		m_CalibrationStep = FIND_CORNER;  //˵��������ҽǵ�Ĺ���
		return TRUE;
	}
	else{
		// ���һ�½��������Ľ��û������
		NewCalibrationResult();
		return FALSE;
	}
}


/***************************************************************************
* �������ƣ�   FindBoardCorner
* ժ����Ҫ��
* ȫ��Ӱ�죺   private
* �Ρ�������   [in]  IplImage * pImg
* �Ρ�������   [out] CvPoint2D32f * pImgCornorPoint
* �Ρ�������   [out] int iCornorPoints
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/15      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::FindBoardCorner(IplImage* pImg, CvPoint2D32f* pImgCornorPoint, int& iCornorPoints)
{
	if (NULL == pImg || NULL == pImgCornorPoint)
		return FALSE;

	int iFoundCornor = 0;
	iFoundCornor = cvFindChessboardCorners(pImg, cvSize((m_stImageInfo->iImageWidth), m_stImageInfo->iImageHeight),
		pImgCornorPoint, &iCornorPoints,
		CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);

	// ����ʧ��
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
	else{// ���ҳɹ�
		// �ȸ���ͼƬ
		IplImage* pGrayImg = cvCloneImage(pImg);

		// ����������
		cvFindCornerSubPix(pGrayImg, pImgCornorPoint, iCornorPoints, WIN_CORNOR, ZERO_CORNOR,
			cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, TERMCRITERIA_MAX_ITER, TERMCRITERIA_EPSILON));

		if (m_bShowImage)
		{
			// ��ͼƬ�ϻ��ƽǵ�
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
* �������ƣ�   ComputerParameter
* ժ����Ҫ��
* ȫ��Ӱ�죺   protected
* �Ρ�������   [in]  ST_CALIBRATION_RESULT & stResult
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/16      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::ComputerParameter(ST_CALIBRATION_RESULT& stResult)
{
	// ���û�������Ѱ�ǵ�Ĺ�����ɾ����
	if (FIND_CORNER != m_CalibrationStep || NULL == m_cvSize)
		return FALSE;

	// ����Ŀ����궨
	cvCalibrateCamera2(m_stCalibrationResult->cvMatObjectPoint,
		m_stCalibrationResult->cvMatLeftPoint, m_stCalibrationResult->cvMatLeftPointCount, *m_cvSize,
		m_stCalibrationResult->cvMatLintrinsicMatrix, m_stCalibrationResult->cvMatLdistortionCoeff,
		m_stCalibrationResult->cvMatLRotationVct, m_stCalibrationResult->cvMatLTranslaVct, 0);///��Ŀ����궨

	// ����Ŀ����궨
	cvCalibrateCamera2(m_stCalibrationResult->cvMatObjectPoint,
		m_stCalibrationResult->cvMatRightPoint, m_stCalibrationResult->cvMatRightPointCount, *m_cvSize,
		m_stCalibrationResult->cvMatRintrinsicMatrix, m_stCalibrationResult->cvMatRdistortionCoeff,
		m_stCalibrationResult->cvMatRRotationVct, m_stCalibrationResult->cvMatRTranslaVct, 0);///��Ŀ����궨

	// ��˫Ŀ���б궨
	cvStereoCalibrate(m_stCalibrationResult->cvMatObjectPoint, m_stCalibrationResult->cvMatLeftPoint,
		m_stCalibrationResult->cvMatRightPoint, m_stCalibrationResult->cvMatLeftPointCount, m_stCalibrationResult->cvMatLintrinsicMatrix,
		m_stCalibrationResult->cvMatLdistortionCoeff, m_stCalibrationResult->cvMatRintrinsicMatrix,
		m_stCalibrationResult->cvMatRdistortionCoeff, *m_cvSize,
		m_stCalibrationResult->cvMatRotationStereo, m_stCalibrationResult->cvMatTranslationStereo,
		m_stCalibrationResult->cvEssentialValueMat, m_stCalibrationResult->cvMatFundamentalMat,
		cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, CALIBRATION_MAX_ITER, CALIBRATION_EPSILON),
		CV_CALIB_USE_INTRINSIC_GUESS);///����궨

	// ˵���Ѿ���ɼ������
	m_CalibrationStep = COMPUTER_PARAMETER;

	return TRUE;
}

/***************************************************************************
* �������ƣ�   StartCalibration
* ժ����Ҫ��
* ȫ��Ӱ�죺   protected
* �Ρ�������   [in]  ST_CALIBRATION_RESULT & stResult
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/16      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::RectifyImg(const char* pcFileName)
{
	if (COMPUTER_PARAMETER != m_CalibrationStep)
		return FALSE;

	if (NULL == pcFileName)
		return TRUE;

	CvMat * pcvProjectionMatLeft = cvCreateMat(PROJECTION_MAT_ROWS, PROJECTION_MAT_COLS, CV_64F);  //��ͶӰ����
	CvMat * pcvProjectionMatRight = cvCreateMat(PROJECTION_MAT_ROWS, PROJECTION_MAT_COLS, CV_64F); //��ͶӰ����	

	CvMat * pcvLeftParallelMat = cvCreateMat(PARALLEL_MAT_ROWS, PARALLEL_MAT_COLS, CV_64F);    //��Ŀ����ж�׼����
	CvMat * pcvRightParallelMat = cvCreateMat(PARALLEL_MAT_ROWS, PARALLEL_MAT_COLS, CV_64F);   //��Ŀ����ж�׼����

	CvMat * pcvReprojectionMat = cvCreateMat(REPROJECTION_MAT_ROWS, REPROJECTION_MAT_COLS, CV_64F);  //��ͶӰ����

	CvMat* pcvMapXLeft = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F);  //��Ŀ���x����У��ӳ�����mapx1
	CvMat* pcvMapYLeft = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F);  //��Ŀ���y����У��ӳ�����mapy1
	CvMat* pcvMapXRight = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F); // ��Ŀ���x����У��ӳ�����mapx1
	CvMat* pcvMapYRight = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F); // ��Ŀ���y����У��ӳ�����mapy1 correspondences

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

	char*  pcFileNameBuf = new char[BUFFER_SIZE]; //�½����ֻ���
	IplImage* pRectifyImg = cvCreateImage(*m_cvSize, IPL_DEPTH_8U, PIXEL_SINGLE_CHANNELS);     // У��֮������ͼ��
	CvMat* pcvMatPairImg = cvCreateMat(m_cvSize->height, m_cvSize->width * 2, CV_8UC3);        // ��Ҫ����ͼƬ
	CvMat cvSubMat;

	for (int i = 0; i < m_stImageInfo->iImgCounts; i++)
	{
		// ����������ͼƬ��ֹpRectifyImgָ����
		cvRemap(m_vcLImage.at(i), pRectifyImg, pcvMapXLeft, pcvMapYLeft);

		// ��ʾ����
		if (m_bShowImage){
			cvNamedWindow("RectifyLeftImage");
			cvShowImage("RectifyLeftImage", pRectifyImg);
		}

		// ������ͼ
		cvGetCols(pcvMatPairImg, &cvSubMat, START_ROWS, m_cvSize->width);
		cvCvtColor(pRectifyImg, &cvSubMat, CV_GRAY2BGR);

		// ����ͼƬ
		sprintf_s(pcFileNameBuf, BUFFER_SIZE, "");                                          // �������
		sprintf_s(pcFileNameBuf, BUFFER_SIZE, "%s_LeftCremeraImg_%d.bmp", pcFileName, i);   // д������
		cvSaveImage(pcFileNameBuf, pRectifyImg);                                            // ����ͼƬ

		// ��������ͼƬ
		cvRemap(m_vcRImage.at(i), pRectifyImg, pcvMapXRight, pcvMapYRight);

		// ��ʾ����
		if (m_bShowImage){
			cvNamedWindow("RectifyRightImage");
			cvShowImage("RectifyRightImage", pRectifyImg);
		}

		// ƴ����ͼ
		cvGetCols(pcvMatPairImg, &cvSubMat, m_cvSize->width, m_cvSize->width * 2);
		cvCvtColor(pRectifyImg, &cvSubMat, CV_GRAY2BGR);

		// ����ͼƬ
		sprintf_s(pcFileNameBuf, BUFFER_SIZE, "");
		sprintf_s(pcFileNameBuf, BUFFER_SIZE, "%s_RightCremeraImg_%d.bmp", pcFileName, i);
		cvSaveImage(pcFileNameBuf, pRectifyImg);

		/// �����������Ա�鿴����ͼ���Ƿ��ж�׼
		for (int j = 0; j < m_cvSize->height; j += INTERVAL_PIXEL)
			cvLine(pcvMatPairImg, cvPoint(START_ROWS, j), cvPoint(m_cvSize->width * 2, j), CV_RGB(0, 255, 0));

		sprintf_s(pcFileNameBuf, BUFFER_SIZE, "");
		sprintf_s(pcFileNameBuf, BUFFER_SIZE, "%s_MergeLeftAndRightImg_%d.bmp", pcFileName, i);
		cvSaveImage(pcFileNameBuf, pcvMatPairImg);
	}

	// �ͷ���Щ�ռ�
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

	// ˵���Ѿ�����˱궨
	m_CalibrationStep = CALIBRATION;

	return TRUE;
}

/***************************************************************************
* �������ƣ�   GetImgSize
* ժ����Ҫ��
* ȫ��Ӱ�죺   public
* ����ֵ����   CvSize
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/16      ���ǲ�        ���
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
* �������ƣ�   ClearImageCache
* ժ����Ҫ��
* ȫ��Ӱ�죺   protected
* �Ρ�������   [in]  const int flags
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/17      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::ClearImageCache(const int flags)
{
	if ((LEFT_CAMERA_IMAGE != flags) ||
		(RIGHT_CAMERA_IMAGE != flags))
		return FALSE;

	if (LEFT_CAMERA_IMAGE == flags)// ˵���л���
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

	// ��״̬���ú�
	if ((m_CalibrationStep != UNSTART) || (m_CalibrationStep != CALIBRATION_ERROR))
		m_CalibrationStep = SET_INFO;

	return TRUE;
}

/***************************************************************************
* �������ƣ�   CheckImgSize
* ժ����Ҫ��
* ȫ��Ӱ�죺   private
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/17      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::CheckImgSize(IplImage* pImg)
{
	// �����һû���½�ָ���Ҿ�
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
* �������ƣ�   RectifyImg
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  IplImage * pImg
* �Ρ�������   [in]  int flags
* ����ֵ����   BOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/03/21      ���ǲ�        ���
***************************************************************************/
BOOL CameraCalibration::RectifyImg(IplImage* pImg, int flags)
{
	if (nullptr == pImg || nullptr == m_stCalibrationResult)
		return FALSE;

	CvMat* pcvProjectionMatLeft = cvCreateMat(PROJECTION_MAT_ROWS, PROJECTION_MAT_COLS, CV_64F);  //��ͶӰ����
	CvMat* pcvProjectionMatRight = cvCreateMat(PROJECTION_MAT_ROWS, PROJECTION_MAT_COLS, CV_64F); //��ͶӰ����	
	CvMat* pcvLeftParallelMat = cvCreateMat(PARALLEL_MAT_ROWS, PARALLEL_MAT_COLS, CV_64F);    //��Ŀ����ж�׼����
	CvMat* pcvRightParallelMat = cvCreateMat(PARALLEL_MAT_ROWS, PARALLEL_MAT_COLS, CV_64F);   //��Ŀ����ж�׼����
	CvMat* pcvReprojectionMat = cvCreateMat(REPROJECTION_MAT_ROWS, REPROJECTION_MAT_COLS, CV_64F);  //��ͶӰ����
	CvMat* pcvMapXLeft = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F);  //��Ŀ���x����У��ӳ�����mapx1
	CvMat* pcvMapYLeft = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F);  //��Ŀ���y����У��ӳ�����mapy1
	CvMat* pcvMapXRight = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F); // ��Ŀ���x����У��ӳ�����mapx1
	CvMat* pcvMapYRight = cvCreateMat(m_cvSize->height, m_cvSize->width, CV_32F); // ��Ŀ���y����У��ӳ�����mapy1 correspondences

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

	// ����������ͼƬ��ֹpRectifyImgָ����
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