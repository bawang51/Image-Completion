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
#include "natext.h"
#include "pois.h"

using namespace std;
using namespace cv;

int main(){
	int winr = 11; //窗口大小，半�?
	int winxir = 3;//加入外面可选点附近的点的窗口大小，半径
	Mat input = imread("test.png");
	Mat mask = getMask(input);
	vector<icPoint> allpoint = getPoints(input, mask);
	cout << "allpoint.size() : " << allpoint.size() << endl;
	for (int i = 0; i < allpoint.size(); i++){
		cout << allpoint[i].pos << endl;
	}
	waitKey();
	allpoint = complete(input, mask, allpoint);

	for (int i = 0; i < allpoint.size(); i++){
		if (allpoint[i].type == icPoint::INNER){
			for (int jj = -winr; jj <= winr; jj++){
				if (allpoint[i].pos.y + jj >= 0 && allpoint[i].pos.y + jj < input.rows){
					for (int ii = -winr; ii <= winr; ii++){
						if (allpoint[i].pos.x + ii >= 0 && allpoint[i].pos.x + ii < input.cols){
							mask.at<uchar>(allpoint[i].pos.y + jj, allpoint[i].pos.x + ii) = 0;
							input.at<Vec3b>(allpoint[i].pos.y + jj, allpoint[i].pos.x + ii)[0] = input.at<Vec3b>(allpoint[i].from.y + jj, allpoint[i].from.x + ii)[0];
							input.at<Vec3b>(allpoint[i].pos.y + jj, allpoint[i].pos.x + ii)[1] = input.at<Vec3b>(allpoint[i].from.y + jj, allpoint[i].from.x + ii)[1];
							input.at<Vec3b>(allpoint[i].pos.y + jj, allpoint[i].pos.x + ii)[2] = input.at<Vec3b>(allpoint[i].from.y + jj, allpoint[i].from.x + ii)[2];
						}
					}
				}
			}
		}
	}
	Mat ret = natGenerate(input, mask, 11);
	input = ret;
	for (int i = 0; i < allpoint.size(); i++){
		if (allpoint[i].type == icPoint::INNER){
			Mat ii;
			ii = input.rowRange(allpoint[i].pos.y - winr, allpoint[i].pos.y + winr);
			ii = ii.colRange(allpoint[i].pos.x - winr, allpoint[i].pos.x + winr);

			possion_blending(input, ii, allpoint[i].pos.x - winr, allpoint[i].pos.y - winr);

		}
	}

	imshow("result", input);
	waitKey();
	return 0;
}
