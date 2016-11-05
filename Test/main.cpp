// 该项目只是为了测试这些苦是否可以被使用
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
	int	 iId;						/// 点的编号
	bool bIsWhite;					/// 当前像素是否为白色点
	bool bIsVisited;				/// 当前像素是否已经被标记过（用于四连通域标记中）
	bool bIsSum;					/// 在统计区域中像素点个数时，用于标识当前点是否统计过
}ST_PIXEL_INFO, *PST_PIXEL_INFO;

/// 区域信息
typedef struct _NCHU_REGION_INFO
{
	int iCounts;    				/// 区域中像素点个数
}ST_REGION_INFO, *PST_REGION_INFO;

typedef struct _NCHU_IMAGE_INFO
{
	int iRegionCounts;				/// 图像中区域个数
	PST_PIXEL_INFO pstPixelInfo;	/// 图像中每个像素点信息
	PST_REGION_INFO pstRegionInfo;	/// 图像中区域信息
}ST_IMAGE_INFO, *PST_IMAGE_INFO;


typedef unsigned char BYTE;

/////////////////////////////连通域标记（种子生长法）/////////////////////////////////////////////

bool LabelSet(IplImage* pImage, PST_PIXEL_INFO& pstPelsInfo, int iRowNum, int iColNum, int iLabel, int iMode)
{


	///int iWidth = pImage->width;									/// 图像宽度
	int iHeight = pImage->height;								/// 图像高度
	int iWidthStep = pImage->widthStep;							/// 一行的字节数
	BYTE * pbyData = (BYTE*)pImage->imageData;					/// 图像数据
	int im, ip, jm, jp;											/// 邻域坐标
	pstPelsInfo[iRowNum * iWidthStep + iColNum].iId = iLabel;	/// 符合条件，则在输出标记图像中标记当前点
	int xs, ys;													///当前点的坐标

	stack<int> labelStack;										/// 空堆栈  存储为标记点的标号

	int iId = iRowNum * iWidthStep + iColNum;					/// 当前像素点的编号

	labelStack.push(iId);										/// 将当前点压入堆栈

	while (labelStack.empty() == false)
	{
		iId = labelStack.top();					/// 获取栈顶值
		labelStack.pop();						/// 删除栈顶元素
		xs = iId % iWidthStep;					/// 获取当前点的X坐标
		ys = iId / iWidthStep;					/// 获取当前点的Y坐标

		/// 当前点的邻域坐标
		im = xs - 1;
		ip = xs + 1;
		jm = ys - 1;
		jp = ys + 1;

		/// 保护边界
		if (im < 0)
			im = 0;
		if (ip >= iWidthStep)
			ip = iWidthStep - 1;
		if (jm < 0)
			jm = 0;
		if (jp >= iHeight)
			jp = iHeight - 1;

		if (*(pbyData + jm * iWidthStep + im) == 255 && (pstPelsInfo[jm * iWidthStep + im].iId == 0) && iMode == 8)	// 左上
		{
			pstPelsInfo[jm * iWidthStep + im].iId = iLabel;
			pstPelsInfo[jm * iWidthStep + im].bIsVisited = TRUE;
			iId = jm * iWidthStep + im;								/// 当前像素点的编号
			labelStack.push(iId);									/// 将编号压入堆栈
		}

		if (*(pbyData + jm * iWidthStep + xs) == 255 && (pstPelsInfo[jm * iWidthStep + xs].iId == 0))	// 上
		{
			pstPelsInfo[jm * iWidthStep + xs].iId = iLabel;
			pstPelsInfo[jm * iWidthStep + xs].bIsVisited = TRUE;
			iId = jm * iWidthStep + xs;								/// 当前像素点的编号
			labelStack.push(iId);									///将编号压入堆栈
		}

		if (*(pbyData + jm * iWidthStep + ip) == 255 && (pstPelsInfo[jm * iWidthStep + ip].iId == 0) && iMode == 8)	// 右上
		{
			pstPelsInfo[jm * iWidthStep + ip].iId = iLabel;
			pstPelsInfo[jm * iWidthStep + ip].bIsVisited = TRUE;
			iId = jm * iWidthStep + ip;								/// 当前像素点的编号
			labelStack.push(iId);									///将编号压入堆栈
		}

		if (*(pbyData + ys * iWidthStep + im) == 255 && (pstPelsInfo[ys * iWidthStep + im].iId == 0))	// 左
		{
			pstPelsInfo[ys * iWidthStep + im].iId = iLabel;
			pstPelsInfo[ys * iWidthStep + im].bIsVisited = TRUE;
			iId = ys * iWidthStep + im;								/// 当前像素点的编号
			labelStack.push(iId);									/// 将编号压入堆栈
		}

		if (*(pbyData + ys * iWidthStep + ip) == 255 && (pstPelsInfo[ys * iWidthStep + ip].iId == 0))	// 右
		{
			pstPelsInfo[ys * iWidthStep + ip].iId = iLabel;
			pstPelsInfo[ys * iWidthStep + ip].bIsVisited = TRUE;
			iId = ys * iWidthStep + ip;								/// 当前像素点的编号
			labelStack.push(iId);									/// 将编号压入堆栈
		}

		if (*(pbyData + jp * iWidthStep + im) == 255 && (pstPelsInfo[jp * iWidthStep + im].iId == 0) && iMode == 8)	// 左下
		{
			pstPelsInfo[jp * iWidthStep + im].iId = iLabel;
			pstPelsInfo[jp * iWidthStep + im].bIsVisited = TRUE;
			iId = jp * iWidthStep + im;								/// 当前像素点的编号
			labelStack.push(iId);									///将编号压入堆栈
		}

		if (*(pbyData + jp * iWidthStep + xs) == 255 && (pstPelsInfo[jp * iWidthStep + xs].iId == 0))	/// 下
		{
			pstPelsInfo[jp * iWidthStep + xs].iId = iLabel;
			pstPelsInfo[jp * iWidthStep + xs].bIsVisited = TRUE;
			iId = jp * iWidthStep + xs;								/// 当前像素点的编号
			labelStack.push(iId);									/// 将编号压入堆栈
		}

		if (*(pbyData + jp * iWidthStep + ip) == 255 && (pstPelsInfo[jp * iWidthStep + ip].iId == 0) && iMode == 8)	/// 右下
		{
			pstPelsInfo[jp * iWidthStep + ip].iId = iLabel;
			pstPelsInfo[jp * iWidthStep + ip].bIsVisited = TRUE;
			iId = jp * iWidthStep + ip;								/// 当前像素点的编号
			labelStack.push(iId);									/// 将编号压入堆栈
		}
	}
	return TRUE;
}



/////////////////////////连通域标记 去除过大连通域/////////////////////////////////////////////////

int Sign(IplImage* pImage, PST_IMAGE_INFO& pstImageInfo, int iMode)
{


	///int iWidth = pImage->width;						/// 图像宽度
	int iHeight = pImage->height;					/// 图像高度
	int iWidthStep = pImage->widthStep;				/// 一行的字节数
	BYTE * pbyData = (BYTE*)pImage->imageData;		/// 图像数据

	if (!pImage || (iMode != 4 && iMode != 8))
		return 0;

	int iLabel = 1;					/// 标记初始值
	int iRegionCounts = 0;			/// 区域数量

	for (int j = 0; j < iHeight; j++)
	{
		for (int i = 0; i < iWidthStep; i++)
		{
			/// 判断输入图像中的当前点是否为1,并且在输出标记图像中是否没有标记
			if (*(pbyData + j * iWidthStep + i) == 255 && pstImageInfo->pstPixelInfo[j * iWidthStep + i].iId == 0)
			{
				/// 如果当前点为白色点,并且没有在输出标记图像中标记数字 (除0外)
				/// 则从当前点开始向其邻阈寻找未标记的，在输入图像中为1的点，并对其标记
				if (!LabelSet(pImage, pstImageInfo->pstPixelInfo, j, i, iLabel, iMode))
					return 0;

				iLabel++;  /// 标记序号加1
			}
		}
	}

	/// 区域数量
	iRegionCounts = iLabel - 1;

	/// 统计同一区域内像素点的个数
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

	/// 删除过大的区域，并将正常区域的iId重新编号
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

/////////////////////////////初始化区域信息 并开始进行标记/////////////////////////////////////////////

void RegionSign(IplImage* &pImage, PST_IMAGE_INFO& pstImgInfo)
{

	int iHeight = pImage->height;
	///int iWidth = pImage->width;
	int iWithStep = pImage->widthStep;

	/// 初始化图像每个像素的信息数组
	pstImgInfo->pstPixelInfo = new ST_PIXEL_INFO[iHeight * iWithStep]; /// HACK:点信息数组头指针，记录图像每个点的黑白、ID、是否是边缘等信息，此处记得释放

	for (int i = 0; i < iHeight; i++)
	{
		for (int j = 0; j < iWithStep; j++)
		{
			BYTE byPixVal = 0;

			// 指向DIB第i行，第j个象素的指针
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

	///8邻域连通域标记
	Sign(pImage, pstImgInfo, 8);
}

struct RotateLineInfo
{
	int Max = 0;							//白色像素最大值
	int rowwidth = 0;						//对应行数
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
	//建立数组rowwidth用来存储每行白色像素数
	memset(rowwidth, 0, pSize.height*sizeof(int));

	double  val;
	for (int i = pSize.height*0.3; i < pSize.height*0.70; i++)
	{
		for (int j = 0; j < pSize.width; j += 2)
		{
			val = cvGetReal2D(BinaryImage, i, j);
			if (val > 0.0)
			{
				//统计白色的像素
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
	//建立数组rowwidth用来存储每行白色像素数
	memset(rowwidth, 0, length*sizeof(int));

 	double  val;
 	for (int i = pSize.height - 1; i > iheight; i--)
 	{
 		for (int j = 0; j < pSize.width; j += 2)
 		{
 			val = cvGetReal2D(BinaryImage, i, j);
 			if (val > 0.0)
 			{
 				//统计白色的像素
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
//旋转图像内容不变，尺寸相应变大  
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
		double angle = i  * CV_PI / 180.; // 弧度    
		double a = sin(angle), b = cos(angle);
		int width = img->width;
		int height = img->height;
		int width_rotate = int(height * fabs(a) + width * fabs(b));
		int height_rotate = int(width * fabs(a) + height * fabs(b));
		//旋转数组map  
		// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]  
		// [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]  

		// 旋转中心  
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
	double angle = rotateImgInfo.angle  * CV_PI / 180.; // 弧度    
	int width = img->width;
	int height = img->height;
	int width_rotate = size.width;
	int height_rotate = size.height;
	//旋转数组map  
	// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]  
	// [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]  

	// 旋转中心  
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
	//对图像做仿射变换  
	//CV_WARP_FILL_OUTLIERS - 填充所有输出图像的象素。  
	//如果部分象素落在输入图像的边界外，那么它们的值设定为 fillval.  
	//CV_WARP_INVERSE_MAP - 指定 map_matrix 是输出图像到输入图像的反变换，  
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

		IplImage* src = cvLoadImage(scene_filename, 1);						  //加载测试图像文件
		IplImage* dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);		  //加载测试图像文件

		IplImage* r = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);		  //创建R单通道图像
		IplImage* g = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);		  //创建G单通道图像
		IplImage* b = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);		  //创建B单通道图像

		IplImage*	imageCanny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage*	imageCanny1 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage*	imageCanny2 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage*	imageCanny3 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

		IplImage*	dst1 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage*	dst2 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage*	dst3 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);



		float high[9] = { -1, -1, -1, -1, 9, -1, -1, -1, -1 };//高通滤波核
		CvMat km = cvMat(3, 3, CV_32FC1, high);  //构造单通道浮点矩阵，将图像IplImage结构转换为图像数组 
		cvSmooth(src, src, CV_GAUSSIAN, 7);

		cvSplit(src, r, g, b, NULL);

		cvCvtColor(src, dst, CV_BGR2GRAY);

		cvCanny(dst, imageCanny, 20, 80, 3);							//利用Canny算子进行边缘检测和二值化处理
		cvCanny(r, imageCanny1, 20, 80, 3);							//利用Canny算子进行边缘检测和二值化处理
		cvCanny(g, imageCanny2, 20, 80, 3);							//利用Canny算子进行边缘检测和二值化处理
		cvCanny(b, imageCanny3, 20, 80, 3);							//利用Canny算子进行边缘检测和二值化处理


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
		double angle = rotateImgInfo.angle  * CV_PI / 180.; // 弧度    
		int width = src->width;
		int height = src->height;
		int width_rotate = res->width;
		int height_rotate = res->height;

		// 旋转中心 
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
		// 		double fMinLineLen = 80;   //最小线段长度  
		// 		     
		// 		double fMinLineGap = 15;   //最小线段间隔  
		// 		     
		// 		CvSeq *pcvSeqLines = cvHoughLines2(RotateImage
		// 		    , pcvMStorage, CV_HOUGH_PROBABILISTIC, fRho, fTheta, nMaxLineNumber, fMinLineLen, fMinLineGap);
		// 
		// 		vector<CvPoint*> RLine;							//右倾直线
		// 		vector<CvPoint*> LLine;							//左倾直线
		// 		vector<CvPoint*> ULine;							//上端直线
		// 		vector<CvPoint*> DLine;							//下端直线
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
		sprintf(save_filename, ".\\三通道倾斜校正图像\\(%d).jpg", i + 1);
		cvSaveImage(save_filename, RotateImage);

		printf("当前图像为第%d张！\n", i + 1);

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
// 	//canny边缘检测  
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
// 	// 用于检测内存泄露
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
// 	// 用于存放数据
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
// 			std::cout << "图片无法打开！！！" << std::endl;
// 			break;
// 		}
// 
// 		cvShowImage("src", pImage);
// 		cvWaitKey(0);
// 
// 		f.FindCircleByWaterThed(pImage, params, centers, TRUE);
// 		//         cv::Mat contoursImage;
// 		//         cvtColor(cv::Mat(pImage), contoursImage, CV_GRAY2RGB);
// 		//         std::vector < std::vector<cv::Point> > contours; // 轮廓标记点存储的位置
// 		//         // 轮廓图片
// 		//         findContours(cv::Mat(pImage).clone(), contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
// 		// 
// 		//         drawContours(contoursImage, contours, -1, cv::Scalar(255, 0, 0));
// 		//         imshow("ContourImg", contoursImage);
// 		//         cvWaitKey(0);
// 
// 
// 		// 加入高斯噪声的代码
// 		//for (int j = 0; j < 10; j++)
// 		//    {
// 		//             IplImage* dst = AddGaussianNoise(pImage, 0, i / 50.0);
// 		//             sprintf(filePath, "%s测试结果\\0 (%d).bmp", rootFile, i);
// 		//             cvSaveImage(filePath, dst);
// 		//}
// 		//cvShowImage("dst",dst);
// 		//cvWaitKey(0);
// 
// 		std::cout << "*********************第" << i << "张********************" << std::endl;
// 		std::cout << "***********************时间测试*************************" << std::endl;
// 		// 论文中找圆的方法
// 		f.FindCircleBySamplingTriangles(pImage, centers, false);
// 		std::cout << "三点等腰三角形方法找到：" << centers.size() << std::endl;
// 
// 		cvtColor(cv::Mat(pImage), keypointsImage, CV_GRAY2RGB);
// 		for (int j = 0; j < (int)centers.size(); j++)
// 		{
// 			ST_CENTER center;
// 			center = centers.at(j);
// 			cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // 画一个圆 
// 			cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // 画一个圆 
// 		}
// 		sprintf(filePath, "%s测试结果\\3 (%d).bmp", rootFile, i);
// 		cvSaveImage(filePath, &IplImage(keypointsImage));
// 
// 		sprintf(filePath, "%s测试结果\\1 (%d).bmp", rootFile, i);
// 		f.SetFileName(filePath);
// 		// 我们的方法
// 		f.FindCircleByCICImproved(pImage, params, centers, false);
// 		std::cout << "CIC方法找到：" << centers.size() << std::endl;
// 		// 师兄的方法
// 		//    f.FindCircleCenter(pImage, params, Vct_ponits, false);
// 		std::cout << "阈值分割法找到：" << Vct_ponits.size() << std::endl;
// 		std::cout << "***********************效果展示**************************" << std::endl;
// 		// 论文中找圆的方法
// 		//f.FindCircleBySamplingTriangles(pImage, centers, true);
// 		// 我们的方法
// 		f.FindCircleByCICImproved(pImage, params, centers, true);
// 
// 		cvtColor(cv::Mat(pImage), keypointsImage, CV_GRAY2RGB);
// 		for (int j = 0; j < (int)centers.size(); j++)
// 		{
// 			ST_CENTER center;
// 			center = centers.at(j);
// 			cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // 画一个圆 
// 			cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // 画一个圆 
// 		}
// 		sprintf(filePath, "%s测试结果\\2 (%d).bmp", rootFile, i);
// 		cvSaveImage(filePath, &IplImage(keypointsImage));
// 
// 		// 师兄的方法
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

// 将 DEPTH_8U型二值图像进行细化  经典的Zhang并行快速细化算法  
// void thin(const cv::Mat &src, cv::Mat &dst, const int iterations)
// {
// 	const int height = src.rows - 1;
// 	const int width = src.cols - 1;
// 
// 	//拷贝一个数组给另一个数组  
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
// 		isFinished = FALSE;   //一次 先行后列扫描 开始  
// 		//扫描过程一 开始  
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
// 			} //扫描过程一 结束  
// 
// 
// 			dst.copyTo(tmpImg);
// 			//扫描过程二 开始  
// 			for (i = 1; i<height; i++)  //一次 先行后列扫描 开始  
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
// 			} //一次 先行后列扫描完成            
// 			//如果在扫描过程中没有删除点，则提前退出  
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
// // 		std::cout << "图片无法打开！！！" << std::endl;
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
// 		//kernel建立矩阵卷积核
// 		cvInitMatHeader(&kern00, 5, 5, CV_32FC1, L0);
// 		cvInitMatHeader(&kern45, 5, 5, CV_32FC1, L45);
// 		cvInitMatHeader(&kern90, 5, 5, CV_32FC1, L90);
// 		cvInitMatHeader(&kern135, 5, 5, CV_32FC1, L135);
// 		//距离变换
// 		cvDistTransform(src, distsrc, CV_DIST_L2, 5);
// 		//过滤，其实就是卷积
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