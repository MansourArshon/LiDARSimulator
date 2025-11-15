#pragma once
#include <GLFW/glfw3.h>
#include <iostream>

class SrtmView
{
	public:
		SrtmView(const std::string filepath, const int size);
		int showSrtmData(const std::vector<float>& vertices);

		int initGlad(GLFWwindow* window, bool& retFlag);

		GLFWwindow* initGlfWindow();

private:
	const std::string filepath;
	const int size;
};

