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
#include "icUI.h"

using namespace std;
using namespace cv;

int main(){
	Mat input = imread("test.png");
	Mat mask = getMask(input);
	vector<icPoint> allpoint = getPoints(input, mask);
  cout << "allpoint.size() : " << allpoint.size() << endl;
  for (int i = 0; i < allpoint.size(); i++)
  {
    cout << allpoint[i].pos << endl;
  }
	allpoint = complete(input, mask, allpoint);
	waitKey();
	return 0;
}