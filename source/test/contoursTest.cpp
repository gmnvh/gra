#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "threshold.h"
#include "contours.h"
using namespace cv;
using namespace std;

/**
 * Como melhor retornas um pedaço da lista de contornos ?
 * Seria melhor aplicar filtro na lista  e ja eliminar os indesejados, ou
 * criar duas listas ?
 */
void contoursTest(void)
{
	Mat imgOriginal, imgProc;

	/* Open image */
	imgOriginal = imread("lenna.png", CV_LOAD_IMAGE_COLOR);

	if (imgOriginal.data == NULL) {
		printf("Unable to open file!\r\n");
		return;
	}

	/* Convert to gray scale */
	cvtColor(imgOriginal, imgProc, CV_BGR2GRAY);

	/* Apply the threshold */
	double thres = threshold(imgProc, imgProc, 255, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	printf("Otsu threshold: %f\r\n", thres);

	/* Find contours with no hierarchy */
	vector<vector<Point> > contours;
	vector<Point> biggestCon;
	biggestContour *c = new biggestContour();

	c->get(imgProc, biggestCon);
	c->getAll(contours);
	//findContours(imgProc, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
	printf("Number of contours: %u\r\n", (unsigned)contours.size());

	/* Draw all contours */
	RNG rng(12345);

	for (unsigned i=0; i < contours.size(); i++) {
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(imgProc, contours, i, color, 2);
		printf("Number of point for contour %u: %u\r\n", i, contours[i].size());
	}

	/* Create windows */
	namedWindow("Original", CV_WINDOW_AUTOSIZE);
	namedWindow("Processed", CV_WINDOW_AUTOSIZE);

	/* Show images */
	imshow("Original", imgOriginal);
	imshow("Processed", imgProc);

	cvWaitKey(0);
	imwrite("0_08_contours.jpg", imgProc);
	return;
}
