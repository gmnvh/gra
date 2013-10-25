/**
 *  The GRA Project
 *  Gesture Recognition for Automobiles
 */

#include <opencv/cv.h>
#include <opencv/highgui.h>
//#include <opencv/cvaux.h>
//#include <opencv/cxcore.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{

	CvCapture *lCapWebCam;
	IplImage *lImgOriginal;
	IplImage *lImgProcessed;
	char lCheckForEscKey;
//	CvMemStorage *p_strStorage;
//	CvSeq *p_seqCircles;
//	float *p_fltXYRadius;
//	int i;

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

//		cvInRangeS(p_imgOriginal, CV_RGB(70, 70, 170), 		/* min filtering */
//								  CV_RGB(90, 90, 250),		/* max filtering */
//								  p_imgProcessed);
//
//		p_strStorage = cvCreateMemStorage(0);
//
//		cvSmooth(p_imgProcessed, p_imgProcessed, CV_GAUSSIAN, 9, 9, 0, 0);
//
//		p_seqCircles = cvHoughCircles(p_imgProcessed, p_strStorage, CV_HOUGH_GRADIENT,
//									  2, (p_imgProcessed->height /4), 100, 50, 10, 400);
//
//		for (i=0; i < p_seqCircles->total; i++) {
//			p_fltXYRadius = (float*)cvGetSeqElem(p_seqCircles, i);
//			printf("ball position x = %f, y = %f, r = %f\n", p_fltXYRadius[0], p_fltXYRadius[1], p_fltXYRadius[2]);
//
//			cvCircle(p_imgOriginal, cvPoint(cvRound(p_fltXYRadius[0]), cvRound(p_fltXYRadius[1])),
//					 3, CV_RGB(0,255,0), CV_FILLED);
//
//			cvCircle(p_imgOriginal, cvPoint(cvRound(p_fltXYRadius[0]), cvRound(p_fltXYRadius[1])),
//					 cvRound(p_fltXYRadius[2]), CV_RGB(255,0,0), 3);
//		}

		cvShowImage("Original", lImgOriginal);
		cvShowImage("Processed", lImgProcessed);
//		cvReleaseMemStorage(&p_strStorage);

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
