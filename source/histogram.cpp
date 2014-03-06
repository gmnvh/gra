#include "opencv2/imgproc/imgproc.hpp"
#include "histogram.h"

using namespace cv;

/* Histogram height */
#define HIST_H 400

/* Histogram width */
#define HIST_W 512

void showHistogram(Mat &img, Mat &dest, int thres)
{
	showHistogram(img, dest);
	line(dest, Point(thres, 0), Point(thres, HIST_H), Scalar(255, 0, 0), 2, 8, 0);
}

void showHistogram(Mat &img, Mat &dest)
{
	  /* Separate the image in 3 places ( B, G and R ) */
	  vector<Mat> bgrPlanes;
	  split(img, bgrPlanes);

	  /* Establish the number of bins */
	  int histSize = 256;

	  /* Set the ranges ( for B,G,R) ) */
	  float range[] = {0, 256} ;
	  const float *histRange = {range};

	  bool uniform = true;
	  bool accumulate = false;

	  Mat b_hist, g_hist, r_hist;

	  /* Compute the histograms: */
	  calcHist(&bgrPlanes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);

	  if (bgrPlanes.size() == 3) {
		  calcHist(&bgrPlanes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
		  calcHist(&bgrPlanes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
	  }

	  /* Draw the histograms for B, G and R */
	  int hist_w = HIST_W;
	  int hist_h = HIST_H;
	  int bin_w = cvRound((double) hist_w/histSize);
	  Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0,0,0));

	  /* Normalize the result to [ 0, histImage.rows ] */
	  normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	  if (bgrPlanes.size() == 3) {
		  normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
		  normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	  }

	  Scalar firstChannelColor;
	  if (bgrPlanes.size() == 3) {
		  firstChannelColor = Scalar(255, 0, 0);
	  } else {
		  firstChannelColor = Scalar(200, 200, 200);
	  }

	  /* Draw for each channel */
	  for (int i = 1; i < histSize; i++) {
	      line(histImage, Point(bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1))),
	                      Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
	                      firstChannelColor, 2, 8, 0);

	      if (bgrPlanes.size() == 3) {
			  line(histImage, Point(bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1))),
							  Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
							  Scalar(0, 255, 0), 2, 8, 0);
			  line(histImage, Point(bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1))),
							  Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
							  Scalar(0, 0, 255), 2, 8, 0);
	      }
	  }

	  dest = histImage;
}
