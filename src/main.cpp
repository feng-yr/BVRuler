// main.cpp : Defining entry points for console applications
//

#include <iostream>
#include <string>

#include "tools.h"
#include "imgseg.h"

//- Image size
#define WIDTH		640
#define HEIGHT		480

//- Camera parameters
#define FX			449.18938  //- Focal length
#define BASE_LINE	170.0  //- Baseline distance

const cv::Scalar scalar[] =
{
	cv::Scalar(0, 0, 255), cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0),
	cv::Scalar(255, 0, 255), cv::Scalar(255, 255, 0), cv::Scalar(255, 0, 255),
	cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0)
};

cv::Size imageSize = cv::Size(WIDTH, HEIGHT);

// After the image is corrected, the image will be cropped. 
// The validROI here refers to the area after clipping.
cv::Rect validROIL; 
cv::Rect validROIR;

cv::Mat mapLx, mapLy, mapRx, mapRy;     // Mapping table
cv::Mat Rl, Rr, Pl, Pr, Q;              // Correction of rotation matrix R, projection matrix P, reprojection matrix Q

/*
* The camera parameters are calibrated in advance
*
* fx 0  cx
* 0  fy cy
* 0  0  1
*/
cv::Mat cameraMatrixL = (cv::Mat_<double>(3, 3) << 449.18938, 0, 293.15028,
	0, 446.72591, 229.19348,
	0, 0, 1);

cv::Mat distCoeffL = (cv::Mat_<double>(5, 1) << 0.05028, -0.10557, 0.00059, 0.00041, 0.00000);

cv::Mat cameraMatrixR = (cv::Mat_<double>(3, 3) << 449.69815, 0, 297.65690,
	0, 448.31079, 229.37970,
	0, 0, 1);

cv::Mat distCoeffR = (cv::Mat_<double>(5, 1) << 0.07010, -0.14093, -0.00271, 0.00051, 0.00000);

cv::Mat T = (cv::Mat_<double>(3, 1) << -167.47122, -1.87536, 0.35618);  // T平移向量
cv::Mat rec = (cv::Mat_<double>(3, 1) << -0.01422, -0.00571, 0.01468); // rec旋转向量
cv::Mat R;  // Rotation matrix
cv::Mat xyz;  // Three-dimensional coordinate



/**
* @function main
* @brief Main function
*/
int main()
{
	//-- Stereoscopic correction
	cv::Rodrigues(rec, R); // Rodrigues transformation
	cv::stereoRectify(cameraMatrixL, distCoeffL, cameraMatrixR, distCoeffR, imageSize, R, T, Rl, Rr, Pl, Pr, Q, cv::CALIB_ZERO_DISPARITY,
		0, imageSize, &validROIL, &validROIR);
	cv::initUndistortRectifyMap(cameraMatrixL, distCoeffL, Rl, Pr, imageSize, CV_32FC1, mapLx, mapLy);
	cv::initUndistortRectifyMap(cameraMatrixR, distCoeffR, Rr, Pr, imageSize, CV_32FC1, mapRx, mapRy);

	//-- Calling the constructor of StereoBM
	int ndisparities = 16 * 7;      // Range of disparity
	int SADWindowSize = 21;         // The size of the block window must be odd

	cv::Ptr<cv::StereoBM> sbm = cv::StereoBM::create(ndisparities, SADWindowSize);

	cv::VideoCapture inputVideo;
	inputVideo.open(0);

	//-- Set the size of the image to be collected
	inputVideo.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH * 2);
	inputVideo.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);

	if (!inputVideo.isOpened())
	{
		std::cout << "Could not open the camera !" << std::endl;
		return -1;
	}

	cv::Mat frame;
	double sumTime = 0;
	int id = 0;
	while (1)  // Processing collected images and displaying
	{
		inputVideo >> frame;              // Read the image

		if (frame.empty()) break;

		double t = (double)cv::getTickCount();
		cv::Mat grayImage;
		cv::cvtColor(frame, grayImage, CV_BGR2GRAY);    // Convert to grayscale

		//-- Split the image into left and right views
		cv::Mat leftGrayImage = grayImage(cv::Range(0, frame.rows), cv::Range(0, frame.cols / 2));
		cv::Mat rightGrayImage = grayImage(cv::Range(0, frame.rows), cv::Range(frame.cols / 2, frame.cols));
		cv::Mat showImage = frame(cv::Range(0, frame.rows), cv::Range(0, frame.cols / 2)).clone();

		cv::Mat leftRectImage, rightRectImage;
		
		//-- After remap, the image of the left and right cameras is coplanar and aligned
		cv::remap(leftGrayImage, leftRectImage, mapLx, mapLy, cv::INTER_LINEAR);
		cv::remap(rightGrayImage, rightRectImage, mapRx, mapRy, cv::INTER_LINEAR);

		//-- Create images, store disparity
		cv::Mat imgDisparity16S = cv::Mat(leftRectImage.rows, leftRectImage.cols, CV_16S);
		cv::Mat imgDisparity8U = cv::Mat(leftRectImage.rows, leftRectImage.cols, CV_8UC1);

		if (leftRectImage.empty() || rightRectImage.empty())
		{
			std::cout << "Error reading images !" << std::endl;
			return -1;
		}

		//-- Computing disparity map
		sbm->compute(leftRectImage, rightRectImage, imgDisparity16S);

		//-- Solving extreme value
		double minVal; double maxVal;

		cv::minMaxLoc(imgDisparity16S, &minVal, &maxVal);

		if (maxVal < 0) continue;

		//-- Display the disparity map
		imgDisparity16S.convertTo(imgDisparity8U, CV_8UC1, 255 / (maxVal - minVal));
		cv::Mat imgDisparity8UC3;
		cv::applyColorMap(imgDisparity8U, imgDisparity8UC3, cv::COLORMAP_JET);

		std::vector<cv::Mat> seg;

		segment(imgDisparity16S, seg, 50, 10, 5000);

		for (int i = 0; i < seg.size() && i < 8; i++)
		{
			cv::Mat mask = seg[i];
			cv::Mat region;

			//-- Morphological operations
			cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(21, 21));

			//-- Closed operation
			cv::morphologyEx(mask, region, cv::MORPH_CLOSE, element);

			//-- Open operation 
			cv::morphologyEx(region, region, cv::MORPH_OPEN, element);

			cv::Rect rect = getMinRect(region);
			double meanDisparity = cv::mean(imgDisparity16S, mask)[0];

			cv::rectangle(showImage, rect, scalar[i], 2);
			cv::Point cpt = getCenterPoint(rect);

			std::string text = std::to_string(FX * BASE_LINE / meanDisparity * 16 / 1000);
			text = text.substr(0, text.size() - 4) + "m";
			cv::putText(showImage, text, cpt, cv::FONT_HERSHEY_COMPLEX, 1, scalar[i], 2);

		}

		//-- Time consuming
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		sumTime += t;
		std::cout << "Time consuming: " << t << std::endl;
		id++;

		//-- Display images
		cv::imshow("Camera", frame); 
		cv::imshow("Disparity", imgDisparity8UC3);
		cv::imshow("Result", showImage);
		
		char key = cv::waitKey(10);
		if (key == 27) break;
		if (key == 's' || key == 'S')
		{
			//-- Write images
			cv::imwrite("../leftImage.jpg", leftGrayImage);
			cv::imwrite("../rightImage.jpg", rightGrayImage);
			cv::imwrite("../Disparity.jpg", imgDisparity8U);
		}
	}
	cv::destroyAllWindows();
	double avgTime = sumTime / id;
	std::cout << "The average time consuming: " << avgTime << std::endl;

	system("pause");
	return 0;
}
