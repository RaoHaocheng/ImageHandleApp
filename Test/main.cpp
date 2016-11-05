// ����Ŀֻ��Ϊ�˲�����Щ���Ƿ���Ա�ʹ��
// #include "..\CameraCalibration\CameraCalibration.h"
// #include "..\CameraCalibration\FindCircularMarker.h"
// #include <windows.h>
// #include <stdio.h>
// #include <tchar.h>
// #include <iostream>
// #include "..\BaseFunction\BaseFunction.h"
// 
// #define _CRTDBG_MAP_ALLOC
// 
// typedef unsigned char uchar;




#include <stdio.h>
#include <cv.h>
#include <highgui.h>  
#include <math.h>  
#include <vector>
#include <functional>
#include <algorithm>
//#include "define.h"
#include <iostream>  
#include <stack>
using namespace cv;
using namespace std;

typedef struct _NCHU_PIXEL_INFO
{
	int	 iId;						/// ��ı��
	bool bIsWhite;					/// ��ǰ�����Ƿ�Ϊ��ɫ��
	bool bIsVisited;				/// ��ǰ�����Ƿ��Ѿ�����ǹ�����������ͨ�����У�
	bool bIsSum;					/// ��ͳ�����������ص����ʱ�����ڱ�ʶ��ǰ���Ƿ�ͳ�ƹ�
}ST_PIXEL_INFO, *PST_PIXEL_INFO;

/// ������Ϣ
typedef struct _NCHU_REGION_INFO
{
	int iCounts;    				/// ���������ص����
}ST_REGION_INFO, *PST_REGION_INFO;

typedef struct _NCHU_IMAGE_INFO
{
	int iRegionCounts;				/// ͼ�����������
	PST_PIXEL_INFO pstPixelInfo;	/// ͼ����ÿ�����ص���Ϣ
	PST_REGION_INFO pstRegionInfo;	/// ͼ����������Ϣ
}ST_IMAGE_INFO, *PST_IMAGE_INFO;


typedef unsigned char BYTE;

/////////////////////////////��ͨ���ǣ�������������/////////////////////////////////////////////

bool LabelSet(IplImage* pImage, PST_PIXEL_INFO& pstPelsInfo, int iRowNum, int iColNum, int iLabel, int iMode)
{


	///int iWidth = pImage->width;									/// ͼ����
	int iHeight = pImage->height;								/// ͼ��߶�
	int iWidthStep = pImage->widthStep;							/// һ�е��ֽ���
	BYTE * pbyData = (BYTE*)pImage->imageData;					/// ͼ������
	int im, ip, jm, jp;											/// ��������
	pstPelsInfo[iRowNum * iWidthStep + iColNum].iId = iLabel;	/// ��������������������ͼ���б�ǵ�ǰ��
	int xs, ys;													///��ǰ�������

	stack<int> labelStack;										/// �ն�ջ  �洢Ϊ��ǵ�ı��

	int iId = iRowNum * iWidthStep + iColNum;					/// ��ǰ���ص�ı��

	labelStack.push(iId);										/// ����ǰ��ѹ���ջ

	while (labelStack.empty() == false)
	{
		iId = labelStack.top();					/// ��ȡջ��ֵ
		labelStack.pop();						/// ɾ��ջ��Ԫ��
		xs = iId % iWidthStep;					/// ��ȡ��ǰ���X����
		ys = iId / iWidthStep;					/// ��ȡ��ǰ���Y����

		/// ��ǰ�����������
		im = xs - 1;
		ip = xs + 1;
		jm = ys - 1;
		jp = ys + 1;

		/// �����߽�
		if (im < 0)
			im = 0;
		if (ip >= iWidthStep)
			ip = iWidthStep - 1;
		if (jm < 0)
			jm = 0;
		if (jp >= iHeight)
			jp = iHeight - 1;

		if (*(pbyData + jm * iWidthStep + im) == 255 && (pstPelsInfo[jm * iWidthStep + im].iId == 0) && iMode == 8)	// ����
		{
			pstPelsInfo[jm * iWidthStep + im].iId = iLabel;
			pstPelsInfo[jm * iWidthStep + im].bIsVisited = TRUE;
			iId = jm * iWidthStep + im;								/// ��ǰ���ص�ı��
			labelStack.push(iId);									/// �����ѹ���ջ
		}

		if (*(pbyData + jm * iWidthStep + xs) == 255 && (pstPelsInfo[jm * iWidthStep + xs].iId == 0))	// ��
		{
			pstPelsInfo[jm * iWidthStep + xs].iId = iLabel;
			pstPelsInfo[jm * iWidthStep + xs].bIsVisited = TRUE;
			iId = jm * iWidthStep + xs;								/// ��ǰ���ص�ı��
			labelStack.push(iId);									///�����ѹ���ջ
		}

		if (*(pbyData + jm * iWidthStep + ip) == 255 && (pstPelsInfo[jm * iWidthStep + ip].iId == 0) && iMode == 8)	// ����
		{
			pstPelsInfo[jm * iWidthStep + ip].iId = iLabel;
			pstPelsInfo[jm * iWidthStep + ip].bIsVisited = TRUE;
			iId = jm * iWidthStep + ip;								/// ��ǰ���ص�ı��
			labelStack.push(iId);									///�����ѹ���ջ
		}

		if (*(pbyData + ys * iWidthStep + im) == 255 && (pstPelsInfo[ys * iWidthStep + im].iId == 0))	// ��
		{
			pstPelsInfo[ys * iWidthStep + im].iId = iLabel;
			pstPelsInfo[ys * iWidthStep + im].bIsVisited = TRUE;
			iId = ys * iWidthStep + im;								/// ��ǰ���ص�ı��
			labelStack.push(iId);									/// �����ѹ���ջ
		}

		if (*(pbyData + ys * iWidthStep + ip) == 255 && (pstPelsInfo[ys * iWidthStep + ip].iId == 0))	// ��
		{
			pstPelsInfo[ys * iWidthStep + ip].iId = iLabel;
			pstPelsInfo[ys * iWidthStep + ip].bIsVisited = TRUE;
			iId = ys * iWidthStep + ip;								/// ��ǰ���ص�ı��
			labelStack.push(iId);									/// �����ѹ���ջ
		}

		if (*(pbyData + jp * iWidthStep + im) == 255 && (pstPelsInfo[jp * iWidthStep + im].iId == 0) && iMode == 8)	// ����
		{
			pstPelsInfo[jp * iWidthStep + im].iId = iLabel;
			pstPelsInfo[jp * iWidthStep + im].bIsVisited = TRUE;
			iId = jp * iWidthStep + im;								/// ��ǰ���ص�ı��
			labelStack.push(iId);									///�����ѹ���ջ
		}

		if (*(pbyData + jp * iWidthStep + xs) == 255 && (pstPelsInfo[jp * iWidthStep + xs].iId == 0))	/// ��
		{
			pstPelsInfo[jp * iWidthStep + xs].iId = iLabel;
			pstPelsInfo[jp * iWidthStep + xs].bIsVisited = TRUE;
			iId = jp * iWidthStep + xs;								/// ��ǰ���ص�ı��
			labelStack.push(iId);									/// �����ѹ���ջ
		}

		if (*(pbyData + jp * iWidthStep + ip) == 255 && (pstPelsInfo[jp * iWidthStep + ip].iId == 0) && iMode == 8)	/// ����
		{
			pstPelsInfo[jp * iWidthStep + ip].iId = iLabel;
			pstPelsInfo[jp * iWidthStep + ip].bIsVisited = TRUE;
			iId = jp * iWidthStep + ip;								/// ��ǰ���ص�ı��
			labelStack.push(iId);									/// �����ѹ���ջ
		}
	}
	return TRUE;
}



/////////////////////////��ͨ���� ȥ��������ͨ��/////////////////////////////////////////////////

int Sign(IplImage* pImage, PST_IMAGE_INFO& pstImageInfo, int iMode)
{


	///int iWidth = pImage->width;						/// ͼ����
	int iHeight = pImage->height;					/// ͼ��߶�
	int iWidthStep = pImage->widthStep;				/// һ�е��ֽ���
	BYTE * pbyData = (BYTE*)pImage->imageData;		/// ͼ������

	if (!pImage || (iMode != 4 && iMode != 8))
		return 0;

	int iLabel = 1;					/// ��ǳ�ʼֵ
	int iRegionCounts = 0;			/// ��������

	for (int j = 0; j < iHeight; j++)
	{
		for (int i = 0; i < iWidthStep; i++)
		{
			/// �ж�����ͼ���еĵ�ǰ���Ƿ�Ϊ1,������������ͼ�����Ƿ�û�б��
			if (*(pbyData + j * iWidthStep + i) == 255 && pstImageInfo->pstPixelInfo[j * iWidthStep + i].iId == 0)
			{
				/// �����ǰ��Ϊ��ɫ��,����û����������ͼ���б������ (��0��)
				/// ��ӵ�ǰ�㿪ʼ��������Ѱ��δ��ǵģ�������ͼ����Ϊ1�ĵ㣬��������
				if (!LabelSet(pImage, pstImageInfo->pstPixelInfo, j, i, iLabel, iMode))
					return 0;

				iLabel++;  /// �����ż�1
			}
		}
	}

	/// ��������
	iRegionCounts = iLabel - 1;

	/// ͳ��ͬһ���������ص�ĸ���
	int* piReigon = new int[iLabel - 1];
	memset(piReigon, 0, sizeof(int)* (iLabel - 1));
	for (int j = 0; j < iHeight; j++)
	{
		for (int i = 0; i < iWidthStep; i++)
		{
			if (0 == pstImageInfo->pstPixelInfo[j * iWidthStep + i].iId)
				continue;

			piReigon[pstImageInfo->pstPixelInfo[j * iWidthStep + i].iId - 1]++;
		}
	}

	/// ɾ����������򣬲������������iId���±��
	int iID = 1;
	for (int n = 0; n < iLabel - 1; n++)
	{
		if (/*piReigon[n] > 600 ||*/ piReigon[n] < 300)
		{
			for (int i = 0; i < iHeight; i++)
			{
				for (int j = 0; j < iWidthStep; j++)
				{
					if (n == (pstImageInfo->pstPixelInfo[i * iWidthStep + j].iId - 1))
					{
						pstImageInfo->pstPixelInfo[i * iWidthStep + j].bIsWhite = FALSE;
						pstImageInfo->pstPixelInfo[i * iWidthStep + j].iId = 0;
						*(pImage->imageData + i * iWidthStep + j) = 0;
					}
				}
			}

			iRegionCounts--;
		}
		else
		{
			for (int i = 0; i < iHeight; i++)
			{
				for (int j = 0; j < iWidthStep; j++)
				{
					if (n == (pstImageInfo->pstPixelInfo[i * iWidthStep + j].iId - 1))
					{
						pstImageInfo->pstPixelInfo[i * iWidthStep + j].iId = iID;
					}
				}
			}
			iID++;
		}
	}

	delete[] piReigon;
	piReigon = NULL;

	pstImageInfo->iRegionCounts = iRegionCounts;
	cout << "region" << iRegionCounts << endl;
	return iRegionCounts;
}

/////////////////////////////��ʼ��������Ϣ ����ʼ���б��/////////////////////////////////////////////

void RegionSign(IplImage* &pImage, PST_IMAGE_INFO& pstImgInfo)
{

	int iHeight = pImage->height;
	///int iWidth = pImage->width;
	int iWithStep = pImage->widthStep;

	/// ��ʼ��ͼ��ÿ�����ص���Ϣ����
	pstImgInfo->pstPixelInfo = new ST_PIXEL_INFO[iHeight * iWithStep]; /// HACK:����Ϣ����ͷָ�룬��¼ͼ��ÿ����ĺڰס�ID���Ƿ��Ǳ�Ե����Ϣ���˴��ǵ��ͷ�

	for (int i = 0; i < iHeight; i++)
	{
		for (int j = 0; j < iWithStep; j++)
		{
			BYTE byPixVal = 0;

			// ָ��DIB��i�У���j�����ص�ָ��
			byPixVal = pImage->imageData[i * iWithStep + j];
			if (byPixVal == 255)
			{
				pstImgInfo->pstPixelInfo[i * iWithStep + j].bIsWhite = TRUE;
			}
			else
			{
				pstImgInfo->pstPixelInfo[i * iWithStep + j].bIsWhite = FALSE;
			}
			pstImgInfo->pstPixelInfo[i * iWithStep + j].iId = 0;
			pstImgInfo->pstPixelInfo[i * iWithStep + j].bIsVisited = FALSE;
			pstImgInfo->pstPixelInfo[i * iWithStep + j].bIsSum = FALSE;
		}
	}

	///8������ͨ����
	Sign(pImage, pstImgInfo, 8);
}

struct RotateLineInfo
{
	int Max = 0;							//��ɫ�������ֵ
	int rowwidth = 0;						//��Ӧ����
};

struct RotateImgInfo
{
	int angle = 0;
	int iheight = 0;
};

RotateLineInfo CutRow(IplImage *BinaryImage)
{
	// 	cvNamedWindow("BinaryImage");
	// 	cvShowImage("BinaryImage", BinaryImage);
	// 	cvWaitKey(0);
	RotateLineInfo Info;
	CvSize pSize;
	int Max = 0;
	pSize.width = BinaryImage->width;
	pSize.height = BinaryImage->height;
	int *rowwidth = new int[pSize.height];
	//��������rowwidth�����洢ÿ�а�ɫ������
	memset(rowwidth, 0, pSize.height*sizeof(int));

	double  val;
	for (int i = pSize.height*0.3; i < pSize.height*0.70; i++)
	{
		for (int j = 0; j < pSize.width; j += 2)
		{
			val = cvGetReal2D(BinaryImage, i, j);
			if (val > 0.0)
			{
				//ͳ�ư�ɫ������
				rowwidth[i] = rowwidth[i] + 1;
			}

		}
		if (rowwidth[i] > Max)
		{
			Max = rowwidth[i];
			Info.Max = Max;
			Info.rowwidth = i;
		}
	}
	return Info;
}

int Row(IplImage *BinaryImage, int &iheight)
{
	// 	cvNamedWindow("BinaryImage");
	// 	cvShowImage("BinaryImage", BinaryImage);
	// 	cvWaitKey(0);
	CvSize pSize;
	pSize.width = BinaryImage->width;
	pSize.height = BinaryImage->height;
	int length = BinaryImage->height - iheight;
	int *rowwidth = new int[length];
	//��������rowwidth�����洢ÿ�а�ɫ������
	memset(rowwidth, 0, length*sizeof(int));

 	double  val;
 	for (int i = pSize.height - 1; i > iheight; i--)
 	{
 		for (int j = 0; j < pSize.width; j += 2)
 		{
 			val = cvGetReal2D(BinaryImage, i, j);
 			if (val > 0.0)
 			{
 				//ͳ�ư�ɫ������
 				rowwidth[i] = rowwidth[i] + 1;
 			}
 
 		}
 		if (rowwidth[i] > 30)
 		{
 			return i;
 			break;
 		}
 	}

	
	return BinaryImage->height;
}
//��תͼ�����ݲ��䣬�ߴ���Ӧ���  
IplImage* rotateImage1(IplImage* img, int&degree, RotateImgInfo & rotateImgInfo)
{
	RotateLineInfo Info;
	float map[6];
	CvMat map_matrix = cvMat(2, 3, CV_32F, map);
	//IplImage* img_Rotate = NULL;
	int iheight = 0;
	int dheight = 0;
	int WhiteMax = 0;
	CvSize size;
	for (int i = -degree; i <= degree; i++)
	{
		double angle = i  * CV_PI / 180.; // ����    
		double a = sin(angle), b = cos(angle);
		int width = img->width;
		int height = img->height;
		int width_rotate = int(height * fabs(a) + width * fabs(b));
		int height_rotate = int(width * fabs(a) + height * fabs(b));
		//��ת����map  
		// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]  
		// [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]  

		// ��ת����  
		CvPoint2D32f center = cvPoint2D32f(width / 2, height / 2);
		cv2DRotationMatrix(center, i, 1.0, &map_matrix);
		map[2] += (width_rotate - width) / 2;
		map[5] += (height_rotate - height) / 2;
		IplImage*img_rotate = NULL;
		img_rotate = cvCreateImage(cvSize(width_rotate, height_rotate), 8, 1);
		cvWarpAffine(img, img_rotate, &map_matrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, cvScalarAll(0));
		// 		cvNamedWindow("img_rotate");
		// 		cvShowImage("img_rotate", img_rotate);
		// 		cvWaitKey(0);
		Info = CutRow(img_rotate);
		if (Info.Max > WhiteMax)
		{
			WhiteMax = Info.Max;
			rotateImgInfo.iheight = Info.rowwidth;
			size = cvGetSize(img_rotate);
			rotateImgInfo.angle = i;
			iheight = Info.rowwidth;

		}
		cvReleaseImage(&img_rotate);
	}
	double angle = rotateImgInfo.angle  * CV_PI / 180.; // ����    
	int width = img->width;
	int height = img->height;
	int width_rotate = size.width;
	int height_rotate = size.height;
	//��ת����map  
	// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]  
	// [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]  

	// ��ת����  
	CvPoint2D32f center = cvPoint2D32f(width / 2, height / 2);
	cv2DRotationMatrix(center, rotateImgInfo.angle, 1.0, &map_matrix);
	map[2] += (width_rotate - width) / 2;
	map[5] += (height_rotate - height) / 2;
	IplImage*img_Rotate = cvCreateImage(cvSize(width_rotate, height_rotate), 8, 1);
	cvWarpAffine(img, img_Rotate, &map_matrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, cvScalarAll(0));


	CvPoint pt3, pt4;
	CvPoint pt1, pt2;
	pt1.x = 0;
	pt1.y = iheight;
	pt2.x = img_Rotate->width;
	pt2.y = iheight;
	cvLine(img_Rotate, pt1, pt2, CV_RGB(0, 0, 255), 2);

	cvNamedWindow("cvLine");
	cvShowImage("cvLine", img_Rotate);
	cvWaitKey(0);

 	IplImage* BinaryImage = cvCreateImage(cvGetSize(img_Rotate), 8, 1);
 	cvCopyImage(img_Rotate, BinaryImage);
 	dheight = Row(BinaryImage, iheight);
	//dheight = 30;
 	//int height2 = 407;
 	pt3.x = 0;
 	pt3.y = dheight;
 	pt4.x = img_Rotate->width;
 	pt4.y = dheight;
 
 	cvLine(img_Rotate, pt3, pt4, CV_RGB(0, 0, 255), 2);
 
 	cvNamedWindow("pt3");
 	cvShowImage("pt3", img_Rotate);
 	cvWaitKey(0);

	//img_Rotate = cvCreateImage(size, 8, 1);
	//��ͼ��������任  
	//CV_WARP_FILL_OUTLIERS - ����������ͼ������ء�  
	//�������������������ͼ��ı߽��⣬��ô���ǵ�ֵ�趨Ϊ fillval.  
	//CV_WARP_INVERSE_MAP - ָ�� map_matrix �����ͼ������ͼ��ķ��任��  
	//cvReleaseImage(&img_rotate);


	return img_Rotate;
}


double GetDistan(CvPoint* &line)
{
	int xd = (line[0].x - line[1].x);
	int yd = (line[0].y - line[1].y);
	double dit = pow(xd, 2) + pow(yd, 2);
	double Dis = sqrt(dit);
	return Dis;
}

CvPoint* GetLine(vector<CvPoint*> &VLine, double Dist)
{
	int Length = VLine.size();
	for (int i = 0; i < Length; i++)
	{
		int xd = (VLine[i][0].x - VLine[i][1].x);
		int yd = (VLine[i][0].y - VLine[i][1].y);
		double dit = pow(xd, 2) + pow(yd, 2);
		double Dis = sqrt(dit);
		if (Dis == Dist)
		{
			return VLine[i];
		}
	}
	return NULL;
}

struct MaxLine
{
	double distance = 0.0;
	double angle = 0.0;
	int Slinex = 0;
	int Sliney = 0;
	int Elinex = 0;
	int Eliney = 0;
};

int main()
{
	int num = 1;
	char scene_filename[180] = "";
	char save_filename[180] = "";
	char save1_filename[180] = "";
	for (size_t i = 0; i < num; i++)
	{
		sprintf(scene_filename, " ");
		sprintf(scene_filename, "C:\\Users\\haocheng\\Desktop\\(%d).jpg", i + 1);

		IplImage* src = cvLoadImage(scene_filename, 1);						  //���ز���ͼ���ļ�
		IplImage* dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);		  //���ز���ͼ���ļ�

		IplImage* r = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);		  //����R��ͨ��ͼ��
		IplImage* g = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);		  //����G��ͨ��ͼ��
		IplImage* b = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);		  //����B��ͨ��ͼ��

		IplImage*	imageCanny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage*	imageCanny1 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage*	imageCanny2 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage*	imageCanny3 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

		IplImage*	dst1 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage*	dst2 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage*	dst3 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);



		float high[9] = { -1, -1, -1, -1, 9, -1, -1, -1, -1 };//��ͨ�˲���
		CvMat km = cvMat(3, 3, CV_32FC1, high);  //���쵥ͨ��������󣬽�ͼ��IplImage�ṹת��Ϊͼ������ 
		cvSmooth(src, src, CV_GAUSSIAN, 7);

		cvSplit(src, r, g, b, NULL);

		cvCvtColor(src, dst, CV_BGR2GRAY);

		cvCanny(dst, imageCanny, 20, 80, 3);							//����Canny���ӽ��б�Ե���Ͷ�ֵ������
		cvCanny(r, imageCanny1, 20, 80, 3);							//����Canny���ӽ��б�Ե���Ͷ�ֵ������
		cvCanny(g, imageCanny2, 20, 80, 3);							//����Canny���ӽ��б�Ե���Ͷ�ֵ������
		cvCanny(b, imageCanny3, 20, 80, 3);							//����Canny���ӽ��б�Ե���Ͷ�ֵ������


		cvOr(imageCanny1, imageCanny3, dst1);
		cvOr(dst1, imageCanny2, dst2);
		cvOr(dst2, imageCanny, dst3);



		PST_IMAGE_INFO  pstImgInfo = new ST_IMAGE_INFO;
		RegionSign(dst3, pstImgInfo);

		// 		cvNamedWindow("RegionSign");
		// 		cvShowImage("RegionSign", dst5);
		// 		cvWaitKey(0);


		int degree = 15;
		RotateImgInfo rotateImgInfo;
		IplImage*RotateImage = rotateImage1(dst3, degree, rotateImgInfo);

		IplImage*res = cvCreateImage(cvGetSize(RotateImage), 8, 3);
		double angle = rotateImgInfo.angle  * CV_PI / 180.; // ����    
		int width = src->width;
		int height = src->height;
		int width_rotate = res->width;
		int height_rotate = res->height;

		// ��ת���� 
		float map[6];
		CvMat map_matrix = cvMat(2, 3, CV_32F, map);
		CvPoint2D32f center = cvPoint2D32f(width / 2, height / 2);
		cv2DRotationMatrix(center, rotateImgInfo.angle, 1.0, &map_matrix);
		map[2] += (width_rotate - width) / 2;
		map[5] += (height_rotate - height) / 2;
		IplImage*img_Rotate = cvCreateImage(cvSize(width_rotate, height_rotate), 8, 1);
		cvWarpAffine(src, res, &map_matrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, cvScalarAll(0));

		// 		cvNamedWindow("RotateImage");
		// 		cvShowImage("RotateImage", RotateImage);
		// 		cvWaitKey(0);
		// 			  
		// 		CvMemStorage *pcvMStorage = cvCreateMemStorage();
		// 		     
		// 		double fRho = 1;
		// 		     
		// 		double fTheta = CV_PI / 180;
		// 		     
		// 		int nMaxLineNumber = 50;   // 
		// 		     
		// 		double fMinLineLen = 80;   //��С�߶γ���  
		// 		     
		// 		double fMinLineGap = 15;   //��С�߶μ��  
		// 		     
		// 		CvSeq *pcvSeqLines = cvHoughLines2(RotateImage
		// 		    , pcvMStorage, CV_HOUGH_PROBABILISTIC, fRho, fTheta, nMaxLineNumber, fMinLineLen, fMinLineGap);
		// 
		// 		vector<CvPoint*> RLine;							//����ֱ��
		// 		vector<CvPoint*> LLine;							//����ֱ��
		// 		vector<CvPoint*> ULine;							//�϶�ֱ��
		// 		vector<CvPoint*> DLine;							//�¶�ֱ��
		// 		MaxLine RightLine, LeftLine, UpLine, DownLine;
		// 
		// 		    for (int n = 0; n < pcvSeqLines->total; n++)	      
		// 		    {
		// 		      	CvPoint* line = (CvPoint*)cvGetSeqElem(pcvSeqLines, n);
		// 		      	double tanValue = 0.0;
		// 		      	double angle = 0.0;
		// 		      	tanValue = atan2(line[1].y - line[0].y, line[1].x - line[0].x);
		// 		      	angle = tanValue * 57.29577951;
		// 				double distance = GetDistan(line);
		// 		      
		// 		      	if (angle >= 0)
		// 		      	{
		// 		      		if (angle > 75)
		// 		      		{
		// 		      			LLine.push_back(line);
		// 		      
		// 		      			if (distance>LeftLine.distance)
		// 		      			{
		// 							LeftLine.distance = distance;
		// 							LeftLine.angle = angle;
		// 							LeftLine.Slinex = line[0].x;
		// 							LeftLine.Sliney = line[0].y;
		// 							LeftLine.Elinex = line[1].x;
		// 							LeftLine.Eliney = line[1].y;
		// 		      			}
		// 		      			cvLine(src, line[0], line[1], CV_RGB(255, 0, 0), 2);
		// 		      		}
		// 		      		else if (angle <= 10)
		// 		      		{
		// 						if ((abs(line[0].y - rotateImgInfo.iheight)<5) && (abs(line[1].y - rotateImgInfo.iheight)<5))
		// 						{
		// 							ULine.push_back(line);
		// 							if (distance>UpLine.distance)
		// 							{
		// 								UpLine.distance = distance;
		// 								UpLine.angle = angle;
		// 								UpLine.Slinex = line[0].x;
		// 								UpLine.Sliney = line[0].y;
		// 								UpLine.Elinex = line[1].x;
		// 								UpLine.Eliney = line[1].y;
		// 							}
		// 						}
		// 						else if ((line[0].y - rotateImgInfo.iheight>30) && (line[1].y - rotateImgInfo.iheight>30))
		// 						{
		// 							DLine.push_back(line);
		// 							if (distance > DownLine.distance)
		// 							{
		// 								DownLine.distance = distance;
		// 								DownLine.angle = angle;
		// 								DownLine.Slinex = line[0].x;
		// 								DownLine.Sliney = line[0].y;
		// 								DownLine.Elinex = line[1].x;
		// 								DownLine.Eliney = line[1].y;
		// 							}
		// 						}
		// 						
		// 		      			cvLine(src, line[0], line[1], CV_RGB(0, 255, 0), 2);
		// 		      		}
		// 		      	}
		// 		      	else if (angle < 0)
		// 		      	{
		// 		      		if (angle <= -75)
		// 		      		{
		// 		      			RLine.push_back(line);
		// 						if (distance > RightLine.distance)
		// 						{
		// 							RightLine.distance = distance;
		// 							RightLine.angle = angle;
		// 							RightLine.Slinex = line[0].x;
		// 							RightLine.Sliney = line[0].y;
		// 							RightLine.Elinex = line[1].x;
		// 							RightLine.Eliney = line[1].y;
		// 						}
		// 		      			cvLine(src, line[0], line[1], CV_RGB(0, 0, 255), 2);
		// 		      		}
		// 		      		else if (angle > -10)
		// 		      		{
		// 						if ((abs(line[0].y - rotateImgInfo.iheight) < 5) && (abs(line[1].y - rotateImgInfo.iheight) < 5))
		// 						{
		// 							ULine.push_back(line);
		// 							if (distance > UpLine.distance)
		// 							{
		// 								UpLine.distance = distance;
		// 								UpLine.angle = angle;
		// 								UpLine.Slinex = line[0].x;
		// 								UpLine.Sliney = line[0].y;
		// 								UpLine.Elinex = line[1].x;
		// 								UpLine.Eliney = line[1].y;
		// 							}
		// 						}
		// 						else if ((line[0].y - rotateImgInfo.iheight > 30) && (line[1].y - rotateImgInfo.iheight > 30))
		// 						{
		// 							DLine.push_back(line);
		// 							if (distance > DownLine.distance)
		// 							{
		// 								DownLine.distance = distance;
		// 								DownLine.angle = angle;
		// 								DownLine.Slinex = line[0].x;
		// 								DownLine.Sliney = line[0].y;
		// 								DownLine.Elinex = line[1].x;
		// 								DownLine.Eliney = line[1].y;
		// 							}
		// 						}
		// 		      			cvLine(src, line[0], line[1], CV_RGB(0, 255, 0), 2);
		// 		      		}
		// 		      	}
		// 		    }
		// 
		// 			int RLength = RLine.size();
		// 			int LLength = LLine.size();
		// 			int ULength = ULine.size();
		// 			int DLength = DLine.size();
		// 
		// 			if (ULength!=0)
		// 			{
		// 				CvPoint*line = GetLine(ULine, UpLine.distance);
		// 				for (int i = 0; i < ULength; i++)
		// 				{
		// 					if ((abs(ULine[i][0].y - rotateImgInfo.iheight) < 5) && (abs(ULine[i][1].y - rotateImgInfo.iheight) < 5))
		// 					{
		// 						if (ULine[i][0].x<line[0].x)
		// 						{
		// 							line[0].x = ULine[i][0].x;
		// 							if (ULine[i][0].y<line[0].y)
		// 							{
		// 								line[0].y = ULine[i][0].y;
		// 							}
		// 						}
		// 						else if (ULine[i][1].x>line[1].x)
		// 						{
		// 							line[1].x = ULine[i][1].x;
		// 							if (ULine[i][1].y < line[1].y)
		// 							{
		// 								line[1].y = ULine[i][1].y;
		// 							}
		// 						}
		// 						double distance = GetDistan(line);
		// 						UpLine.distance = distance;
		// 						UpLine.Slinex = line[0].x;
		// 						UpLine.Sliney = line[0].y;
		// 						UpLine.Elinex = line[1].x;
		// 						UpLine.Eliney = line[1].y;
		// 					}
		// 				}
		// 			}
		// 			


		sprintf(save_filename, " ");
		sprintf(save_filename, ".\\��ͨ����бУ��ͼ��\\(%d).jpg", i + 1);
		cvSaveImage(save_filename, RotateImage);

		printf("��ǰͼ��Ϊ��%d�ţ�\n", i + 1);

		cvReleaseImage(&src);
		cvReleaseImage(&dst);
		cvReleaseImage(&r);
		cvReleaseImage(&g);
		cvReleaseImage(&b);

		cvReleaseImage(&imageCanny);
		cvReleaseImage(&imageCanny1);
		cvReleaseImage(&imageCanny2);
		cvReleaseImage(&imageCanny3);


		cvReleaseImage(&dst1);
		cvReleaseImage(&dst2);
		cvReleaseImage(&dst3);

		//cvReleaseImage(&pColorImage);
		cvReleaseImage(&RotateImage);
	}

	return 0;
}






// LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException){
// 	return EXCEPTION_EXECUTE_HANDLER;
// }
// 
// //IplImage* pImage;
// //IplImage* pCannyImage;
// //IplImage* dst = NULL;
// 
// void on_trackbar(int threshold){
// 	//canny��Ե���  
// 	//    cvCanny(pImage, pCannyImage, threshold, threshold * 3, 3);
// 	//cvShowImage("canny", pCannyImage);
// 	//std::cout << "/*******************canny: " << threshold << std::endl;
// }
// int hello();
// 
// int f(void*m)
// {
// 	for (;;)
// 	{
// 		unsigned int x = *((unsigned int*)m);
// 		std::cout << x << std::endl;
// 	}
// 	return 0;
// }
// int f2(void*m)
// {
// 	unsigned int x = *((unsigned int*)m);
// 	std::cout << x << std::endl;
// 	return 0;
// }
// 
// 
// int main()
// {
// 	ThreadManage* t=ThreadManage::CreateInstance(1, 10);
// 	//for (;;)
// 	//{
// 	t->Execute(f); 
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 	t->Execute(f);
// 	Sleep(10);
// 
// 	Sleep(10);
// 	Sleep(10);
// 	Sleep(10);
// 		//std::cout << "hello" << std::endl;
// 		
// 	//}
// 
// 	system("pause");
// 	return 0;
// }
// 
// 
// int mainx()
// {
// 	// ���ڼ���ڴ�й¶
// 	_CrtDumpMemoryLeaks();
// 
// #ifdef _DEBUG
// #ifdef  _WIN64
// 	SetDllDirectory("..\\DLLs\\x64\\Debug\\");
// #else
// 	SetDllDirectory("..\\DLLs\\x86\\Debug\\");
// #endif //  _win64
// #else
// #ifdef  _WIN64
// 	SetDllDirectory("..\\DLLs\\x64\\Release\\");
// #else
// 	SetDllDirectory("..\\DLLs\\x86\\Release\\");
// #endif //  _win64
// #endif // DEBUG
// 
// 
// 	cv::SimpleBlobDetector::Params params;
// 	params.filterByArea = true;
// 	params.filterByCircularity = true;
// 	params.filterByInertia = true;
// 	params.filterByConvexity = true;
// 	params.filterByColor = false;
// 	params.minThreshold = 5;
// 	params.thresholdStep = 5;
// 	params.minArea = 40;
// 	params.minConvexity = 0.8f;
// 	params.minInertiaRatio = 0.73f;
// 	params.minCircularity = 0.8f;
// 	params.maxArea = 10000000000;
// 	params.blobColor = 0;
// 	params.maxThreshold = 65;
// 	params.maxConvexity = 1.2f;
// 	params.maxCircularity = 1.2f;
// 	params.maxInertiaRatio = 1.2f;
// 	params.minDistBetweenBlobs = 1;
// 
// 	// ���ڴ������
// 	std::vector<ST_CENTER> centers;
// 	std::vector<cv::KeyPoint> Vct_ponits;
// 	FindCircularMarker f;
// 
// 	// const char* rootFile = ".\\Img\\";
// 	const char* rootFile = "C:\\Users\\haocheng\\Desktop\\Img\\";
// 
// 
// 	IplImage* pImage;
// 	cv::Mat keypointsImage;
// 
// 	char* filePath = new char[255];
// 	for (int i = 1; i <= 50; i++)
// 	{
// 		ZeroMemory(filePath, 255);
// 		sprintf(filePath, "%s0 (%d).bmp", rootFile, i);
// 
// 		//pImage = cvLoadImage("C:\\Users\\haocheng\\Desktop\\Lena.bmp", -1);
// 		pImage = cvLoadImage(filePath, -1);
// 
// 		if (!pImage){
// 			std::cout << "ͼƬ�޷��򿪣�����" << std::endl;
// 			break;
// 		}
// 
// 		cvShowImage("src", pImage);
// 		cvWaitKey(0);
// 
// 		f.FindCircleByWaterThed(pImage, params, centers, TRUE);
// 		//         cv::Mat contoursImage;
// 		//         cvtColor(cv::Mat(pImage), contoursImage, CV_GRAY2RGB);
// 		//         std::vector < std::vector<cv::Point> > contours; // ������ǵ�洢��λ��
// 		//         // ����ͼƬ
// 		//         findContours(cv::Mat(pImage).clone(), contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
// 		// 
// 		//         drawContours(contoursImage, contours, -1, cv::Scalar(255, 0, 0));
// 		//         imshow("ContourImg", contoursImage);
// 		//         cvWaitKey(0);
// 
// 
// 		// �����˹�����Ĵ���
// 		//for (int j = 0; j < 10; j++)
// 		//    {
// 		//             IplImage* dst = AddGaussianNoise(pImage, 0, i / 50.0);
// 		//             sprintf(filePath, "%s���Խ��\\0 (%d).bmp", rootFile, i);
// 		//             cvSaveImage(filePath, dst);
// 		//}
// 		//cvShowImage("dst",dst);
// 		//cvWaitKey(0);
// 
// 		std::cout << "*********************��" << i << "��********************" << std::endl;
// 		std::cout << "***********************ʱ�����*************************" << std::endl;
// 		// ��������Բ�ķ���
// 		f.FindCircleBySamplingTriangles(pImage, centers, false);
// 		std::cout << "������������η����ҵ���" << centers.size() << std::endl;
// 
// 		cvtColor(cv::Mat(pImage), keypointsImage, CV_GRAY2RGB);
// 		for (int j = 0; j < (int)centers.size(); j++)
// 		{
// 			ST_CENTER center;
// 			center = centers.at(j);
// 			cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // ��һ��Բ 
// 			cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // ��һ��Բ 
// 		}
// 		sprintf(filePath, "%s���Խ��\\3 (%d).bmp", rootFile, i);
// 		cvSaveImage(filePath, &IplImage(keypointsImage));
// 
// 		sprintf(filePath, "%s���Խ��\\1 (%d).bmp", rootFile, i);
// 		f.SetFileName(filePath);
// 		// ���ǵķ���
// 		f.FindCircleByCICImproved(pImage, params, centers, false);
// 		std::cout << "CIC�����ҵ���" << centers.size() << std::endl;
// 		// ʦ�ֵķ���
// 		//    f.FindCircleCenter(pImage, params, Vct_ponits, false);
// 		std::cout << "��ֵ�ָ�ҵ���" << Vct_ponits.size() << std::endl;
// 		std::cout << "***********************Ч��չʾ**************************" << std::endl;
// 		// ��������Բ�ķ���
// 		//f.FindCircleBySamplingTriangles(pImage, centers, true);
// 		// ���ǵķ���
// 		f.FindCircleByCICImproved(pImage, params, centers, true);
// 
// 		cvtColor(cv::Mat(pImage), keypointsImage, CV_GRAY2RGB);
// 		for (int j = 0; j < (int)centers.size(); j++)
// 		{
// 			ST_CENTER center;
// 			center = centers.at(j);
// 			cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // ��һ��Բ 
// 			cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // ��һ��Բ 
// 		}
// 		sprintf(filePath, "%s���Խ��\\2 (%d).bmp", rootFile, i);
// 		cvSaveImage(filePath, &IplImage(keypointsImage));
// 
// 		// ʦ�ֵķ���
// 		//f.FindCircleCenter(pImage, params, Vct_ponits, true);
// 	}
// 
// 	delete[] filePath;
// 
// 	system("pause");
// 
// 	return 0;
// }
// 

// �� DEPTH_8U�Ͷ�ֵͼ�����ϸ��  �����Zhang���п���ϸ���㷨  
// void thin(const cv::Mat &src, cv::Mat &dst, const int iterations)
// {
// 	const int height = src.rows - 1;
// 	const int width = src.cols - 1;
// 
// 	//����һ���������һ������  
// 	if (src.data != dst.data)
// 	{
// 		src.copyTo(dst);
// 	}
// 
// 
// 	int n = 0, i = 0, j = 0;
// 	cv::Mat tmpImg;
// 	uchar *pU, *pC, *pD;
// 	BOOL isFinished = FALSE;
// 
// 	for (n = 0; n < iterations; n++)
// 	{
// 		dst.copyTo(tmpImg);
// 		isFinished = FALSE;   //һ�� ���к���ɨ�� ��ʼ  
// 		//ɨ�����һ ��ʼ  
// 		for (i = 1; i<height; i++)
// 		{
// 			pU = tmpImg.ptr<uchar>(i - 1);
// 			pC = tmpImg.ptr<uchar>(i);
// 			pD = tmpImg.ptr<uchar>(i + 1);
// 			for (int j = 1; j<width; j++)
// 			{
// 				if (pC[j] > 0)
// 				{
// 					int ap = 0;
// 					int p2 = (pU[j] >0);
// 					int p3 = (pU[j + 1] > 0);
// 					if (p2 == 0 && p3 == 1)
// 					{
// 						ap++;
// 					}
// 					int p4 = (pC[j + 1] > 0);
// 					if (p3 == 0 && p4 == 1)
// 					{
// 						ap++;
// 					}
// 					int p5 = (pD[j + 1] > 0);
// 					if (p4 == 0 && p5 == 1)
// 					{
// 						ap++;
// 					}
// 					int p6 = (pD[j] > 0);
// 					if (p5 == 0 && p6 == 1)
// 					{
// 						ap++;
// 					}
// 					int p7 = (pD[j - 1] > 0);
// 					if (p6 == 0 && p7 == 1)
// 					{
// 						ap++;
// 					}
// 					int p8 = (pC[j - 1] > 0);
// 					if (p7 == 0 && p8 == 1)
// 					{
// 						ap++;
// 					}
// 					int p9 = (pU[j - 1] > 0);
// 					if (p8 == 0 && p9 == 1)
// 					{
// 						ap++;
// 					}
// 					if (p9 == 0 && p2 == 1)
// 					{
// 						ap++;
// 					}
// 					if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7)
// 					{
// 						if (ap == 1)
// 						{
// 							if ((p2*p4*p6 == 0) && (p4*p6*p8 == 0))
// 							{
// 								dst.ptr<uchar>(i)[j] = 0;
// 								isFinished = TRUE;
// 							}
// 
// 							//   if((p2*p4*p8==0)&&(p2*p6*p8==0))  
// 							//    {                             
// 							//         dst.ptr<uchar>(i)[j]=0;  
// 							//         isFinished =TRUE;                              
// 							//    }  
// 
// 						}
// 					}
// 				}
// 
// 			} //ɨ�����һ ����  
// 
// 
// 			dst.copyTo(tmpImg);
// 			//ɨ����̶� ��ʼ  
// 			for (i = 1; i<height; i++)  //һ�� ���к���ɨ�� ��ʼ  
// 			{
// 				pU = tmpImg.ptr<uchar>(i - 1);
// 				pC = tmpImg.ptr<uchar>(i);
// 				pD = tmpImg.ptr<uchar>(i + 1);
// 				for (int j = 1; j<width; j++)
// 				{
// 					if (pC[j] > 0)
// 					{
// 						int ap = 0;
// 						int p2 = (pU[j] >0);
// 						int p3 = (pU[j + 1] > 0);
// 						if (p2 == 0 && p3 == 1)
// 						{
// 							ap++;
// 						}
// 						int p4 = (pC[j + 1] > 0);
// 						if (p3 == 0 && p4 == 1)
// 						{
// 							ap++;
// 						}
// 						int p5 = (pD[j + 1] > 0);
// 						if (p4 == 0 && p5 == 1)
// 						{
// 							ap++;
// 						}
// 						int p6 = (pD[j] > 0);
// 						if (p5 == 0 && p6 == 1)
// 						{
// 							ap++;
// 						}
// 						int p7 = (pD[j - 1] > 0);
// 						if (p6 == 0 && p7 == 1)
// 						{
// 							ap++;
// 						}
// 						int p8 = (pC[j - 1] > 0);
// 						if (p7 == 0 && p8 == 1)
// 						{
// 							ap++;
// 						}
// 						int p9 = (pU[j - 1] > 0);
// 						if (p8 == 0 && p9 == 1)
// 						{
// 							ap++;
// 						}
// 						if (p9 == 0 && p2 == 1)
// 						{
// 							ap++;
// 						}
// 						if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7)
// 						{
// 							if (ap == 1)
// 							{
// 								//   if((p2*p4*p6==0)&&(p4*p6*p8==0))  
// 								//   {                             
// 								//         dst.ptr<uchar>(i)[j]=0;  
// 								//         isFinished =TRUE;                              
// 								//    }  
// 
// 								if ((p2*p4*p8 == 0) && (p2*p6*p8 == 0))
// 								{
// 									dst.ptr<uchar>(i)[j] = 0;
// 									isFinished = TRUE;
// 								}
// 
// 							}
// 						}
// 					}
// 
// 				}
// 
// 			} //һ�� ���к���ɨ�����            
// 			//�����ɨ�������û��ɾ���㣬����ǰ�˳�  
// 			if (isFinished == FALSE)
// 			{
// 				break;
// 			}
// 		}
// 
// 	}
// }




//  int main()
// {
// 
// 	IplImage* pImage;
// 	const char* rootFile = "C:\\Users\\haocheng\\Desktop\\1\\";
// 
// // 	pImage = cvLoadImage(rootFile, -1);
// // 
// // 	if (!pImage){
// // 		std::cout << "ͼƬ�޷��򿪣�����" << std::endl;
// // 		return 0;
// // 	}
// 
// // 	cv::Mat dst;
// // 	thin(cv::Mat(pImage), dst, 10);
// // 
// // 	imshow("FindCircle", dst);
// // 	cvWaitKey(0);
// 
// 
// 	IplImage *src;
// 	IplImage *distsrc;
// 	IplImage *out;
// 	IplImage *S00;
// 	IplImage *S45;
// 	IplImage *S90;
// 	IplImage *S135;
// 	CvMat kern00, kern45, kern90, kern135;
// 	float Smax = 0;
// 
// 	float L0[] = {
// 		-1, -1, -1, -1, -1,
// 		0, 0, 0, 0, 0,
// 		2, 2, 2, 2, 2,
// 		0, 0, 0, 0, 0,
// 		-1, -1, -1, -1, -1
// 	};
// 	float L45[] = {
// 		0, -1, -1, 0, 2,
// 		-1, -1, 0, 2, 0,
// 		-1, 0, 2, 0, -1,
// 		0, 2, 0, -1, -1,
// 		2, 0, -1, -1, 0
// 	};
// 	float L90[] = {
// 		-1, 0, 2, 0, -1,
// 		-1, 0, 2, 0, -1,
// 		-1, 0, 2, 0, -1,
// 		-1, 0, 2, 0, -1,
// 		-1, 0, 2, 0, -1
// 	};
// 	float L135[] = {
// 		2, 0, -1, -1, 0,
// 		0, 2, 0, -1, -1,
// 		-1, 0, 2, 0, -1,
// 		-1, -1, 0, 2, 0,
// 		0, -1, -1, 0, 2
// 	};
// 
// 	char* filePath = new char[255];
// 	for (int i = 1; i <= 44; i++)
// 	{
// 	
// 		ZeroMemory(filePath, 255);
// 		sprintf(filePath, "%s%d.bmp", rootFile, i);
// 
// 		src = cvLoadImage(filePath, 0);
// 		IplImage* dst = cvLoadImage(filePath, 0);
// 		distsrc = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
// 		S00 = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
// 		S45 = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
// 		S90 = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
// 		S135 = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
// 		out = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
// 		//kernel������������
// 		cvInitMatHeader(&kern00, 5, 5, CV_32FC1, L0);
// 		cvInitMatHeader(&kern45, 5, 5, CV_32FC1, L45);
// 		cvInitMatHeader(&kern90, 5, 5, CV_32FC1, L90);
// 		cvInitMatHeader(&kern135, 5, 5, CV_32FC1, L135);
// 		//����任
// 		cvDistTransform(src, distsrc, CV_DIST_L2, 5);
// 		//���ˣ���ʵ���Ǿ��
// 		cvFilter2D(distsrc, S00, &kern00);
// 		cvFilter2D(distsrc, S45, &kern45);
// 		cvFilter2D(distsrc, S90, &kern90);
// 		cvFilter2D(distsrc, S135, &kern135);
// 		//
// 		//Smax = MAX(S00,S45,S90,S135)
// 		//     / Smax, Smax >= 0
// 		// g = |
// 		//     \ 0 , others
// 		//
// 		for (int y = 0; y < out->height; y++){
// 			for (int x = 0; x< out->width; x++){
// 				Smax = MAX(
// 					MAX(((float*)(S00->imageData + y* S00->widthStep))[x], ((float*)(S45->imageData + y* S45->widthStep))[x]),
// 					MAX(((float*)(S90->imageData + y* S90->widthStep))[x], ((float*)(S135->imageData + y* S135->widthStep))[x]));
// 				((float*)(out->imageData + y* out->widthStep))[x] = Smax > 0 ? Smax : 0.0;
// 			}
// 		}
// 		cvThreshold(out, out, 7, 1, CV_THRESH_BINARY);
// 
// 		for (int y = 0; y < out->height; y++){
// 			for (int x = 0; x < out->width; x++){
// 				if (((float*)(out->imageData + y* out->widthStep))[x]> 0.1)
// 					((dst->imageData + y* dst->widthStep))[x] = 0;
// 			}
// 		}
// 		// 	cvNamedWindow("S00", 1);
// 		// 	cvNamedWindow("S45", 1);
// 		// 	cvNamedWindow("S90", 1);
// 		// 	cvNamedWindow("S135", 1);
// 		// 	cvNamedWindow("out", 1);
// 		// 	cvShowImage("S00", S00);
// 		// 	cvShowImage("S45", S45);
// 		// 	cvShowImage("S90", S90);
// 		// 	cvShowImage("S135", S135);
// 		cvShowImage("hello", dst);
// 	//	cvShowImage("out", out);
// 		cvWaitKey(0);

// 		cvCvtScaleAbs(S00, src, 32, 0);
// 		cvSaveImage("S00.png", src);
// 		cvCvtScaleAbs(S45, src, 32, 0);
// 		cvSaveImage("S45.png", src);
// 		cvCvtScaleAbs(S90, src, 32, 0);
// 		cvSaveImage("S90.png", src);
// 		cvCvtScaleAbs(S135, src, 32, 0);
// 		cvSaveImage("S135.png", src);
// 		cvCvtScaleAbs(out, src, 255, 0);
// 		cvSaveImage("", dst);

// 		//return 0;
// 	}
//}