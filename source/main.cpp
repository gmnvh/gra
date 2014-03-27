/**
 *  The GRA Project
 *  Gesture Recognition for Automobiles.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv2/ml/ml.hpp"
#include "histogram.h"

#include "threshold.h"
#include "gra.h"

#define THIS_MODULE "MAIN"
#define THIS_LEVEL gOption.trace
#include "trace.h"

#include "main.h"

using namespace cv;

//CvSVM SVM;
CvSVM mySVM;

/**
 * Global variables
 */

/** Program options */
mainTOption gOption = mainTOptionInit();

int options(int argc, char **argv)
{
	int lRsp = 0;
	int lOpt;

	while ((lOpt = getopt(argc, argv, "diwep:v:")) != -1) {
		switch (lOpt) {
		case 'd':
			/* Configure trace level for debug */
			gOption.trace = TRACE_LV_DEBUG;
			lRsp = 0;
			break;
		case 'i':
			/* Configure trace level for info */
			gOption.trace = TRACE_LV_INFO;
			lRsp = 0;
			break;
		case 'w':
			/* Configure trace level for info */
			gOption.trace = TRACE_LV_WARNING;
			lRsp = 0;
			break;
		case 'e':
			/* Configure trace level for info */
			gOption.trace = TRACE_LV_ERROR;
			lRsp = 0;
			break;
		case 'p':
			/* Configure the input as photo */
			gOption.input = INPUT_PHOTO;
			gOption.inputFile = optarg;
			lRsp = 0;
			break;
		case 'v':
			/* Configure input as video */
			gOption.input = INPUT_VIDEO;
			gOption.inputFile = optarg;
			lRsp = 0;
			break;
		case '?':
			/* Help */
			printf(OPT_HELP);
			lRsp = -2;
			break;
		default:
			/* Unknown option */
			printf(OPT_HELP);
			lRsp = -3;
			break;
		}
	}

	return lRsp;
}

void contours(void)
{
	Mat imgOriginal, imgProc;

	/* Open image */
	imgOriginal = imread("open_hand.jpg", CV_LOAD_IMAGE_COLOR);

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

	findContours(imgProc, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
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

void proc01(Mat &image, globalThreshold *thresh) {

	double lTh = thresh->apply(image, image);
	//TRACE_INFO("Otsu: %lf", lTh);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/* Find contours */
	findContours(image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Get the moments
	vector<Moments> mu(contours.size());
	double muhu[contours.size()][7];

	for( unsigned i = 0; i < contours.size(); i++ ) {
		mu[i] = moments( contours[i], false );
		HuMoments(mu[i], muhu[i]);
	}

	///  Get the mass centers:
	vector<Point2f> mc( contours.size() );
	for( int i = 0; i < contours.size(); i++ )
	{ mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); }

	vector<double> mdelta(contours.size());
	for (unsigned i=0; i < contours.size(); i++) {
		double a = (mu[i].m20 / mu[i].m00) - (mc[i].x * mc[i].x);
		double b = 2 * ((mu[i].m11 / mu[i].m00) - (mc[i].x * mc[i].y));
		double c = (mu[i].m02 / mu[i].m00) - (mc[i].y * mc[i].y);

		mdelta[i] = atan2(b,(a-c))/2;
		//TRACE_INFO("Delta: %lf", mdelta[i]);
	}

	RNG rng(12345);

	/// Draw contours
	//Mat drawing = Mat::zeros( image.size(), CV_8UC3 );
	for( int i = 0; i< contours.size(); i++ )
	{
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		drawContours(image, contours, i, color, 2, 8, hierarchy, 0, Point() );
		circle(image, mc[i], 4, color, -1, 8, 0 );
	}


	//printf("\t Info: Area and Contour Length \n");
	for( int i = 0; i< contours.size(); i++ )
	{
		//printf(" * Contour[%d] - Area (M_00) = %.2f - Area OpenCV: %.2f - Length: %.2f \n", i, mu[i].m00, contourArea(contours[i]), arcLength( contours[i], true ) );
		//printf("Hu: {%f, %f, %f, %f, %f, %f, %f}\n", muhu[i][0],muhu[i][1],muhu[i][2],muhu[i][3],muhu[i][4],muhu[i][5],muhu[i][6]);
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		drawContours(image, contours, i, color, 2, 8, hierarchy, 0, Point() );
		circle(image, mc[i], 4, color, -1, 8, 0 );

		Point massc = Point((int)mc[i].x, (int)mc[i].y);

		if (mu[i].m00 > 20000) {
			line(image,
					massc,
					Point(massc.x + (100 * cos(mdelta[i])), massc.y + (100 * sin(mdelta[i]))),
					color, 2, CV_AA, 0);

			Mat sampleMat = (Mat_<float>(1,7) << muhu[i][0], muhu[i][1], muhu[i][2], muhu[i][3], muhu[i][4], muhu[i][5], muhu[i][6]);
			float rsp = mySVM.predict(sampleMat);
			if (rsp == 1) {
				TRACE_PR_DEBUG(60, "Dedo\n");
			} else if (rsp == -1) {
				TRACE_PR_DEBUG(60, "Open Hand\n");
			} else {
				TRACE_PR_DEBUG(60, "Nada\n");
			}
		}
	}
}

struct select {
			double kd[4];
			unsigned ki[4];
			Mat im[4];
		};

void getClose(unsigned d1i, double d1, Mat &image, double *v, unsigned size, unsigned *vi, Mat *im)
{
	for (unsigned i=0; i < size; i++) {
		if (d1 < v[i]) {
			getClose(vi[i], v[i], image, v, size-i, vi, im);
			v[i] = d1;
			vi[i] = d1i;
			image.copyTo(im[i]);

			return;
		}
	}
}

#include <fstream>
#include <iostream>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

char Pose[9][20] = {
		"Open Hand",
		"One",
		"Two",
		"Three",
		"Four",
		"Revolver",
		"Close fingers",
		"Close hand",
		"Ok"
};

void getHuMoments(void)
{
	Mat searchImage, poseImage, image;
	globalThreshold *thresh = new otsuThreshold();
	gra searchGra(searchImage, *thresh);
	gra poseGra(poseImage, *thresh);

	DIR *db, *dt;
	struct dirent *dirb, *dirt;

	namedWindow("1", CV_WINDOW_AUTOSIZE);
	namedWindow("2", CV_WINDOW_AUTOSIZE);
	namedWindow("3", CV_WINDOW_AUTOSIZE);
	namedWindow("4", CV_WINDOW_AUTOSIZE);

	db = opendir("..\\..\\..\\photos\\ir_training");
	if (db == NULL) {
	    cout << "Unable to open directory";
	    return;
	}

	readdir(db);
	readdir(db);

	while ((dirb = readdir(db))) {

		stringstream ss(dirb->d_name);
		string item;
		int group = 0;

		getline(ss, item, '_');
		group = atoi(item.c_str());

		string dirName("..\\..\\..\\photos\\ir_training\\");
		string searchName(dirb->d_name);
		string searchPath = dirName + searchName;
		TRACE_INFO("Search: %s -> %s", searchPath.c_str(), Pose[group]);

		image = imread(searchPath.c_str(), CV_LOAD_IMAGE_COLOR);
		cvtColor(image, searchImage, CV_BGR2GRAY);
		searchGra.threshold();
		searchGra.findContours(false);
		int searchContour = searchGra.getContour();
		searchGra.findMoments(searchContour);
		searchGra.findHuMoments();
		searchGra.showHuMoments();

		dt = opendir("..\\..\\..\\photos\\ir_training");
		if (dt == NULL) {
			cout << "Unable to open directory";
			return;
		}

		readdir(dt);
		readdir(dt);

		select poseSelect;
		poseSelect.kd[0] = 100;
		poseSelect.kd[1] = 100;
		poseSelect.kd[2] = 100;
		poseSelect.kd[3] = 100;

		while ((dirt = readdir(dt))) {

			stringstream ss(dirt->d_name);
			string item;
			unsigned int poseGroup = 0;

			getline(ss, item, '_');
			poseGroup = atoi(item.c_str());

			//TRACE_INFO("Pose: %s -> %s", (dirName + string(dirt->d_name)).c_str(), Pose[poseGroup]);
			image = imread((dirName + string(dirt->d_name)).c_str(), CV_LOAD_IMAGE_COLOR);
			cvtColor(image, poseImage, CV_BGR2GRAY);
			poseGra.threshold();
			poseGra.findContours(false);
			int poseContour = poseGra.getContour();
			poseGra.findMoments(poseContour);
			poseGra.findHuMoments();
			//poseGra.showHuMoments();
			poseGra.printContour(poseContour);

			double hu[7];
			searchGra.copyHuMoments(&hu);
			vector<double> c1(hu, hu + sizeof(hu)/sizeof(*hu));
			poseGra.copyHuMoments(&hu);
			vector<double> c2(hu, hu + sizeof(hu)/sizeof(*hu));

			double d = norm(InputArray(c1), InputArray(c2));
			//TRACE_INFO("Dist: %f", d);
			getClose(poseGroup, d, *poseGra.image, &poseSelect.kd[0], 4, &poseSelect.ki[0], &poseSelect.im[0]);
			//printf("%f, %f, %f, %f - ", kd[0], kd[1], kd[2], kd[3]);
			//printf("%u, %u, %u, %u\n", ki[0], ki[1], ki[2], ki[3]);

		}

		printf("%u, %u, %u, %u\n", poseSelect.ki[0], poseSelect.ki[1], poseSelect.ki[2], poseSelect.ki[3]);
		printf("********************************************************************\n");

		imshow("1", poseSelect.im[0]);
		imshow("2", poseSelect.im[1]);
		imshow("3", poseSelect.im[2]);
		imshow("4", poseSelect.im[3]);

		char c = cvWaitKey(0);
		if (c == 27) return;
	}

	return;
#if 0
	for (unsigned i=0; i < 37; i++) {

		for (unsigned j=i; j < 33; j++) {
			vector<double> c1(huMoments[i], huMoments[i] + sizeof(huMoments[i])/sizeof(*huMoments[i]));
			vector<double> c2(huMoments[j], huMoments[j] + sizeof(huMoments[j])/sizeof(*huMoments[j]));

			printf(" - ");
			printf("%f", norm(InputArray(c1), InputArray(c2)));
		}
		printf("\n");
	}

	for (unsigned i=0; i < 37; i++) {

		double kd[4] = {100, 100, 100, 100};
		unsigned ki[4] = {50, 50, 50, 50};

		printf("%u: ", i);

		for (unsigned j=0; j < 33; j++) {
			if (i == j) continue;

			vector<double> c1(huMoments[i], huMoments[i] + sizeof(huMoments[i])/sizeof(*huMoments[i]));
			vector<double> c2(huMoments[j], huMoments[j] + sizeof(huMoments[j])/sizeof(*huMoments[j]));

			double d = norm(c1,c2);
			getClose(j, d, &kd[0], 4, &ki[0]);
		}

		printf("%u, %u, %u, %u", ki[0], ki[1], ki[2], ki[3]);
		printf("\n");
	}
#endif
}

int lowThreshold = 0;
Mat lImgOriginal;
Mat lImgProcessed;
globalThreshold *thresh = new otsuThreshold();
gra mygra(lImgProcessed, *thresh);

void approxPolygon(int, void*) {
	Mat imgTemp;
	vector<Point> approxPoly;
	vector<Point> *p = mygra.getContourPtr(mygra.getContour());

	cvtColor(lImgOriginal, imgTemp, CV_BGR2GRAY);

	approxPolyDP(InputArray(*p), approxPoly, lowThreshold, true);
	TRACE_INFO("Number of points: %u", approxPoly.size());


	drawContours(imgTemp, vector<vector<Point> >(1, approxPoly), 0, Scalar(255, 100, 0), 1, CV_AA);
	for (unsigned i=0; i < approxPoly.size(); i++) {
		circle(imgTemp, approxPoly[i], 4, Scalar(255, 255, 255), -1, 8, 0);
	}

	for (unsigned i=0; i < p->size(); i++) {
		circle(lImgProcessed, (*p)[i], 4, Scalar(255, 255, 255), -1, 8, 0);
	}

	imshow("Original", imgTemp);
	imshow("Processed", lImgProcessed);
}

float angleBetween(const Point &v1, const Point &v2, const Point &v3)
{
	float x = ((v1.x - v3.x) * (v2.x - v3.x)) + ((v1.y - v3.y) * (v2.y - v3.y));

	x = x / (norm(v1 - v3) * norm(v2 - v3));
	return acos(x);
}

#define COLOR_BLUE 		Scalar(240, 90, 110)
#define COLOR_RED		Scalar(15, 15, 210)
#define COLOR_DARk_RED	Scalar(12, 12, 148)

void image(void)
{
	double palmCenterRadius;
	Point palmCenterLoc;
	float maxCircleRadius;
	Point2f maxCircleLoc;
	vector<int> fingers;

	lImgOriginal = imread(gOption.inputFile, CV_LOAD_IMAGE_COLOR);

	if (lImgOriginal.data == NULL) {
		TRACE_ERROR("Unable to open file: %s", gOption.inputFile);
		return;
	}

	/* It is necessary to convert to gray scale before apply the filter */
	cvtColor(lImgOriginal, lImgProcessed, CV_BGR2GRAY);

	double t = mygra.threshold();
	TRACE_INFO("Global threshold: %u", (unsigned) t);

	/* Find the palm center */
	{
		Mat imgDistance;
		double minVal;
		Point minLoc;

		distanceTransform(lImgProcessed, imgDistance, CV_DIST_L2, CV_DIST_MASK_PRECISE);
		minMaxLoc(imgDistance, &minVal, &palmCenterRadius, &minLoc, &palmCenterLoc);
		TRACE_INFO("Palm center radius: %u: x=%u, y=%u", (int)palmCenterRadius, palmCenterLoc.x, palmCenterLoc.y);
	}

	/* Draw the palm center */
	circle(lImgOriginal, palmCenterLoc, 3, COLOR_RED, -1, 8, 0);
	circle(lImgOriginal, palmCenterLoc, palmCenterRadius, COLOR_RED, 1, 8, 0);
	circle(lImgOriginal, palmCenterLoc, (int)(3.5 * palmCenterRadius), Scalar(100, 0, 255), 1, 8, 0);

	/* Find the contours */
	size_t contoursSize = mygra.findContours(true);
	TRACE_INFO("Contours size: %u", (unsigned) contoursSize);

	/* Get the biggest contour */
	int contourIdx = mygra.getContour();
	TRACE_INFO("Select contour: %u", (unsigned)contourIdx);

	vector<Point> *contour = mygra.getContourPtr(contourIdx);
	TRACE_INFO("Number of points: %u", contour->size());

	TRACE_INFO("Re factoring contour inside 3.5 * palm radius");
	for (unsigned i=0; i < contour->size(); i++) {
		Point p = (*contour)[i];
		double d = norm(p - palmCenterLoc);
		if (d > (palmCenterRadius * 3.5)) {
			(*contour).erase(contour->begin() + i);
		}
	}
	TRACE_INFO("Number of points: %u", contour->size());

	/* Maximum enclosing circle */
	minEnclosingCircle(*contour, maxCircleLoc, maxCircleRadius);
	circle(lImgOriginal, maxCircleLoc, 4, Scalar(0, 0, 100), -1, 8, 0);
	circle(lImgOriginal, maxCircleLoc, (int)maxCircleRadius, Scalar(0, 0, 100), 1, 8, 0);

	/* Find the convex hull */
	vector<int> convHull;
	convexHull(*contour, convHull);
//	drawContours(lImgOriginal, vector<vector<Point> >(1, hull), 0, Scalar(0,100,0), 2, 8);

	vector<Vec4i> defects;
	convexityDefects(*contour, convHull, defects);
	TRACE_INFO("Convexity Defects size: %u", defects.size());

	for (unsigned i=0; i < defects.size(); i++) {

		Vec4i s = defects[i];
		int startIdx = s.val[0];
		int endIdx = s.val[1];
		int defectPtIdx = s.val[2];
		double depth = static_cast<double>(s.val[3]) / 256.0;

		//Point2f p(defectPtIdx, defectPtIdx);
		Scalar lineColor = Scalar(0, 100, 0);

		if (depth > 5) {
			bool discarded = false;

			TRACE_INFO("%u : %u : %u : %f", startIdx, endIdx, defectPtIdx, depth);
			if (depth < (palmCenterRadius / 3)) {
				TRACE_INFO("Discarded due depth < 1/3 palm radius");
				discarded = true;
			}

			float da = angleBetween((*contour)[startIdx], (*contour)[endIdx], (*contour)[defectPtIdx]);
			printf("Delta A: %f\r\n", (da * 180.0 / CV_PI));
			if (da > (100 * CV_PI/180)) {
				TRACE_INFO("Discarded due delta A > 100\r\n");
				discarded = true;
			}

			if (discarded == false) {
				float selectK = 2 * CV_PI;
				int selectI = 0;
				for (int j=-10; j <= 10; j++) {

					int lastIdx = contour->size()-1;
					int up = startIdx + 15 + j;
					int down = startIdx - 15 + j;
					int point = startIdx + j;

					if (up > lastIdx) up = up - lastIdx;
					if (down < 0) down = lastIdx + down;
					if (point < 0) point = lastIdx + point;
					if (point > lastIdx) point = point - lastIdx;


					float deltaK;
					deltaK = angleBetween((*contour)[up], (*contour)[down], (*contour)[point]);
					TRACE_INFO("Points: %d, %d, %d : deltaK: %f", up, down, point, (deltaK * 180.0/CV_PI));

					if (deltaK < selectK && deltaK >=0) {
						selectK = deltaK;
						selectI = point;
					}
				}

				printf("Selected point: %d\r\n", selectI);
				if (selectK < (60 * CV_PI / 180.0)) {
					fingers.push_back(selectI);

					int lastIdx = contour->size()-1;
					int up = selectI + 15;
					int down = selectI - 15;

					if (up > lastIdx) up = up - lastIdx;
					if (down < 0) down = lastIdx + down;
					line(lImgOriginal, (*contour)[selectI], (*contour)[up], Scalar(10, 255, 10));
					line(lImgOriginal, (*contour)[selectI], (*contour)[down], Scalar(10, 255, 10));
				}

				selectK = 2 * CV_PI;
				selectI = 0;
				for (int j=-10; j <= 10; j++) {
					int lastIdx = contour->size()-1;
					int up = endIdx + 15 + j;
					int down = endIdx -15 + j;
					int point = endIdx + j;

					if (up > lastIdx) up = up - lastIdx;
					if (down < 0) down = lastIdx + down;
					if (point < 0) point = lastIdx + point;
					if (point > lastIdx) point = point - lastIdx;

					printf("Points: %d, %d, %d", up, down, point);

					float deltaK;
					deltaK = angleBetween((*contour)[up], (*contour)[down], (*contour)[point]);
					printf("DeltaK: %f\r\n", (deltaK * 180.0/CV_PI));

					if (deltaK < selectK && deltaK >=0) {
						selectK = deltaK;
						selectI = point;
					}
				}

				printf("Selected point: %d\r\n", selectI);
				if (selectK < (60 * CV_PI / 180.0)) {
					fingers.push_back(selectI);
				}
			}

			circle(lImgOriginal, (*contour)[startIdx] , 2, Scalar(0, 100, 0), -1, 8, 0 );
			circle(lImgOriginal, (*contour)[endIdx] , 2, Scalar(100, 0, 0), -1, 8, 0 );
			circle(lImgOriginal, (*contour)[defectPtIdx] , 2, Scalar(0, 0, 100), -1, 8, 0 );

			line(lImgOriginal, (*contour)[startIdx], (*contour)[endIdx], lineColor);
			line(lImgOriginal, (*contour)[endIdx], (*contour)[defectPtIdx], lineColor);
			line(lImgOriginal, (*contour)[defectPtIdx], (*contour)[startIdx], lineColor);


			imshow("Original", lImgOriginal);
			//cvWaitKey(0);

		}

	}

	printf("Fingers: %u\r\n", fingers.size());
	printf("Re factoring the fingers, eliminate duplicates\r\n");
	for (unsigned i=1; i < fingers.size(); i++) {
		Point v1 = (*contour)[fingers[i]];
		Point v2 = (*contour)[fingers[i-1]];

		double n = norm(v1 - v2);
		printf("Norm between %u and %u = %f\r\n", i, (i-1), n);

		if (n < 5) {
			printf("Eliminate %u\r\n", i);
			fingers.erase(fingers.begin() + i);
			i--;
		}
	}
	printf("Fingers: %u\r\n", fingers.size());

	/* Draw the finger points */
	for (unsigned i=0; i < fingers.size(); i++) {
		circle(lImgOriginal, (*contour)[fingers[i]] , 4, Scalar(200, 0, 200), -1, 8, 0 );
		circle(lImgProcessed, (*contour)[fingers[i]] , 4, Scalar(200, 0, 200), -1, 8, 0 );

		Point x = (*contour)[fingers[i]] - palmCenterLoc;
		x = x + (*contour)[fingers[i]];
		line(lImgOriginal, palmCenterLoc, x, Scalar(50, 50, 250));
	}

	pyrUp( lImgOriginal, lImgOriginal, Size( lImgOriginal.cols*2, lImgOriginal.rows*2 ));
	imshow("Original", lImgOriginal);
	imshow("Processed", lImgProcessed);

	char c = cvWaitKey(0);
	if (c == 27) return;
}

void video(void)
{
	//Mat frame;
	Mat lImgProcessed, lImgTemp;
	CvCapture *capture = cvCreateFileCapture(gOption.inputFile);
	//CvMemStorage *storage = cvCreateMemStorage(0);
	//CvMemStorage* hullStorage = cvCreateMemStorage(0);

	lImgProcessed = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
	lImgTemp = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);

	globalThreshold *thresh = new otsuThreshold();
	gra mygra(lImgProcessed, *thresh);

	while (1) {
		char c;

		lImgOriginal = cvQueryFrame(capture);
		if (lImgOriginal.data == NULL) {
			printf("Frame error");
			break;
		}

		{
			/* It is necessary to convert to gray scale before apply the filter */
			cvtColor(lImgOriginal, lImgProcessed, CV_BGR2GRAY);

			double t = mygra.threshold();
				TRACE_INFO("Global threshold: %u", (unsigned) t);

				Mat imgDistance;
				distanceTransform(lImgProcessed, imgDistance, CV_DIST_L2, CV_DIST_MASK_PRECISE);
				double maxVal, minVal;
				Point minLoc, maxLoc;

				minMaxLoc(imgDistance, &minVal, &maxVal, &minLoc, &maxLoc);
				TRACE_INFO("Max value: %u: x=%u, y=%u", (int)maxVal, maxLoc.x, maxLoc.y);

				circle(lImgOriginal, maxLoc, 4, Scalar(100, 0, 255), -1, 8, 0);
				circle(lImgOriginal, maxLoc, (int)maxVal, Scalar(100, 0, 255), 1, 8, 0);
				circle(lImgOriginal, maxLoc, (int)(3.5 * maxVal), Scalar(100, 0, 255), 1, 8, 0);

				size_t contoursSize = mygra.findContours(true);
				TRACE_INFO("Contours size: %u", (unsigned) contoursSize);

				int co = mygra.getContour();
				TRACE_INFO("Select contour: %u", (unsigned)co);

				vector<Point> *p = mygra.getContourPtr(co);
				TRACE_INFO("Number of points: %u", (*p).size());

				Point2f center;
				float radius;
				minEnclosingCircle(InputArray(*p), center, radius);

				circle(lImgOriginal, center, 4, Scalar(0, 0, 100), -1, 8, 0);
				circle(lImgOriginal, center, (int)radius, Scalar(0, 0, 100), 1, 8, 0);

				vector<int> hull;
				convexHull(InputArray(*p), hull);
				//drawContours(lImgOriginal, vector<vector<Point> >(1, hull), 0, Scalar(0,100,0), 2, 8);

				vector<Vec4i> defects;
				convexityDefects(*p, hull, defects);
				TRACE_INFO("Convexity Defects size: %u", defects.size());

				for(unsigned i=0; i < defects.size(); i++) {
					Vec4i s = defects[i];

					int startIdx = s.val[0];

					int endIdx = s.val[1];

					int defectPtIdx = s.val[2];

					double depth = static_cast<double>(s.val[3]) / 256.0;


					//Point2f p(defectPtIdx, defectPtIdx);
					if (depth > 5) {
						circle(lImgOriginal, (*p)[startIdx] , 4, Scalar(0, 100, 0), -1, 8, 0 );
						circle(lImgOriginal, (*p)[endIdx] , 4, Scalar(0, 0, 100), -1, 8, 0 );
						circle(lImgOriginal, (*p)[defectPtIdx] , 4, Scalar(100, 0, 0), -1, 8, 0 );

						line(lImgOriginal, (*p)[startIdx], (*p)[endIdx], Scalar(0, 100, 0));
						line(lImgOriginal, (*p)[endIdx], (*p)[defectPtIdx], Scalar(0, 100, 0));
						line(lImgOriginal, (*p)[defectPtIdx], (*p)[startIdx], Scalar(0, 100, 0));

						std::cout << startIdx << ' ' << endIdx << ' ' << defectPtIdx << ' ' << depth << '\n' << '\n' << std::endl;
					}

				}

			imshow("Original", lImgOriginal);
			pyrUp( lImgProcessed, lImgProcessed, Size( lImgProcessed.cols*2, lImgProcessed.rows*2 ));
			imshow("Processed", lImgProcessed);

			c = cvWaitKey(0);
			if (c == 27) break;
			else mygra.key(c);
		}
	}

	cvReleaseCapture(&capture);
}

void webcam(void)
{
	CvCapture *lCapWebCam;
	//Mat lImgOriginal;
	//Mat lImgProcessed;
	char lCheckForEscKey;

	lCapWebCam = cvCaptureFromCAM(0);
	if (lCapWebCam == NULL) {
		printf("Error: camera capture is NULL\r\n");
		return;
	}

	lImgProcessed = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);

	while(1) {
		lImgOriginal = cvQueryFrame(lCapWebCam);


		if (lImgOriginal.data == NULL) {
			printf("Error: frame is NULL\n");
			break;
		}

#if 0
		{
			printf("Channel: %d, %d\r\n", lImgOriginal->alphaChannel, lImgProcessed->alphaChannel);
			printf("Width: %d, %d\r\n", lImgOriginal->width, lImgProcessed->width);
			printf("Depth: %d, %d\r\n", lImgOriginal->depth, lImgProcessed->depth);
		}
#endif

		/* Applying OTSU threshold */

		{
			cvtColor(lImgOriginal, lImgProcessed, CV_BGR2GRAY);
			double t = mygra.threshold();
			//TRACE_INFO("Global threshold: %u", (unsigned) t);

			Mat imgDistance;
			distanceTransform(lImgProcessed, imgDistance, CV_DIST_L2, CV_DIST_MASK_PRECISE);
			double maxVal, minVal;
			Point minLoc, maxLoc;

			minMaxLoc(imgDistance, &minVal, &maxVal, &minLoc, &maxLoc);
			//TRACE_INFO("Max value: %u: x=%u, y=%u", (int)maxVal, maxLoc.x, maxLoc.y);

			circle(lImgOriginal, maxLoc, 4, Scalar(100, 0, 255), -1, 8, 0);
			circle(lImgOriginal, maxLoc, (int)maxVal, Scalar(100, 0, 255), 1, 8, 0);
			circle(lImgOriginal, maxLoc, (int)(3.5 * maxVal), Scalar(100, 0, 255), 1, 8, 0);
//
//			size_t contoursSize = mygra.findContours(true);
//			//TRACE_INFO("Contours size: %u", (unsigned) contoursSize);
//
//			int co = mygra.getContour();
//			//TRACE_INFO("Select contour: %u", (unsigned)co);
//
//			vector<Point> *p = mygra.getContourPtr(co);
//			//TRACE_INFO("Number of points: %u", (*p).size());
//
//			Point2f center;
//			float radius;
//			minEnclosingCircle(InputArray(*p), center, radius);
//
//			circle(lImgOriginal, center, 4, Scalar(0, 0, 100), -1, 8, 0);
//			circle(lImgOriginal, center, (int)radius, Scalar(0, 0, 100), 1, 8, 0);
//
//			if ((*p).size() > 20) {
//				vector<int> hull;
//				convexHull(InputArray(*p), hull);
//				//drawContours(lImgOriginal, vector<vector<Point> >(1, hull), 0, Scalar(0,100,0), 2, 8);
//
//				vector<Vec4i> defects;
//				convexityDefects(*p, hull, defects);
//				//TRACE_INFO("Convexity Defects size: %u", defects.size());
//
//				for(unsigned i=0; i < defects.size(); i++) {
//					Vec4i s = defects[i];
//
//					int startIdx = s.val[0];
//
//					int endIdx = s.val[1];
//
//					int defectPtIdx = s.val[2];
//
//					double depth = static_cast<double>(s.val[3]) / 256.0;
//
//					//std::cout << startIdx << ' ' << endIdx << ' ' << defectPtIdx << ' ' << depth << '\n' << '\n' << std::endl;
//
//					//Point2f p(defectPtIdx, defectPtIdx);
//					if (depth > 5) {
//						circle(lImgOriginal, (*p)[startIdx] , 4, Scalar(0, 100, 0), -1, 8, 0 );
//						circle(lImgOriginal, (*p)[endIdx] , 4, Scalar(0, 0, 100), -1, 8, 0 );
//						circle(lImgOriginal, (*p)[defectPtIdx] , 4, Scalar(100, 0, 0), -1, 8, 0 );
//
//						line(lImgOriginal, (*p)[startIdx], (*p)[endIdx], Scalar(0, 100, 0));
//						line(lImgOriginal, (*p)[endIdx], (*p)[defectPtIdx], Scalar(0, 100, 0));
//						line(lImgOriginal, (*p)[defectPtIdx], (*p)[startIdx], Scalar(0, 100, 0));
//					}
//
//				}
//			}

			imshow("Original", lImgOriginal);
			imshow("Processed", lImgProcessed);

		lCheckForEscKey = cvWaitKey(10);
		if (lCheckForEscKey == 27) {
			break;
		}
	}
	}

	/* Do not forget to release the camera, otherwise the program does not exit */
	cvReleaseCapture(&lCapWebCam);
}

#include <math.h>
void globalThresholdTest(void);

int main(int argc, char **argv)
{
	setbuf(stdout, NULL);
	globalThresholdTest();
	return 0;

	/* Process the command options */
	if (options(argc, argv) < 0) {
		/* Option error */
		return (-1);
	}

	//Mat M(300, 300, CV_8UC1, 255);
	//Mat D(300, 300, CV_8UC1, 255);
//	Mat M = imread("01.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//	Mat D = cvCreateImage(M.size(), IPL_DEPTH_8U, 1);
//
//	for (int i=(M.rows/2); i < M.rows; i++) {
//		for (int j=0; j < M.cols; j++) {
//			M.at<unsigned char>(i, j) = 0u;
//		}
//	}

//	for (int i=0; i < M.rows; i++) {
//		for (int j=0; j < M.cols; j++) {
//			M.at<unsigned char>(i, j) = (unsigned char)16 * i;
//		}
//	}

//	double c;
//	for (int i=1; i < M.rows; i++) {
//		for (int j=1; j < M.cols; j++) {
//
//			c = atan2(M.at<unsigned char>(i,j) - M.at<unsigned char>(i-1,j), M.at<unsigned char>(i,j)-M.at<unsigned char>(i,j-1));
//			c = fabs(c);
//			c = c * 180 / 3.14159265;
//			D.at<unsigned char>(i,j) = (unsigned char)c;
//		}
//	}
//
//	namedWindow("Original", CV_WINDOW_AUTOSIZE);
//	namedWindow("Proc", CV_WINDOW_AUTOSIZE);
//	imshow("Original", M);
//	imshow("Proc", D);
//
//
//	waitKey(-1);
//	return 0;
//
//
//	/* Welcome message */
//	TRACE_DEBUG("The GRA Project");
//	TRACE_DEBUG("Input type: %d, %s", gOption.input, gOption.inputFile);
//
//	//getHuMoments();
//	//return 0;
//
//	/* Training */
//	float labels[2] = {1.0, -1.0};
//    Mat labelsMat(2, 1, CV_32FC1, labels);
//
//    float trainingData[2][7] = {  {0.331761, 0.077379, 0.001497, 0.000141, -0.000000, -0.000026, 0.000000} ,
//    							  {0.303489, 0.049587, 0.002935, 0.000395, 0.000000, 0.000087, 0.000000}
//    						   };
//    Mat trainingDataMat(2, 7, CV_32FC1, trainingData);
//
//    // Set up SVM's parameters
//    CvSVMParams params;
//    params.svm_type    = CvSVM::C_SVC;
//    params.kernel_type = CvSVM::LINEAR;
//    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
//
//    // Train the SVM
//    mySVM.train(trainingDataMat, labelsMat, Mat(), Mat(), params);

	/* Create windows */
	namedWindow("Original", CV_WINDOW_AUTOSIZE);
	namedWindow("Processed", CV_WINDOW_AUTOSIZE);

	switch(gOption.input) {
	case INPUT_WEBCAM:
		webcam();
		break;
	case INPUT_PHOTO:
		image();
		break;
	case INPUT_VIDEO:
		video();
		break;
	}

	cvDestroyWindow("Original");
	cvDestroyWindow("Processed");
	return 0;
}
