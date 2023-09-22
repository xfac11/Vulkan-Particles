#include<iostream>
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/common.hpp>

#include<vector>

#include"Application.h"

int main()
{
	Application app("Particles", 1920, 1080);
	app.initialize();
	app.run();
	app.clear();
	return 0;
}