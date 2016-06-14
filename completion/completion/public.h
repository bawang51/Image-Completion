#include <opencv2\opencv.hpp>
using namespace cv;

#ifndef _IC_PUBLIC_H
#define _IC_PUBLIC_H

struct icPoint
{
	Point2i pos;
	Point2i from;
	int num;
	vector<int> neighbors;
};

#endif