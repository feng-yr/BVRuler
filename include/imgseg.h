#ifndef _IMGSEG_H_
#define _IMGSEG_H_

#include <opencv2/opencv.hpp>
#include <vector>

void segment(cv::Mat &inputImage, std::vector<cv::Mat> &seg, int threshold, int maxInter, int minArea);

#endif // _IMGSEG_H_