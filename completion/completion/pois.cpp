#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#include "pois.h"

void gauss_seidel(Mat &b, Mat &x) {
    double *b_data = (double *) b.data;
    double *x_data = (double *) x.data;
    double TOLERANCE = 0.05;
    double err = (numeric_limits<double>::max)();
    int w = x.rows;
    int h = x.cols;
    imwrite("testf.png", b);
    while (err > TOLERANCE) {
        err = 0;
        for (int i = 0; i < w; i++) {
            for (int j = 0; j < h; j++) {
                double tem = x_data[i * h + j];
                x_data[i * h + j] = b_data[i * h + j];
                // cout << tem << endl;
                if (i != 0) {
                    x_data[i * h + j] -= x_data[(i - 1) * h + j];
                }
                if (i != w - 1) {
                    x_data[i * h + j] -= x_data[(i + 1) * h + j];
                }
                if (j != 0) {
                    x_data[i * h + j] -= x_data[i * h + j - 1];
                }
                if (j != h - 1) {
                    x_data[i * h + j] -= x_data[i * h + j + 1];
                }

                x_data[i * h + j] /= -4.0;

                double delta = abs(x_data[i * h + j] - tem);
                if (delta > err) {
                    err = delta;
                }

            }
        }
        //return;
        cout << err << endl;
    }
}

Mat __single_channle(Mat &origin, Mat &insert, int x, int y) {
    Mat f_o, f_i, b;
    origin.convertTo(f_o, CV_64F);
    insert.convertTo(f_i, CV_64F);

    Rect roi(y, x, f_i.cols, f_i.rows);

    Mat roif_o;
    f_o(roi).copyTo(roif_o);   //ç›´æŽ¥ä½¿ç”¨roi çš„è¯ é€šè¿‡.data æ‹¿åˆ°çš„æ•°æ®ä¸æ˜¯roiçš„æ•°ç»„

    Laplacian(f_i, b, CV_64F);

    int w = f_i.rows;
    int h = f_i.cols;
    cout << w << " " << h<< " " << x << " " << y << endl;
//ÉÏÏÂ×óÓÒµÄ±ß 
    for (int i = 0; i < h; i++) {
        b.at<double>(0, i) -= f_o.at<double>(x - 1, y + i);
        b.at<double>(w - 1, i) -= f_o.at<double>(x + w, y + i);
    }
    for (int i = 0; i < w; i++) {
        //cout << x - 1 << " " << y + i << " "<< f_o.at<double>(x + i, y + h) <<  endl;
        b.at<double>(i, 0) -= f_o.at<double>(x + i, y - 1);
        b.at<double>(i, h - 1) -= f_o.at<double>(x + i, y + h);
    }
    // cout <<  b;
    gauss_seidel(b, roif_o);
    roif_o.copyTo(f_o(roi));
    return f_o;
}

//x,yÊÇ×óÉÏ½ÇµÄÎ»ÖÃ 
Mat possion_blending(Mat &origin, Mat &insert, int x, int y) {
    vector<Mat> origin_channel;
    vector<Mat> insert_channel;
    vector<Mat> inserted;
    Mat res;

    split(origin, origin_channel);
    split(insert, insert_channel);
    for (int i = 0; i < origin_channel.size(); i++) {
        inserted.push_back(__single_channle(origin_channel[i], insert_channel[i], x, y));
        //break;
    }
    merge(inserted, res);
    return res;
}
