#include "DemTerrain.h"
#include <limits>
#include <algorithm>
#include <cmath>

DemTerrain::DemTerrain(const std::string& filepath, int size)
	: width_(size), height_(size), elevationData_(static_cast<size_t>(size) * static_cast<size_t>(size))
{
    std::ifstream file(filepath, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open HGT file: " + filepath);

    for (size_t row = 0; row < static_cast<size_t>(height_); ++row) {
        for (size_t col = 0; col < static_cast<size_t>(width_); ++col) {
            uint8_t bytes[2];
            file.read(reinterpret_cast<char*>(bytes), 2);
            if (file.gcount() != 2) throw std::runtime_error("Unexpected EOF in HGT file: " + filepath);

            // Proper big-endian assembly without sign-extension
            uint16_t u = (static_cast<uint16_t>(bytes[0]) << 8) | static_cast<uint16_t>(bytes[1]);
            int16_t val = static_cast<int16_t>(u);

            // HGT void value is -32768
            if (val == INT16_MIN) val = 0;

            elevationData_[row * static_cast<size_t>(width_) + col] = static_cast<double>(val);
        }
    }
}

double DemTerrain::getElevationAt(int row, int col) const
{
    if (row < 0 || row >= height_ || col < 0 || col >= width_) {
        throw std::out_of_range("Row or column index out of range");
    }

    return static_cast<double>(elevationData_[row * width_ + col]);
}

double DemTerrain::getMinElevation() const
{
    if (elevationData_.empty()) return 0.0;
    return *std::min_element(elevationData_.begin(), elevationData_.end());
}

double DemTerrain::getMaxElevation() const
{
    if (elevationData_.empty()) return 0.0;
    return *std::max_element(elevationData_.begin(), elevationData_.end());
}

std::vector<float> DemTerrain::getAllPoints()
{
    const int size = getWidth(); // assuming square
    const int step = 4;
    const int samplesX = (size + step - 1) / step;
    const int samplesY = (size + step - 1) / step;
    std::vector<float> vertices;
    vertices.reserve(static_cast<size_t>(samplesX) * static_cast<size_t>(samplesY) * 6);

    const float scale = 0.001f;
    const float offset = size / 2.0f;

    double minElev = getMinElevation();
    double maxElev = getMaxElevation();
    const double range = maxElev - minElev;
    const bool zeroRange = std::abs(range) < std::numeric_limits<double>::epsilon();

    for (int y = 0; y < size; y += step) {
        for (int x = 0; x < size; x += step) {
            double elev = getElevationAt(y, x);
            float h = static_cast<float>(elev * scale);

            float px = (x - offset) / offset;
            float py = (y - offset) / offset;

            float t = zeroRange ? 0.5f : static_cast<float>((elev - minElev) / range);
            t = std::min(1.0f, std::max(0.0f, t));

            float r = t < 0.5f ? 0.0f : (t - 0.5f) * 2.0f;
            float g = t < 0.5f ? t * 2.0f : (1.0f - (t - 0.5f)) * 2.0f;
            float b = 1.0f - t;

            vertices.push_back(px);
            vertices.push_back(py);
            vertices.push_back(h);

            vertices.push_back(r);
            vertices.push_back(g);
            vertices.push_back(b);
        }
    }

    return vertices;
}


