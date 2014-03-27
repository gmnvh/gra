#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "threshold.h"
using namespace cv;

void globalThresholdTest(void)
{
	Mat imgOriginal, imgGray, imgProc, imgHist;
	unsigned i, j, k;

	/* List of images to calculate */
	char imgList[][20] = {
			"lenna.png",
			"cupcake.jpg"
	};

	/* List of thresholds to test */
	struct {globalThreshold *thresInst; char desc[20];} thresList[] = {
			{new otsuThreshold(), "Otsu"},
			{new meanThreshold(), "Mean"}
	};

	for (i=0; i < (sizeof(imgList)/sizeof(imgList[0])); i++) {

		/* Open image */
		imgOriginal = imread(imgList[i], CV_LOAD_IMAGE_COLOR);

		if (imgOriginal.data == NULL) {
			printf("Unable to open file %s!\r\n", imgList[i]);
			continue;
		}
		printf("\r\nProcessing %s:\r\n", imgList[i]);

		/* Convert to gray scale */
		cvtColor(imgOriginal, imgGray, CV_BGR2GRAY);

		/* Run thresholds */
		for (j=0; j < (sizeof(thresList)/sizeof(thresList[0])); j++) {
			int64 t1 = getTickCount();
			double thres;

			t1 = getTickCount();
			for(k=0; k < 10000; k++) {
				thres = thresList[j].thresInst->apply(imgGray, imgProc);
			}
			printf("%s: %f in %f ms\r\n", thresList[j].desc, thres, (((double)(getTickCount() - t1))/(10 * getTickFrequency())));

			/* Create output files */
			char filename[20];
			sprintf(filename, "%s_%s.jpg", imgList[i], thresList[j].desc);
			imwrite(filename, imgProc);
		}
	}

	return;
}
