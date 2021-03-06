// MOD.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>
#include <opencv2/opencv.hpp>    
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

#define BLUR_SIZE			5
#define SENSITIVITY_VALUE	62
#define THRESHOLD           0.021f

#define WHITE_COLOR			255

#define WIDTH		1280
#define HEIGHT		720
#define ROI_X		WIDTH * 0.2
#define ROI_Y		HEIGHT * 0.6

void Draw_ROI(Mat srcImg)
{
	line(srcImg, Point(ROI_X, ROI_Y), Point(WIDTH, ROI_Y), Scalar(255, 0, 0), 2);
	line(srcImg, Point(ROI_X, ROI_Y), Point(ROI_X, HEIGHT), Scalar(255, 0, 0), 2);
	line(srcImg, Point(WIDTH, HEIGHT), Point(ROI_X, HEIGHT), Scalar(255, 0, 0), 2);
	line(srcImg, Point(WIDTH, HEIGHT), Point(WIDTH, ROI_Y), Scalar(255, 0, 0), 2);
}

int Get_num_non_zero_in_ROI(Mat srcImg,Rect roi)
{
	int ret_count  = 0;
	int idx1, idx2;

	for (idx1 = roi.y; idx1 < roi.height; idx1++)
	{
		for (idx2 = roi.x; idx2 < roi.width; idx2++)
		{
			int imgIdx = idx1 * roi.width + idx2;

			if (srcImg.data[imgIdx] > 0)
			{
				ret_count++;
			}
		}
	}

	return ret_count;
}

float Get_percentage_non_zero(Mat srcImg, Rect roi)
{
	float num_non_zero_in_ROI = (float)Get_num_non_zero_in_ROI(srcImg, roi);
	int total_area            = (roi.width - roi.x) * (roi.height - roi.y);

	return num_non_zero_in_ROI / (float)total_area;
}

int main(int argc, char* argv[])
{
	VideoCapture capture("project_video.mp4");

	if (!capture.isOpened())
	{
		std::cout << "video not open." << std::endl;
		return 1;
	}

	cv::Mat originFrame, currentFrame, prevFrame, diffFrame;

	bool stop(false);
	while (!stop)
	{
		if (!capture.read(originFrame))
		{
			std::cout << "no video frame" << std::endl;
			break;
		}

		//imshow("video", currentFrame);

		cvtColor(originFrame, currentFrame, COLOR_BGR2GRAY);

		if (prevFrame.empty())
		{
			currentFrame.copyTo(prevFrame);
			continue;
		}

		absdiff(currentFrame, prevFrame, diffFrame);

		//blur the image to get rid of the noise.
		blur(diffFrame, diffFrame, cv::Size(BLUR_SIZE, BLUR_SIZE));

		//set threshold again to obtain binary image from blur output
		threshold(diffFrame, diffFrame, SENSITIVITY_VALUE, WHITE_COLOR, THRESH_BINARY);

		Draw_ROI(diffFrame);
		Draw_ROI(originFrame);

		float non_zero_percent = Get_percentage_non_zero(diffFrame, Rect(ROI_X, ROI_Y, WIDTH, HEIGHT));

		currentFrame.copyTo(prevFrame);

		if (non_zero_percent > THRESHOLD)
		{
			//printf("moving detect\n");
			circle(originFrame, Point(WIDTH - 80, 80), 14, Scalar(0, 0, 255), 14);
		}

		imshow("video", originFrame);

		//printf("width:%d, height:%d\n", currentFrame.cols, currentFrame.rows);
		//printf("non_zero_percent:%f\n", non_zero_percent);

		waitKey(10);
	}

	capture.release();
	waitKey(0);
	return 0;
}

