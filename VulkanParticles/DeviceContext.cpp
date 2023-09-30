#include "DeviceContext.h"
#include<iostream>
DeviceContext::DeviceContext()
{
}

void DeviceContext::initialize()
{

}

void DeviceContext::clear()
{
	vkDestroyDevice(mVkDevice, nullptr);
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


void DeviceContext::createLogicalDevice(std::vector<const char*>& desiredExtensions, VkPhysicalDeviceFeatures& desiredPhysicalDeviceFeatures)
{
	if (!checkDesiredDeviceExtensions(desiredExtensions))
	{
		throw std::runtime_error("could not find all desired device extensions");
	}

	if (!checkDesiredDeviceFeatures())
	{
		throw std::runtime_error("Desired device features not present in physical device");
	}
	VkPhysicalDeviceFeatures desiredFeatures{};
	desiredFeatures.geometryShader = 1;

	findQueueFamilies();
	std::vector<QueueInfo> requestQueues;
	requestQueues.push_back(QueueInfo{ mGraphicsQueueInfo.familyIndex, {1.0f} });
	if (mGraphicsQueueInfo.familyIndex != mComputeQueueInfo.familyIndex)
	{
		requestQueues.push_back(QueueInfo{ mComputeQueueInfo.familyIndex, {1.0f} });
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (int i = 0; i < requestQueues.size(); i++)
	{
		VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
		deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfo.pQueuePriorities = requestQueues[i].priorities.data();
		deviceQueueCreateInfo.queueCount = requestQueues[i].priorities.size();
		deviceQueueCreateInfo.queueFamilyIndex = requestQueues[i].familyIndex;
		deviceQueueCreateInfo.flags = 0;
		deviceQueueCreateInfo.pNext = nullptr;
		queueCreateInfos.push_back(deviceQueueCreateInfo);
	}

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = desiredExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = desiredExtensions.data();
	deviceCreateInfo.pEnabledFeatures = &desiredFeatures;


	if (vkCreateDevice(mVkPhysicalDevice, &deviceCreateInfo, nullptr, &mVkDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create device");
	}

	vkGetDeviceQueue(mVkDevice, mComputeQueueInfo.familyIndex, 0, &mComputeQueue);
	vkGetDeviceQueue(mVkDevice, mGraphicsQueueInfo.familyIndex, 0, &mGraphicsQueue);
	if (mGraphicsQueue == nullptr || mComputeQueue == nullptr)
	{
		throw std::runtime_error("failed to get device queue");
	}

	std::cout << "Created device and queues: \nGraphics queue index: " << mGraphicsQueueInfo.familyIndex << "\n";
	std::cout << "Compute queue index: " << mComputeQueueInfo.familyIndex << "\n";
}
void DeviceContext::findQueueFamilies()
{
	uint32_t queueFamilyPropertyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &queueFamilyPropertyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	queueFamilyProperties.resize(queueFamilyPropertyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());


	for (int i = 0; i < queueFamilyPropertyCount; i++)
	{
		int queueIndex = i;
		if ((queueFamilyProperties[queueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
		{
			mGraphicsQueueInfo.familyIndex = queueIndex;
			break;
		}
	}
	mGraphicsQueueInfo.priorities.push_back(1.0f);
	bool foundOnlyComputeQueue = false;
	for (int i = 0; i < queueFamilyPropertyCount; i++)
	{
		int queueIndex = i;
		if ((queueFamilyProperties[queueIndex].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0 && queueIndex != mGraphicsQueueInfo.familyIndex)
		{
			mComputeQueueInfo.familyIndex = queueIndex;
			foundOnlyComputeQueue = true;
			break;
		}
	}
	if (!foundOnlyComputeQueue)
	{
		if ((queueFamilyProperties[mGraphicsQueueInfo.familyIndex].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
		{
			mComputeQueueInfo.familyIndex = mGraphicsQueueInfo.familyIndex;
		}
		else
		{
			throw std::runtime_error("no queue family with compute capability found");
		}
	}
}
bool DeviceContext::checkDesiredDeviceFeatures()
{
	VkPhysicalDeviceFeatures physicalDeviceFeatures{};
	vkGetPhysicalDeviceFeatures(mVkPhysicalDevice, &physicalDeviceFeatures);
	if (physicalDeviceFeatures.geometryShader == 1)
	{
		return true;
	}
	return false;
}
bool DeviceContext::checkDesiredDeviceExtensions(std::vector<const char*>& desiredExtensions)
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(mVkPhysicalDevice, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtenions;
	availableExtenions.resize(extensionCount);

	vkEnumerateDeviceExtensionProperties(mVkPhysicalDevice, nullptr, &extensionCount, availableExtenions.data());

	uint32_t desiredFoundCount = 0;
	bool foundAll = false;
	for (size_t i = 0; i < availableExtenions.size(); i++)
	{
		const char* availableExtensionName = availableExtenions[i].extensionName;
		for (size_t j = 0; j < desiredExtensions.size(); j++)
		{
			if (strcmp(availableExtensionName, desiredExtensions[j]) == 0)
			{
				desiredFoundCount++;
				break;
			}
		}
		if (desiredFoundCount == desiredExtensions.size())
		{
			foundAll = true;
			break;
		}
	}

	return foundAll;
}
const VkDevice DeviceContext::getDevice() const
{
	return mVkDevice;
}

const VkPhysicalDevice DeviceContext::getPhysicalDevice() const
{
	return mVkPhysicalDevice;
}

const VkQueue DeviceContext::getGraphicsQueue() const
{
	return mGraphicsQueue;
}

const VkQueue DeviceContext::getComputeQueue() const
{
	return mComputeQueue;
}
