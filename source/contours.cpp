#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "contours.h"

using namespace cv;

biggestContour::biggestContour()
{

}

unsigned biggestContour::get(OutputArray image, vector<Point> &out)
{
	findContours(image, contoursList, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

	int biggestArea = 0;
	vector<Point> *c;

	c = &contoursList[0];
	for (unsigned i=1; i < contoursList.size(); i++) {
		if (contourArea(contoursList[i]) > contourArea(*c)) {
			c = &contoursList[i];
			biggestArea = i;
		}
	}

	out = contoursList[biggestArea];
	return biggestArea;
}

void biggestContour::getAll(vector<vector<Point> > &out)
{
	out = contoursList;
}
