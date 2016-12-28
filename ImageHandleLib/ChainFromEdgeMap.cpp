#include "ChainFromEdgeMap.h"
#include "QuickSort.h"
#include <math.h>
#include <omp.h>
#include <iostream>
using namespace std;

ChainFromEdgeMap::ChainFromEdgeMap()
{

}

ChainFromEdgeMap::~ChainFromEdgeMap()
{

}

void ChainFromEdgeMap::run( cv::Mat &image, cv::Mat &edgeMap, std::vector<std::vector<cv::Point> > &edgeChain )
{
	int cols = image.cols;
	int rows = image.rows;

	cv::Mat imgNew;
	if ( image.channels() == 3 )
	{
		cv::cvtColor( image, imgNew, CV_RGB2GRAY );
	}
	else
	{
		imgNew = image;
	}

	// get image information
	gradientMap = cv::Mat::zeros( rows, cols, CV_64FC1 );
	orientationMap = cv::Mat::zeros( rows, cols, CV_8UC1 );
	maskImage = cv::Mat::zeros( rows, cols, CV_64FC1 );

	cv::Mat dx(rows, cols, CV_16S, Scalar(0));
	cv::Mat dy(rows, cols, CV_16S, Scalar(0));

	cv::Sobel( imgNew, dx, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_REPLICATE);
	cv::Sobel( imgNew, dy, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_REPLICATE);

	double anglePer = CV_PI / 8.0;
	for ( int i = 0; i< rows; ++i )
	{
		double *ptrG = gradientMap.ptr<double>(i);
		uchar *ptrO = orientationMap.ptr<uchar>(i);
		short *ptrX = dx.ptr<short>(i);
		short *ptrY = dy.ptr<short>(i);

		for ( int j = 0; j<cols; ++j )
		{
			double gx = ptrX[j];
			double gy = ptrY[j];

			ptrG[j] = abs(gx) + abs(gy);

			int idx = int( ( atan2(gx, -gy) + CV_PI ) / anglePer );
			ptrO[j] = idx;
			if ( idx == 16 )
			{
				ptrO[j] = 0;
			}
		}
	}

	// get edge chains
	std::vector<Point> gradientPoints;
	std::vector<double> gradientValue;

	uchar *ptrE = (uchar*)edgeMap.data;
	double *ptrM = (double*)maskImage.data;
	double *ptrG = (double*)gradientMap.data;
	for ( int i = 0; i< rows; ++i )
	{
		for ( int j = 0; j<cols; ++j )
		{
			if (*ptrE++)
			{
				*ptrM++ = 1;
				gradientPoints.push_back( cv::Point(j, i) );
				gradientValue.push_back(ptrG[i*cols + j]);
			}
			else
			{
				*ptrM++;
			}
		}
	}

	ptrM = (double*)maskImage.data;
	uchar *ptrO = (uchar*)orientationMap.data;

	int numGradientPoints = (int)gradientPoints.size();
	QuickSort<double, cv::Point>::SortDescent( &gradientValue[0], 0, numGradientPoints - 1, &gradientPoints[0] );
	//std::sort( dataset.begin(), dataset.end(), []( const double& lhs, const double& rhs ){ return lhs < rhs; } );

	//find strings
	int thMeaningfulLength = int( 2.0 * log( (double) rows * cols ) / log(8.0) + 0.5 );	
	for ( int i = 0; i<numGradientPoints; ++i )
	{
		std::vector<cv::Point> chain;

		int x = gradientPoints[i].x;
		int y = gradientPoints[i].y;
		do
		{
			chain.push_back( cv::Point( x, y ) );
			if ( x == 74 && y == 37 )
			{
				int aa = 0;
			}
			ptrM[y*cols + x] = 0;
		} while ( next(x, y) );

		cv::Point temp;
		for ( int m = 0, n = (int)chain.size() - 1; m<n; ++m, --n )
		{
			temp = chain[m];
			chain[m] = chain[n];
			chain[n] = temp;
		}

		// Find and add feature pixels to the begin of the string.
		x = gradientPoints[i].x;
		y = gradientPoints[i].y;
		if ( next(x, y) )
		{
			do
			{
				chain.push_back( cv::Point( x, y ) );

				if ( x == 74 && y == 37 )
				{
					int aa = 0;
				}

				ptrM[y*cols + x] = 0;
			} while (next(x, y));
		}

		if ( chain.size() > thMeaningfulLength)
		{
			edgeChain.push_back( chain );
		}
	}
}

bool ChainFromEdgeMap::next( int &xSeed, int &ySeed )
{
	int cols = maskImage.cols;
	int rows = maskImage.rows;

	double *ptrM = (double *)maskImage.data;
	uchar *ptrO = (uchar*)orientationMap.data;

	int direction = ptrO[ySeed*cols + xSeed];
	int direction0 = direction - 1;
	if (direction0<0) direction0 = 15;
	int direction1 = direction;
	int direction2 = direction + 1;
	if (direction2 == 16) direction2 = 0;

	static const int X_OFFSET[8] = { 0, 1, 0, -1, 1, -1, -1, 1 };
	static const int Y_OFFSET[8] = { 1, 0, -1, 0, 1, 1, -1, -1 };

	int x, y;
	for (size_t i = 0; i != 8; ++i)
	{
		x = xSeed + X_OFFSET[i];
		if ((0 <= x) && (x < (int)cols))
		{
			y = ySeed + Y_OFFSET[i];
			if ((0 <= y) && (y < (int)rows))
			{
				if (ptrM[y*cols + x])
				{
					int directionTemp = ptrO[y*cols + x];
					if (directionTemp == direction0 || directionTemp == direction1 || directionTemp == direction2)
					{
						xSeed = x;
						ySeed = y;
						return true;
					}
				}
			}
		}
	}
	return false;
}
