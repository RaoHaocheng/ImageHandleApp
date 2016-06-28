// һЩ��Ҫʹ�õ����ݽṹ
#ifndef DEFINE_H  // ��ֹͷ�ļ��ظ�����
#define DEFINE_H

// ������׼���е�����
#include <vector>    // ����
#include <iostream>  // ��׼�������

// OpenCV�ṩ�Ŀ�
#include <highgui.h>
#include <cv.h>

// windows������
#include <windows.h>

// ������뻷��
#ifdef  _DLL
#define  DLLS_PORT   __declspec(dllexport)      // ��̬���ӿ⵼��
#else
#define  DLLS_PORT
#endif

// ���峣�õĺ�
#define  PI                  3.141592			// Բ���ʵĶ���
#define  CHESSNUM            2					// �궨�����
#define  LEFT_CAMERA_IMAGE   1                  // �������ĳ�����ͼƬ
#define  RIGHT_CAMERA_IMAGE  2                  // �ֱ�����ĳ�����ͼƬ
#define  TIME_TEST                              // ʱ����Ժ꣬����ȥ��
//#define  STEP_TIME_TEST                         // ����ÿ��ʱ�����
#define  _TEST_						            // ���Ժ�

// ���峣��
const int GRAY_SCALE_VALUE = 256;			     // �Ҷ�ֵ�ķ�Χ,0~255
const int BUFFER_SIZE = 255;                     // �ַ��Ļ��泤��
const int CAMREA_NUM = 2;                        // ���������

const int SYSTEM_XYZ = 3;                        // XYZ������
const int SYSTEM_XY = 2;                         // XY������
const int SYSTEM_X = 1;                          // X������

const int INTRINSIC_MATRIX_ROWS = 3;             // �ڲξ���������
const int INTRINSIC_MATRIX_COLS = 3;             // �ڲξ���������

const int DISTORTION_COEFFS_ROWS = 5;            // �������������
const int DISTORTION_COEFFS_COLS = 1;            // �������������

const int ROTATION_VCT = 3;                      // ��ת�����Ĵ�С
const int TRANSLA_VCT = 3;                       // ƽ�������Ĵ�С

const int OBJECT_VET_COLS = 3;                   // ��������ϵ��������

const CvSize WIN_CORNOR = cvSize(11, 11);        // ����������
const CvSize ZERO_CORNOR = cvSize(-1, -1);       // û������

const int TERMCRITERIA_MAX_ITER = 30;            // ��Ѱ�ǵ��㷨������������
const double TERMCRITERIA_EPSILON = 0.1;         // ��Ѱ�ǵ��㷨�����ľ���

const int CALIBRATION_MAX_ITER = 100;            // �궨ʱ�������������
const double CALIBRATION_EPSILON = 1e-5;         // �궨ʱ��ľ���Ҫ��

const int POINTS_X = 0;                          // x����
const int POINTS_Y = 1;                          // y����
const int POINTS_Z = 2;                          // z����

const double CHESS_BOARD_WIDTH = 24.7;           // ���̸���С��λ�Ŀ��

const int ESSENTIALVALUE_MAT = 3;          // ��֤����3*3
const int FUNDAMENTAL_MAT = 3;             // ��������3*3

const int LEAST_IMAGE_NUM = 10;            // ���Ҫ�궨�ɹ�������Ҫ10����Ƭ

const int PROJECTION_MAT_ROWS = 3;         // ͶӰ����Ĵ�С
const int PROJECTION_MAT_COLS = 4;         // ͶӰ����Ĵ�С

const int PARALLEL_MAT_ROWS = 3;		   // ƽ�о���Ĵ�С
const int PARALLEL_MAT_COLS = 3;           // ƽ�о���Ĵ�С

const int REPROJECTION_MAT_ROWS = 4;       // ��ͶӰ����
const int REPROJECTION_MAT_COLS = 4;       // ��ͶӰ����

const int PIXEL_SINGLE_CHANNELS = 1;       // ���ص�ͨ��

const int START_ROWS = 0;                  // ��ʼ�ĺ�����
const int STRAT_COLS = 0;                  // ��ʼ��������

const int INTERVAL_PIXEL = 16;             // ���16�����ص�
const int PARAMETER_XYZW_NUM = 4;          // 4������
const int PARAMETER_XYZ_NUM = 3;           // 3������

const int CAL_PONIT_NUM = 72;              // ÿ��5��

const double GAUSSIAN_MEAN = 0.0;          // ��˹�ֲ���ƽ��ֵΪ0;
const double GAUSSIAN_VARIANCE = 1.0;      // ��˹�ֲ��ķ���Ϊ1;

const UINT CONVERT_SCALE = 255;            // ת������

// ���õ����ݽṹ
// ����
typedef struct CIRCLE_POINT
{
	double x;
	double y;
	double size;
	double distance;
	float dex;
	float dey;
}ST_CIRCLE_POINT, *PST_CIRCLE_POINT;
typedef std::vector<std::vector<ST_CIRCLE_POINT>> VEC_POINTS;

// ���̸�ԭ��
typedef struct ORGIN_POINTS
{
	double x;
	double y;
	double size;
}ST_ORGIN_POINTST, *PST_ORGIN_POINTS;

// ��������
typedef struct OBJECT_POINTS
{
	double x;
	double y;
	double z;
}ST_OBJECT_POINTS, *PST_OBJECT_POINTS;

// Բ����Ϣ
typedef struct CENTER
{
	cv::Point2d location;
	double radius;
	double confidence;
}ST_CENTER, *PST_CENTER;

#endif