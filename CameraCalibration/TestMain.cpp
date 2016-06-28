#include"highgui.h"
#include"cv.h"
#include <iostream>
using namespace std;
using namespace cv;
#include "define.h"
#include "BaseImageHandleFunction.h"

CvMat* m_pcvObjectPoints[CHESSNUM];			// 角点的物理坐标矩阵
CvMat* m_pcvImgPoints_Left[CHESSNUM];	    // 左目相机M幅图像中所有角点像素坐标矩阵
CvMat* m_pcvImgPoints_Right[CHESSNUM];		// 右目相机M幅图像中所有角点像素坐标矩阵
CvMat* m_pcvPointCounts_Left[CHESSNUM];		// 所有用于标定左目相机的图像的角点个数组成的矩阵
CvMat* m_pcvPointCounts_Right[CHESSNUM];    // 所有用于标定左目相机的图像的角点个数组成的矩阵
//CU8HUMS_CSCTDlg  *PDlg;
// 摄像机内参数
//CvMat* m_pcvNewIntrinsicMat_Left;		// 左摄像机参数矩阵
//CvMat* m_pcvNewIntrinsicMat_Right;	// 右摄像机参数矩阵
//CvMat* m_pcvDistortionCoeffs_Left;	// 左摄像机畸变系数
//CvMat* m_pcvDistortionCoeffs_Right;	// 右摄像机畸变系数

// 摄像机外参数
CvMat* m_pcvRotationVct_Left[CHESSNUM];		// 棋盘绕左目摄像机坐标系坐标轴的旋转矩阵，即learning OpenCv 428页的Rl矩阵
CvMat* m_pcvRotationVct_Right[CHESSNUM];		       // 棋盘绕右目摄像机坐标系坐标轴的旋转矩阵，即learning OpenCv 428页的Rr矩阵
CvMat* m_pcvTranslationVct_Left[CHESSNUM];	       // 棋盘绕左目摄像机坐标系坐标轴的平移矩阵，即learning OpenCv 428页的Tl矩阵
CvMat* m_pcvTranslationVct_Right[CHESSNUM];	      // 棋盘绕右目摄像机坐标系坐标轴的平移矩阵，即learning OpenCv 428页的Tr矩阵
//CvMat* m_pcvRotatonStereo;			// 右目相机坐标系转换到左目相机坐标系所需的旋转矩阵，即learning OpenCv 428页的R矩阵
//CvMat* m_pcvTranslationStereo;		// 右目相机坐标系转换到左目相机坐标系所需的平移矩阵，即learning OpenCv 428页的T矩阵

// 基础矩阵和本征矩阵
CvMat* m_pcvFundamentalMat;			   // 基础矩阵,即learning OpenCv 425页的F矩阵
CvMat* m_pcvEigenValueMat;			   // 本征矩阵，即learning OpenCv 425页的E矩阵

CvMat* m_pcvIntrinsicMat_Left[CHESSNUM];		// 左摄像机参数矩阵
CvMat* m_pcvIntrinsicMat_Right[CHESSNUM];		// 右摄像机参数矩阵
CvMat* m_pcvDistortionCoeffs_Left[CHESSNUM];	// 左摄像机畸变系数
CvMat* m_pcvDistortionCoeffs_Right[CHESSNUM];	// 右摄像机畸变系数

CvMat* m_pcvRotatonStereo;			// 右目相机坐标系转换到左目相机坐标系所需的旋转矩阵，即learning OpenCv 428页的R矩阵
CvMat* m_pcvTranslationStereo;		// 右目相机坐标系转换到左目相机坐标系所需的平移矩阵，即learning OpenCv 428页的T矩阵
CvMat* m_pcvProjectionMat_Left;		// 左目相机投影方程，即3*4的矩阵Pl
CvMat* m_pcvProjectionMat_Right;	// 右目相机投影方程，即3*4的矩阵Pr
CvMat* m_pcvRrojectionMat;	        // 右目相机投影方程，即3*4的矩阵Pr
CvMat* m_pcvAvgErrMat;              // 标定误差矩阵


void thresh_callback(int Pos, void* usrdata);//回调函数1
void thresh_callback2(int Pos, void* usrdata);//回调函数
int GobalThreHold;

void CalCircleCentrePoint(IplImage* &pImage, vector<KeyPoint>&Vct_ponits, int ipointscounts, int iChesscount)
{
	for (int iCirclenCounts = 0; iCirclenCounts < (int)Vct_ponits.size(); iCirclenCounts++)
	{
		/// 粗略的圆心坐标
		int iCentreX = (int)Vct_ponits.at(iCirclenCounts).pt.x;
		int iCentreY = (int)Vct_ponits.at(iCirclenCounts).pt.y;

		///扫描到的边缘点集
		int m_iPos[72][2];

		memset(m_iPos, 0, sizeof(int)* 72 * 2);

		/// 以0°（水平向右）为起点，沿逆时针方向每隔5°在圆周上取一点，并向圆心方向搜索，若某一点的像素值为255时，则为边缘点。
		for (int iAngle = 0; iAngle < 360; iAngle += 5)
		{
			for (int iR = 1; iR < 25; iR++)
			{
				int iX = (int)(iCentreX + (iR * cos((double)iAngle * PI / 180) + (((iAngle >= 0 && iAngle <= 90) || (iAngle >= 270 && iAngle < 360)) ? 0.5 : -0.5)));		// 待判断点的X坐标，式中的0.5为修正值
				int iY = (int)(iCentreY - (iR * sin((double)iAngle * PI / 180) + ((iAngle >= 0 && iAngle < 180) ? 0.5 : -0.5)));		// 待判断点的Y坐标，式中的0.5为修正值

				/// 判断是否超出图像的范围
				if (iX < 0 || iX >= pImage->width || iY < 0 || iY >= pImage->height)
				{
					m_iPos[iAngle / 5][0] = iX - iCentreX;					// 扫描到的边缘点集（iPos）是以（iCentreX，iCentreY）为坐标中心的坐标
					m_iPos[iAngle / 5][1] = iY - iCentreY;
					break;
				}

				uchar ucVal = (uchar)*(pImage->imageData + iY * pImage->widthStep + iX * pImage->nChannels + 0);
				if (255 == ucVal)
				{
					m_iPos[iAngle / 5][0] = iX - iCentreX;					// 扫描到的边缘点集（iPos）是以（iCentreX，iCentreY）为坐标中心的坐标
					m_iPos[iAngle / 5][1] = iY - iCentreY;
					break;
				}
			}
		}

		/// 采用交换排序法对扫描到的边缘点集按其到圆心的距离由近到远进行排序
		for (int i = 0; i < 72; i++)
		{
			double iDistance = sqrt((double)(m_iPos[i][0] * m_iPos[i][0] + m_iPos[i][1]/* -iCentreY*/ * m_iPos[i][1]));
			for (int j = i + 1; j < 72; j++)
			{
				double iTemp = sqrt((double)(m_iPos[j][0] * m_iPos[j][0] + m_iPos[j][1]/* -iCentreY*/ * m_iPos[j][1]));
				if (iTemp - iDistance < 0)
				{
					int x = m_iPos[i][0];
					int y = m_iPos[i][1];
					m_iPos[i][0] = m_iPos[j][0];
					m_iPos[i][1] = m_iPos[j][1];
					m_iPos[j][0] = x;
					m_iPos[j][1] = y;

					iDistance = sqrt((double)(m_iPos[i][0] * m_iPos[i][0] + m_iPos[i][1]/* -iCentreY*/ * m_iPos[i][1]));
				}
			}
		}

		/// 取位于iPos中间的12个点，用于拟合边缘圆。
		int iFitPos[36][2];				// 拟合点数组
		memset(iFitPos, 0, sizeof(int)* 36 * 2);
		for (int i = 0; i < 36; i++)
		{
			iFitPos[i][0] = m_iPos[24 + i][0];
			iFitPos[i][1] = m_iPos[24 + i][1];
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
	}
}

//椭圆拟合
void CalElipseCentrePoint(IplImage* & pImage, vector<KeyPoint>&Vct_ponits, int ipointscounts, int iChesscount)
{
	//存储扫描到的点
	int m_iPos[72][2];
	for (int iCirCleCounts = 0; iCirCleCounts < (int)Vct_ponits.size(); iCirCleCounts++)
	{
		// 圆心坐标
		int iCentreX =(int) Vct_ponits.at(iCirCleCounts).pt.x;
		int iCentreY =(int) Vct_ponits.at(iCirCleCounts).pt.y;

		// 扫描到的边缘点集
		//int iPos[72][2];
		memset(m_iPos, 0, sizeof(int)* 72 * 2);

		// 以0°（水平向右）为起点，沿逆时针方向每隔5°在圆周上取一点，并向圆心方向搜索，若某一点的像素值为255时，则为边缘点。
		for (int iAngle = 0; iAngle < 360; iAngle += 5)
		{
			for (int iR = 1; iR < 25; iR++)
			{
				int iX = (int)(iCentreX + (iR * cos((double)iAngle * PI / 180) + (((iAngle >= 0 && iAngle <= 90) || (iAngle >= 270 && iAngle < 360)) ? 0.5 : -0.5)));		// 待判断点的X坐标，式中的0.5为修正值
				int iY = (int)(iCentreY - (iR * sin((double)iAngle * PI / 180) + ((iAngle >= 0 && iAngle < 180) ? 0.5 : -0.5)));		// 待判断点的Y坐标，式中的0.5为修正值

				// 判断是否超出图像的范围
				if (iX < 0 || iX >= pImage->width || iY < 0 || iY >= pImage->height)
				{
					m_iPos[iAngle / 5][0] = iX - iCentreX;					// 扫描到的边缘点集（iPos）是以（iCentreX，iCentreY）为坐标中心的坐标
					m_iPos[iAngle / 5][1] = iY - iCentreY;
					break;
				}



				uchar ucVal = (uchar)*(pImage->imageData + iY * pImage->widthStep + iX * pImage->nChannels + 0);
				if (255 == ucVal)
				{
					m_iPos[iAngle / 5][0] = iX - iCentreX;					// 扫描到的边缘点集（iPos）是以（iCentreX，iCentreY）为坐标中心的坐标
					m_iPos[iAngle / 5][1] = iY - iCentreY;
					break;
				}
			}
		}

		// 采用交换排序法对扫描到的边缘点集按其到圆心的距离由近到远进行排序
		for (int i = 0; i < 72; i++)
		{
			double iDistance = sqrt((double)(m_iPos[i][0]/* - iCentreX*/ * m_iPos[i][0] + m_iPos[i][1]/* -iCentreY*/ * m_iPos[i][1]));
			for (int j = i + 1; j < 72; j++)
			{
				double iTemp = sqrt((double)(m_iPos[j][0]/* - iCentreX*/ * m_iPos[j][0] + m_iPos[j][1]/* -iCentreY*/ * m_iPos[j][1]));
				if (iTemp - iDistance < 0)
				{
					int x = m_iPos[i][0];
					int y = m_iPos[i][1];
					m_iPos[i][0] = m_iPos[j][0];
					m_iPos[i][1] = m_iPos[j][1];
					m_iPos[j][0] = x;
					m_iPos[j][1] = y;

					iDistance = sqrt((double)(m_iPos[i][0]/* - iCentreX*/ * m_iPos[i][0] + m_iPos[i][1]/* -iCentreY*/ * m_iPos[i][1]));
				}
			}
		}

		// 取位于iPos中间的36个点，用于拟合边缘圆。
		int iFitPos[36][2];				// 拟合点数组
		memset(iFitPos, 0, sizeof(int)* 36 * 2);
		for (int i = 0; i < 36; i++)
		{
			iFitPos[i][0] = m_iPos[24 + i][0];
			iFitPos[i][1] = m_iPos[24 + i][1];
		}

		// 以下是最小二乘拟合求解圆心坐标及半径

		int count = 72;
		CvPoint2D32f *PointArray2D32f;
		CvBox2D32f *box;
		CvPoint center;
		CvSize size;

		IplImage *pImage1 = NULL;

		if (NULL == pImage1)
		{
			pImage1 = cvCreateImage(cvSize(pImage->width, pImage->height), IPL_DEPTH_8U, 3);	// 用于角点检测的灰度图
			pImage1->origin = 1;										// 图像像素原点在图像左下角(windows bitmap形式)
		}
		cvCvtColor(pImage, pImage1, CV_GRAY2RGB);

		PointArray2D32f = (CvPoint2D32f*)malloc(sizeof(CvPoint2D32f)* 72);
		box = (CvBox2D32f *)malloc(sizeof(CvBox2D32f)* 72);

		memset(PointArray2D32f, 0, sizeof(CvPoint2D32f)* 72);
		memset(box, 0, sizeof(CvBox2D32f)* 72);

		for (int i = 0; i < count; i++)
		{
			PointArray2D32f[i].x = (float)m_iPos[i][0];
			PointArray2D32f[i].y = (float)m_iPos[i][1];
		}

		cvFitEllipse(PointArray2D32f, count, box);

		center.x = cvRound(box->center.x) + iCentreX;
		center.y = cvRound(box->center.y) + iCentreY;
		if (center.x <0 || center.x> pImage->width || center.y<0 || center.y> pImage->height) break;
		size.width = cvRound(box->size.width*0.5);
		size.height = cvRound(box->size.height*0.5);
		//box->angle=-box->angle;

		cvEllipse(pImage1, center, size, box->angle, 0, 360, CV_RGB(255, 0, 0), 1, CV_AA, 0);




		double dbResultX = box->center.x + iCentreX;
		double dbResultY = box->center.y + iCentreY;

		Vct_ponits.at(iCirCleCounts).pt.x = (float) dbResultX;
		Vct_ponits.at(iCirCleCounts).pt.y = (float) dbResultY;
		Vct_ponits.at(iCirCleCounts).angle = (float) box->angle;
		// Vct_ponits.at(iCirCleCounts).size=size.width*size.height;


		// 重新画点
		*(pImage->imageData + (int)dbResultY * pImage->widthStep + (int)dbResultX * pImage->nChannels) = 255;
		char FileName[200] = "";
		char LeftSaveDirectory[] = "C:\\Documents and Settings\\xjingjing\\桌面\\Calibrate 7.8\\2015.6.14\\CirCleCalibrate\\Temp\\";
		sprintf(FileName, "%sTempImg%d_L.bmp", LeftSaveDirectory, 1);
		cvSaveImage(FileName, pImage);

		//free(PointArray2D32f);
		//free(box);
		delete[] PointArray2D32f;
		delete[] box;
		//delete [] pImage1;
		//pImage1=NULL;
		//delete [] pcImgPath;
		//pcImgPath=NULL;
	}
}

void showthescanfPro(IplImage* & pImage, vector<KeyPoint>&Vct_ponits, int ipointscounts, int iChesscount)
{

	char FileName[200] = "";
	char LeftSaveDirectory[] = "C:\\Documents and Settings\\xjingjing\\桌面\\Calibrate 7.8\\2015.6.14\\CirCleCalibrate\\Temp\\";
	//存储扫描到的点
	int m_iPos[72][2];
	for (int iCirCleCounts = 0; iCirCleCounts < (int)Vct_ponits.size(); iCirCleCounts++)
	{

		// 圆心坐标
		int iCentreX = (int) Vct_ponits.at(iCirCleCounts).pt.x;
		int iCentreY = (int) Vct_ponits.at(iCirCleCounts).pt.y;

		// 扫描到的边缘点集
		//int iPos[72][2];
		memset(m_iPos, 0, sizeof(int)* 72 * 2);

		// 以0°（水平向右）为起点，沿逆时针方向每隔5°在圆周上取一点，并向圆心方向搜索，若某一点的像素值为255时，则为边缘点。
		for (int iAngle = 0; iAngle < 360; iAngle += 5)
		{
			for (int iR = 1; iR < 25; iR++)
			{
				int iX = (int)(iCentreX + (iR * cos((double)iAngle * PI / 180) + (((iAngle >= 0 && iAngle <= 90) || (iAngle >= 270 && iAngle < 360)) ? 0.5 : -0.5)));		// 待判断点的X坐标，式中的0.5为修正值
				int iY = (int)(iCentreY - (iR * sin((double)iAngle * PI / 180) + ((iAngle >= 0 && iAngle < 180) ? 0.5 : -0.5)));		// 待判断点的Y坐标，式中的0.5为修正值

				// 判断是否超出图像的范围
				if (iX < 0 || iX >= pImage->width || iY < 0 || iY >= pImage->height)
				{
					m_iPos[iAngle / 5][0] = iX - iCentreX;					// 扫描到的边缘点集（iPos）是以（iCentreX，iCentreY）为坐标中心的坐标
					m_iPos[iAngle / 5][1] = iY - iCentreY;
					break;
				}


				*(pImage->imageData + iY * pImage->widthStep + iX * pImage->nChannels) = 255;
			}

		}

		sprintf(FileName, "%sTempImg%d_L.bmp", LeftSaveDirectory, 1);
		cvSaveImage(FileName, pImage);
		// 采用交换排序法对扫描到的边缘点集按其到圆心的距离由近到远进行排序
	}
}

//计算所有圆点阵列所有的点
BOOL CalcPonitDistance(ST_CIRCLE_POINT UpperLeftPonits, vector<ST_CIRCLE_POINT>&Vct_Points)
{
	//double distance;
	double xcoordinate = UpperLeftPonits.x;
	double ycoordinate = UpperLeftPonits.y;
	if (Vct_Points.size() == 0)
	{
		return false;
	}
	else
	{
		for (int i = 0; i < Vct_Points.size(); i++)
		{
			if (Vct_Points.at(i).x != xcoordinate && Vct_Points.at(i).y != ycoordinate)
			{
				double dx = fabs(Vct_Points.at(i).x - xcoordinate);
				double dy = fabs(Vct_Points.at(i).y - ycoordinate);
				Vct_Points.at(i).distance = sqrt(dx*dx + dy*dy);
			}
			else
			{
				Vct_Points.at(i).distance = 0.0;
			}
		}
	}

	return true;
}

//得到距原点最近的两个点
double  GetMinDistance(vector<ST_CIRCLE_POINT>&Vct_ponits)
{
	double MinDistValue;
	if (Vct_ponits.size() == 0)
	{
		return 0;
	}
	else
	{
		MinDistValue =/*Vct_ponits.at(0).distance*/300;
		for (int i = 0; i < Vct_ponits.size(); i++)
		{
			if (Vct_ponits.at(i).distance<MinDistValue && Vct_ponits.at(i).distance>0)
			{
				MinDistValue = Vct_ponits.at(i).distance;
			}
		}
	}
	return MinDistValue;
}

//获得棋盘标记点坐标
void GetChessborardMark(vector<ST_CIRCLE_POINT>&Vct_points, int ChessboardCount, PST_ORGIN_POINTS &pst_orginPoints)
{
	//分配空间 
	pst_orginPoints = new ST_ORGIN_POINTST[ChessboardCount];
	int temp;
	for (int i = 0; i < ChessboardCount; i++)
	{
		float sizeMax = 0.0;
		temp = 0;
		for (int j = 0; j<Vct_points.size(); j++)
		{
			//按照圆面积的大小 棋盘的标记点      
			if (Vct_points.at(j).size>sizeMax)
			{
				temp = j;
				sizeMax = Vct_points.at(j).size;
			}
		}

		pst_orginPoints[i].x = Vct_points.at(temp).x;
		pst_orginPoints[i].y = Vct_points.at(temp).y;
		pst_orginPoints[i].size = Vct_points.at(temp).size;
		Vct_points.erase(Vct_points.begin() + temp);
	}
}

//获得棋盘格原点
bool GetChessboardOrginPoints(vector<ST_CIRCLE_POINT>&Vct_points, PST_ORGIN_POINTS& pst_OringPonit, int ChessboardCount)
{
	if (pst_OringPonit == NULL || Vct_points.size() == 0)
	{
		return false;
	}

	//暂存变量，用来保存位置
	int  temp;
	for (int i = 0; i < ChessboardCount; i++)
	{
		temp = 0;
		//
		double xcoordinate = pst_OringPonit[i].x;
		double ycoordinate = pst_OringPonit[i].y;

		//计算距离
		for (int j = 0; j < Vct_points.size(); j++)
		{

			double dx = fabs(Vct_points.at(j).x - xcoordinate);
			double dy = fabs(Vct_points.at(j).y - ycoordinate);
			Vct_points.at(j).distance = sqrt(dx*dx + dy*dy);
		}

		//计算最小距离
		double distance;
		distance = Vct_points.at(0).distance;

		for (int k = 0; k < Vct_points.size(); k++)
		{
			if (Vct_points.at(k).distance < distance)
			{
				distance = Vct_points.at(k).distance;
				//最小值在向量中的位置
				temp = k;

			}
		}

		pst_OringPonit[i].x = Vct_points.at(temp).x;
		pst_OringPonit[i].y = Vct_points.at(temp).y;
		pst_OringPonit[i].size = Vct_points.at(temp).size;
	}

	return true;
}


//初始化物理坐标
BOOL InitObjectPoints(float* ObjectPoints, int chessnum, int Img_Count, CvMat*m_pcvObjectPoints[])
{
	//圆点直径
	double d = 20;
	//边缘距离
	double h1 = 10;
	//靶标距离
	double h2 = 45 /*19*/;
	for (int Imgcount = 0; Imgcount < Img_Count; Imgcount++)
	{
		for (int k = 0; k < chessnum; k++)
		{
			if (k == 0)
			{
				for (int i = 0; i < 10; i++)
				{
					for (int j = 0; j < 6; j++)
					{
						ObjectPoints[Imgcount*chessnum * 60 * 3 + i * 6 * 3 + (j * 3) + 0] = i *(d + h1);
						ObjectPoints[Imgcount*chessnum * 60 * 3 + i * 6 * 3 + (j * 3) + 1] = j *(d + h1);
						ObjectPoints[Imgcount*chessnum * 60 * 3 + i * 6 * 3 + (j * 3) + 2] = 0;
					}
				}
			}
			if (k == 1)
			{
				for (int i = 0; i < 10; i++)
				{
					for (int j = 0; j < 6; j++)
					{
						ObjectPoints[Imgcount*chessnum * 60 * 3 + i * 6 * 3 + (j * 3) + 0 + 60 * 3] = (d + h1) * 9 - i *(d + h1);
						ObjectPoints[Imgcount*chessnum * 60 * 3 + i * 6 * 3 + (j * 3) + 1 + 60 * 3] = ((d + h1) * 10) + h2 + d - j *(d + h1);
						ObjectPoints[Imgcount*chessnum * 60 * 3 + i * 6 * 3 + (j * 3) + 2 + 60 * 3] = 0;
					}
				}
			}
		}
	}

	for (int i = 0; i < CHESSNUM; i++)
	{
		(*m_pcvObjectPoints[i]) = cvMat(chessnum * 60 * Img_Count, 3, CV_32FC1, ObjectPoints);
	}

	//写文件
	FILE * pf = NULL;
	char pfilename[25] = "objectpoint.txt";
	//float x=key_points.at(10).pt.x;
	pf = fopen(pfilename, "w");
	int countNumber = 0;

	float *pfdata = m_pcvObjectPoints[0]->data.fl;
	int istep = m_pcvObjectPoints[0]->step / sizeof(float);
	for (int i = 0; i < chessnum * 60 * Img_Count; i++)
	{
		fprintf(pf, "%f %f %f %d", pfdata[i*istep + 0], pfdata[i*istep + 1], pfdata[i*istep + 2], i);
		fprintf(pf, "\r\n");
	}
	fclose(pf);
	pf = NULL;

	return TRUE;
}


//需找标记点点圆心
BOOL FindCircleCenter(IplImage*img, int minThreshold, int MaxThreshold, int shresholdStep, int minArea, int MaxArea, float minConvexity,
	float minInertiaRatio, float minCircularity,
	vector<KeyPoint>&Vct_ponits, int ipointscounts, int iChesscount)
{
	CvSize m_imageSize;
	m_imageSize = cvSize(img->width, img->height);
	Mat mtx(img);

	Mat binaryImage(img);
	SimpleBlobDetector::Params params;
	params.filterByCircularity = true;
	params.minThreshold = minThreshold;
	params.maxThreshold = MaxThreshold;
	params.thresholdStep = shresholdStep;
	params.minArea = minArea;
	params.maxArea = MaxArea;
	//斑点的凸度
	params.minConvexity = minConvexity;

	params.minInertiaRatio = minInertiaRatio;

	params.blobColor = 0;
	params.minCircularity = minCircularity;
	params.maxCircularity = 1.3;
	params.maxInertiaRatio = 1.2;
	params.minRepeatability = 5;
	SimpleBlobDetector detector(params);
	detector.detect(mtx, Vct_ponits);
	//  if (Vct_ponits.size()<ipointscounts*iChesscount+2)
	//  {
	//cout<<"图片中未找到所有标记点"<<endl;
	////return FALSE;
	//  }
	//	else
	//	{
	cout << "成功找到所有标记点" << endl;
	drawKeypoints(mtx, Vct_ponits, mtx, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	IplImage imgTemp = IplImage(mtx);
	namedWindow("SimpleBlobDetector");
	imshow("SimpleBlobDetector", mtx);

	cout << "标记点精确定位" << endl;

	int thresh = 10;
	int max_thresh = 255;

	createTrackbar("thresh:", "SimpleBlobDetector", &thresh, max_thresh, thresh_callback, &binaryImage);
	//  cvThreshold(img,)

	cvWaitKey(0);

	return  TRUE;
	//}
	// Mat mtx(img);
}


void thresh_callback(int Pos, void*Usrdata)
{
	Mat binarizedImage;
	Mat dst;
	binarizedImage = *(Mat*)(Usrdata);
	Canny(binarizedImage, dst, Pos, Pos * 2, 3);

	//threshold(binarizedImage, dst, Pos, 255, THRESH_BINARY);
	GobalThreHold = getTrackbarPos("thresh:", "SimpleBlobDetector");
	imshow("SimpleBlobDetector", dst);
}

void thresh_callback2(int Pos, void*Usrdata)
{
	Mat binarizedImage;
	Mat dst;
	binarizedImage = *(Mat*)(Usrdata);

	threshold(binarizedImage, dst, Pos, 255, THRESH_BINARY);
	GobalThreHold = getTrackbarPos("thresh:", "SimpleBlobDetector");
	imshow("SimpleBlobDetector", dst);
}

////初始化相机内外参数
//
//void InitCameraParams(int m_iImgCounts,int iPointCounts,int ChessboardCount )
//{
//	m_pcvObjectPoints = cvCreateMat(m_iImgCounts * iPointCounts*ChessboardCount, 3, CV_32FC1);
//	m_pcvImgPoints_Left = cvCreateMat(m_iImgCounts * iPointCounts*ChessboardCount, 1, CV_32FC2);
//	m_pcvImgPoints_Right = cvCreateMat(m_iImgCounts * iPointCounts*ChessboardCount, 1, CV_32FC2);
//	m_pcvPointCounts_Left = cvCreateMat(m_iImgCounts, 1, CV_32SC1);
//	m_pcvPointCounts_Right = cvCreateMat(m_iImgCounts, 1, CV_32SC1);
//
//
//
//	// 摄像机外参数
//	m_pcvRotationVct_Left = cvCreateMat(m_iImgCounts, 3, CV_64FC1);
//	m_pcvRotationVct_Right = cvCreateMat(m_iImgCounts, 3, CV_64FC1);
//	m_pcvTranslationVct_Left = cvCreateMat(m_iImgCounts, 3, CV_64FC1);
//	m_pcvTranslationVct_Right = cvCreateMat(m_iImgCounts, 3, CV_64FC1);
//	//m_pcvRotatonVctStereo = cvCreateMat(3, 3, CV_64FC1);
//	//m_pcvTranslationVctStereo = cvCreateMat(3, 1, CV_64FC1);
//
//	// 基础矩阵和本征矩阵
//	m_pcvFundamentalMat = cvCreateMat(3, 3, CV_64FC1);
//	m_pcvEigenValueMat = cvCreateMat(3, 3, CV_64FC1);
//	// 摄像机内参数
//	m_pcvIntrinsicMat_Left		= cvCreateMat(3, 3, CV_64FC1);
//	m_pcvIntrinsicMat_Right		= cvCreateMat(3, 3, CV_64FC1);
//	m_pcvDistortionCoeffs_Left	= cvCreateMat(5, 1, CV_64FC1);
//	m_pcvDistortionCoeffs_Right = cvCreateMat(5, 1, CV_64FC1);
//
//	//  创建摄像机外参数矩阵
//	m_pcvRotatonStereo		 = cvCreateMat(3, 3, CV_64FC1);
//	m_pcvTranslationStereo	 = cvCreateMat(3, 1, CV_64FC1);
//	m_pcvProjectionMat_Left	 = cvCreateMat(3, 4, CV_64F);
//	m_pcvProjectionMat_Right = cvCreateMat(3, 4, CV_64F);
//	m_pcvAvgErrMat=cvCreateMat(1, 1, CV_64FC1);
//	m_pcvRrojectionMat=cvCreateMat(4,4,CV_64F);
//
//
//}
//初始化相机内外参数
void InitCameraParamsSingle(int m_iImgCounts, int iPointCounts, int ChessboardCount)
{
	//初始化多块内参
	for (int i = 0; i < ChessboardCount; i++)
	{
		m_pcvObjectPoints[i] = cvCreateMat(m_iImgCounts * iPointCounts, 3, CV_32FC1);
		m_pcvImgPoints_Left[i] = cvCreateMat(m_iImgCounts * iPointCounts, 1, CV_32FC2);
		m_pcvImgPoints_Right[i] = cvCreateMat(m_iImgCounts * iPointCounts, 1, CV_32FC2);
		m_pcvPointCounts_Left[i] = cvCreateMat(m_iImgCounts, 1, CV_32SC1);
		m_pcvPointCounts_Right[i] = cvCreateMat(m_iImgCounts, 1, CV_32SC1);

		// 摄像机外参数
		m_pcvRotationVct_Left[i] = cvCreateMat(m_iImgCounts, 3, CV_64FC1);
		m_pcvRotationVct_Right[i] = cvCreateMat(m_iImgCounts, 3, CV_64FC1);
		m_pcvTranslationVct_Left[i] = cvCreateMat(m_iImgCounts, 3, CV_64FC1);
		m_pcvTranslationVct_Right[i] = cvCreateMat(m_iImgCounts, 3, CV_64FC1);

		// 摄像机内参数
		m_pcvIntrinsicMat_Left[i] = cvCreateMat(3, 3, CV_64FC1);
		m_pcvIntrinsicMat_Right[i] = cvCreateMat(3, 3, CV_64FC1);
		m_pcvDistortionCoeffs_Left[i] = cvCreateMat(5, 1, CV_64FC1);
		m_pcvDistortionCoeffs_Right[i] = cvCreateMat(5, 1, CV_64FC1);
	}

	// 基础矩阵和本征矩阵
	m_pcvFundamentalMat = cvCreateMat(3, 3, CV_64FC1);
	m_pcvEigenValueMat = cvCreateMat(3, 3, CV_64FC1);

	//  创建摄像机外参数矩阵
	m_pcvRotatonStereo = cvCreateMat(3, 3, CV_64FC1);
	m_pcvTranslationStereo = cvCreateMat(3, 1, CV_64FC1);
	m_pcvProjectionMat_Left = cvCreateMat(3, 4, CV_64F);
	m_pcvProjectionMat_Right = cvCreateMat(3, 4, CV_64F);
	m_pcvAvgErrMat = cvCreateMat(1, 1, CV_64FC1);
	m_pcvRrojectionMat = cvCreateMat(4, 4, CV_64F);
}




//对一副图像的所有特征点进行排序
void SortPoints(VEC_POINTS&Vec_points, vector<ST_CIRCLE_POINT>&Circle_Ponits, PST_ORGIN_POINTS Pst_orgin_points, int Chessnum, int Pointsnum)
{
	//图像原点
	ST_CIRCLE_POINT stUpperLeft;
	//距离原点最近的两点
	ST_CIRCLE_POINT NearstPoints[2];
	//两快标定板特征点排序
	for (int ChessNum = 0; ChessNum < Chessnum; ChessNum++)
	{
		stUpperLeft.x = Pst_orgin_points[ChessNum].x;
		stUpperLeft.y = Pst_orgin_points[ChessNum].y;
		stUpperLeft.size = Pst_orgin_points[ChessNum].size;

		//计算所有点距离原点的位置，距1号的原点
		CalcPonitDistance(stUpperLeft, Circle_Ponits);

		//计算最小距离
		double MinValue = GetMinDistance(Circle_Ponits);

		//测试用
		//sprintf(pfilename,"");
		//sprintf(pfilename,"gridcorner1.txt");
		//pf=fopen(pfilename,"w");
		//for (int i=0;i<Circle_Ponits.size();i++)
		//{

		//	fprintf(pf,"%f %f %f",Circle_Ponits.at(i).x,Circle_Ponits.at(i).y,Circle_Ponits.at(i).distance);
		//	fprintf(pf,"\r\n");

		//}
		//fclose(pf);
		//pf=NULL;

		///////取与原点最接近的两个点计算四个变化率///////////
		int Count = 0;
		for (int i = 0; i<Circle_Ponits.size(); i++)
		{
			if (Circle_Ponits.at(i).distance <= MinValue + 3.5 && Circle_Ponits.at(i).distance>0.0)
			{
				NearstPoints[Count].x = Circle_Ponits.at(i).x;
				NearstPoints[Count].y = Circle_Ponits.at(i).y;
				NearstPoints[Count].size = Circle_Ponits.at(i).size;

				NearstPoints[Count].distance = Circle_Ponits.at(i).distance;
				Count++;
			}
			if (Count == 2)
				break;
		}

		//判断哪个是大点
		float dx1, dy1, dx, dy;

		if (NearstPoints[0].size > NearstPoints[1].size)
		{
			//竖直方向X轴变化率
			dx1 = NearstPoints[0].x - stUpperLeft.x;

			//竖直方向Y轴变化率
			dy1 = NearstPoints[0].y - stUpperLeft.y;
			//水平方向X轴变换率
			dx = NearstPoints[1].x - stUpperLeft.x;

			//水平方向Y轴变化率
			dy = NearstPoints[1].y - stUpperLeft.y;
		}
		else
		{
			//竖直方向X轴变化率
			dx = NearstPoints[0].x - stUpperLeft.x;

			//竖直方向Y轴变化率
			dy = NearstPoints[0].y - stUpperLeft.y;
			//水平方向X轴变换率
			dx1 = NearstPoints[1].x - stUpperLeft.x;

			//水平方向Y轴变化率
			dy1 = NearstPoints[1].y - stUpperLeft.y;
		}

		//原点坐标
		float x = Pst_orgin_points[ChessNum].x;
		float y = Pst_orgin_points[ChessNum].y;

		//点都存在 vector<ST_CIRCLE_POINT>Circle_Ponits里面 
		ST_CIRCLE_POINT ComputerPonits;
		ComputerPonits.x = x;
		ComputerPonits.y = y;

		//左上角原点
		//Computer_points.push_back(stUpperLeft);

		//暂存
		float tempx = 0;
		float tempy = 0;

		tempx = x;
		tempy = y;
		//计算值与理论值匹配的点数目
		int findnumber = 0;
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 6; j++)
			{
				//计算理论点
				// tempx=x+dx;
				//tempy=y+dy;
				//寻找测量点中与理论点中最接近的点，并赋值
				for (int k = 0; k < Circle_Ponits.size(); k++)
				{

					if (fabs((Circle_Ponits.at(k).x - tempx)) <= 6 && fabs((Circle_Ponits.at(k).y - tempy)) <= 6)
					{
						ComputerPonits.dex = Circle_Ponits.at(k).x - tempx;
						ComputerPonits.dey = Circle_Ponits.at(k).y - tempy;
						tempx = Circle_Ponits.at(k).x;
						tempy = Circle_Ponits.at(k).y;


						ComputerPonits.x = tempx;
						ComputerPonits.y = tempy;

						Vec_points[ChessNum].push_back(ComputerPonits);



						tempx = tempx + dx;
						tempy = tempy + dy;



						findnumber++;

						//到下一行则变化量 不同
						if (findnumber > 0 && findnumber % 6 == 0)
						{

							tempx = Vec_points[ChessNum].at(findnumber - 6).x + dx1;//竖直方向X
							tempy = Vec_points[ChessNum].at(findnumber - 6).y + dy1;//竖直方向Y

						}

						Circle_Ponits.erase(Circle_Ponits.begin() + k);

						break;
					}
				}
			}
		}
		//打印
	}
}

//计算标定板的重投影误差
BOOL CalcReprjectionError(CvMat*Objectpoints, CvMat* IntrinsicMat, CvMat*DistortionMat, CvMat*ReprjectMat, CvMat*RotationVec, CvMat*TranslationVec, int iIPointsCounts, int iIiamgeCounts, int iIndex)
{
	//用来存储畸变系数
	double k[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	//
	double fx, fy, cx, cy;
	double *pdata = NULL;
	double *pdata1 = NULL;
	float *fpdata = NULL;
	float *fpdata1 = NULL;

	//旋转矩阵
	CvMat* cvRotortMat;
	cvRotortMat = cvCreateMat(3, 1, CV_64FC1);

	//3*3的旋转矩阵
	CvMat* cvRotortMatDst;
	cvRotortMatDst = cvCreateMat(3, 3, CV_64FC1);

	//平移矩阵
	CvMat* cvTranslationMat;
	cvTranslationMat = cvCreateMat(3, 1, CV_64FC1);
	//物理坐标
	CvMat* pobjecvPoints;
	pobjecvPoints = cvCreateMat(iIPointsCounts, 3, CV_32FC1);

	//重投影图像坐标
	CvMat* pimagepoints;
	pimagepoints = cvCreateMat(iIPointsCounts, 2, CV_32FC1);

	CvMat _k;

	_k = cvMat(DistortionMat->rows, DistortionMat->cols,
		CV_MAKETYPE(CV_64F, CV_MAT_CN(DistortionMat->type)), k);
	cvConvert(DistortionMat, &_k);

	//步长初始化
	int istep;
	int istep1;
	//计算重投影
	for (int i = 0; i < iIiamgeCounts; i++)
	{
		istep = 0;
		istep1 = 0;
		istep = (Objectpoints->step) / sizeof(float);

		fpdata = Objectpoints->data.fl;
		fpdata1 = pobjecvPoints->data.fl;
		for (int ObjCount = 0; ObjCount < iIPointsCounts; ObjCount++)
		{

			fpdata1[ObjCount*istep] = fpdata[ObjCount*istep + i*iIPointsCounts * 3];
			fpdata1[ObjCount*istep + 1] = fpdata[ObjCount*istep + 1 + i*iIPointsCounts * 3];
			fpdata1[ObjCount*istep + 2] = fpdata[ObjCount*istep + 2 + i*iIPointsCounts * 3];
		}
		fpdata = NULL;
		fpdata1 = NULL;

		istep = cvRotortMat->step / sizeof(double);
		istep1 = RotationVec->step / sizeof(double);
		pdata = RotationVec->data.db;
		pdata1 = cvRotortMat->data.db;

		for (int conutRotor = 0; conutRotor < 3; conutRotor++)
		{
			double temp1 = pdata[conutRotor + i*istep1];
			pdata1[conutRotor*istep] = pdata[conutRotor + i*istep1];

		}

		cvRodrigues2(cvRotortMat, cvRotortMatDst, NULL);

		pdata = NULL;
		pdata1 = NULL;
		istep = cvTranslationMat->step / sizeof(double);
		istep1 = TranslationVec->step / sizeof(double);
		pdata = TranslationVec->data.db;
		pdata1 = cvTranslationMat->data.db;
		//3*1
		for (int conutTrans = 0; conutTrans < 3; conutTrans++)
		{
			pdata1[conutTrans*istep] = pdata[conutTrans + i*istep1];
		}
		pdata1 = NULL;
		pdata = NULL;

		pdata1 = IntrinsicMat->data.db;
		fx = pdata1[0]; fy = pdata1[4];
		cx = pdata1[2]; cy = pdata1[5];
		pdata1 = NULL;

		//计算重投影
		for (int points = 0; points < iIPointsCounts; points++)
		{
			fpdata = pobjecvPoints->data.fl;
			istep = pobjecvPoints->step / sizeof(float);
			//物理坐标
			double X = fpdata[points*istep], Y = fpdata[points*istep + 1], Z = fpdata[points*istep + 2];

			pdata = cvRotortMatDst->data.db;
			pdata1 = cvTranslationMat->data.db;

			double x = pdata[0] * X + pdata[1] * Y + pdata[2] * Z + pdata1[0];
			double y = pdata[3] * X + pdata[4] * Y + pdata[5] * Z + pdata1[1];
			double z = pdata[6] * X + pdata[7] * Y + pdata[8] * Z + pdata1[2];

			double r2, r4, r6, a1, a2, a3, cdist, icdist2;
			double xd, yd;

			z = z ? 1. / z : 1;
			x *= z; y *= z;

			r2 = x*x + y*y;
			r4 = r2*r2;
			r6 = r4*r2;
			a1 = 2 * x*y;
			a2 = r2 + 2 * x*x;
			a3 = r2 + 2 * y*y;
			cdist = 1 + k[0] * r2 + k[1] * r4 + k[4] * r6;
			icdist2 = 1. / (1 + k[5] * r2 + k[6] * r4 + k[7] * r6);
			xd = x*cdist*icdist2 + k[2] * a1 + k[3] * a2;
			yd = y*cdist*icdist2 + k[2] * a3 + k[3] * a1;

			float mx = xd*fx + cx;
			float my = yd*fy + cy;

			pdata = NULL;
			pdata1 = NULL;

			fpdata1 = pimagepoints->data.fl;
			istep = pimagepoints->step / sizeof(float);

			fpdata1[points*istep] = mx;
			fpdata1[points*istep + 1] = my;
			fpdata1 = NULL;

		}
		//  cvProjectPoints2(pobjecvPoints,cvRotortMat,cvTranslationMat,IntrinsicMat,DistortionMat,pimagepoints);

		pdata = NULL;
		pdata1 = NULL;
		fpdata = NULL;
		fpdata1 = NULL;

		istep = pimagepoints->step / sizeof(float);
		fpdata = pimagepoints->data.fl;
		fpdata1 = ReprjectMat->data.fl;
		for (int pointscount = 0; pointscount < iIPointsCounts; pointscount++)
		{
			fpdata1[pointscount*istep + i*iIPointsCounts * 2] = fpdata[pointscount*istep];
			fpdata1[pointscount*istep + 1 + i*iIPointsCounts * 2] = fpdata[pointscount*istep + 1];
		}
	}

	//将重投影点记录成文件
	char FileName[30] = "";
	sprintf(FileName, "ReprojectionMat%d.txt", iIndex);
	FILE *pf;
	pf = fopen(FileName, "w");
	float *pfdata = NULL;
	pfdata = ReprjectMat->data.fl;
	istep = ReprjectMat->step / sizeof(float);

	for (int i = 0; i < iIiamgeCounts*iIPointsCounts; i++)
	{
		fprintf(pf, "%f %f %d", pfdata[i*istep + 0], pfdata[i*istep + 1], i);
		fprintf(pf, "\r\n");

	}
	fclose(pf);
	pf = NULL;
	sprintf(FileName, "");
	return TRUE;
}

#ifdef _TEST_
int main()
#else
int TestMain()
#endif
{
	//图片读取路径
	char LeftReadDirectory[] = "D:\\文档\\QQ文件\\Calibrate 7.17\\Calibrate 7.17\\Calibrate 7.17\\2015.6.14\\CirCleCalibrate\\ChessboardImg\\LeftCamera\\";
	//图像保存路径
	char LeftSaveDirectory[] = "D:\\文档\\QQ文件\\Calibrate 7.17\\Calibrate 7.17\\2015.6.14\\CirCleCalibrate\\LeftCirclePoints\\";
	char FileName[200] = "";

	//标定图像个数
	int m_iImgCounts = 12;
	//行列
	int rowcount = 6;
	int Colcount = 10;
	//棋盘格的数目
	int ChessboardCount = 1;
	int iPointCounts = rowcount*Colcount;

	//根据标定板的块数，初始化多组内参数
	InitCameraParamsSingle(m_iImgCounts, iPointCounts, CHESSNUM);

	int* piCornerPointNum_Left = NULL;				// 每幅左图像中包含的角点个数数组指针，数组中的每一个元素存放一副左图像中所包含的角点的个数。
	int* piCornerPointNum_Right = NULL;				// 每幅右图像中包含的角点个数数组指针，数组中的每一个元素存放一副右图像中所包含的角点的个数。
	float* pfObjectPoints = NULL;					// 角点物理坐标指针

	piCornerPointNum_Left = new int[m_iImgCounts];																// 存放每一副左图像中角点的个数值
	piCornerPointNum_Right = new int[m_iImgCounts];																// 存放每一副右图像中角点的个数值
	pfObjectPoints = new float[m_iImgCounts*ChessboardCount * iPointCounts * 3];				// 存放角点的物理坐标

	///////////////求物理坐标
	InitObjectPoints(pfObjectPoints, ChessboardCount, m_iImgCounts, m_pcvObjectPoints);

	vector<KeyPoint> key_points;
	vector<ST_CIRCLE_POINT>Circle_Ponits;

	//存放排序好的点
	VEC_POINTS Computer_points(CHESSNUM);

	// 棋盘格原点坐标
	ST_CIRCLE_POINT stUpperLeft;

	//棋盘格标记点
	PST_ORGIN_POINTS pstOrginPoints;
	CvSize m_imageSize;
	IplImage*ptemp;

	//对左目相机标定
	for (int iimageCount = 0; iimageCount < m_iImgCounts; iimageCount++)
	{
		//读取图片
		sprintf(FileName, "%sChessboardImg%d_L.bmp", LeftReadDirectory, iimageCount + 1); // 写入文件的名字
		IplImage* img = cvLoadImage(FileName, -1);  //读取图片

		if (img == NULL)   // 读取图片失败的时候返回结果
		{
			MessageBox(NULL, "Open Image Failed!", "Information", MB_ICONWARNING);
			return 0;
		}

		ptemp = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
		uchar*ptr = (uchar*)img->imageData;

		int height = img->height;
		int width = img->width;
		int pixpbyte = img->nChannels;
		int step = img->widthStep;

		m_imageSize = cvSize(width, height);
		namedWindow("SimpleBlobDetector");
		Mat binaryImage(img);
		int threshold = 0;
		createTrackbar("thresh:", "SimpleBlobDetector", &threshold, 255, thresh_callback2, &binaryImage);
		cvWaitKey(0);

		//大津阈值分割
		//threshold =Otsu(img);
		//寻找标记点圆心
		FindCircleCenter(img,/*threshold-60*/5,/*GobalThreHold*/threshold, 5, 50, 2500, 0.01f, 0.01f, 0.01f, key_points, iPointCounts, ChessboardCount);

		cvWaitKey(0);

		cvCanny(img, ptemp, GobalThreHold, GobalThreHold * 2, 3);

		namedWindow("SimpleBlobDetector");
		// imshow("SimpleBlobDetector",dest);
		// cvWaitKey(0);
		cvShowImage("SimpleBlobDetector", ptemp);
		cvWaitKey(0);

		//showthescanfPro(ptemp,key_points,iPointCounts,ChessboardCount);
		// CalElipseCentrePoint(ptemp,key_points,iPointCounts,ChessboardCount);
		CalCircleCentrePoint(ptemp, key_points, iPointCounts, ChessboardCount);
		cvReleaseImage(&ptemp);

		//将圆心画在圆上
		for (int i = 0; i < key_points.size(); i++)
		{
			int x = (int)key_points.at(i).pt.x;
			int y = (int)key_points.at(i).pt.y;
			*(ptr + y*width + x) = (uchar)255;
		}

		namedWindow("SimpleBlobDetector");
		cvShowImage("SimpleBlobDetector", img);

		sprintf(FileName, "");
		sprintf(FileName, "%sDetectedImg%d_L.bmp", LeftSaveDirectory, iimageCount + 1);
		cvSaveImage(FileName, img);
		cvWaitKey(0);

		cout << "记录所有找到的标记点坐标，并存为文件" << endl;
		//记录下第一块板上的所有坐标
		FILE * pf = NULL;
		char pfilename[25] = "gridcorner.txt";
		//float x=key_points.at(10).pt.x;
		pf = fopen(pfilename, "w");
		int countNumber = 0;

		for (int i = 0; i < key_points.size(); i++)
		{
			ST_CIRCLE_POINT CirclePoints;
			CirclePoints.x = key_points.at(i).pt.x;
			CirclePoints.y = key_points.at(i).pt.y;
			CirclePoints.size = key_points.at(i).size;
			CirclePoints.distance = 0;
			countNumber++;
			Circle_Ponits.push_back(CirclePoints);
			fprintf(pf, "%f %f %d", key_points.at(i).pt.x, key_points.at(i).pt.y, countNumber);
			fprintf(pf, "\r\n");
		}
		fclose(pf);
		pf = NULL;

		//获得棋盘格的标记点坐标
		GetChessborardMark(Circle_Ponits, CHESSNUM, pstOrginPoints);
		//获得棋盘格的原点坐标
		GetChessboardOrginPoints(Circle_Ponits, pstOrginPoints, CHESSNUM);
		SortPoints(Computer_points, Circle_Ponits, pstOrginPoints, CHESSNUM, iPointCounts);

		//将顺序画在图上
		for (int i = 0; i < CHESSNUM; i++)
		{
			for (int j = 0; j < (int) Computer_points[i].size() - 1; j++)
			{
				float x = Computer_points[i].at(j).x;
				float y = Computer_points[i].at(j).y;

				float x1 = Computer_points[i].at(j + 1).x;
				float y1 = Computer_points[i].at(j + 1).y;

				CvPoint pt1;
				CvPoint pt2;
				pt1.x = x;
				pt1.y = y;
				pt2.x = x1;
				pt2.y = y1;

				cvLine(img, pt1, pt2, cvScalar(255, 255, 255), 1, 8, 0);
			}
		}

		cvShowImage("SimpleBlobDetector", img);
		cvWaitKey(0);
		sprintf(FileName, "");
		sprintf(FileName, "%sDetectedSortedImg%d_L.bmp", LeftSaveDirectory, iimageCount + 1);
		cvSaveImage(FileName, img);

		//将排列好的标记点赋值给矩阵
		float *pfdata = NULL;
		//pfdata=m_pcvImgPoints_Left->data.fl;
		int istep = 0;
		for (int chessnumber = 0; chessnumber < CHESSNUM; chessnumber++)
		{
			sprintf(pfilename, "");
			sprintf(pfilename, "ComputerPoints_[%d]_%d.txt", chessnumber, iimageCount);
			pf = fopen(pfilename, "w");

			pfdata = m_pcvImgPoints_Left[chessnumber]->data.fl;
			istep = m_pcvImgPoints_Left[chessnumber]->step / sizeof(float);
			for (int i = 0; i < Computer_points[chessnumber].size(); i++)
			{

				for (int j = 0; j < m_pcvImgPoints_Left[chessnumber]->cols; j++)
				{
					//给矩阵赋值
					pfdata[i*istep + j * 2 + iimageCount*iPointCounts * 2] = Computer_points[chessnumber].at(i).x;
					pfdata[i*istep + j * 2 + 1 + iimageCount*iPointCounts * 2] = Computer_points[chessnumber].at(i).y;

					fprintf(pf, "%f %f %d ", pfdata[i*istep + j * 2 + iimageCount*iPointCounts * 2], pfdata[i*istep + j * 2 + 1 + iimageCount*iPointCounts * 2], i);
					fprintf(pf, "\r\n");
				}

			}
			pfdata = NULL;

			fclose(pf);
			pf = NULL;
			sprintf(pfilename, "");

		}

		piCornerPointNum_Left[iimageCount] = ChessboardCount*iPointCounts;

		Circle_Ponits.clear();
		for (int chessnumber = 0; chessnumber < CHESSNUM; chessnumber++)
		{
			Computer_points[chessnumber].clear();
		}

		sprintf(FileName, "");
	}

	//ST_OBJECT_POINTS ObjectPoints[70];
	//detector.detect(mtx,key_points);  
	//Mat output_img;  
	//理论值
	//vector<CIRCLE_POINT>Computer_points;

	for (int i = 0; i < CHESSNUM; i++)
	{
		cvSetData(m_pcvPointCounts_Left[i], piCornerPointNum_Left, sizeof(int));
	}

	////////////////////////////////测试用,测试矩阵中图像的特征点//////////////////////////////////////////
	//float *pfdata=NULL;
	//pfdata=m_pcvObjectPoints->data.fl;
	//FILE * pf=NULL;
	//char pfilename[30]="ObjectPointsAll.txt";
	////float x=key_points.at(10).pt.x;
	//pf=fopen(pfilename,"w");
	//int istep=m_pcvObjectPoints->step/sizeof(float);
	//for
	//for (int i=0;i<m_iImgCounts*iPointCounts;i++)
	//{
	//	fprintf(pf,"%f %f %f %d",pfdata[i*istep+0],pfdata[i*istep+1],pfdata[i*istep+2],i);
	//	fprintf(pf,"\r\n");
	//}
	//fclose(pf);
	//pf=NULL;	
	////////////////////////////将每块标定板，所有角点都记录下来//////////////////////////////////////////////
	char pfilename[30] = "";
	FILE * pf = NULL;
	float *pfdata = NULL;
	sprintf(pfilename, "");

	int istep = 0;
	for (int i = 0; i < CHESSNUM; i++)
	{
		sprintf(pfilename, "ComputerPointsAll[%d].txt", i);
		pfdata = m_pcvImgPoints_Left[i]->data.fl;
		istep = m_pcvImgPoints_Left[i]->step / sizeof(float);
		pf = fopen(pfilename, "w");

		for (int i = 0; i < m_iImgCounts*iPointCounts; i++)
		{
			fprintf(pf, "%f %f %d", pfdata[i*istep + 0], pfdata[i*istep + 1], i);
			fprintf(pf, "\r\n");

		}

		fclose(pf);
		pf = NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	//对多块标定板分别标定，得到多组参数，多个内参，多组外参数
	for (int i = 0; i < CHESSNUM; i++)
	{
		cvCalibrateCamera2(m_pcvObjectPoints[i],
			m_pcvImgPoints_Left[i],
			m_pcvPointCounts_Left[i],
			m_imageSize,
			m_pcvIntrinsicMat_Left[i],
			m_pcvDistortionCoeffs_Left[i],
			m_pcvRotationVct_Left[i],
			m_pcvTranslationVct_Left[i],
			0);

	}

	cvSave("m_pcvIntrinsicMat_Left_0.xml", m_pcvIntrinsicMat_Left[0]);
	cvSave("pcvDistortionCoeffs_Left_0.xml", m_pcvDistortionCoeffs_Left[0]);
	cvSave("cvRotorMat_0.xml", m_pcvRotationVct_Left[0]);
	cvSave("cvTranslationMat_0.xml", m_pcvRotationVct_Left[0]);

	cvSave("m_pcvIntrinsicMat_Left_1.xml", m_pcvIntrinsicMat_Left[1]);
	cvSave("pcvDistortionCoeffs_Left_1.xml", m_pcvDistortionCoeffs_Left[1]);
	cvSave("cvRotorMat_1.xml", m_pcvRotationVct_Left[1]);
	cvSave("cvTranslationMat_1.xml", m_pcvRotationVct_Left[1]);

	//sprintf(FileName,"");
	//sprintf(FileName,"%sChessboardImg0%d_L.bmp",LeftReadDirectory,1);
	//IplImage* image=cvLoadImage(FileName,-1);
	//
	/////////////////////////////图片校正///////////////////////////////////////////////
	//	IplImage* mapx = cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
	//	IplImage* mapy = cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
	//
	//	cvInitUndistortMap(m_pcvIntrinsicMat_Left,m_pcvDistortionCoeffs_Left,mapx,mapy);
	//	cvNamedWindow("undistort");
	//
	//
	//		IplImage *t = cvCloneImage(image);
	//		cvShowImage("Calibration",image);
	//		cvRemap(t,image,mapx,mapy);
	//		cvReleaseImage(&t);
	//		cvShowImage("Undistort",image);
	//		cvWaitKey(0);
	//sprintf(FileName,"");
	//sprintf(FileName,"%sChessboardImgUndisorted0%d_L.bmp",LeftSaveDirectory,1);
	//cvSaveImage(FileName,image);

	CvMat*m_pcvImgPoints_LeftReProjection;
	m_pcvImgPoints_LeftReProjection = cvCreateMat(m_iImgCounts * iPointCounts, 2, CV_32FC1);

	//计算标定误差
	CalcReprjectionError(m_pcvObjectPoints[0], m_pcvIntrinsicMat_Left[1], m_pcvDistortionCoeffs_Left[1], m_pcvImgPoints_LeftReProjection, m_pcvRotationVct_Left[1], m_pcvTranslationVct_Left[1], iPointCounts, m_iImgCounts, 1);

	cvWaitKey(0);
}