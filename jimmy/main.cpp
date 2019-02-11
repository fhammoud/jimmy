#include "pi.h"
#include "vision.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#define FRAME_WIDTH 1024
#define FRAME_HEIGHT 576
// #define FPS 20

// consts
cv::Mat input, output;
const std::string window_name = "Feed";
// const string filename = "/home/pi/projects/WiringPi/test_videos/solidYellowLeft.mp4";
const std::string filename = "/home/pi/projects/WiringPi/test_images/solidYellowCurve.jpg";

// Display distance reading on screen
void display_distance(const double& distance)
{
	std::stringstream stringstream;
	stringstream << std::setprecision(2) << std::fixed << "Distance: " << distance << " cm";
	const std::string dist_string = stringstream.str();
	cv::putText(output, dist_string, cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 0, 0));
}

// Display framerate reading on screen
void display_framerate(const int& fps)
{
	const std::string fps_string = "FPS: " + std::to_string(fps);
	cv::putText(output, fps_string, cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 0, 0));
}

int main()
{
	// Initialize hardware
	pi_init();
	// return 0;

	//create a gui window:
	cv::namedWindow(window_name, cv::WINDOW_NORMAL);
	cv::resizeWindow(window_name, FRAME_WIDTH, FRAME_HEIGHT);

	// Read image
	/*input = cv::imread(filename, cv::IMREAD_COLOR);
	if (input.empty())
	{
		std::cout << "Could not open or find the image!\n";
		return -1;
	}
	output = input.clone();

	// Select region of interest
	set_lane_roi(input.size());

	// createTrackbar("Min Threshold:", window_name, &lowThreshold, max_threshold, find_lanes);
	find_lanes(0, nullptr);

	// Show image
	imshow(window_name, output);*/

	// Read video
	cv::VideoCapture feed(0);
	feed.set(cv::CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	feed.set(cv::CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	// feed.set(cv::CAP_PROP_FPS, FPS);

	// cv::VideoCapture feed(filename);
	if (!feed.isOpened())
	{
		std::cout << "Could not open video!\n";
		return -1;
	}

	// Select region of interest
	const size_t rows = feed.get(cv::CAP_PROP_FRAME_HEIGHT);
	const size_t cols = feed.get(cv::CAP_PROP_FRAME_WIDTH);
	set_lane_roi(rows, cols);

	// Process frames
	const int num_frames = 60;
	int frame_count = 0, fps = 0;
	auto start = std::chrono::steady_clock::now();

	while (true)
	{
		// Get frame
		feed >> input;
		if (input.empty())
			break;
		frame_count++;

		// Display framerate
		if (frame_count % num_frames == 0)
		{
			auto stop = std::chrono::steady_clock::now();
			const auto elapsed = std::chrono::duration<double>(stop - start).count();
			fps = static_cast<int>(num_frames / elapsed);
			start = stop;
		}

		// Clone to output
		output = input.clone();

		// Find lanes
		find_lanes();

		// Get distance to nearest front object
		double distance = get_distance();

		// Display distance reading
		display_distance(distance);

		// Dsiplay framerate
		display_framerate(fps);
		
		// Show image
		cv::imshow(window_name, output);

		// Break if key is pressed
		if (cv::waitKey(1) >= 0)
			break;
	}

	std::cout << "Video done!\n";
	cv::waitKey(0);

	return 0;
}