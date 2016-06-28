// 一些需要使用的数据结构
#ifndef DEFINE_H  // 防止头文件重复定义
#define DEFINE_H

// 包含标准库中的内容
#include <vector>    // 容器
#include <iostream>  // 标准输入输出

// OpenCV提供的库
#include <highgui.h>
#include <cv.h>

// windows基本库
#include <windows.h>

// 定义编译环境
#ifdef  _DLL
#define  DLLS_PORT   __declspec(dllexport)      // 动态链接库导出
#else
#define  DLLS_PORT
#endif

// 定义常用的宏
#define  PI                  3.141592			// 圆周率的定义
#define  CHESSNUM            2					// 标定板块数
#define  LEFT_CAMERA_IMAGE   1                  // 左边相机拍出来的图片
#define  RIGHT_CAMERA_IMAGE  2                  // 又边相机拍出来的图片
#define  TIME_TEST                              // 时间测试宏，可以去除
//#define  STEP_TIME_TEST                         // 用于每部时间测试
#define  _TEST_						            // 测试宏

// 定义常量
const int GRAY_SCALE_VALUE = 256;			     // 灰度值的范围,0~255
const int BUFFER_SIZE = 255;                     // 字符的缓存长度
const int CAMREA_NUM = 2;                        // 摄像机个数

const int SYSTEM_XYZ = 3;                        // XYZ三方向
const int SYSTEM_XY = 2;                         // XY两方向
const int SYSTEM_X = 1;                          // X单方想

const int INTRINSIC_MATRIX_ROWS = 3;             // 内参矩阵行向量
const int INTRINSIC_MATRIX_COLS = 3;             // 内参矩阵列向量

const int DISTORTION_COEFFS_ROWS = 5;            // 畸变矩阵行向量
const int DISTORTION_COEFFS_COLS = 1;            // 畸变矩阵列向量

const int ROTATION_VCT = 3;                      // 旋转向量的大小
const int TRANSLA_VCT = 3;                       // 平移向量的大小

const int OBJECT_VET_COLS = 3;                   // 物理坐标系的列向量

const CvSize WIN_CORNOR = cvSize(11, 11);        // 搜索的区域
const CvSize ZERO_CORNOR = cvSize(-1, -1);       // 没有死角

const int TERMCRITERIA_MAX_ITER = 30;            // 找寻角点算法迭代的最大次数
const double TERMCRITERIA_EPSILON = 0.1;         // 找寻角点算法迭代的精度

const int CALIBRATION_MAX_ITER = 100;            // 标定时候的最大迭代次数
const double CALIBRATION_EPSILON = 1e-5;         // 标定时候的精度要求

const int POINTS_X = 0;                          // x方向
const int POINTS_Y = 1;                          // y方向
const int POINTS_Z = 2;                          // z方向

const double CHESS_BOARD_WIDTH = 24.7;           // 棋盘格最小单位的宽度

const int ESSENTIALVALUE_MAT = 3;          // 本证矩阵3*3
const int FUNDAMENTAL_MAT = 3;             // 基础矩阵3*3

const int LEAST_IMAGE_NUM = 10;            // 如果要标定成功至少需要10张照片

const int PROJECTION_MAT_ROWS = 3;         // 投影矩阵的大小
const int PROJECTION_MAT_COLS = 4;         // 投影矩阵的大小

const int PARALLEL_MAT_ROWS = 3;		   // 平行矩阵的大小
const int PARALLEL_MAT_COLS = 3;           // 平行矩阵的大小

const int REPROJECTION_MAT_ROWS = 4;       // 重投影矩阵
const int REPROJECTION_MAT_COLS = 4;       // 重投影矩阵

const int PIXEL_SINGLE_CHANNELS = 1;       // 像素单通道

const int START_ROWS = 0;                  // 起始的横坐标
const int STRAT_COLS = 0;                  // 起始的纵坐标

const int INTERVAL_PIXEL = 16;             // 间隔16个像素点
const int PARAMETER_XYZW_NUM = 4;          // 4个参数
const int PARAMETER_XYZ_NUM = 3;           // 3个参数

const int CAL_PONIT_NUM = 72;              // 每隔5度

const double GAUSSIAN_MEAN = 0.0;          // 高斯分布的平均值为0;
const double GAUSSIAN_VARIANCE = 1.0;      // 高斯分布的方差为1;

const UINT CONVERT_SCALE = 255;            // 转换比例

// 常用的数据结构
// 点阵
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

// 棋盘格原点
typedef struct ORGIN_POINTS
{
	double x;
	double y;
	double size;
}ST_ORGIN_POINTST, *PST_ORGIN_POINTS;

// 物理坐标
typedef struct OBJECT_POINTS
{
	double x;
	double y;
	double z;
}ST_OBJECT_POINTS, *PST_OBJECT_POINTS;

// 圆的信息
typedef struct CENTER
{
	cv::Point2d location;
	double radius;
	double confidence;
}ST_CENTER, *PST_CENTER;

#endif