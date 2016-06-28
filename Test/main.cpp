// ����Ŀֻ��Ϊ�˲�����Щ���Ƿ���Ա�ʹ��
#include "..\CameraCalibration\CameraCalibration.h"
#include "..\CameraCalibration\FindCircularMarker.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
//#include <cv.h>
//#include <highgui.h>

#define _CRTDBG_MAP_ALLOC

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException){
	return EXCEPTION_EXECUTE_HANDLER;
}

//IplImage* pImage;
//IplImage* pCannyImage;
//IplImage* dst = NULL;

void on_trackbar(int threshold){
	//canny��Ե���  
	//    cvCanny(pImage, pCannyImage, threshold, threshold * 3, 3);
	//cvShowImage("canny", pCannyImage);
	//std::cout << "/*******************canny: " << threshold << std::endl;
}
int hello();

int main()
{
	// ���ڼ���ڴ�й¶
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

	// ���ڴ������
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
			std::cout << "ͼƬ�޷��򿪣�����" << std::endl;
			break;
		}

		cvShowImage("src", pImage);
		cvWaitKey(0);

		f.FindCircleByWaterThed(pImage, params, centers, TRUE);
		//         cv::Mat contoursImage;
		//         cvtColor(cv::Mat(pImage), contoursImage, CV_GRAY2RGB);
		//         std::vector < std::vector<cv::Point> > contours; // ������ǵ�洢��λ��
		//         // ����ͼƬ
		//         findContours(cv::Mat(pImage).clone(), contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
		// 
		//         drawContours(contoursImage, contours, -1, cv::Scalar(255, 0, 0));
		//         imshow("ContourImg", contoursImage);
		//         cvWaitKey(0);


		// �����˹�����Ĵ���
		//for (int j = 0; j < 10; j++)
		//    {
		//             IplImage* dst = AddGaussianNoise(pImage, 0, i / 50.0);
		//             sprintf(filePath, "%s���Խ��\\0 (%d).bmp", rootFile, i);
		//             cvSaveImage(filePath, dst);
		//}
		//cvShowImage("dst",dst);
		//cvWaitKey(0);

		std::cout << "*********************��" << i << "��********************" << std::endl;
		std::cout << "***********************ʱ�����*************************" << std::endl;
		// ��������Բ�ķ���
		f.FindCircleBySamplingTriangles(pImage, centers, false);
		std::cout << "������������η����ҵ���" << centers.size() << std::endl;

		cvtColor(cv::Mat(pImage), keypointsImage, CV_GRAY2RGB);
		for (int j = 0; j < (int)centers.size(); j++)
		{
			ST_CENTER center;
			center = centers.at(j);
			cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // ��һ��Բ 
			cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // ��һ��Բ 
		}
		sprintf(filePath, "%s���Խ��\\3 (%d).bmp", rootFile, i);
		cvSaveImage(filePath, &IplImage(keypointsImage));

		sprintf(filePath, "%s���Խ��\\1 (%d).bmp", rootFile, i);
		f.SetFileName(filePath);
		// ���ǵķ���
		f.FindCircleByCICImproved(pImage, params, centers, false);
		std::cout << "CIC�����ҵ���" << centers.size() << std::endl;
		// ʦ�ֵķ���
		//    f.FindCircleCenter(pImage, params, Vct_ponits, false);
		std::cout << "��ֵ�ָ�ҵ���" << Vct_ponits.size() << std::endl;
		std::cout << "***********************Ч��չʾ**************************" << std::endl;
		// ��������Բ�ķ���
		//f.FindCircleBySamplingTriangles(pImage, centers, true);
		// ���ǵķ���
		f.FindCircleByCICImproved(pImage, params, centers, true);

		cvtColor(cv::Mat(pImage), keypointsImage, CV_GRAY2RGB);
		for (int j = 0; j < (int)centers.size(); j++)
		{
			ST_CENTER center;
			center = centers.at(j);
			cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // ��һ��Բ 
			cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // ��һ��Բ 
		}
		sprintf(filePath, "%s���Խ��\\2 (%d).bmp", rootFile, i);
		cvSaveImage(filePath, &IplImage(keypointsImage));

		// ʦ�ֵķ���
		//f.FindCircleCenter(pImage, params, Vct_ponits, true);
	}

	delete[] filePath;

	system("pause");

	return 0;
}