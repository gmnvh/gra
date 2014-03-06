#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;

extern void showHistogram(Mat &img, Mat &dest);
extern void showHistogram(Mat &img, Mat &dest, int thres);

#endif
