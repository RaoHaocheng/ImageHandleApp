#ifndef FINDCONTOURSMETHOD_H
#define FINDCONTOURSMETHOD_H

#include "define.h"

class DLLS_PORT FindContoursMethod
{
public:
	FindContoursMethod();
	~FindContoursMethod();

	BOOL findContoursByEDPF(const cv::Mat& src,cv::Mat& dst);

private:

};




#endif