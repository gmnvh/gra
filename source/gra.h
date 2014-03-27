#ifndef _GRA_H_
#define _GRA_H_

#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "threshold.h"
#include "trace.h"

class gra {
	traceLevel trace;
	globalThreshold *thres;
	cv::Moments moments;
	cv::Point2f massCenter;
	std::vector<std::vector<cv::Point> > contours;
	double hu[7];

public:
	cv::Mat *image;

	gra(cv::Mat &image, globalThreshold &thres);
	gra(cv::Mat &image, globalThreshold &thres, traceLevel trace);
	void key(char key);
	double threshold();
	size_t findContours(bool print=true);
	int getContour();
	std::vector<cv::Point> * getContourPtr(int index=0);
	void printContour(int contour);
	void findMoments(int contour);
	void printMassCenter();
	void showMoments();
	void findHuMoments();
	void showHuMoments();
	void copyHuMoments(double (*phu)[7]);

private:
	void init(cv::Mat &image, globalThreshold &thres, traceLevel trace);
};

#endif
