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

#include "threshold.h"

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

void proc01(Mat &image) {

	/* It is necessary to convert to gray scale before apply the filter */
	cvtColor(image, image, CV_BGR2GRAY);
}

void image(void)
{
	Mat lImgOriginal = imread(gOption.inputFile, CV_LOAD_IMAGE_COLOR);
	Mat lImgProcessed, lImgTemp;
	char lCheckForEscKey;

	if (lImgOriginal.data == NULL) {
		TRACE_ERROR("Unable to open file: %s", gOption.inputFile);
		return;
	}

#if 0
	/* Threshold test */
	{
		Mat lImgOtsu, lImgAdaptive;
		globalThreshold *thresh = new globalThreshold();
		adaptThreshold *adaptThresh = new adaptThreshold();

		namedWindow("Otsu", CV_WINDOW_AUTOSIZE);
		namedWindow("Adaptive", CV_WINDOW_AUTOSIZE);

		cvtColor(lImgOriginal, lImgOtsu, CV_BGR2GRAY);
		cvtColor(lImgOriginal, lImgAdaptive, CV_BGR2GRAY);

		double lTh = thresh->apply(lImgOtsu, lImgOtsu);
		TRACE_INFO("Otsu threshold: %lf", lTh);

		adaptThresh->apply(lImgAdaptive, lImgAdaptive);

		imshow("Original", lImgOriginal);
		imshow("Otsu", lImgOtsu);
		imshow("Adaptive", lImgAdaptive);
	}
#endif
#if 1
	{
		RNG rng(12345);

		/* It is necessary to convert to gray scale before apply the filter */
		cvtColor(lImgOriginal, lImgProcessed, CV_BGR2GRAY);

		globalThreshold *thresh = new globalThreshold();
		double lTh = thresh->apply(lImgProcessed, lImgProcessed);
		TRACE_INFO("Otsu: %lf", lTh);

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		/// Find contours
		findContours( lImgProcessed, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

		/// Get the moments
		vector<Moments> mu(contours.size());
		double muhu[contours.size()][7];

		for( unsigned i = 0; i < contours.size(); i++ ) {
			mu[i] = moments( contours[i], false );
			HuMoments(mu[i], muhu[i]);
			//muhu[0][0] = 0.1;
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
			TRACE_INFO("Delta: %lf", mdelta[i]);

		}

		/// Draw contours
		Mat drawing = Mat::zeros( lImgProcessed.size(), CV_8UC3 );
		for( int i = 0; i< contours.size(); i++ )
		{
			Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
			drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
			circle( drawing, mc[i], 4, color, -1, 8, 0 );
		}


		printf("\t Info: Area and Contour Length \n");
		for( int i = 0; i< contours.size(); i++ )
		{
			printf(" * Contour[%d] - Area (M_00) = %.2f - Area OpenCV: %.2f - Length: %.2f \n", i, mu[i].m00, contourArea(contours[i]), arcLength( contours[i], true ) );
			printf("Hu: {%f, %f, %f, %f, %f, %f, %f}\n", muhu[i][0],muhu[i][1],muhu[i][2],muhu[i][3],muhu[i][4],muhu[i][5],muhu[i][6]);
			Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
			drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
			circle( drawing, mc[i], 4, color, -1, 8, 0 );

			Point massc = Point((int)mc[i].x, (int)mc[i].y);

			if (mu[i].m00 > 1000) {
				line(drawing,
						massc,
						Point(massc.x + (100 * cos(mdelta[i])), massc.y + (100 * sin(mdelta[i]))),
						color, 2, CV_AA, 0);

				Mat sampleMat = (Mat_<float>(1,7) << muhu[i][0], muhu[i][1], muhu[i][2], muhu[i][3], muhu[i][4], muhu[i][5], muhu[i][6]);
				float rsp = mySVM.predict(sampleMat);
				if (rsp == 1) {
					printf("Dedo\n");
				} else if (rsp == -1) {
					printf("Open Hand\n");
				} else {
					printf("Nada\n");
				}
			}
		}
		imshow("Processed", drawing);

	}
#endif
	imshow("Original", lImgOriginal);
	//imshow("Processed", lImgProcessed);

	lCheckForEscKey = cvWaitKey(0);
	if (lCheckForEscKey == 27) {
		return;
	}
}

void video(void)
{
//	Mat frame;
//	Mat lImgProcessed, lImgTemp;
//	CvCapture *capture = cvCreateFileCapture(gOption.inputFile);
//	CvMemStorage *storage = cvCreateMemStorage(0);
//	CvMemStorage* hullStorage = cvCreateMemStorage(0);
//
//	lImgProcessed = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
//	lImgTemp = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
//
//	while (1) {
//		char c;
//
//		frame = cvQueryFrame( capture );
//		if ( !frame ) {
//			printf("Frame error");
//			break;
//		}
//
//		{
//			/* It is necessary to convert to gray scale before apply the filter */
//			cvCvtColor(frame, lImgProcessed, CV_BGR2GRAY);
//
//			threshold thresh = new threshold();
//			double lTh = thresh.apply(lImgProcessed, lImgProcessed);
//
////			double lTh = cvThreshold(lImgProcessed,  	     				/* src */
////					                 lImgProcessed, 						/* dst */
////					                 127,           						/* thresh */
////					                 255,                         			/* maxval */
////					                 CV_THRESH_BINARY | CV_THRESH_OTSU   	/* type */
////					                 );
//
//			TRACE_PR_DEBUG(50, "Otsu: %lf", lTh);
//		}
//
//
//		cvMorphologyEx(lImgProcessed, lImgProcessed, lImgTemp, NULL, CV_MOP_OPEN, 5);
//		cvShowImage("Processed", lImgProcessed);
//
//#if 1
//		{
//			double biggestArea = 0, currentArea = 0;
//			CvSeq *contours = 0;//cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage);
//			CvSeq *biggestContour = 0;
//			cvFindContours(lImgProcessed, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL,
//					CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
//			TRACE_DEBUG("contours->total = %d\n", contours->total);
//
//			for (; contours != 0; contours = contours->h_next)
//			{
//				currentArea = cvContourArea(contours, CV_WHOLE_SEQ, 0);
//				TRACE_DEBUG("Area: %f", currentArea);
//
//			    if (currentArea > biggestArea) {
//			    	biggestContour = contours;
//			    	biggestArea = currentArea;
//			    	TRACE_DEBUG("Biggest: %f, %d", biggestArea, contours->total);
//
//			    }
//			    //TRACE_INFO("Contour");
//			}
//			cvDrawContours(frame, biggestContour, CV_RGB(255, 0, 0), CV_RGB(255,0,255), 0, 0, 8, cvPoint(0,0));
//			{
//				CvSeq *hulls = cvConvexHull2(biggestContour, hullStorage, CV_CLOCKWISE, 1);
//				CvRect rec = cvBoundingRect(biggestContour, 0);
//				cvDrawContours(frame, hulls, CV_RGB(0,255,0), CV_RGB(0, 255, 255), 0, 0, 8, cvPoint(0,0));
//				cvRectangle(frame, cvPoint(rec.x, rec.y), cvPoint(rec.x+rec.width, rec.y+rec.height), cvScalar(255, 0, 0, 0), 1, 8, 0);
//			}
//			cvShowImage("Original", frame);
//		}
//#endif
//
//		c = cvWaitKey(0);
//		if ( c == 27 ) break;
//	}
//
//	cvReleaseCapture(&capture);
}

void webcam(void)
{
	CvCapture *lCapWebCam;
	IplImage *lImgOriginal;
	IplImage *lImgProcessed;
	char lCheckForEscKey;

	lCapWebCam = cvCaptureFromCAM(0);
	if (lCapWebCam == NULL) {
		printf("Error: camera capture is NULL\r\n");
		return;
	}

	lImgProcessed = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);

	while(1) {
		lImgOriginal = cvQueryFrame(lCapWebCam);


		if (lImgOriginal == NULL) {
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
#if 1
		{
			/* It is necessary to convert to gray scale before apply the filter */
			cvCvtColor(lImgOriginal, lImgProcessed, CV_BGR2GRAY);

			double lTh = cvThreshold(lImgProcessed,  	     				/* src */
					                 lImgProcessed, 						/* dst */
					                 127,           						/* thresh */
					                 255,                         			/* maxval */
					                 CV_THRESH_BINARY | CV_THRESH_OTSU   	/* type */
					                 );

			TRACE_PR_DEBUG(50, "Otsu: %lf", lTh);
		}
#endif

		cvShowImage("Original", lImgOriginal);
		cvShowImage("Processed", lImgProcessed);

		lCheckForEscKey = cvWaitKey(10);
		if (lCheckForEscKey == 27) {
			break;
		}
	}

	/* Do not forget to release the camera, otherwise the program does not exit */
	cvReleaseCapture(&lCapWebCam);
}

int main(int argc, char **argv)
{
	setbuf(stdout, NULL);

	/* Process the command options */
	if (options(argc, argv) < 0) {
		/* Option error */
		return (-1);
	}

	/* Welcome message */
	TRACE_DEBUG("The GRA Project");
	TRACE_DEBUG("Input type: %d, %s", gOption.input, gOption.inputFile);

	/* Training */
	float labels[2] = {1.0, -1.0};
    Mat labelsMat(2, 1, CV_32FC1, labels);

    float trainingData[2][7] = {  {0.331761, 0.077379, 0.001497, 0.000141, -0.000000, -0.000026, 0.000000} ,
    							  {0.303489, 0.049587, 0.002935, 0.000395, 0.000000, 0.000087, 0.000000}
    						   };
    Mat trainingDataMat(2, 7, CV_32FC1, trainingData);

    // Set up SVM's parameters
    CvSVMParams params;
    params.svm_type    = CvSVM::C_SVC;
    params.kernel_type = CvSVM::LINEAR;
    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

    // Train the SVM
    mySVM.train(trainingDataMat, labelsMat, Mat(), Mat(), params);

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
