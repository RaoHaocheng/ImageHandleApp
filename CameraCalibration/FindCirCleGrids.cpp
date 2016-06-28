#include"highgui.h"
#include"cv.h"
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;
int main()
{

	IplImage* img=cvLoadImage("RotorImg001_L.bmp",-1);
	int height=img->height;
	int width=img->width;
	int pixpbyte=img->nChannels;
	int step=img->widthStep;
	uchar*ptr=(uchar*)img->imageData;
    CvSize pic_size;
	pic_size=cvSize(640,480);
	CvPoint2D32f*points_buff;
	points_buff=new CvPoint2D32f[2];
	Mat mtx(img);
    CvMat * pointmat = cvCreateMat(2,1,CV_32FC2);
    Mat mtx1(pointmat);
	findCirclesGrid(mtx,pic_size,mtx1,2);
  
	for (int i=0; i<2; i++)  
	{  
		for (int j=0; j<2; j++)  
		{  
			cout<<mtx1.at<float>(i,j)<<endl;
		    
			
		}  
	}  
 KeyPoint
	/*for (int i=0; i<96; i++)  
	{  
		for (int j=0; j<2; j++)  
		{  
			cout<<mtx1.at<float>(i,j)<<endl;
          img->imageData[i - 1 * step + pstImgInfo->pstRegionInfo[i].iCenter_Y] = (char)0;

		}  
	} */ 

	
}