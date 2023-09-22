#include "DeviceContext.h"
#include<iostream>
DeviceContext::DeviceContext()
{
}

void DeviceContext::initialize()
{

}

void DeviceContext::selectPhysicalDevice(const VkInstance instance)
{
	uint32_t devicesCount = 0;

	vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr);

	std::vector<VkPhysicalDevice> physicalDevices;
	std::vector<uint32_t> points;
	points.resize(devicesCount, 0);
	physicalDevices.resize(devicesCount);

	vkEnumeratePhysicalDevices(instance, &devicesCount, physicalDevices.data());

	for (uint32_t i = 0; i < devicesCount; i++)
	{
		VkPhysicalDevice physicalDevice = physicalDevices[i];
		VkPhysicalDeviceFeatures deviceFeatures{};
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

		VkPhysicalDeviceProperties physicalDeviceProperties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			points[i] += 5;
		}
	}

	std::cout << " Select which device to use: " << std::endl;
	for (uint32_t i = 0; i < devicesCount; i++)
	{

		VkPhysicalDevice physicalDevice = physicalDevices[i];
		VkPhysicalDeviceFeatures deviceFeatures{};
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

		VkPhysicalDeviceProperties physicalDeviceProperties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		std::cout << i << " " << physicalDeviceProperties.deviceName << " score: " << points[i];
	}
	std::cout << "\nType 0 - " << devicesCount - 1 << " and press Enter to confirm: ";

	uint32_t chosenDevice = 0;
	std::cin >> chosenDevice;
	while (chosenDevice > physicalDevices.size())
	{
		std::cout << "\n Chosen device doesnt exist, pick again: ";
		std::cin >> chosenDevice;
	}
	mVkPhysicalDevice = physicalDevices[chosenDevice];

}

const VkDevice DeviceContext::getDevice() const
{
	return mVkDevice;
}

const VkPhysicalDevice DeviceContext::getPhysicalDevice() const
{
	return mVkPhysicalDevice;
}
