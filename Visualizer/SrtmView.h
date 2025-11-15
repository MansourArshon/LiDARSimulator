#pragma once
#include <GLFW/glfw3.h>
#include <iostream>

class SrtmView
{
	public:
		SrtmView(const std::string filepath, const int size);
		int showSrtmData();

private:
	const std::string filepath;
	const int size;
};

