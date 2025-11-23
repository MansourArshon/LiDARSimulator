#pragma once
#include <vector>
#include <random>

struct Point {
	float x_, y_, z_; 
	Point() = default;
	Point(float x, float y, float z)
		: x_(x), y_(y), z_(z) {
	}
};

// Simple FlightPath generator that returns a flat vector of floats:
// { startX, startY, altitude, endX, endY, altitude }
class FlightPath
{
public:
	FlightPath(int minX, int minY, int maxX, int maxY, float altitude, unsigned int seed);
	std::vector<Point> GenerateFlightPath() const;

private:
	// GenerateRandomNumber now accepts an explicit min/max range.
	float GenerateRandomNumber(int min, int max) const;

	int minX_, minY_, maxX_, maxY_;
	float altitude_;
	mutable std::mt19937 rng_;
};

