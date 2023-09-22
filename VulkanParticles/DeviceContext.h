#pragma once
#include<vulkan/vulkan.hpp>
class DeviceContext
{
public:
	DeviceContext();
	void initialize();
	void selectPhysicalDevice(const VkInstance instance);
	const VkDevice getDevice()const;
	const VkPhysicalDevice getPhysicalDevice()const;
private:
	VkDevice mVkDevice;
	VkPhysicalDevice mVkPhysicalDevice;

};

