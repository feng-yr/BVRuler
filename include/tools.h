#ifndef _TOOL_H_
#define _TOOL_H_

#include <opencv2/opencv.hpp>

void getHist(cv::Mat &inputImage, cv::Mat &hist, int &histSize);
void getHistImg(cv::Mat &inputImage, cv::Mat &histImage);
void thresh(cv::Mat &src, cv::Mat &dst, int lower, int upper);
cv::Rect getMinRect(cv::Mat &inputImg);
cv::Point getCenterPoint(cv::Rect rect);

#endif  // _TOOL_H_