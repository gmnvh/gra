#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "threshold.h"

double globalThreshold::apply(cv::InputArray src, cv::OutputArray dst)
{
	return cv::threshold(src,  	     						/* src */
					 	 dst, 								/* dst */
					 	 255,           					/* thresh */
					 	 255,                         		/* maxval */
					 	 CV_THRESH_BINARY | CV_THRESH_OTSU  /* type */
	);
}

double globalThreshold::apply(cv::InputArray src, cv::OutputArray dst, double thresh)
{
	return cv::threshold(src,  	     		/* src */
					 	 dst, 				/* dst */
					 	 thresh,           	/* thresh */
					 	 255,               /* maxval */
					 	 CV_THRESH_BINARY   /* type */
	);
}
