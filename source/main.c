/**
 *  The GRA Project
 *  Gesture Recognition for Automobiles.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define THIS_MODULE "MAIN"
#define THIS_LEVEL gOption.trace
#include "trace.h"

#include "main.h"

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

void image(void)
{
	IplImage *lImgOriginal = cvLoadImage(gOption.inputFile, CV_LOAD_IMAGE_COLOR);
	IplImage *lImgProcessed, *lImgTemp;
	char lCheckForEscKey;

	if (lImgOriginal == NULL) {
		TRACE_ERROR("Unable to open file: %s", gOption.inputFile);
		return;
	}

	lImgProcessed = cvCreateImage(cvSize(lImgOriginal->width, lImgOriginal->height), IPL_DEPTH_8U, 1);
	lImgTemp = cvCreateImage(cvSize(lImgOriginal->width, lImgOriginal->height), IPL_DEPTH_8U, 1);

	{
		/* It is necessary to convert to gray scale before apply the filter */
		cvCvtColor(lImgOriginal, lImgProcessed, CV_BGR2GRAY);

		double lTh = cvThreshold(lImgProcessed,  	     				/* src */
								 lImgProcessed, 						/* dst */
								 127,           						/* thresh */
								 255,                         			/* maxval */
								 CV_THRESH_BINARY | CV_THRESH_OTSU   	/* type */
								 );

		TRACE_INFO("Otsu: %lf", lTh);

		cvMorphologyEx(lImgProcessed, lImgProcessed, lImgTemp, NULL, CV_MOP_OPEN, 10);
		cvShowImage("Processed", lImgProcessed);


		{
			int i;
			CvMemStorage *storage = cvCreateMemStorage(0);
			CvSeq *contours = 0;//cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage);
			cvFindContours(lImgProcessed, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL,
					CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
			TRACE_INFO("contours->total = %d\n", contours->total);

			for (; contours != 0; contours = contours->h_next)
			{
			    cvDrawContours(lImgOriginal, contours, CV_RGB(255, 0, 0), CV_RGB(255,0,255), 1, 0, 8, cvPoint(0,0));
			    //TRACE_INFO("Contour");
			}
		}

	}

	cvShowImage("Original", lImgOriginal);
	//cvShowImage("Processed", lImgProcessed);

	lCheckForEscKey = cvWaitKey(0);
	if (lCheckForEscKey == 27) {
		return;
	}
}

void video(void)
{
	IplImage *frame;
	IplImage *lImgProcessed, *lImgTemp;
	CvCapture *capture = cvCreateFileCapture(gOption.inputFile);
	CvMemStorage *storage = cvCreateMemStorage(0);
	CvMemStorage* hullStorage = cvCreateMemStorage(0);

	lImgProcessed = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
	lImgTemp = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);

	while (1) {
		char c;

		frame = cvQueryFrame( capture );
		if ( !frame ) {
			printf("Frame error");
			break;
		}

		{
			/* It is necessary to convert to gray scale before apply the filter */
			cvCvtColor(frame, lImgProcessed, CV_BGR2GRAY);

			double lTh = cvThreshold(lImgProcessed,  	     				/* src */
					                 lImgProcessed, 						/* dst */
					                 127,           						/* thresh */
					                 255,                         			/* maxval */
					                 CV_THRESH_BINARY | CV_THRESH_OTSU   	/* type */
					                 );

			TRACE_PR_DEBUG(50, "Otsu: %lf", lTh);
		}


		cvMorphologyEx(lImgProcessed, lImgProcessed, lImgTemp, NULL, CV_MOP_OPEN, 5);
		cvShowImage("Processed", lImgProcessed);

#if 1
		{
			double biggestArea = 0, currentArea = 0;
			CvSeq *contours = 0;//cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage);
			CvSeq *biggestContour = 0;
			cvFindContours(lImgProcessed, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL,
					CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
			TRACE_DEBUG("contours->total = %d\n", contours->total);

			for (; contours != 0; contours = contours->h_next)
			{
				currentArea = cvContourArea(contours, CV_WHOLE_SEQ, 0);
				TRACE_DEBUG("Area: %f", currentArea);

			    if (currentArea > biggestArea) {
			    	biggestContour = contours;
			    	biggestArea = currentArea;
			    	TRACE_DEBUG("Biggest: %f, %d", biggestArea, contours->total);

			    }
			    //TRACE_INFO("Contour");
			}
			cvDrawContours(frame, biggestContour, CV_RGB(255, 0, 0), CV_RGB(255,0,255), 0, 0, 8, cvPoint(0,0));
			{
				CvSeq *hulls = cvConvexHull2(biggestContour, hullStorage, CV_CLOCKWISE, 1);
				CvRect rec = cvBoundingRect(biggestContour, 0);
				cvDrawContours(frame, hulls, CV_RGB(0,255,0), CV_RGB(0, 255, 255), 0, 0, 8, cvPoint(0,0));
				cvRectangle(frame, cvPoint(rec.x, rec.y), cvPoint(rec.x+rec.width, rec.y+rec.height), cvScalar(255, 0, 0, 0), 1, 8, 0);
			}
			cvShowImage("Original", frame);
		}
#endif

		c = cvWaitKey(0);
		if ( c == 27 ) break;
	}

	cvReleaseCapture(&capture);
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
	/* Process the command options */
	if (options(argc, argv) < 0) {
		/* Option error */
		return (-1);
	}

	/* Welcome message */
	TRACE_DEBUG("The GRA Project");
	TRACE_DEBUG("Input type: %d, %s", gOption.input, gOption.inputFile);

	/* Create windows */
	cvNamedWindow("Original", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Processed", CV_WINDOW_AUTOSIZE);

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
