#include "FlightPath.h"
#include <algorithm>

FlightPath::FlightPath(int minX, int minY, int maxX, int maxY, float altitude, unsigned int seed)
	: minX_(minX), minY_(minY), maxX_(maxX), maxY_(maxY), altitude_(altitude) {
	if (seed == 0) {
		rng_.seed(seed);
	}
	else {
        std::random_device rnd;
		rng_.seed(rnd());
	}
}

std::vector<Point> FlightPath::GenerateFlightPath() const
{
	// Defensive: ensure min <= max
	int minX = minX_, maxX = maxX_;
	int minY = minY_, maxY = maxY_;
	if (minX > maxX) std::swap(minX, maxX);
	if (minY > maxY) std::swap(minY, maxY);

	// Start and end coordinates sampled inside the provided ranges
	float startX = GenerateRandomNumber(minX, maxX);
	float startY = GenerateRandomNumber(minY, maxY);

	float endX = GenerateRandomNumber(minX, maxX);
	float endY = GenerateRandomNumber(minY, maxY);

	std::vector<Point> result;
	result.reserve(2);

	result.emplace_back(startX,startY, altitude_);
	result.emplace_back(endX, endY, altitude_);

	return result;
}

float FlightPath::GenerateRandomNumber(int min, int max) const
{
	// uniform real in [min, max]
	std::uniform_real_distribution<float> dist(static_cast<float>(min), static_cast<float>(max));
	return dist(rng_);
}
