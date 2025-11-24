#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
#include "../Simulator/DemMaker.h"

class SrtmView
{
	public:
		SrtmView(const std::string filepath, const int size);
		int showSrtmData();
		void setFlightPath(const std::vector<Point>& flightPath);
		void setSrtmData(const std::vector<Point>& srtmData);


private:
	const std::string filepath;
	const int size;
	std::vector<float> flightPath_;
	std::vector<float> srtmData_;
	GLFWwindow* initGlfWindow();
	int initGlad(GLFWwindow* window, bool& retFlag);
};

