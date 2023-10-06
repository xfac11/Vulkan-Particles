#pragma once
#include<vulkan/vulkan.hpp>
#include<GLFW/glfw3.h>
struct QueueInfo
{
	uint32_t familyIndex;
	std::vector<float> priorities;
};
class DeviceContext
{
public:
	DeviceContext();
	void initialize();
	void clear(const VkInstance instance);
	void selectPhysicalDevice(const VkInstance instance);
	void createLogicalDevice(std::vector<const char*>& desiredExtensions, VkPhysicalDeviceFeatures& desiredPhysicalDeviceFeatures);
	void createSwapchain(VkExtent2D imageSize);
	void createSurface(VkInstance instance, GLFWwindow* window);
	void findQueueFamilies();
	const VkDevice getDevice()const;
	const VkPhysicalDevice getPhysicalDevice()const;
	const VkQueue getGraphicsQueue()const;
	const VkQueue getComputeQueue()const;
private:
	bool checkDesiredDeviceFeatures();
	bool checkDesiredDeviceExtensions(std::vector<const char*>& desiredExtensions);
	void getQueueFamilyProperties(std::vector<VkQueueFamilyProperties>& queueFamilyProperties);
	VkDevice mVkDevice;
	VkPhysicalDevice mVkPhysicalDevice;

	VkQueue mGraphicsQueue;
	QueueInfo mGraphicsQueueInfo;
	VkQueue mComputeQueue;
	QueueInfo mComputeQueueInfo;

	VkSwapchainKHR mVkSwapchain;
	std::vector<VkImage> swapchainImages;
	VkSurfaceKHR mVkSurface;

};

