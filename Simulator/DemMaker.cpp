#include "DemMaker.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <stdexcept>

std::vector<Point> getNormalizePoints(const std::vector<float>& elevations, int step)
{
    if (step <= 0) throw std::invalid_argument("step must be >= 1");
    if (elevations.empty()) return {};

    // Ensure input represents a square grid
    const size_t n = elevations.size();
    const double root = std::sqrt(static_cast<double>(n));
    const size_t size = static_cast<size_t>(std::lround(root));
    if (size * size != n) {
        throw std::invalid_argument("elevations.size() must be a perfect square");
    }

    // Find min/max elevation
    auto minIt = std::min_element(elevations.begin(), elevations.end());
    auto maxIt = std::max_element(elevations.begin(), elevations.end());
    const float minElevation = *minIt;
    const float maxElevation = *maxIt;
    const float elevationRange = maxElevation - minElevation;

    // Offset maps grid coords to [-1,1].
    const float offset = (size > 1) ? static_cast<float>(size - 1) / 2.0f : 1.0f;

    // Estimate number of samples to reserve (avoid many reallocations)
    const size_t stepU = static_cast<size_t>(std::max(1, step));
    const size_t samplesX = (size + stepU - 1) / stepU;
    const size_t samplesY = (size + stepU - 1) / stepU;
    std::vector<Point> result;
    result.reserve(samplesX * samplesY);

    for (size_t y = 0; y < size; y += stepU) {
        for (size_t x = 0; x < size; x += stepU) {
            const size_t idx = y * size + x;
            const float elev = elevations[idx];

            // Normalize X/Y to [-1,1]
            const float px = (offset == 0.0f) ? 0.0f : (static_cast<float>(x) - offset) / offset;
            const float py = (offset == 0.0f) ? 0.0f : (static_cast<float>(y) - offset) / offset;

            // Normalize elevation to -1..1. Handle constant elevation (avoid div-by-zero).
            float pz;
            if (elevationRange == 0.0f) {
                pz = 0.0f; // all elevations are equal -> center plane
            } else {
                const float h = (elev - minElevation) / elevationRange; // 0..1
                pz = h * 2.0f - 1.0f; // -1..1
            }

            result.emplace_back(px, py, pz);
        }
    }

    return result;
}