#include <opencv2\opencv.hpp>
using namespace cv;

#include <iostream>
#include <vector>
using namespace std;
#include "bp.h"
#include "public.h"



int winr=10;
int winxir=3;

//计算树的直径，式中的T
int bfs(vector<icPoint>allp){
}


//计算式中的d(a, b),点的距离的平方和
int d(int i, int j){

}

//对xi, i的情况计算式中的ei
int eii(int xi, int i){
	if (allp[i].type == BORDER){

	}
	else return 0;
}


int e1(int xi, int i){
	int ks = 1;
	int ki = 1;
	int es = d(i, xi) + d(xi, i);
	int ei = eii(xi, i);
	return ks * es + ki * ei;
}

int e2(int xi, int xj){

}

vector<icPoint> complete(Mat img, Mat mask, vector<icPoint> allp){
	int num = allp.size();//区域内点的个数
	int numm = 0;//区域外点的个数
	for (int i = 0; i < num; i++)
		if (allp[i].type == OUTER) numm++;
	num = num - numm;
	int xn = numm * winxir * winxir;//xi的个数

	vector<int> m[100][100];

	int t=bfs(allp);//计算树的直径，式中的T
	
	//对M进行初始化
	for (int i = 0; i < num; i++)
		for (int j = 0; j < num; j++)
			for (int xi = 0; xi < xn; xi++)
				m[i][j][xi] = 0;

	//在T次更新中，更新Mij
	for (int ti = 0; ti < t; ti++){
		for (int i = 0; i < num; i++){
			for (int j = 0; j < num; j++){
				for (int xi = 0; xi < xn; xi++){
					int e1=e1(xi);

					int sum_mki = 0;
					for (int k = 0; k < num; k++)
						if (k != j)
							sum_mki += m[k][i][xi];
					
					int min_e2 = 2012345678;
					for (int xj = 0; xj < xn; xj++){
						int n_e2 = e2(xi, xj);
						if (n_e2 < min_e2) min_e2 = n_e2;
					}

					m[i][j][xi] = e1+min_e2+sum_mki;
				}
			}

		}
	}

	for (int i = 0; i < num; i++){
		int min_xi;
		int min_sum = 2012345678;
		for (int xi = 0; xi < xn; xi++){
			int e1=e1(xi);
			int sum_mki = 0;
			for (int k = 0; k < num; k++)
				sum_mki += m[k][i][xi];
			if (sum_mki < min_sum ){
				min_sum = sum_mki;
				min_xi = xi;
			}
		}
		allp[i].from = allp[xi].pos;
	}

	return allp;
}