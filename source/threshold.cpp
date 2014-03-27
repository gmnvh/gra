#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "threshold.h"

double otsuThreshold::apply(cv::InputArray src, cv::OutputArray dst)
{
	return cv::threshold(src,  	     						/* src */
					 	 dst, 								/* dst */
					 	 255,           					/* thresh */
					 	 255,                         		/* maxval */
					 	 CV_THRESH_BINARY | CV_THRESH_OTSU  /* type */
	);
}

double adaptThreshold::apply(cv::InputArray src, cv::OutputArray dst)
{
	cv::adaptiveThreshold(src, dst, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 7, 0);
	return 0;
}

double meanThreshold::apply(cv::InputArray src, cv::OutputArray dst)
{
	cv::Scalar t = mean(src);
	return cv::threshold(src,  	     		/* src */
						 dst, 				/* dst */
						 t.val[0],        	/* thresh */
						 255,               /* maxval */
						 CV_THRESH_BINARY   /* type */
		);
}
