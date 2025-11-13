#include "DemTerrain.h"


DemTerrain::DemTerrain(const std::string& filepath, int size)
	: width_(size), height_(size), elevationData_(size* size)
{
    std::ifstream file(filepath, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open HGT file");

    for (int row = 0; row < height_; ++row) {
        for (int col = 0; col < width_; ++col) {
            uint8_t bytes[2];
            file.read(reinterpret_cast<char*>(bytes), 2);
            if (!file) throw std::runtime_error("Unexpected EOF in HGT file");

            // Convert big-endian bytes to unsigned signed 16-bit
            uint16_t raw = (static_cast<int16_t>(bytes[0]) << 8) | bytes[1];

			int16_t val = static_cast<int16_t>(raw);

            if (raw == INT16_MIN) val= 0;
            
			elevationData_[row * width_ + col] = static_cast<double>(val);
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

