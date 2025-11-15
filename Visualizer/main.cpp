#include "main.h"
#include "../Simulator/SrtmReader.h"
#include "SrtmView.h"
#include <limits>

int main()
{
	const std::string filepath = "C:\\Dev\\LidarSimulator\\SRTM\\N33W118.hgt";
	const int size = 3601;
	const int step = 5;
	SrtmReader dem(filepath, size);
	std::vector<float> vertices = dem.getElevationData();

	SrtmView view(filepath, size);
	return view.showSrtmData(vertices);
}