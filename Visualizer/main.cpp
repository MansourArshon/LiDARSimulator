#include "main.h"
#include "../Simulator/SrtmReader.h"
#include "../Simulator/DemMaker.h"
#include "../Simulator/FlightPath.h"
#include "SrtmView.h"
#include <limits>
#include <vector>
#include <string>


int main()
{
    const std::string filepath = "C:\\Dev\\LidarSimulator\\SRTM\\N33W118.hgt";
    const int size = 3601;
    const int step = 5;

    SrtmReader dem(filepath, size);
    std::vector<float> vertices = dem.getElevationData();

    // Obtain normalized 3D points from elevation data
    std::vector<Point> points = getNormalizePoints(vertices, step);

    FlightPath fp(-1, -1, 1, 1, 1, 0);
    std::vector<Point> flightLinePointys = fp.GenerateFlightPath();

    // If showSrtmData expects elevations only, pass the appropriate vector.
    // Here we demonstrate passing the converted float array of xyzs.
    SrtmView view(filepath, size);
	view.setSrtmData(points);
    view.setFlightPath(flightLinePointys);
    view.showSrtmData();

    return 0;
}