#include "pi.h"
#include "vision.h"
#include <iostream>
#include <opencv2/highgui.hpp>

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define FPS 30

// consts
// Mat input, output; externed from vision.h
const std::string window_name = "Feed";
// const string filename = "/home/pi/projects/WiringPi/test_videos/solidYellowLeft.mp4";
const std::string filename = "/home/pi/projects/WiringPi/test_images/solidYellowCurve.jpg";

int main()
{
	// Initialize hardware
	// pi_init();
	// return 0;

	//create a gui window:
	cv::namedWindow(window_name, cv::WINDOW_NORMAL);
	cv::resizeWindow(window_name, FRAME_WIDTH, FRAME_HEIGHT);

	// Read image
	input = cv::imread(filename, cv::IMREAD_COLOR);
	if (input.empty())
	{
		std::cout << "Could not open or find the image!\n";
		return -1;
	}
	output = input.clone();

	// Select region of interest
	set_lane_roi(input.size());

	// createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, find_lanes);
	find_lanes(0, nullptr);

	// Show image
	imshow(window_name, output);

	// Read video
	cv::VideoCapture feed(0);
	feed.set(cv::CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	feed.set(cv::CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	feed.set(cv::CAP_PROP_FPS, FPS);

	cv::VideoCapture feed(filename);
	if (!feed.isOpened())
	{
		std::cout << "Could not open video!\n";
		return -1;
	}

	// Select region of interest
	const size_t rows = feed.get(cv::CAP_PROP_FRAME_HEIGHT);
	const size_t cols = feed.get(cv::CAP_PROP_FRAME_WIDTH);
	const cv::Size size(cols, rows);
	set_lane_roi(size);

	// Process frames
	while (true)
	{
		// Get frame
		feed >> input;
		if (input.empty())
			break;

		// Clone to output
		output = input.clone();

		// Find lanes
		find_lanes(0, nullptr);

		// Show image
		imshow(window_name, output);

		// Break if key is pressed
		if (cv::waitKey(10) >= 0)
			break;
	}

	std::cout << "Video done!\n";
	cv::waitKey(0);

	return 0;
}