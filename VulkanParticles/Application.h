#pragma once
#include<string>
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include<vector>
#include<iostream>
#include"DeviceContext.h"
#include"Window.h"
class Application
{
public:
	Application(std::string name, int width, int height);
	int initialize();
	int run();
	int clear();
private:
	std::string mName;
	VkInstance mVkInstance;
	DeviceContext mDeviceContext;
	Window mWindow;
	std::vector<VkExtensionProperties> mAvailableExtensions;
	std::vector<const char*> mDesiredExtensions;
	void update();
	void render();
};