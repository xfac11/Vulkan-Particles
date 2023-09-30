#include "Window.h"
#include<iostream>
Window::Window()
{
	mGlfwWindow = nullptr;
	mVkSurface = nullptr;
	mTitle = "";
	mWidth = 0;
	mHeight = 0;
}
Window::Window(uint32_t width, uint32_t height, const char* title) :mWidth(width),mHeight(height)
{
	mGlfwWindow = nullptr;
	mVkSurface = nullptr;
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
	vkDestroySurfaceKHR(instance, mVkSurface, nullptr);
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

void Window::createSurface(VkInstance instance)
{
	glfwCreateWindowSurface(instance, mGlfwWindow, nullptr, &mVkSurface);
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(mGlfwWindow);
}

const int Window::getWidth() const
{
	return 0;
}

const int Window::getHeight() const
{
	return 0;
}

const VkSurfaceKHR Window::getSurface() const
{
	return mVkSurface;
}
