#include "CannyPF.h"

using namespace cv;
using namespace std;

CannyPF::CannyPF(void)
{
}


CannyPF::~CannyPF(void)
{
}


void CannyPF::cannyPF( cv::Mat &image, int gaussianSize, float VMGradient, cv::Mat &edgeMap )
{
	int i,j,m,n;
	int grayLevels=255;
	float gNoise=1.3333;  //1.3333 
	float thGradientLow=gNoise;

	int cols = image.cols;
	int rows = image.rows;
	int cols_1=cols-1;
	int rows_1=rows-1;

	//meaningful Length
	int thMeaningfulLength=int(2.0*log((float)rows*cols)/log(8.0)+0.5);
	float thAngle=2*atan(2.0/float(thMeaningfulLength));

	//get gray image
	cv::Mat grayImage;
	int aa=image.channels();

	if ( image.channels() == 1 )
		grayImage = image;
	else
		cv::cvtColor(image, grayImage, CV_BGR2GRAY);

	//gaussian filter
	cv::Mat filteredImage;
	cv::GaussianBlur(grayImage, filteredImage, cv::Size(gaussianSize,gaussianSize), 1.0);
	grayImage.release();

	//get gradient map and orientation map
	cv::Mat gradientMap = cv::Mat::zeros(filteredImage.rows,filteredImage.cols,CV_32FC1);
	cv::Mat dx(filteredImage.rows, filteredImage.cols, CV_16S,Scalar(0));
	cv::Mat dy(filteredImage.rows, filteredImage.cols, CV_16S,Scalar(0));

	int apertureSize=3;
	cv::Sobel(filteredImage, dx, CV_16S, 1, 0, apertureSize, 1, 0, cv::BORDER_REPLICATE);
	cv::Sobel(filteredImage, dy, CV_16S, 0, 1, apertureSize, 1, 0, cv::BORDER_REPLICATE);

	//calculate gradient and orientation
	int totalNum=0;
	int times=8;
	std::vector<int> histogram(times*grayLevels,0);
	for (i=0;i<rows;++i)
	{
		float *ptrG=gradientMap.ptr<float>(i);
		short *ptrX=dx.ptr<short>(i);
		short *ptrY=dy.ptr<short>(i);
		for (j=0;j<cols;++j)
		{
			float gx=ptrX[j];
			float gy=ptrY[j];

			ptrG[j]=abs(gx)+abs(gy);
			if (ptrG[j]>thGradientLow)
			{
				histogram[int(ptrG[j]+0.5)]++;
				totalNum++;
			}
			else
				ptrG[j]=0.0;
		}
	}

	//gradient statistic
	float N2=0;
	for (i=0;i<histogram.size();++i)
	{
		if (histogram[i])
			N2+=(float)histogram[i]*(histogram[i]-1);
	}
	float pMax=1.0/exp((log(N2)/thMeaningfulLength));
	float pMin=1.0/exp((log(N2)/sqrt((float)cols*rows)));

	std::vector<float> greaterThan(times*grayLevels,0);
	int count=0;
	for (i=times*grayLevels-1;i>=0;--i)
	{
		count+=histogram[i];
		float probabilityGreater=float(count)/float(totalNum);
		greaterThan[i]=probabilityGreater;
	}
	count=0;

	//get two gradient thresholds
	int thGradientHigh=0;
	for (i=times*grayLevels-1;i>=0;--i)
	{
		if (greaterThan[i]>pMax)
		{
			thGradientHigh=i;
			break;
		}
	}
	for (i=times*grayLevels-1;i>=0;--i)
	{
		if (greaterThan[i]>pMin)
		{
			thGradientLow=i;
			break;
		}
	}
	if (thGradientLow<gNoise) thGradientLow=gNoise;

	//cout<<thGradientLow<<"  "<<thGradientHigh<<endl;

	//convert probabilistic meaningful to visual meaningful
	thGradientHigh=sqrt(thGradientHigh*VMGradient);
	
	//canny
	cv::Canny(filteredImage,edgeMap,thGradientLow,thGradientHigh,apertureSize);
}