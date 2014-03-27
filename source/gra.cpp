#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "threshold.h"
#include "gra.h"

#define THIS_MODULE "GRA"
#define THIS_LEVEL this->trace
#include "trace.h"

using namespace cv;

gra::gra(Mat &image, globalThreshold &thres)
{
	init(image, thres, TRACE_LV_INFO);
}

gra::gra(Mat &image, globalThreshold &thres, traceLevel trace)
{
	init(image, thres, trace);
}

void gra::init(cv::Mat &image, globalThreshold &thres, traceLevel trace)
{
	this->image = &image;
	this->thres = &thres;
	this->trace = trace;
}

void gra::key(char key)
{
	static unsigned fileCounter = 0;

	switch (key) {
	case 's':
	{
		char file[10];
		sprintf(file, "%02u.jpg", fileCounter++);
		bool rsp = imwrite(file, *image);
		TRACE_INFO("File %s write: %u", file, (unsigned)rsp);

	}
		break;
	}
}

double gra::threshold()
{
	return this->thres->apply(*image, *image);
}

size_t gra::findContours(bool print)
{
	RNG rng(12345);
	vector<Vec4i> hierarchy;

	/* Find contours */
	cv::findContours(*image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	if (print == true) {
		for (unsigned int i = 0; i< contours.size(); i++) {
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
			drawContours(*image, contours, i, color, 2, 8, hierarchy, 0, Point());
		}
	}

	return contours.size();
}

void gra::printContour(int contour)
{
	RNG rng(12345);
	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
	drawContours(*image, contours, contour, color, 2, 8);
}

int gra::getContour()
{
	int rsp = 0;
	vector<Point> *c;

	c = &contours[0];
	for (unsigned i=1; i < contours.size(); i++) {
		if (contourArea(contours[i]) > contourArea(*c)) {
			c = &contours[i];
			rsp = i;
		}
	}

	return rsp;
}

vector<Point> * gra::getContourPtr(int index)
{
	return &contours[index];
}

void gra::findMoments(int contour)
{
	moments = cv::moments(contours[contour], false);
	massCenter = Point2f(moments.m10/moments.m00 , moments.m01/moments.m00);
}

void gra::printMassCenter()
{
	RNG rng(12345);
	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
	circle(*image, massCenter, 4, color, -1, 8, 0);
}

void gra::showMoments()
{
	TRACE_INFO("Area: %f", moments.m00);
	TRACE_INFO("Mass center: %f, %f", massCenter.x, massCenter.y);
}

void gra::findHuMoments()
{
	HuMoments(moments, hu);
}

void gra::showHuMoments()
{
	TRACE_INFO("Hu Moments: %f, %f, %f, %f, %f, %f, %f", hu[0], hu[1], hu[2], hu[3], hu[4], hu[5], hu[6]);
}

void gra::copyHuMoments(double (*phu)[7])
{
	for (unsigned i=0; i < 7; i++) {
		(*phu)[i] = hu[i];
	}
}
