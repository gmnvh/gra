/**
 *  The GRA Project
 *  Gesture Recognition for Automobiles
 */
#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

/* \todo [GMN] Get input from a video file */

int main(int argc, char **argv)
{

	CvCapture *lCapWebCam;
	IplImage *lImgOriginal;
	IplImage *lImgProcessed;
	char lCheckForEscKey;

	/* Welcome message */
	printf("The GRA project\r\n");

	lCapWebCam = cvCaptureFromCAM(0);
	if (lCapWebCam == NULL) {
		printf("Error: camera capture is NULL\r\n");
		return (-1);
	}

	cvNamedWindow("Original", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Processed", CV_WINDOW_AUTOSIZE);

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

			printf("Otsu: %lf\r\n", lTh);

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
	cvDestroyWindow("Original");
	cvDestroyWindow("Processed");
	return 0;
}
