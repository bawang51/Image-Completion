#include <opencv2\opencv.hpp>
using namespace cv;

#include <iostream>
#include <vector>
#include <queue>
using namespace std;
#include "bp.h"
#include "public.h"

int winr=5; //窗口大小，半径
int winxir=3;//加入外面可选点附近的点的窗口大小，半径

//计算树的直径，式中的T
int bfs(vector<icPoint>allp){
  int T = 0;
  int *dist = new int[allp.size()];
  for (int i = 0; i < allp.size(); i++)
  {
    cout << "Round #" << i << endl;
    memset(dist, 0, allp.size()*sizeof(int));
    int count = 0;
    for (int j = 0; j < allp[i].neighbors.size(); j++)
    {
      if (allp[i].neighbors[j]>0 && 
        allp[allp[i].neighbors[j]].type != icPoint::OUTER)
      {
        count++;
      }
    }
    if (count == 1)
    {
      queue<int> que; que.push(i);
      while (!que.empty())
      {
        int now = que.front();
        cout << "que head : " << now << endl;
        for (int j = 0; j < allp[now].neighbors.size(); j++)
        {
          int nei = allp[now].neighbors[j];
          if (nei<0) continue;
          cout << "    nei : " << nei << endl;
          if (allp[nei].type != icPoint::OUTER)
          {
            if ((dist[nei] == 0 && nei != i) ||
              (dist[nei]>dist[now] + 1))
            {
              dist[nei] = dist[now] + 1;
              que.push(nei);
            }
          }
        }
        que.pop();
      }
      for (int j = 0; j < allp.size(); j++)
      if (T < dist[j]) T = dist[j];
    }
  }
  return T;
}


//计算式中的d(a, b),点的距离的平方和
double d(Point2i i, Point2i xi, Mat line){
  Mat m1 = line.colRange(Range(i.x - winr, i.x + winr)).rowRange(Range(i.y - winr, i.y + winr));
  Mat m2 = line.colRange(Range(xi.x - winr, xi.x + winr)).rowRange(Range(xi.y - winr, xi.y + winr));
  double dist = 0;
  vector<Point2i> ml1, ml2;
  for (int y = 0; y < m1.rows; y++)
  for (int x = 0; x < m1.cols; x++)
  {
    if (m1.at<uchar>(y, x))
    {
      ml1.push_back(Point2i(x, y));
    }
  }
  for (int y = 0; y < m2.rows; y++)
  for (int x = 0; x < m2.cols; x++)
  {
    if (m2.at<uchar>(y, x))
    {
      ml2.push_back(Point2i(x, y));
    }
  }
  for (int l1 = 0; l1 < ml1.size(); l1++)
  {
    double min = 1E20;
    for (int l2 = 0; l2 < ml2.size(); l2++)
    {
      Point2i vec = ml1[l1] - ml2[l2];
      double dd = sqrt(vec.x*vec.x + vec.y*vec.y);
      if (dd < min) min = dd;
    }
    dist += min;
  }
  return dist / ml1.size();
}

//对xi, i的情况计算式中的ei,也就是边界重叠部分
//注意这里使用i的位置和xi的颜色信息
int eii(Point2i xi, icPoint i, Mat mask, Mat img){
  double ret = 0;
  int count = 0;
	if (i.type == icPoint::BORDER){
    Mat toput = img.colRange(Range(xi.x - winr, xi.x + winr))
      .rowRange(Range(xi.y - winr, xi.y + winr));
    for (int y = 0; y < toput.rows; y++)
    {
      for (int x = 0; x < toput.cols; x++)
      {
        int yy = y + i.pos.y - winr;
        int xx = x + i.pos.x - winr;
        if (mask.at<uchar>(yy, xx))
        {
          int dr = img.at<Vec3b>(yy, xx)[0] - toput.at<Vec3b>(y, x)[0];
          int dg = img.at<Vec3b>(yy, xx)[1] - toput.at<Vec3b>(y, x)[1];
          int db = img.at<Vec3b>(yy, xx)[2] - toput.at<Vec3b>(y, x)[2];
          ret += sqrt(dr*dr + dg*dg + db*db);
          count++;
        }
      }
    }
    return ret / count;
	}//是边界则计算，不是则置0
	else return 0;
}

//计算式中e1
int e1(Point2i xi, icPoint i, Mat line, Mat mask, Mat img){
	int ks = 1;
	int ki = 1;
  double es = d(i.pos, xi, line) + d(xi, i.pos, line);
	double ei = eii(xi, i, mask, img);
	return ks * es + ki * ei;
}

//计算式中的e2
int e2(Point2i xi, Point2i xj, Point2i i, Point2i j, Mat img){
  double ret = 0;
  int count = 0;
  Mat toput = img.colRange(Range(xi.x - winr, xi.x + winr))
    .rowRange(Range(xi.y - winr, xi.y + winr));
  for (int y = 0; y < toput.rows; y++)
  {
    for (int x = 0; x < toput.cols; x++)
    {
      int yy = y + i.y - winr;
      int xx = x + i.x - winr;
      if (yy>=j.y-winr && yy<j.y+winr &&
        xx>=j.x-winr && yy<j.x+winr)
      {
        int dr = img.at<Vec3b>(y + j.y - winr, x + j.x - winr)[0] - toput.at<Vec3b>(y, x)[0];
        int dg = img.at<Vec3b>(y + j.y - winr, x + j.x - winr)[1] - toput.at<Vec3b>(y, x)[1];
        int db = img.at<Vec3b>(y + j.y - winr, x + j.x - winr)[2] - toput.at<Vec3b>(y, x)[2];
        ret += sqrt(dr*dr + dg*dg + db*db);
        count++;
      }
    }
  }
  return ret / count;
}

static int m[100][100][1000];//概率向量

//信任传播主要部分
vector<icPoint> complete(Mat img, Mat mask, vector<icPoint> allpp){
  Mat linegraph(img.size(), CV_8U, Scalar::all(0));
  for (int i = 0; i < allpp.size(); i++)
  {
    for (int j = 0; j < allpp[i].neighbors.size(); j++)
    {
      int nei = allpp[i].neighbors[j];
      line(linegraph, allpp[i].pos, allpp[nei].pos, Scalar::all(255));
    }
  }
  imshow("linegraph", linegraph);

	int num = allpp.size();//区域内点的个数
	int numm = 0;//区域外点的个数
  vector<Point2i> outers;
	for (int i = 0; i < num; i++)
  if (allpp[i].type == icPoint::OUTER) outers.push_back(allpp[i].pos);
  numm = outers.size();
	num = num - numm;
	int xn = numm * winxir * winxir * 4;//xi的个数

	//把直线上，mask外的点扔到allx中，内的点扔到allp
  cout << "allx-allp" << endl;
	vector<icPoint> allp;
  int list[1000];
  cout << "List[]:" << endl;
  for (int i = 0; i < allpp.size(); i++)
  {
    if (allpp[i].type != icPoint::OUTER)
    {
      allp.push_back(allpp[i]);
      list[i] = allp.size() - 1;
    }
    else
    {
      list[i] = -1;
    }
    cout << "  " << list[i] << endl;
  }
  cout << "haha" << endl;
  for (int i = 0; i < num; i++)
  {
    allp[i].num = list[i];
    for (int j = 0; j < allp[i].neighbors.size(); j++)
    {
      allp[i].neighbors[j] = list[allp[i].neighbors[j]];
    }
  }
  cout << "calculate T" << endl;
	int t=bfs(allp);//计算树的直径，式中的T
  cout << "done T" << endl;
	//对M进行初始化
	for (int i = 0; i < num; i++)
		for (int j = 0; j < num; j++)
    for (int xi = 0; xi < xn; xi++)
      m[i][j][xi] = 0;

	//在T次更新中，更新Mij
  cout << "update Mij" << endl;
	for (int ti = 0; ti < t; ti++){
		for (int i = 0; i < num; i++){
			for (int nn = 0; nn < allp[i].neighbors.size(); nn++){
        int j = allp[i].neighbors[nn];
        if (j < 0) continue;
        int xi = 0;
        for (int outer = 0; outer < numm; outer++)
        {
          int nx = outers[outer].x, ny = outers[outer].y;
          for (int dy = -winxir; dy < winxir; dy++)
          {
            for (int dx = -winxir; dx < winxir; dx++)
            {
              int e11 = e1(Point2i(nx + dx, ny + dy), allp[i]
                , linegraph, mask, img);

              int sum_mki = 0;
              for (int k = 0; k < num; k++)
              if (k != j)
                sum_mki += m[k][i][xi];

              int min_e2 = 2012345678;

              for (int oj = 0; oj < numm; oj++)
              {
                int nxx = outers[oj].x, nyy = outers[oj].y;
                for (int ddy = -winxir; ddy < winxir; ddy++)
                {
                  for (int ddx = -winxir; ddx < winxir; ddx++)
                  {
                    int n_e2 = e2(
                      Point2i(nx+dx, ny+dy),
                      Point2i(nxx+ddx, nyy+ddy),
                      allp[i].pos, allp[j].pos, img);
                    if (n_e2 < min_e2) min_e2 = n_e2;
                  }
                }
              }

              m[i][j][xi] = e11 + min_e2 + sum_mki;
              xi++;
            }
          }
        }
			}

		}
	}

	//最后计算xi最小值的部分
  cout << "min xi" << endl;
	for (int i = 0; i < num; i++){
    cout << "min x" << i << endl;
		int min_xi;
		int min_sum = 2012345678;
    int xi = 0;
    for (int oi = 0; oi < numm; oi++)
    {
      cout << "oi = " << oi << endl;
      int nx = outers[oi].x, ny = outers[oi].y;
      for (int dx = -winxir; dx < winxir; dx++)
      {
        for (int dy = -winxir; dy < winxir; dy++)
        {
          cout << "dx-dy = " << dx << " " << dy << endl;
          int e11 = e1(Point2i(nx + dx, ny + dy), allp[i],
            linegraph, mask, img);
          int sum_mki = 0;
          for (int k = 0; k < allp[i].neighbors.size(); k++){
            if (allp[i].neighbors[k] < 0) continue;
            sum_mki += m[allp[i].neighbors[k]][i][xi];
          }
          if (sum_mki + e11 < min_sum){
            min_sum = sum_mki+e11;
            min_xi = xi;
          }
          xi++;
        }
      }
    }
    allp[i].from = allp[xi].pos;
	}

	return allp;
}