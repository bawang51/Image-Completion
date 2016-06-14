#define _USE_MATH_DEFINES

#include <string>
#include <opencv2\opencv.hpp>
#include <vector>
#include <ctime>
#include <math.h>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "bp.h"
#include "public.h"

using namespace std;
using namespace cv;

int main(){
	Mat input = imread("test.jpg");
	Mat mask = imread("mask.jpg");
	input.convertTo(input, CV_8UC3);
	mask.convertTo(mask, CV_8UC3);
	Mat res = complete();
	imshow("result", res);
	waitKey();
	return 0;
}