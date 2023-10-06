#pragma once
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
class Window
{
public:
	Window();
	Window(uint32_t width, uint32_t height, const char* title);
	void initialize();
	void clear(VkInstance instance);
	void createWindow();
	bool shouldClose();
	const uint32_t getWidth()const;
	const uint32_t getHeight()const;
	GLFWwindow* getGLFWWindow()const;
private:
	GLFWwindow *mGlfwWindow;
	uint32_t mWidth;
	uint32_t mHeight;
	const char* mTitle;
};

