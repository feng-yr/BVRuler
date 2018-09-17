// imgseg.cpp : Image segmentation
//

#include "imgseg.h"
#include "tools.h"

void segment(cv::Mat &inputImage, std::vector<cv::Mat> &seg, int threshold, int maxInter, int minArea)
{
	//- Computing histogram
	cv::Mat hist;
	int histSize;
	getHist(inputImage, hist, histSize);

	int lowerLimit = -1, upperLimit = -1, interval = 0, area = 0;

	for (int i = histSize - 1; i >= 0; i--)
	{
		if (upperLimit < 0)
		{
			if (hist.at<float>(i) >= threshold)
			{
				upperLimit = i;
				area += hist.at<float>(i);
			}
		}
		else
		{
			if (hist.at<float>(i) >= threshold)
			{
				area += hist.at<float>(i);
				interval = 0;
				lowerLimit = -1;
			}
			else
			{
				interval++;
				if (interval > maxInter)
				{
					if (area >= minArea)
					{
						cv::Mat mask;
						thresh(inputImage, mask, lowerLimit, upperLimit);
						seg.push_back(mask);
					}
					lowerLimit = -1;
					upperLimit = -1;
					area = 0;
				}
				else
				{
					if (lowerLimit < 0)
						lowerLimit = i + 1;
					area += hist.at<float>(i);
				}
			}
		}

	}
}