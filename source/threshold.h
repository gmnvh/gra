#ifndef _THRESHOLD_H_
#define _THRESHOLD_H_

#include "opencv2/imgproc/imgproc.hpp"

class globalThreshold {

public:
	double apply(cv::InputArray src, cv::OutputArray dst);
	double apply(cv::InputArray src, cv::OutputArray dst, double thresh);
};

class adaptThreshold : globalThreshold {
public:
	double apply(cv::InputArray src, cv::OutputArray dst);
};

#endif
