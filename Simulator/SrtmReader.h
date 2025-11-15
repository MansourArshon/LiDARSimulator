#pragma once
#include <string>
#include <vector>

class SrtmReader
{
public:
    SrtmReader(const std::string& filepath, size_t size)
        : filepath_(filepath), size_(size) {
    }

    // Read and return 'size_*size_' elevation samples in row-major order.
    // Each sample is one signed 16-bit big-endian value stored as float.
    std::vector<float> getElevationData() const;

private:
    size_t size_;
    const std::string filepath_;
};
