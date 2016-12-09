#include"highgui.h"
#include"cv.h"
#include <iostream>
using namespace std;
using namespace cv;
#include "define.h"
#include "BaseImageHandleFunction.h"

CvMat* m_pcvObjectPoints[CHESSNUM];			// �ǵ�������������
CvMat* m_pcvImgPoints_Left[CHESSNUM];	    // ��Ŀ���M��ͼ�������нǵ������������
CvMat* m_pcvImgPoints_Right[CHESSNUM];		// ��Ŀ���M��ͼ�������нǵ������������
CvMat* m_pcvPointCounts_Left[CHESSNUM];		// �������ڱ궨��Ŀ�����ͼ��Ľǵ������ɵľ���
CvMat* m_pcvPointCounts_Right[CHESSNUM];    // �������ڱ궨��Ŀ�����ͼ��Ľǵ������ɵľ���
//CU8HUMS_CSCTDlg  *PDlg;
// ������ڲ���
//CvMat* m_pcvNewIntrinsicMat_Left;		// ���������������
//CvMat* m_pcvNewIntrinsicMat_Right;	// ���������������
//CvMat* m_pcvDistortionCoeffs_Left;	// �����������ϵ��
//CvMat* m_pcvDistortionCoeffs_Right;	// �����������ϵ��

// ����������
CvMat* m_pcvRotationVct_Left[CHESSNUM];		// ��������Ŀ���������ϵ���������ת���󣬼�learning OpenCv 428ҳ��Rl����
CvMat* m_pcvRotationVct_Right[CHESSNUM];		       // ��������Ŀ���������ϵ���������ת���󣬼�learning OpenCv 428ҳ��Rr����
CvMat* m_pcvTranslationVct_Left[CHESSNUM];	       // ��������Ŀ���������ϵ�������ƽ�ƾ��󣬼�learning OpenCv 428ҳ��Tl����
CvMat* m_pcvTranslationVct_Right[CHESSNUM];	      // ��������Ŀ���������ϵ�������ƽ�ƾ��󣬼�learning OpenCv 428ҳ��Tr����
//CvMat* m_pcvRotatonStereo;			// ��Ŀ�������ϵת������Ŀ�������ϵ�������ת���󣬼�learning OpenCv 428ҳ��R����
//CvMat* m_pcvTranslationStereo;		// ��Ŀ�������ϵת������Ŀ�������ϵ�����ƽ�ƾ��󣬼�learning OpenCv 428ҳ��T����

// ��������ͱ�������
CvMat* m_pcvFundamentalMat;			   // ��������,��learning OpenCv 425ҳ��F����
CvMat* m_pcvEigenValueMat;			   // �������󣬼�learning OpenCv 425ҳ��E����

CvMat* m_pcvIntrinsicMat_Left[CHESSNUM];		// ���������������
CvMat* m_pcvIntrinsicMat_Right[CHESSNUM];		// ���������������
CvMat* m_pcvDistortionCoeffs_Left[CHESSNUM];	// �����������ϵ��
CvMat* m_pcvDistortionCoeffs_Right[CHESSNUM];	// �����������ϵ��

CvMat* m_pcvRotatonStereo;			// ��Ŀ�������ϵת������Ŀ�������ϵ�������ת���󣬼�learning OpenCv 428ҳ��R����
CvMat* m_pcvTranslationStereo;		// ��Ŀ�������ϵת������Ŀ�������ϵ�����ƽ�ƾ��󣬼�learning OpenCv 428ҳ��T����
CvMat* m_pcvProjectionMat_Left;		// ��Ŀ���ͶӰ���̣���3*4�ľ���Pl
CvMat* m_pcvProjectionMat_Right;	// ��Ŀ���ͶӰ���̣���3*4�ľ���Pr
CvMat* m_pcvRrojectionMat;	        // ��Ŀ���ͶӰ���̣���3*4�ľ���Pr
CvMat* m_pcvAvgErrMat;              // �궨������


void thresh_callback(int Pos, void* usrdata);//�ص�����1
void thresh_callback2(int Pos, void* usrdata);//�ص�����
int GobalThreHold;

void CalCircleCentrePoint(IplImage* &pImage, vector<KeyPoint>&Vct_ponits, int ipointscounts, int iChesscount)
{
	for (int iCirclenCounts = 0; iCirclenCounts < (int)Vct_ponits.size(); iCirclenCounts++)
	{
		/// ���Ե�Բ������
		int iCentreX = (int)Vct_ponits.at(iCirclenCounts).pt.x;
		int iCentreY = (int)Vct_ponits.at(iCirclenCounts).pt.y;

		///ɨ�赽�ı�Ե�㼯
		int m_iPos[72][2];

		memset(m_iPos, 0, sizeof(int)* 72 * 2);

		/// ��0�㣨ˮƽ���ң�Ϊ��㣬����ʱ�뷽��ÿ��5����Բ����ȡһ�㣬����Բ�ķ�����������ĳһ�������ֵΪ255ʱ����Ϊ��Ե�㡣
		for (int iAngle = 0; iAngle < 360; iAngle += 5)
		{
			for (int iR = 1; iR < 25; iR++)
			{
				int iX = (int)(iCentreX + (iR * cos((double)iAngle * PI / 180) + (((iAngle >= 0 && iAngle <= 90) || (iAngle >= 270 && iAngle < 360)) ? 0.5 : -0.5)));		// ���жϵ��X���꣬ʽ�е�0.5Ϊ����ֵ
				int iY = (int)(iCentreY - (iR * sin((double)iAngle * PI / 180) + ((iAngle >= 0 && iAngle < 180) ? 0.5 : -0.5)));		// ���жϵ��Y���꣬ʽ�е�0.5Ϊ����ֵ

				/// �ж��Ƿ񳬳�ͼ��ķ�Χ
				if (iX < 0 || iX >= pImage->width || iY < 0 || iY >= pImage->height)
				{
					m_iPos[iAngle / 5][0] = iX - iCentreX;					// ɨ�赽�ı�Ե�㼯��iPos�����ԣ�iCentreX��iCentreY��Ϊ�������ĵ�����
					m_iPos[iAngle / 5][1] = iY - iCentreY;
					break;
				}

				uchar ucVal = (uchar)*(pImage->imageData + iY * pImage->widthStep + iX * pImage->nChannels + 0);
				if (255 == ucVal)
				{
					m_iPos[iAngle / 5][0] = iX - iCentreX;					// ɨ�赽�ı�Ե�㼯��iPos�����ԣ�iCentreX��iCentreY��Ϊ�������ĵ�����
					m_iPos[iAngle / 5][1] = iY - iCentreY;
					break;
				}
			}
		}

		/// ���ý������򷨶�ɨ�赽�ı�Ե�㼯���䵽Բ�ĵľ����ɽ���Զ��������
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

		/// ȡλ��iPos�м��12���㣬������ϱ�ԵԲ��
		int iFitPos[36][2];				// ��ϵ�����
		memset(iFitPos, 0, sizeof(int)* 36 * 2);
		for (int i = 0; i < 36; i++)
		{
			iFitPos[i][0] = m_iPos[24 + i][0];
			iFitPos[i][1] = m_iPos[24 + i][1];
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
	}
}

//��Բ���
void CalElipseCentrePoint(IplImage* & pImage, vector<KeyPoint>&Vct_ponits, int ipointscounts, int iChesscount)
{
	//�洢ɨ�赽�ĵ�
	int m_iPos[72][2];
	for (int iCirCleCounts = 0; iCirCleCounts < (int)Vct_ponits.size(); iCirCleCounts++)
	{
		// Բ������
		int iCentreX =(int) Vct_ponits.at(iCirCleCounts).pt.x;
		int iCentreY =(int) Vct_ponits.at(iCirCleCounts).pt.y;

		// ɨ�赽�ı�Ե�㼯
		//int iPos[72][2];
		memset(m_iPos, 0, sizeof(int)* 72 * 2);

		// ��0�㣨ˮƽ���ң�Ϊ��㣬����ʱ�뷽��ÿ��5����Բ����ȡһ�㣬����Բ�ķ�����������ĳһ�������ֵΪ255ʱ����Ϊ��Ե�㡣
		for (int iAngle = 0; iAngle < 360; iAngle += 5)
		{
			for (int iR = 1; iR < 25; iR++)
			{
				int iX = (int)(iCentreX + (iR * cos((double)iAngle * PI / 180) + (((iAngle >= 0 && iAngle <= 90) || (iAngle >= 270 && iAngle < 360)) ? 0.5 : -0.5)));		// ���жϵ��X���꣬ʽ�е�0.5Ϊ����ֵ
				int iY = (int)(iCentreY - (iR * sin((double)iAngle * PI / 180) + ((iAngle >= 0 && iAngle < 180) ? 0.5 : -0.5)));		// ���жϵ��Y���꣬ʽ�е�0.5Ϊ����ֵ

				// �ж��Ƿ񳬳�ͼ��ķ�Χ
				if (iX < 0 || iX >= pImage->width || iY < 0 || iY >= pImage->height)
				{
					m_iPos[iAngle / 5][0] = iX - iCentreX;					// ɨ�赽�ı�Ե�㼯��iPos�����ԣ�iCentreX��iCentreY��Ϊ�������ĵ�����
					m_iPos[iAngle / 5][1] = iY - iCentreY;
					break;
				}



				uchar ucVal = (uchar)*(pImage->imageData + iY * pImage->widthStep + iX * pImage->nChannels + 0);
				if (255 == ucVal)
				{
					m_iPos[iAngle / 5][0] = iX - iCentreX;					// ɨ�赽�ı�Ե�㼯��iPos�����ԣ�iCentreX��iCentreY��Ϊ�������ĵ�����
					m_iPos[iAngle / 5][1] = iY - iCentreY;
					break;
				}
			}
		}

		// ���ý������򷨶�ɨ�赽�ı�Ե�㼯���䵽Բ�ĵľ����ɽ���Զ��������
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

		// ȡλ��iPos�м��36���㣬������ϱ�ԵԲ��
		int iFitPos[36][2];				// ��ϵ�����
		memset(iFitPos, 0, sizeof(int)* 36 * 2);
		for (int i = 0; i < 36; i++)
		{
			iFitPos[i][0] = m_iPos[24 + i][0];
			iFitPos[i][1] = m_iPos[24 + i][1];
		}

		// ��������С����������Բ�����꼰�뾶

		int count = 72;
		CvPoint2D32f *PointArray2D32f;
		CvBox2D32f *box;
		CvPoint center;
		CvSize size;

		IplImage *pImage1 = NULL;

		if (NULL == pImage1)
		{
			pImage1 = cvCreateImage(cvSize(pImage->width, pImage->height), IPL_DEPTH_8U, 3);	// ���ڽǵ���ĻҶ�ͼ
			pImage1->origin = 1;										// ͼ������ԭ����ͼ�����½�(windows bitmap��ʽ)
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


		// ���»���
		*(pImage->imageData + (int)dbResultY * pImage->widthStep + (int)dbResultX * pImage->nChannels) = 255;
		char FileName[200] = "";
		char LeftSaveDirectory[] = "C:\\Documents and Settings\\xjingjing\\����\\Calibrate 7.8\\2015.6.14\\CirCleCalibrate\\Temp\\";
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
	char LeftSaveDirectory[] = "C:\\Documents and Settings\\xjingjing\\����\\Calibrate 7.8\\2015.6.14\\CirCleCalibrate\\Temp\\";
	//�洢ɨ�赽�ĵ�
	int m_iPos[72][2];
	for (int iCirCleCounts = 0; iCirCleCounts < (int)Vct_ponits.size(); iCirCleCounts++)
	{

		// Բ������
		int iCentreX = (int) Vct_ponits.at(iCirCleCounts).pt.x;
		int iCentreY = (int) Vct_ponits.at(iCirCleCounts).pt.y;

		// ɨ�赽�ı�Ե�㼯
		//int iPos[72][2];
		memset(m_iPos, 0, sizeof(int)* 72 * 2);

		// ��0�㣨ˮƽ���ң�Ϊ��㣬����ʱ�뷽��ÿ��5����Բ����ȡһ�㣬����Բ�ķ�����������ĳһ�������ֵΪ255ʱ����Ϊ��Ե�㡣
		for (int iAngle = 0; iAngle < 360; iAngle += 5)
		{
			for (int iR = 1; iR < 25; iR++)
			{
				int iX = (int)(iCentreX + (iR * cos((double)iAngle * PI / 180) + (((iAngle >= 0 && iAngle <= 90) || (iAngle >= 270 && iAngle < 360)) ? 0.5 : -0.5)));		// ���жϵ��X���꣬ʽ�е�0.5Ϊ����ֵ
				int iY = (int)(iCentreY - (iR * sin((double)iAngle * PI / 180) + ((iAngle >= 0 && iAngle < 180) ? 0.5 : -0.5)));		// ���жϵ��Y���꣬ʽ�е�0.5Ϊ����ֵ

				// �ж��Ƿ񳬳�ͼ��ķ�Χ
				if (iX < 0 || iX >= pImage->width || iY < 0 || iY >= pImage->height)
				{
					m_iPos[iAngle / 5][0] = iX - iCentreX;					// ɨ�赽�ı�Ե�㼯��iPos�����ԣ�iCentreX��iCentreY��Ϊ�������ĵ�����
					m_iPos[iAngle / 5][1] = iY - iCentreY;
					break;
				}


				*(pImage->imageData + iY * pImage->widthStep + iX * pImage->nChannels) = 255;
			}

		}

		sprintf(FileName, "%sTempImg%d_L.bmp", LeftSaveDirectory, 1);
		cvSaveImage(FileName, pImage);
		// ���ý������򷨶�ɨ�赽�ı�Ե�㼯���䵽Բ�ĵľ����ɽ���Զ��������
	}
}

//��������Բ���������еĵ�
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

//�õ���ԭ�������������
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

//������̱�ǵ�����
void GetChessborardMark(vector<ST_CIRCLE_POINT>&Vct_points, int ChessboardCount, PST_ORGIN_POINTS &pst_orginPoints)
{
	//����ռ� 
	pst_orginPoints = new ST_ORGIN_POINTST[ChessboardCount];
	int temp;
	for (int i = 0; i < ChessboardCount; i++)
	{
		float sizeMax = 0.0;
		temp = 0;
		for (int j = 0; j<Vct_points.size(); j++)
		{
			//����Բ����Ĵ�С ���̵ı�ǵ�      
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

//������̸�ԭ��
bool GetChessboardOrginPoints(vector<ST_CIRCLE_POINT>&Vct_points, PST_ORGIN_POINTS& pst_OringPonit, int ChessboardCount)
{
	if (pst_OringPonit == NULL || Vct_points.size() == 0)
	{
		return false;
	}

	//�ݴ��������������λ��
	int  temp;
	for (int i = 0; i < ChessboardCount; i++)
	{
		temp = 0;
		//
		double xcoordinate = pst_OringPonit[i].x;
		double ycoordinate = pst_OringPonit[i].y;

		//�������
		for (int j = 0; j < Vct_points.size(); j++)
		{

			double dx = fabs(Vct_points.at(j).x - xcoordinate);
			double dy = fabs(Vct_points.at(j).y - ycoordinate);
			Vct_points.at(j).distance = sqrt(dx*dx + dy*dy);
		}

		//������С����
		double distance;
		distance = Vct_points.at(0).distance;

		for (int k = 0; k < Vct_points.size(); k++)
		{
			if (Vct_points.at(k).distance < distance)
			{
				distance = Vct_points.at(k).distance;
				//��Сֵ�������е�λ��
				temp = k;

			}
		}

		pst_OringPonit[i].x = Vct_points.at(temp).x;
		pst_OringPonit[i].y = Vct_points.at(temp).y;
		pst_OringPonit[i].size = Vct_points.at(temp).size;
	}

	return true;
}


//��ʼ����������
BOOL InitObjectPoints(float* ObjectPoints, int chessnum, int Img_Count, CvMat*m_pcvObjectPoints[])
{
	//Բ��ֱ��
	double d = 20;
	//��Ե����
	double h1 = 10;
	//�б����
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

	//д�ļ�
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


//���ұ�ǵ��Բ��
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
	//�ߵ��͹��
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
	//cout<<"ͼƬ��δ�ҵ����б�ǵ�"<<endl;
	////return FALSE;
	//  }
	//	else
	//	{
	cout << "�ɹ��ҵ����б�ǵ�" << endl;
	drawKeypoints(mtx, Vct_ponits, mtx, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	IplImage imgTemp = IplImage(mtx);
	namedWindow("SimpleBlobDetector");
	imshow("SimpleBlobDetector", mtx);

	cout << "��ǵ㾫ȷ��λ" << endl;

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

////��ʼ������������
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
//	// ����������
//	m_pcvRotationVct_Left = cvCreateMat(m_iImgCounts, 3, CV_64FC1);
//	m_pcvRotationVct_Right = cvCreateMat(m_iImgCounts, 3, CV_64FC1);
//	m_pcvTranslationVct_Left = cvCreateMat(m_iImgCounts, 3, CV_64FC1);
//	m_pcvTranslationVct_Right = cvCreateMat(m_iImgCounts, 3, CV_64FC1);
//	//m_pcvRotatonVctStereo = cvCreateMat(3, 3, CV_64FC1);
//	//m_pcvTranslationVctStereo = cvCreateMat(3, 1, CV_64FC1);
//
//	// ��������ͱ�������
//	m_pcvFundamentalMat = cvCreateMat(3, 3, CV_64FC1);
//	m_pcvEigenValueMat = cvCreateMat(3, 3, CV_64FC1);
//	// ������ڲ���
//	m_pcvIntrinsicMat_Left		= cvCreateMat(3, 3, CV_64FC1);
//	m_pcvIntrinsicMat_Right		= cvCreateMat(3, 3, CV_64FC1);
//	m_pcvDistortionCoeffs_Left	= cvCreateMat(5, 1, CV_64FC1);
//	m_pcvDistortionCoeffs_Right = cvCreateMat(5, 1, CV_64FC1);
//
//	//  ������������������
//	m_pcvRotatonStereo		 = cvCreateMat(3, 3, CV_64FC1);
//	m_pcvTranslationStereo	 = cvCreateMat(3, 1, CV_64FC1);
//	m_pcvProjectionMat_Left	 = cvCreateMat(3, 4, CV_64F);
//	m_pcvProjectionMat_Right = cvCreateMat(3, 4, CV_64F);
//	m_pcvAvgErrMat=cvCreateMat(1, 1, CV_64FC1);
//	m_pcvRrojectionMat=cvCreateMat(4,4,CV_64F);
//
//
//}
//��ʼ������������
void InitCameraParamsSingle(int m_iImgCounts, int iPointCounts, int ChessboardCount)
{
	//��ʼ������ڲ�
	for (int i = 0; i < ChessboardCount; i++)
	{
		m_pcvObjectPoints[i] = cvCreateMat(m_iImgCounts * iPointCounts, 3, CV_32FC1);
		m_pcvImgPoints_Left[i] = cvCreateMat(m_iImgCounts * iPointCounts, 1, CV_32FC2);
		m_pcvImgPoints_Right[i] = cvCreateMat(m_iImgCounts * iPointCounts, 1, CV_32FC2);
		m_pcvPointCounts_Left[i] = cvCreateMat(m_iImgCounts, 1, CV_32SC1);
		m_pcvPointCounts_Right[i] = cvCreateMat(m_iImgCounts, 1, CV_32SC1);

		// ����������
		m_pcvRotationVct_Left[i] = cvCreateMat(m_iImgCounts, 3, CV_64FC1);
		m_pcvRotationVct_Right[i] = cvCreateMat(m_iImgCounts, 3, CV_64FC1);
		m_pcvTranslationVct_Left[i] = cvCreateMat(m_iImgCounts, 3, CV_64FC1);
		m_pcvTranslationVct_Right[i] = cvCreateMat(m_iImgCounts, 3, CV_64FC1);

		// ������ڲ���
		m_pcvIntrinsicMat_Left[i] = cvCreateMat(3, 3, CV_64FC1);
		m_pcvIntrinsicMat_Right[i] = cvCreateMat(3, 3, CV_64FC1);
		m_pcvDistortionCoeffs_Left[i] = cvCreateMat(5, 1, CV_64FC1);
		m_pcvDistortionCoeffs_Right[i] = cvCreateMat(5, 1, CV_64FC1);
	}

	// ��������ͱ�������
	m_pcvFundamentalMat = cvCreateMat(3, 3, CV_64FC1);
	m_pcvEigenValueMat = cvCreateMat(3, 3, CV_64FC1);

	//  ������������������
	m_pcvRotatonStereo = cvCreateMat(3, 3, CV_64FC1);
	m_pcvTranslationStereo = cvCreateMat(3, 1, CV_64FC1);
	m_pcvProjectionMat_Left = cvCreateMat(3, 4, CV_64F);
	m_pcvProjectionMat_Right = cvCreateMat(3, 4, CV_64F);
	m_pcvAvgErrMat = cvCreateMat(1, 1, CV_64FC1);
	m_pcvRrojectionMat = cvCreateMat(4, 4, CV_64F);
}




//��һ��ͼ��������������������
void SortPoints(VEC_POINTS&Vec_points, vector<ST_CIRCLE_POINT>&Circle_Ponits, PST_ORGIN_POINTS Pst_orgin_points, int Chessnum, int Pointsnum)
{
	//ͼ��ԭ��
	ST_CIRCLE_POINT stUpperLeft;
	//����ԭ�����������
	ST_CIRCLE_POINT NearstPoints[2];
	//����궨������������
	for (int ChessNum = 0; ChessNum < Chessnum; ChessNum++)
	{
		stUpperLeft.x = Pst_orgin_points[ChessNum].x;
		stUpperLeft.y = Pst_orgin_points[ChessNum].y;
		stUpperLeft.size = Pst_orgin_points[ChessNum].size;

		//�������е����ԭ���λ�ã���1�ŵ�ԭ��
		CalcPonitDistance(stUpperLeft, Circle_Ponits);

		//������С����
		double MinValue = GetMinDistance(Circle_Ponits);

		//������
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

		///////ȡ��ԭ����ӽ�������������ĸ��仯��///////////
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

		//�ж��ĸ��Ǵ��
		float dx1, dy1, dx, dy;

		if (NearstPoints[0].size > NearstPoints[1].size)
		{
			//��ֱ����X��仯��
			dx1 = NearstPoints[0].x - stUpperLeft.x;

			//��ֱ����Y��仯��
			dy1 = NearstPoints[0].y - stUpperLeft.y;
			//ˮƽ����X��任��
			dx = NearstPoints[1].x - stUpperLeft.x;

			//ˮƽ����Y��仯��
			dy = NearstPoints[1].y - stUpperLeft.y;
		}
		else
		{
			//��ֱ����X��仯��
			dx = NearstPoints[0].x - stUpperLeft.x;

			//��ֱ����Y��仯��
			dy = NearstPoints[0].y - stUpperLeft.y;
			//ˮƽ����X��任��
			dx1 = NearstPoints[1].x - stUpperLeft.x;

			//ˮƽ����Y��仯��
			dy1 = NearstPoints[1].y - stUpperLeft.y;
		}

		//ԭ������
		float x = Pst_orgin_points[ChessNum].x;
		float y = Pst_orgin_points[ChessNum].y;

		//�㶼���� vector<ST_CIRCLE_POINT>Circle_Ponits���� 
		ST_CIRCLE_POINT ComputerPonits;
		ComputerPonits.x = x;
		ComputerPonits.y = y;

		//���Ͻ�ԭ��
		//Computer_points.push_back(stUpperLeft);

		//�ݴ�
		float tempx = 0;
		float tempy = 0;

		tempx = x;
		tempy = y;
		//����ֵ������ֵƥ��ĵ���Ŀ
		int findnumber = 0;
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 6; j++)
			{
				//�������۵�
				// tempx=x+dx;
				//tempy=y+dy;
				//Ѱ�Ҳ������������۵�����ӽ��ĵ㣬����ֵ
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

						//����һ����仯�� ��ͬ
						if (findnumber > 0 && findnumber % 6 == 0)
						{

							tempx = Vec_points[ChessNum].at(findnumber - 6).x + dx1;//��ֱ����X
							tempy = Vec_points[ChessNum].at(findnumber - 6).y + dy1;//��ֱ����Y

						}

						Circle_Ponits.erase(Circle_Ponits.begin() + k);

						break;
					}
				}
			}
		}
		//��ӡ
	}
}

//����궨�����ͶӰ���
BOOL CalcReprjectionError(CvMat*Objectpoints, CvMat* IntrinsicMat, CvMat*DistortionMat, CvMat*ReprjectMat, CvMat*RotationVec, CvMat*TranslationVec, int iIPointsCounts, int iIiamgeCounts, int iIndex)
{
	//�����洢����ϵ��
	double k[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	//
	double fx, fy, cx, cy;
	double *pdata = NULL;
	double *pdata1 = NULL;
	float *fpdata = NULL;
	float *fpdata1 = NULL;

	//��ת����
	CvMat* cvRotortMat;
	cvRotortMat = cvCreateMat(3, 1, CV_64FC1);

	//3*3����ת����
	CvMat* cvRotortMatDst;
	cvRotortMatDst = cvCreateMat(3, 3, CV_64FC1);

	//ƽ�ƾ���
	CvMat* cvTranslationMat;
	cvTranslationMat = cvCreateMat(3, 1, CV_64FC1);
	//��������
	CvMat* pobjecvPoints;
	pobjecvPoints = cvCreateMat(iIPointsCounts, 3, CV_32FC1);

	//��ͶӰͼ������
	CvMat* pimagepoints;
	pimagepoints = cvCreateMat(iIPointsCounts, 2, CV_32FC1);

	CvMat _k;

	_k = cvMat(DistortionMat->rows, DistortionMat->cols,
		CV_MAKETYPE(CV_64F, CV_MAT_CN(DistortionMat->type)), k);
	cvConvert(DistortionMat, &_k);

	//������ʼ��
	int istep;
	int istep1;
	//������ͶӰ
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

		//������ͶӰ
		for (int points = 0; points < iIPointsCounts; points++)
		{
			fpdata = pobjecvPoints->data.fl;
			istep = pobjecvPoints->step / sizeof(float);
			//��������
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

	//����ͶӰ���¼���ļ�
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
	//ͼƬ��ȡ·��
	char LeftReadDirectory[] = "D:\\�ĵ�\\QQ�ļ�\\Calibrate 7.17\\Calibrate 7.17\\Calibrate 7.17\\2015.6.14\\CirCleCalibrate\\ChessboardImg\\LeftCamera\\";
	//ͼ�񱣴�·��
	char LeftSaveDirectory[] = "D:\\�ĵ�\\QQ�ļ�\\Calibrate 7.17\\Calibrate 7.17\\2015.6.14\\CirCleCalibrate\\LeftCirclePoints\\";
	char FileName[200] = "";

	//�궨ͼ�����
	int m_iImgCounts = 12;
	//����
	int rowcount = 6;
	int Colcount = 10;
	//���̸����Ŀ
	int ChessboardCount = 1;
	int iPointCounts = rowcount*Colcount;

	//���ݱ궨��Ŀ�������ʼ�������ڲ���
	InitCameraParamsSingle(m_iImgCounts, iPointCounts, CHESSNUM);

	int* piCornerPointNum_Left = NULL;				// ÿ����ͼ���а����Ľǵ��������ָ�룬�����е�ÿһ��Ԫ�ش��һ����ͼ�����������Ľǵ�ĸ�����
	int* piCornerPointNum_Right = NULL;				// ÿ����ͼ���а����Ľǵ��������ָ�룬�����е�ÿһ��Ԫ�ش��һ����ͼ�����������Ľǵ�ĸ�����
	float* pfObjectPoints = NULL;					// �ǵ���������ָ��

	piCornerPointNum_Left = new int[m_iImgCounts];																// ���ÿһ����ͼ���нǵ�ĸ���ֵ
	piCornerPointNum_Right = new int[m_iImgCounts];																// ���ÿһ����ͼ���нǵ�ĸ���ֵ
	pfObjectPoints = new float[m_iImgCounts*ChessboardCount * iPointCounts * 3];				// ��Žǵ����������

	///////////////����������
	InitObjectPoints(pfObjectPoints, ChessboardCount, m_iImgCounts, m_pcvObjectPoints);

	vector<KeyPoint> key_points;
	vector<ST_CIRCLE_POINT>Circle_Ponits;

	//�������õĵ�
	VEC_POINTS Computer_points(CHESSNUM);

	// ���̸�ԭ������
	ST_CIRCLE_POINT stUpperLeft;

	//���̸��ǵ�
	PST_ORGIN_POINTS pstOrginPoints;
	CvSize m_imageSize;
	IplImage*ptemp;

	//����Ŀ����궨
	for (int iimageCount = 0; iimageCount < m_iImgCounts; iimageCount++)
	{
		//��ȡͼƬ
		sprintf(FileName, "%sChessboardImg%d_L.bmp", LeftReadDirectory, iimageCount + 1); // д���ļ�������
		IplImage* img = cvLoadImage(FileName, -1);  //��ȡͼƬ

		if (img == NULL)   // ��ȡͼƬʧ�ܵ�ʱ�򷵻ؽ��
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

		//�����ֵ�ָ�
		//threshold =Otsu(img);
		//Ѱ�ұ�ǵ�Բ��
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

		//��Բ�Ļ���Բ��
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

		cout << "��¼�����ҵ��ı�ǵ����꣬����Ϊ�ļ�" << endl;
		//��¼�µ�һ����ϵ���������
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

		//������̸�ı�ǵ�����
		GetChessborardMark(Circle_Ponits, CHESSNUM, pstOrginPoints);
		//������̸��ԭ������
		GetChessboardOrginPoints(Circle_Ponits, pstOrginPoints, CHESSNUM);
		SortPoints(Computer_points, Circle_Ponits, pstOrginPoints, CHESSNUM, iPointCounts);

		//��˳����ͼ��
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

		//�����кõı�ǵ㸳ֵ������
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
					//������ֵ
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
	//����ֵ
	//vector<CIRCLE_POINT>Computer_points;

	for (int i = 0; i < CHESSNUM; i++)
	{
		cvSetData(m_pcvPointCounts_Left[i], piCornerPointNum_Left, sizeof(int));
	}

	////////////////////////////////������,���Ծ�����ͼ���������//////////////////////////////////////////
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
	////////////////////////////��ÿ��궨�壬���нǵ㶼��¼����//////////////////////////////////////////////
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
	//�Զ��궨��ֱ�궨���õ��������������ڲΣ����������
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
	/////////////////////////////ͼƬУ��///////////////////////////////////////////////
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

	//����궨���
	CalcReprjectionError(m_pcvObjectPoints[0], m_pcvIntrinsicMat_Left[1], m_pcvDistortionCoeffs_Left[1], m_pcvImgPoints_LeftReProjection, m_pcvRotationVct_Left[1], m_pcvTranslationVct_Left[1], iPointCounts, m_iImgCounts, 1);

	cvWaitKey(0);
}