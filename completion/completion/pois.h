#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


void gauss_seidel(Mat &b, Mat &x);

Mat __single_channle(Mat &origin, Mat &insert, int x, int y);

Mat possion_blending(Mat &origin, Mat &insert, int x, int y);