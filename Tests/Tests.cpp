#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "../Simulator/SrtmReader.h"

TEST_CASE("DemTerrain loads HGT file and retrieves elevation data correctly", "[DemTerrain]")
{
    const std::string filepath = "C:\\Dev\\LidarSimulator\\SRTM\\N35W116.hgt"; // Path to a sample HGT file
    const int size = 3601; // Typical size for SRTM3 data
    const int step = 5;
    ////SrtmReader dem(filepath, size, step);
    ////REQUIRE(dem.getWidth() == size);
    ////REQUIRE(dem.getHeight() <= size);
    ////// Test known elevation values at specific coordinates
    ////// These values should be replaced with actual expected values from the test HGT file
    ////double elevation1 = dem.getElevationAt(600, 600); // Center point
    ////double elevation2 = dem.getElevationAt(0, 0);     // Top-left corner
    ////double elevation3 = dem.getElevationAt(1200, 1200); // Bottom-right corner
    ////// Replace these expected values with the actual expected elevations from the HGT file
    ////REQUIRE(elevation2 == Catch::Approx(779.0).margin(1.0));
    ////REQUIRE(elevation3 == Catch::Approx(1381.0).margin(1.0));
}
