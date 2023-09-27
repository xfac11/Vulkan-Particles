#pragma once
#include<vulkan/vulkan.hpp>
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
	void clear();
	void selectPhysicalDevice(const VkInstance instance);
	void createLogicalDevice(std::vector<const char*>& desiredExtensions, VkPhysicalDeviceFeatures& desiredPhysicalDeviceFeatures);
	const VkDevice getDevice()const;
	const VkPhysicalDevice getPhysicalDevice()const;
private:
	bool checkDesiredDeviceFeatures();
	bool checkDesiredDeviceExtensions(std::vector<const char*>& desiredExtensions);
	VkDevice mVkDevice;
	VkPhysicalDevice mVkPhysicalDevice;

	VkQueue mGraphicsQueue;
	QueueInfo mGraphicsQueueInfo;
	VkQueue mComputeQueue;
	QueueInfo mComputeQueueInfo;

};

