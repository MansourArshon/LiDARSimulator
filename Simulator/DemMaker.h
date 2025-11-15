#pragma once

#include <cstddef>
#include <vector>

// Simple 3D point used by DEM routines.
struct Point {
	float x_, y_, z_;
	Point() = default;
	Point(float x, float y, float z) : x_(x), y_(y), z_(z) {}
};

// Convert a square grid of elevation samples (row-major) into normalized 3D points.
// - 'elevations' must contain N*N samples (N = sqrt(elevations.size())).
// - 'step' controls subsampling: step==1 -> every sample, step>1 -> skip cells.
// Returns a vector of Points in row-major sampling order.
std::vector<Point> getNormalizePoints(const std::vector<float>& elevations, int step);