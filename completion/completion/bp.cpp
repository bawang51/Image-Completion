#include <opencv2\opencv.hpp>
using namespace cv;

#include <iostream>
#include <vector>
using namespace std;
#include "bp.h"
#include "public.h"



int winr=10; //窗口大小，半径
int winxir=3;//加入外面可选点附近的点的窗口大小，半径

//计算树的直径，式中的T
int bfs(vector<icPoint>allp){
}


//计算式中的d(a, b),点的距离的平方和
int d1(int i, int xi){

}
int d2(int xi, int i){

}

//对xi, i的情况计算式中的ei,也就是边界重叠部分
//注意这里使用i的位置和xi的颜色信息
int eii(int xi, int i){
	if (allp[i].type == BORDER){

	}//是边界则计算，不是则置0
	else return 0;
}

//计算式中e1
int e1(int xi, int i){
	int ks = 1;
	int ki = 1;
	int es = d1(i, xi) + d2(xi, i);
	int ei = eii(xi, i);
	return ks * es + ki * ei;
}

//计算式中的e2
int e2(int xi, int xj, int i, int j){

}

//信任传播主要部分
vector<icPoint> complete(Mat img, Mat mask, vector<icPoint> allpp){
	int num = allpp.size();//区域内点的个数
	int numm = 0;//区域外点的个数
	for (int i = 0; i < num; i++)
		if (allpp[i].type == OUTER) numm++;
	num = num - numm;
	int xn = numm * winxir * winxir;//xi的个数

	//把直线上，mask外的点扔到allx中，内的点扔到allp
	vector<icPoint> allx;
	vector<icPoint> allp;
	for (int i = 0; i < (num + numm); i++){
		if (allpp[i].type == OUTER)
			for (int ii = -winxir; ii < winxir; ii++)
				for (int jj = -winxir; jj < winxir; jj++){
					allpp.[i].pos.x += ii;
					allpp.[i].pos.y += jj;
					allx.push_back(allpp[i]);
					allpp.[i].pos.x -= ii;
					allpp.[i].pos.y -= jj;
				}
		}
		else
			allp.push_back(allpp[i]);
	}

	vector<int> m[100][100];//概率向量

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
					int e11=e1(xi, i);

					int sum_mki = 0;
					for (int k = 0; k < num; k++)
						if (k != j)
							sum_mki += m[k][i][xi];
					
					int min_e2 = 2012345678;
					for (int xj = 0; xj < xn; xj++){
						int n_e2 = e2(xi, xj, i, j);
						if (n_e2 < min_e2) min_e2 = n_e2;
					}

					m[i][j][xi] = e1+min_e2+sum_mki;
				}
			}

		}
	}

	//最后计算xi最小值的部分
	for (int i = 0; i < num; i++){
		int min_xi;
		int min_sum = 2012345678;
		for (int xi = 0; xi < xn; xi++){
			int e11=e1(xi, i);
			int sum_mki = 0;
			for (int k = 0; k < allp[i].neighbors.size(); k++){
				sum_mki += m[allp[i].neighbors[k]][i][xi];
			}
			if (sum_mki + e1 < min_sum ){
				min_sum = sum_mki e11;
				min_xi = xi;
			}
		}
		allp[i].from = all[xi].pos;
	}

	return allp;
}