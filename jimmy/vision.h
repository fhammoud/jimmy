#pragma once

#include <cstddef>

// Find road lanes in an image
void find_lanes();

// Select road lanes region of interest
void set_lane_roi(const size_t &rows, const size_t &cols);