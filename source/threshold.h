#ifndef _THRESHOLD_H_
#define _THRESHOLD_H_

#include "opencv2/imgproc/imgproc.hpp"

class globalThreshold {
public:
	virtual ~globalThreshold(){}
	virtual double apply(cv::InputArray src, cv::OutputArray dst) = 0;
};

class otsuThreshold : public globalThreshold {
public:
	double apply(cv::InputArray src, cv::OutputArray dst);
};

class adaptThreshold : public globalThreshold {
public:
	double apply(cv::InputArray src, cv::OutputArray dst);
};

class meanThreshold : public globalThreshold {
public:
	double apply(cv::InputArray src, cv::OutputArray dst);
};

#endif
