#pragma once
#include <string>
#include <vector>
#include <fstream>

class DemTerrain
{
public:
	DemTerrain(const std::string& filepath, int size);
	double getElevationAt(int row, int col) const;
	double getMinElevation() const;
	double getMaxElevation() const;
	int getWidth() const { return width_; };
	int getHeight() const { return height_; };
	std::vector<float> getAllPoints();

private:
	int width_;
	int height_;
	std::vector<double> elevationData_;
};
