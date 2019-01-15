#include "vision.h"
#include <iostream>
#include <opencv2/opencv.hpp>

#define LANE_ROI_HEIGHT 0.65

using namespace std;
using namespace cv;

// Global variables
Mat input, output;
Mat gray_scale, edges, mask;

// Canny consts
const int lowThreshold = 50;
const int max_lowThreshold = 100;
const int canny_ratio = 3;
const int kernel_size = 3;

// Hough consts
const int rho = 1;
const double theta = CV_PI / 180;
const int hough_threshold = 50;
const int min_line_length = 30;
const int max_line_gap = 50;

// Find road lanes in an image
void find_lanes(int, void*)
{
	// Convert to gray scale
	cvtColor(input, gray_scale, COLOR_BGR2GRAY);

	// Blur
	blur(gray_scale, edges, Size(3, 3));

	// Apply Canny algorithm to detect edges
	Canny(edges, edges, lowThreshold, lowThreshold * canny_ratio, kernel_size);

	// Select region of interest
	bitwise_and(edges, mask, edges);

	// Find lines using Hough transform
	vector<Vec4i> lines;
	HoughLinesP(edges, lines, rho, theta, hough_threshold, min_line_length, max_line_gap);

	int left_count = 0;
	double left_x1_sum = 0;
	double left_y1_sum = 0;
	double left_x2_sum = 0;
	double left_y2_sum = 0;

	int right_count = 0;
	double right_x1_sum = 0;
	double right_y1_sum = 0;
	double right_x2_sum = 0;
	double right_y2_sum = 0;

	// Draw the lane lines
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		const double x1 = l[0];
		const double y1 = l[1];
		const double x2 = l[2];
		const double y2 = l[3];
		const double slope = (y2 - y1) / (x2 - x1);

		// Separate left and right lines and ignore any horizontal lines
		if (slope > 0.4 && slope < 0.8)
		{
			right_count += 1;
			right_x1_sum += x1;
			right_x2_sum += x2;
			right_y1_sum += y1;
			right_y2_sum += y2;
		}
		else if (slope > -0.9 && slope < -0.5)
		{
			left_count += 1;
			left_x1_sum += x1;
			left_x2_sum += x2;
			left_y1_sum += y1;
			left_y2_sum += y2;
		}
	}

	// Find average position for left and right lines
	const double left_x1 = left_x1_sum / left_count;
	const double left_y1 = left_y1_sum / left_count;
	const double left_x2 = left_x2_sum / left_count;
	const double left_y2 = left_y2_sum / left_count;

	const double right_x1 = right_x1_sum / right_count;
	const double right_y1 = right_y1_sum / right_count;
	const double right_x2 = right_x2_sum / right_count;
	const double right_y2 = right_y2_sum / right_count;

	// Find slopes of left and right lines
	const double left_slope = (left_y2 - left_y1) / (left_x2 - left_x1);
	const double right_slope = (right_y2 - right_y1) / (right_x2 - right_x1);

	// Find y-intercept of left and right lines
	const double left_b = left_y1 - left_slope * left_x1;
	const double right_b = right_y1 - right_slope * right_x1;

	// Find new points of extended left and right lines
	const int bottom = input.rows;
	const int top = input.rows * LANE_ROI_HEIGHT;

	const int left_line_x1 = (bottom - left_b) / left_slope;
	const int left_line_y1 = bottom;
	const int left_line_x2 = (top - left_b) / left_slope;
	const int left_line_y2 = top;
	const int right_line_x1 = (bottom - right_b) / right_slope;
	const int right_line_y1 = bottom;
	const int right_line_x2 = (top - right_b) / right_slope;
	const int right_line_y2 = top;

	// Draw left and right lines
	line(output, Point(left_line_x1, left_line_y1), Point(left_line_x2, left_line_y2), Scalar(0, 0, 255), 3, LINE_AA);
	line(output, Point(right_line_x1, right_line_y1), Point(right_line_x2, right_line_y2), Scalar(0, 0, 255), 3, LINE_AA);

	// Draw direction line
	const int center_line_x1 = input.cols / 2;
	const int center_line_y1 = bottom;
	const int center_line_x2 = (left_line_x2 + right_line_x2) / 2;
	const int center_line_y2 = top;
	line(output, Point(center_line_x1, center_line_y1), Point(center_line_x2, center_line_y2), Scalar(0, 0, 0), 3);
}

// Select road lanes region of interest
void set_lane_roi(const cv::Size &size)
{
	const size_t rows = size.height;
	const size_t cols = size.width;
	mask = Mat::zeros(rows, cols, CV_8UC1);

	Point points[1][4];
	points[0][0] = Point(0, rows);
	points[0][1] = Point(cols * 0.45, rows * LANE_ROI_HEIGHT);
	points[0][2] = Point(cols * 0.55, rows * LANE_ROI_HEIGHT);
	points[0][3] = Point(cols, rows);
	const Point* point_list[1] = { points[0] };

	int num_points = 4;
	const int num_polygons = 1;
	fillPoly(mask, point_list, &num_points, num_polygons, Scalar(255));
}