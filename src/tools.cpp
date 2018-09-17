// tool.cpp : Tool function library
//

#include "tools.h"

void getHist(cv::Mat &inputImage, cv::Mat &hist, int &histSize)
{
	int row = inputImage.rows;  // Number of rows
	int col = inputImage.cols * inputImage.channels();  // The number of column x channels = the number of elements in each row
	int depth = inputImage.depth();

	//-- Solving extreme value
	double minVal; double maxVal;
	cv::minMaxLoc(inputImage, &minVal, &maxVal);


	//-- Set the number of bin
	histSize = cvCeil(maxVal) + 1;

	hist = cv::Mat::zeros(1, histSize, CV_32FC1);

	//-- Compute histogram
	switch (depth)
	{
	case CV_8U:

		//- Double loop, traversing all pixel values
		for (int i = 0; i < row; i++)  //- row loops
		{
			uchar* data = inputImage.ptr<uchar>(i);  //- Get the first address of the ith line
			for (int j = 0; j < col; j++)   //- Column loops
			{
				// ---------[Start processing each pixel]-------------  
				hist.at<float>(data[j])++;
				// ----------[End of processing]---------------------
			}   // End of row processing
		}
		break;

	case CV_16S:

		//- Double loop, traversing all pixel values
		for (int i = 0; i < row; i++)  //- row loops
		{
			short* data = inputImage.ptr<short>(i);  //- Get the first address of the ith row
			for (int j = 0; j < col; j++)   //- Column loops
			{
				// ---------[Start processing each pixel]-------------  
				if (data[j] >= 0)
					hist.at<float>(data[j])++;
				// ----------[End of processing]---------------------
			}   // End of row processing
		}
		break;

	default:
		break;
	}
}

void getHistImg(cv::Mat &inputImage, cv::Mat &histImage)
{
	//- Compute histogram
	cv::Mat hist;
	int histSize;
	getHist(inputImage, hist, histSize);

	//- Create histogram canvas
	int hist_w = 2000; int hist_h = 2000;
	int bin_w = cvRound((double)hist_w / histSize);

	histImage = cv::Mat::Mat(hist_w, hist_h, CV_8UC3, cv::Scalar(0, 0, 0));

	hist.at<float>(0, 0) = 0;

	//- Normalize the histogram to range [ 0, histImage.rows ]
	cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

	//- Draw histograms on the histograms of canvas
	for (int i = 1; i < histSize; i++)
	{
		cv::line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
			cv::Point(bin_w*(i), hist_h - cvRound(hist.at<float>(i))),
			cv::Scalar(0, 0, 255), 2, 8, 0);
	}
}

void thresh(cv::Mat &src, cv::Mat &dst, int lower, int upper)
{
	int row = src.rows;  // Number of rows
	int col = src.cols * src.channels();  // The number of column x channels = the number of elements in each row
	int depth = src.depth();

	dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

	//- Compute histogram
	switch (depth)
	{
	case CV_8U:

		//- Double loop, traversing all pixel values
		for (int i = 0; i < row; i++)  //- row loops
		{
			uchar* srcData = src.ptr<uchar>(i);  // Get the first address of the ith row in src
			uchar* dstData = dst.ptr<uchar>(i);  // Get the first address of the ith row in dst
			for (int j = 0; j < col; j++)   // Column loops
			{
				// ---------[Start processing each pixel]-------------  
				
				if (srcData[j] >= lower && srcData[j] <= upper)
					dstData[j] = 255;
				// ----------[End of processing]---------------------
			}   // End of row processing
		}
		break;

	case CV_16S:

		//- Double loop, traversing all pixel values
		for (int i = 0; i < row; i++)  //- row loops
		{
			short* srcData = src.ptr<short>(i);  // Get the first address of the ith row in src
			uchar* dstData = dst.ptr<uchar>(i);  // Get the first address of the ith row in dst
			for (int j = 0; j < col; j++)   //- Column loops
			{
				// ---------[Start processing each pixel]-------------  
				if (srcData[j] >= lower && srcData[j] <= upper)
					dstData[j] = 255;
				// ----------[End of processing]---------------------
			}   // End of row processing
		}
		break;

	default:
		break;
	}
}

cv::Rect getMinRect(cv::Mat &inputImg)
{
	int row = inputImg.rows;  // Number of rows
	int col = inputImg.cols * inputImg.channels();  // The number of column x channels = the number of elements in each row

	int left = inputImg.cols - 1, right = 0, up = inputImg.rows - 1, down = 0;
	//- Double loop, traversing all pixel values
	for (int i = 0; i < row; i++)  //- row loops
	{
		uchar* data = inputImg.ptr<uchar>(i);  // Get the first address of the ith row in src
		for (int j = 0; j < col; j++)   //- Column loops
		{
			// ---------[Start processing each pixel]-------------  
			if (data[j] > 0)
			{
				if (j < left)
					left = j;
				if (j > right)
					right = j;
				if (i < up)
					up = i;
				if (i > down)
					down = i;
			}
			
			// ----------[End of processing]---------------------
		}   // End of row processing
	}
	return cv::Rect(left, up, right - left + 1, down - up + 1);
}

//- Get the center point of rectangle  
cv::Point getCenterPoint(cv::Rect rect)
{
	cv::Point cpt;
	cpt.x = rect.x + cvRound(rect.width / 2.0) - 50;
	cpt.y = rect.y + cvRound(rect.height / 2.0);
	return cpt;
}