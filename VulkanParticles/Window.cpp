#include "Window.h"
#include<iostream>
Window::Window()
{
	mGlfwWindow = nullptr;
	mTitle = "";
	mWidth = 0;
	mHeight = 0;
}
Window::Window(uint32_t width, uint32_t height, const char* title) :mWidth(width),mHeight(height)
{
	mGlfwWindow = nullptr;
	mTitle = title;
}

void Window::initialize()
{
	if (!glfwInit())
	{
		throw std::runtime_error("failed to init glfw!");
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void Window::clear(VkInstance instance)
{
	
}

void Window::createWindow()
{
	mGlfwWindow = glfwCreateWindow(mWidth, mHeight, mTitle, NULL, NULL);
	if (!mGlfwWindow)
	{
		throw std::runtime_error("failed to create window!");
	}
	glfwMakeContextCurrent(mGlfwWindow);
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(mGlfwWindow);
}

const uint32_t Window::getWidth() const
{
	return 0;
}

const uint32_t Window::getHeight() const
{
	return 0;
}

GLFWwindow* Window::getGLFWWindow() const
{
	return mGlfwWindow;
}
