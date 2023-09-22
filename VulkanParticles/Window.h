#pragma once
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
class Window
{
public:
	Window();
	Window(uint32_t width, uint32_t height, const char* title);
	void initialize();
	void createWindow();
	bool shouldClose();
	const int getWidth()const;
	const int getHeight()const;
private:
	GLFWwindow *mGlfwWindow;
	VkSurfaceKHR mVkSurface;
	uint32_t mWidth;
	uint32_t mHeight;
	const char* mTitle;
};

