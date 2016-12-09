#ifndef _EDGE_CHAIN_
#define _EDGE_CHAIN_
#pragma once

#include "opencv/cv.h"
#include "highgui.h"

using namespace std;
using namespace cv;

#ifndef mmax
#define mmax(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef mmin
#define mmin(a,b)            (((a) < (b)) ? (a) : (b))
#endif


class ChainFromEdgeMap 
{
public:
	ChainFromEdgeMap();
	~ChainFromEdgeMap();

	void run( cv::Mat &image, cv::Mat &edgeMap, std::vector<std::vector<cv::Point> > &edgeChain );

	bool next( int &xSeed, int &ySeed );

private:
	cv::Mat gradientMap;
	cv::Mat maskImage;
	cv::Mat orientationMap;
};

#endif // _PLINKAGE_SUPERPIXEL_
