#ifndef _CONTOURS_H_
#define _CONTOURS_H_

#include "opencv2/imgproc/imgproc.hpp"

class contoursImg {
public:
	virtual ~contoursImg(){}
	virtual unsigned get(cv::OutputArray,  std::vector<cv::Point> &) = 0;
};

class biggestContour : public contoursImg {
protected:
	std::vector<std::vector<cv::Point> > contoursList;

public:
	biggestContour();
	unsigned get(cv::OutputArray,  std::vector<cv::Point> &);
	void getAll(std::vector<std::vector<cv::Point> > &);
};

#endif
