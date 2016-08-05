//����Բ�α�ǵ���࣬��Ҫ�Ƿ�װ��OPENCV�еļ�����
#ifndef FINDCIRCULARMARKER_H
#define FINDCIRCULARMARKER_H

// ���ݽṹ������Լ���OpenCV���б���ʹ�õ�����
#include "define.h"                           // һЩ�����ĺ���
#include "BaseImageHandleFunction.h"          // һЩ�����ĺ���

// ������Բ�α�ǵ�ķ���,���ڱ궨�Ļ�������Բ������Ҳ��������궨������ط��Ժ���Ҫ�޸�
class DLLS_PORT FindCircularMarker
{
public:
	FindCircularMarker();
	~FindCircularMarker();
	//void SetCalibrationParameter(ST_CALIBRATION_RESULT& stResult);
	//BOOL Computer3DPos(ST_OBJECT_POINTS& stPoints, IplImage* pLeftImg, IplImage* pRightImg);
	void SetFileName(const char* fileName);                                           // �����ļ���

	// ��Բ
	BOOL FindCircleCenter(IplImage* pImg,
		cv::SimpleBlobDetector::Params params, 
		std::vector<cv::KeyPoint>& vct_ponits, BOOL bShow = FALSE);                                        // OpenCv�ṩ��Բ
	void CalCircleCentrePoint(IplImage* pImg, std::vector<cv::KeyPoint>& vct_ponits, int ipointscounts);   // ����Բ�����������
	void FindCircleBySamplingTriangles(IplImage* pImg, std::vector<ST_CENTER>& vct_ponits,
		BOOL bShow = FALSE);                                                                               // �������ṩ����Բ�ķ���
	void FindCircleBySamplingTriangles(const cv::Mat& cvMatimage, std::vector<ST_CENTER>& vct_ponits,
		BOOL bShow = FALSE);                                                                               // ��������Բ�ķ���
	
	void FindCircleBySamplingTrianglesImproved(IplImage* pImg, std::vector<ST_CENTER>& vct_ponits,
		BOOL bShow = FALSE);                                                                               // �������ṩ����Բ�ķ���

	void FindCircleBySamplingTrianglesImproved(const cv::Mat& cvMatimage, std::vector<ST_CENTER>& vct_ponits,
		BOOL bShow = FALSE);                                                                               // ��������Բ�ķ���
	BOOL FindCircleByThreePoint(const std::vector<cv::Point> counter,
		ST_CENTER& center, const double dbPrecision = 0.1,  const UINT iMinSuccessNum = 1, const UINT iMaxTestNum = 10);         // ͨ������ȷ��һ��Բ
	BOOL FindCircleByThreePoint(const std::vector<cv::Point> counter,
		std::vector<ST_CENTER>& centers, const double dbPrecision = 0.1, const double dbMinConfidence =0.3,
				const UINT iMinSuccessNum = 1, const UINT iMaxTestNum = 1000);                                                     // ͨ������ȷ��һ��Բ

	BOOL FindCircleByThreePoint(IplImage*pImg ,std::vector<ST_CENTER>& vct_ponits, 
		BOOL bShow = FALSE ,const double dbPrecision = 0.1, const UINT iMinSuccessNum = 1, const UINT iMaxTestNum = 10);          // ͨ������ȷ��һ��Բ
	BOOL FindCircleByThreePoint(const cv::Mat& cvMatimage, std::vector<ST_CENTER>& vct_ponits,
		BOOL bShow = FALSE, const double dbPrecision = 0.1, const UINT iMinSuccessNum = 1, const UINT iMaxTestNum = 10);          // ͨ������ȷ��һ��Բ
	void FindCircleByCICImproved(const cv::Mat& cvMatimage, cv::SimpleBlobDetector::Params params,
		std::vector<ST_CENTER>& vct_ponits, BOOL bShow = FALSE);			// ���ǵķ����Ľ���
	void DrawCircle(IplImage* src, cv::Mat dst, std::vector<ST_CENTER>& vct_ponits, const char* saveFileName = NULL);

	void FindCircleByWaterThed(const cv::Mat& cvMatimage, cv::SimpleBlobDetector::Params params,
		std::vector<ST_CENTER>& vct_ponits, BOOL bShow = FALSE);			// ���ǵķ����Ľ���

protected:
	static void Trackbar_callback(int Pos, void* Usrdata);                                                 // �ص�����,������ʾ������
	BOOL FindCircle(IplImage* pImg, cv::SimpleBlobDetector::Params params,
		std::vector<ST_CENTER>& vct_ponits, BOOL bShow = FALSE);                                           // �޸ĺ������Բ����
	BOOL FindCircle(cv::Mat& cvMatimage, cv::SimpleBlobDetector::Params params,
		std::vector<ST_CENTER>& vct_ponits, BOOL bShow = FALSE);                                           // ���غ�����Ϊ�˽��Mat�ĵ��õ�����
	BOOL FindCircleImproved(cv::Mat& cvMatimage, cv::SimpleBlobDetector::Params params,
		std::vector<ST_CENTER>& vct_ponits, BOOL bShow = FALSE);                                           // ���غ�����Ϊ�˽��Mat�ĵ��õ�����

private:
	BOOL CheckReliability(const std::vector<double> src, 
		const double dbVarPrecision = 0.3, const double dbRaPrecision = 0.7);

private:
	static int m_siGobalThreHold;
	char* m_fileName;
};
#endif