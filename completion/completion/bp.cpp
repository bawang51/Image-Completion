#include <opencv2\opencv.hpp>
using namespace cv;

#include <iostream>
#include <vector>
using namespace std;
#include "bp.h"
#include "public.h"

int bfs(vector<icPoint>allp){
}

vector<icPoint> complete(Mat img, Mat mask, vector<icPoint> allp){
	int num = allp.size();
	int m[100][100][100];
	int t=bfs(allp);
	for (int i = 0; i < num; i++)
	for (int j = 0; j < num; j++)
		m[i][j][t] = 0;
	for (int ti = 0; ti < t; ti++){
		int min = 10000;
		for (int k = 0; k < num; k++)
			if (){

		}
	}

}