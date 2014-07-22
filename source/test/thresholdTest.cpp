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
	char imgList[][100] = {
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_01.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_02.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_03.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_04.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_05.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_06.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_07.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_08.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_09.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_10.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_11.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_12.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_13.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_14.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_15.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_16.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_17.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_18.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_19.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_20.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_21.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_22.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\0_23.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\1_01.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\1_02.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\1_03.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\1_04.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\1_05.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\1_06.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\2_01.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\2_02.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\2_03.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\2_04.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\2_05.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\2_06.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\2_07.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\2_08.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\2_09.jpg",
			"D:\\Gustavo\\Pessoal\\Mestrado\\FEI\\Tese\\gra\\photos\\ir_training\\2_10.jpg",

	};

	/* List of thresholds to test */
	struct {globalThreshold *thresInst; char desc[20];} thresList[] = {
			{new otsuThreshold(), "Otsu"},
			{new meanThreshold(), "Mean"},
			{new adaptThreshold(), "Adapt"}
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
			char filename[100];
			sprintf(filename, "%s_%s.jpg", imgList[i], thresList[j].desc);
			imwrite(filename, imgProc);
		}
	}

	return;
}
