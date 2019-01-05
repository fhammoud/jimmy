#pragma once

#include <opencv2/core/mat.hpp>

extern cv::Mat input, output;

// Find road lanes in an image
void find_lanes(int, void*);

// Select road lanes region of interest
void set_lane_roi(const cv::Size &size);