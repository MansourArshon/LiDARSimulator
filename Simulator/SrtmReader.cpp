#include "SrtmReader.h"
#include <limits>
#include <algorithm>
#include <cmath>
#include <fstream>


std::vector<float> SrtmReader::getElevationData() const
{
    std::vector<float> result;
    result.reserve(size_ * size_);

	std::ifstream file(filepath_, std::ios::binary);
    if (!file) {
		throw std::runtime_error("Cannot open HGT file: " + filepath_);
    }

    for (size_t row = 0; row < size_; row++) {
        for (size_t col = 0; col < size_; col++) {
			uint8_t bytres[2];

			file.read(reinterpret_cast<char*>(bytres), 2);
            if (file.gcount() != 2) {
                throw std::runtime_error("Unexpected EOF in HGT file: " + filepath_);
            }

			uint16_t u = (static_cast<uint16_t>(bytres[0]) << 8) | static_cast<uint16_t>(bytres[1]);
			int16_t val = static_cast<int16_t>(u);

            result.push_back(static_cast<float>(val));
        }
    }


    return result;
}






