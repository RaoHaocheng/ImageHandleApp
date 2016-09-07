#include "define.h"

// 是需要参数的，没有参数是不能让其运行
int main(int argc, char** argv)
{
	std::cout << argc << std::endl;
	for (int i = 0; i < argc; i++)
	{
		std::cout << argv[i];
		std::cout << " ";
	}

	std::cout << std::endl;

	CommandLineParse commandLineParser(argc,argv);
	char* filename = NULL;
	if (commandLineParser.IsSet("-o"))
		filename = commandLineParser.Value("-o");
	if (filename !=NULL)
		std::cout << filename << std::endl;

	if (commandLineParser.IsSet("-s"))
		filename = commandLineParser.Value("-s");
	if (filename != NULL)
		std::cout << filename << std::endl;

	IplImage* pImage = NULL;

	cv::SimpleBlobDetector::Params params;
	params.filterByArea = true;
	params.filterByCircularity = true;
	params.filterByInertia = true;
	params.filterByConvexity = true;
	params.filterByColor = false;
	params.minThreshold = 5;
	params.thresholdStep = 5;
	params.minArea = 40;
	//    	params.minConvexity = 0.8f;
	//    	params.minInertiaRatio = 0.73f;
	//    	params.minCircularity = 0.8f;
	params.minConvexity = 0.82f;
	params.minInertiaRatio = 0.70f;
	params.minCircularity = 0.75f;
	params.maxArea = 10000000000;
	params.blobColor = 0;
	params.maxThreshold = 65;
	params.maxConvexity = 1.2f;
	params.maxCircularity = 1.2f;
	params.maxInertiaRatio = 1.2f;
	params.minDistBetweenBlobs = 1;

	FindCircularMarker f;
	cv::Mat keypointsImage;
	std::vector<ST_CENTER> centers;
	f.FindCircleByThreadshold(cv::Mat(pImage), params, centers, FALSE);


	system("pause");
	return 0;
}