#include "pi.h"
#include "vision.h"
#include <iostream>
#include <opencv2/highgui.hpp>

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define FPS 30

using namespace std;
using namespace cv;

// consts
// Mat input, output; externed from vision.h
const string window_name = "Feed";
const string filename = "/home/pi/projects/WiringPi/test_videos/solidYellowLeft.mp4";
// const string filename = "/home/pi/projects/WiringPi/test_images/solidYellowLeft.jpg";

int main(void)
{
	// Initialize hardware
	pi_init();

	//create a gui window:
	namedWindow(window_name, WINDOW_NORMAL);
	resizeWindow(window_name, FRAME_WIDTH, FRAME_HEIGHT);

	// Read image
	/*input = imread(filename, IMREAD_COLOR);
	if (input.empty())
	{
		cout << "Could not open or find the image!\n" << endl;
		return -1;
	}
	output = input.clone();

	// Select region of interest
	set_lane_roi(input.size());

	// createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, find_lanes);
	find_lanes(0, nullptr);

	// Show image
	imshow(window_name, output);*/

	// Read video
	/*VideoCapture feed(0);
	feed.set(CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	feed.set(CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	feed.set(CAP_PROP_FPS, FPS);*/
	
	VideoCapture feed(filename);
	if (!feed.isOpened())
	{
		cout << "Could not open video!\n" << endl;
		return -1;
	}

	// Select region of interest
	const size_t rows = feed.get(CAP_PROP_FRAME_HEIGHT);
	const size_t cols = feed.get(CAP_PROP_FRAME_WIDTH);
	const Size size(cols, rows);
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
		if (waitKey(10) >= 0)
			break;
	}

	cout << "Video done!\n";
	waitKey(0);

	return 0;
}