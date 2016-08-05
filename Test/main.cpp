// 该项目只是为了测试这些苦是否可以被使用
#include "..\CameraCalibration\CameraCalibration.h"
#include "..\CameraCalibration\FindCircularMarker.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
//#include <cv.h>
//#include <highgui.h>

#define _CRTDBG_MAP_ALLOC

typedef unsigned char uchar;

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException){
	return EXCEPTION_EXECUTE_HANDLER;
}

//IplImage* pImage;
//IplImage* pCannyImage;
//IplImage* dst = NULL;

void on_trackbar(int threshold){
	//canny边缘检测  
	//    cvCanny(pImage, pCannyImage, threshold, threshold * 3, 3);
	//cvShowImage("canny", pCannyImage);
	//std::cout << "/*******************canny: " << threshold << std::endl;
}
int hello();

int main()
{
	// 用于检测内存泄露
	_CrtDumpMemoryLeaks();

#ifdef _DEBUG
#ifdef  _WIN64
	SetDllDirectory("..\\DLLs\\x64\\Debug\\");
#else
	SetDllDirectory("..\\DLLs\\x86\\Debug\\");
#endif //  _win64
#else
#ifdef  _WIN64
	SetDllDirectory("..\\DLLs\\x64\\Release\\");
#else
	SetDllDirectory("..\\DLLs\\x86\\Release\\");
#endif //  _win64
#endif // DEBUG


	cv::SimpleBlobDetector::Params params;
	params.filterByArea = true;
	params.filterByCircularity = true;
	params.filterByInertia = true;
	params.filterByConvexity = true;
	params.filterByColor = false;
	params.minThreshold = 5;
	params.thresholdStep = 5;
	params.minArea = 40;
	params.minConvexity = 0.8f;
	params.minInertiaRatio = 0.73f;
	params.minCircularity = 0.8f;
	params.maxArea = 10000000000;
	params.blobColor = 0;
	params.maxThreshold = 65;
	params.maxConvexity = 1.2f;
	params.maxCircularity = 1.2f;
	params.maxInertiaRatio = 1.2f;
	params.minDistBetweenBlobs = 1;

	// 用于存放数据
	std::vector<ST_CENTER> centers;
	std::vector<cv::KeyPoint> Vct_ponits;
	FindCircularMarker f;

	// const char* rootFile = ".\\Img\\";
	const char* rootFile = "C:\\Users\\haocheng\\Desktop\\Img\\";


	IplImage* pImage;
	cv::Mat keypointsImage;

	char* filePath = new char[255];
	for (int i = 1; i <= 50; i++)
	{
		ZeroMemory(filePath, 255);
		sprintf(filePath, "%s0 (%d).bmp", rootFile, i);

		//pImage = cvLoadImage("C:\\Users\\haocheng\\Desktop\\11.bmp", -1);
		pImage = cvLoadImage(filePath, -1);

		if (!pImage){
			std::cout << "图片无法打开！！！" << std::endl;
			break;
		}

		cvShowImage("src", pImage);
		cvWaitKey(0);

		f.FindCircleByWaterThed(pImage, params, centers, TRUE);
		//         cv::Mat contoursImage;
		//         cvtColor(cv::Mat(pImage), contoursImage, CV_GRAY2RGB);
		//         std::vector < std::vector<cv::Point> > contours; // 轮廓标记点存储的位置
		//         // 轮廓图片
		//         findContours(cv::Mat(pImage).clone(), contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
		// 
		//         drawContours(contoursImage, contours, -1, cv::Scalar(255, 0, 0));
		//         imshow("ContourImg", contoursImage);
		//         cvWaitKey(0);


		// 加入高斯噪声的代码
		//for (int j = 0; j < 10; j++)
		//    {
		//             IplImage* dst = AddGaussianNoise(pImage, 0, i / 50.0);
		//             sprintf(filePath, "%s测试结果\\0 (%d).bmp", rootFile, i);
		//             cvSaveImage(filePath, dst);
		//}
		//cvShowImage("dst",dst);
		//cvWaitKey(0);

		std::cout << "*********************第" << i << "张********************" << std::endl;
		std::cout << "***********************时间测试*************************" << std::endl;
		// 论文中找圆的方法
		f.FindCircleBySamplingTriangles(pImage, centers, false);
		std::cout << "三点等腰三角形方法找到：" << centers.size() << std::endl;

		cvtColor(cv::Mat(pImage), keypointsImage, CV_GRAY2RGB);
		for (int j = 0; j < (int)centers.size(); j++)
		{
			ST_CENTER center;
			center = centers.at(j);
			cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // 画一个圆 
			cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // 画一个圆 
		}
		sprintf(filePath, "%s测试结果\\3 (%d).bmp", rootFile, i);
		cvSaveImage(filePath, &IplImage(keypointsImage));

		sprintf(filePath, "%s测试结果\\1 (%d).bmp", rootFile, i);
		f.SetFileName(filePath);
		// 我们的方法
		f.FindCircleByCICImproved(pImage, params, centers, false);
		std::cout << "CIC方法找到：" << centers.size() << std::endl;
		// 师兄的方法
		//    f.FindCircleCenter(pImage, params, Vct_ponits, false);
		std::cout << "阈值分割法找到：" << Vct_ponits.size() << std::endl;
		std::cout << "***********************效果展示**************************" << std::endl;
		// 论文中找圆的方法
		//f.FindCircleBySamplingTriangles(pImage, centers, true);
		// 我们的方法
		f.FindCircleByCICImproved(pImage, params, centers, true);

		cvtColor(cv::Mat(pImage), keypointsImage, CV_GRAY2RGB);
		for (int j = 0; j < (int)centers.size(); j++)
		{
			ST_CENTER center;
			center = centers.at(j);
			cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // 画一个圆 
			cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // 画一个圆 
		}
		sprintf(filePath, "%s测试结果\\2 (%d).bmp", rootFile, i);
		cvSaveImage(filePath, &IplImage(keypointsImage));

		// 师兄的方法
		//f.FindCircleCenter(pImage, params, Vct_ponits, true);
	}

	delete[] filePath;

	system("pause");

	return 0;
}


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