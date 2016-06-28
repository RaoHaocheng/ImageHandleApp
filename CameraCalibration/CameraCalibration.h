// �궨��
#ifndef CAMERACALIBRATION_H
#define CAMERACALIBRATION_H

// ���ݽṹ������Լ���OpenCV���б���ʹ�õ�����
#include "define.h"

typedef struct CALIBRATION_IMAGE_INFO   //ͼƬ��Ϣ
{
	int iImageWidth;      // �궨��Ŀ��
	int iImageHeight;     // �궨��ĸ߶�
	int iImgCounts;       // �궨ͼ�����
}ST_CALIBRATION_IMAGE_INFO, *PST_CALIBRATION_IMAGE_INFO;

typedef struct CALIBRATION_RESULT   //�궨���
{
	CvMat* cvMatLeftPoint;          // ��Ŀ����ǵ�����
	CvMat* cvMatRightPoint;         // ��Ŀ����ǵ�����
	CvMat* cvMatObjectPoint;		// ��������
	CvMat* cvMatLeftPointCount;     // ��Ŀ���̸��������
	CvMat* cvMatRightPointCount;    // ��Ŀ������̸��������
	CvMat* cvMatLintrinsicMatrix;   // ��Ŀ����ڲ���
	CvMat* cvMatRintrinsicMatrix;   // ��Ŀ����ڲ���
	CvMat* cvMatLdistortionCoeff;   // ��Ŀ�������ϵ��
	CvMat* cvMatRdistortionCoeff;   // ��Ŀ�������ϵ��
	CvMat* cvMatLRotationVct;       // ���̸������Ŀ�����ת����
	CvMat* cvMatLTranslaVct;        // ���̸������Ŀ���ƽ������
	CvMat* cvMatRRotationVct;       // ���̸������Ŀ�����ת����
	CvMat* cvMatRTranslaVct;        // ���̸������Ŀ���ƽ������
	CvMat* cvMatRotationStereo;     // ������ת����
	CvMat* cvMatTranslationStereo;  // ����ƽ�ƾ���
	CvMat* cvMatFundamentalMat;     // ��������
	CvMat* cvEssentialValueMat;     // ��֤����
}ST_CALIBRATION_RESULT, *PST_CALIBRATION_RESULT;

typedef struct RECTIFY_IMG_PARAMETER
{
	CvMat* pcvProjectionMatLeft;       // ��ͶӰ����
	CvMat* pcvProjectionMatRight;      // ��ͶӰ����
	CvMat* pcvLeftParallelMat;         // ��Ŀ����ж�׼����
	CvMat* pcvRightParallelMat;        // ��Ŀ����ж�׼����
	CvMat* pcvReprojectionMat;         // ��ͶӰ����
	CvMat* pcvMapXLeft;                // ��Ŀ���x����У��ӳ�����mapx1
	CvMat* pcvMapYLeft;                // ��Ŀ���y����У��ӳ�����mapy1
	CvMat* pcvMapXRight;               // ��Ŀ���x����У��ӳ�����mapx1
	CvMat* pcvMapYRight;               // ��Ŀ���y����У��ӳ�����mapy1 
}ST_RECTIFY_IMG_PARAMETER, *PST_RECTIFY_IMG_PARAMETER;

// ʹ��ģ����ĵ�������
template  class DLLS_PORT std::vector<IplImage*>;

class DLLS_PORT CameraCalibration
{
public:
	CameraCalibration();               // ���캯��
	virtual ~CameraCalibration();      // ��������

	// Ϊ�˷�ֹ�û���Խ������ú���
	enum CalibrationStep      // �궨�Ĳ���
	{
		UNSTART,              // δ��ʼ
		SET_INFO,             // ����ͼƬ��Ϣ
		OPEN_IMAGE,           // ��ͼƬ
		FIND_CORNER,          // ��Ѱ����
		COMPUTER_PARAMETER,   // �������
		CALIBRATION,          // �궨
		CALIBRATION_ERROR     // ����
	};

	void SetCalibrationImageInfo(const ST_CALIBRATION_IMAGE_INFO& stImageInfo);    // ��������Ļ�����Ϣ
	BOOL OpenCalibrationImage(const char* pcFileName, int flags);                  // ��ÿ����Ҫ�궨��ͼƬ
	BOOL Calibration(ST_CALIBRATION_RESULT& stResult);                             // �궨���
	BOOL Calibration(ST_CALIBRATION_RESULT& stResult, const char* pcFileName);     // �궨����Լ�����У�����ͼƬ
	CvSize GetImgSize() const;                                                     // �õ�ͼƬ�Ĵ�С
	inline CalibrationStep CurrentState() const { return m_CalibrationStep; }      // ���ص�ǰ��Ҫִ�еĲ���
	inline void SetShowImage(const bool bShowImage) { m_bShowImage = bShowImage; } // �����Ƿ���ʾͼƬ
	inline bool ShowImage() const { return m_bShowImage; }		                   // ��ǰ�Ƿ���ʾͼƬ
	BOOL ClearImageCache(const int flags);                                         // �����Ƭ
	inline std::vector<IplImage*> CameraLeftImgCache() { return m_vcLImage; }      // ����ͼƬ����
	inline std::vector<IplImage*> CameraRightImgCache() { return m_vcRImage; }     // ����ͼƬ����
	BOOL RectifyImg(IplImage* pImg, int flags);                                    // У��ͼƬ
	BOOL Test();															       // ���ڲ���

protected:
	BOOL FindBoardCorner(IplImage* pImg, 
		CvPoint2D32f* pImgCornorPoint, int& iCornorPoints);                      // ���ұ궨��Ľǵ�
	BOOL ComputerParameter(ST_CALIBRATION_RESULT& stResult);                     // �����������
	BOOL RectifyImg(const char* pcFileName);                                     // У���궨ͼƬ
	BOOL ComputerRectifyParameter(ST_RECTIFY_IMG_PARAMETER& stRectifyParameter,
		ST_CALIBRATION_RESULT& stResult);                                        // ����У������
	
	BOOL NewCalibrationResult(ST_CALIBRATION_RESULT** stCalibrationResult, 
							  const ST_CALIBRATION_IMAGE_INFO& stImageInfo);     // �̳���ʹ�õ��½�
	BOOL DeleteCalibrationResult(ST_CALIBRATION_RESULT** stCalibrationResult);   // �̳���ʹ�õ�ɾ��
	void ClearImageCache();                                                      // ����ͼƬ˫Ŀ����Ļ���

private:
	BOOL NewCalibrationResult();												  // �½��洢������
	BOOL DeleteCalibrationResult();												  // ɾ���洢������
	BOOL SaveLeftImageCache(const char* pcFileName);                              // ������������
	BOOL SaveRightImageCache(const char* pcFileName);                             // ������������
	BOOL FindBoardCorner();                                                       // ����ͼƬ�����е���������
	BOOL CheckImgSize(IplImage* pImg);                                            // ���ͼƬ���Ƿ���ϴ�С

private:
	CalibrationStep m_CalibrationStep;               // �궨�Ĳ���
	ST_CALIBRATION_IMAGE_INFO* m_stImageInfo;        // ͼƬ��Ϣ
	std::vector<IplImage*>  m_vcLImage;              // ��Ŀ���ͼƬ����
	std::vector<IplImage*>  m_vcRImage;              // ��Ŀ���ͼƬ����

	bool m_bShowImage;                               // ������ʾͼƬ
	ST_CALIBRATION_RESULT* m_stCalibrationResult;    // �궨���
	ST_RECTIFY_IMG_PARAMETER* m_stRecitfyPaprameter; // ��������
	CvSize* m_cvSize;                                // ͼƬ��С
};
#endif