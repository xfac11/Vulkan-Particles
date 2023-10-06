#include "DeviceContext.h"
#include<iostream>
DeviceContext::DeviceContext()
{
}

void DeviceContext::initialize()
{

}

void DeviceContext::clear(const VkInstance instance)
{
	vkDestroySwapchainKHR(mVkDevice, mVkSwapchain, nullptr);
	vkDestroySurfaceKHR(instance, mVkSurface, nullptr);

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
void DeviceContext::createSwapchain(VkExtent2D imageSize)
{
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	getQueueFamilyProperties(queueFamilyProperties);
	uint32_t supportedQueueIndex;
	for (int i = 0; i < queueFamilyProperties.size(); i++)
	{
		VkBool32 surfaceSupport = false;
		if (vkGetPhysicalDeviceSurfaceSupportKHR(mVkPhysicalDevice, i, mVkSurface, &surfaceSupport) == VK_SUCCESS)
		{
			if (surfaceSupport == VK_TRUE)
			{
				supportedQueueIndex = i;
				break;
			}
		}
		
	}
	//Selecting present mode
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	VkPresentModeKHR desiredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	uint32_t presentModesCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(mVkPhysicalDevice, mVkSurface, &presentModesCount, nullptr);
	std::vector<VkPresentModeKHR> presentModes;
	presentModes.resize(presentModesCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(mVkPhysicalDevice, mVkSurface, &presentModesCount, presentModes.data());

	for (int i = 0; i < presentModes.size(); i++)
	{
		if (presentModes[i] == desiredPresentMode)
		{
			presentMode = desiredPresentMode;
			break;
		}
	}
	if (desiredPresentMode != presentMode)
	{
		std::cout << "Couldn't find support for desired present mode so defaulting to FIFO_KHR\n";
	}
	//Number of images
	VkSurfaceCapabilitiesKHR surfaceCapabilities{};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mVkPhysicalDevice, mVkSurface, &surfaceCapabilities);
	uint32_t numberOfImages = surfaceCapabilities.minImageCount + 1;
	if (surfaceCapabilities.maxImageCount > 0 && numberOfImages > surfaceCapabilities.maxImageCount)
	{
		numberOfImages = surfaceCapabilities.maxImageCount;
	}
	//Size of swapchain images
	VkExtent2D sizeOfImage{};
	if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
	{
		sizeOfImage = imageSize;

		sizeOfImage.width = std::clamp(sizeOfImage.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		sizeOfImage.height = std::clamp(sizeOfImage.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
	}
	else
	{
		sizeOfImage = surfaceCapabilities.currentExtent;
	}
	
	//Minimized window
	if (sizeOfImage.height <= 0 || sizeOfImage.width <= 0)
	{
	}
	//Image usage (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT always supported but more can be used and if so need to check for support
	VkImageUsageFlags imageUsage{};
	imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	//Supported surface transform (Mobile uses different orientation)
	VkSurfaceTransformFlagBitsKHR surfaceTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	if (!(surfaceCapabilities.supportedTransforms & surfaceTransform))
	{
		surfaceTransform = surfaceCapabilities.currentTransform;
	}

	//Image format and color space
	VkFormat imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
	VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	VkSurfaceFormatKHR desiredSurfaceFormat{imageFormat, colorSpace};
	uint32_t formatsCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(mVkPhysicalDevice, mVkSurface, &formatsCount, nullptr);
	std::vector<VkSurfaceFormatKHR> surfaceFormats;
	surfaceFormats.resize(formatsCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(mVkPhysicalDevice, mVkSurface, &formatsCount, surfaceFormats.data());
	if (!(surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
	{
		bool found = false;
		//Check for pairs
		for (int i = 0; i < surfaceFormats.size(); i++)
		{
			if (surfaceFormats[i].format == desiredSurfaceFormat.format && surfaceFormats[i].colorSpace == desiredSurfaceFormat.colorSpace)
			{
				found = true;
				break;
			}
		}
		//Check for format
		for (int i = 0; i < surfaceFormats.size(); i++)
		{
			if (surfaceFormats[i].format == desiredSurfaceFormat.format)
			{
				desiredSurfaceFormat.colorSpace = surfaceFormats[i].colorSpace;
				found = true;
				break;
			}
		}
		//If no pairs or format found
		if (!found)
		{
			desiredSurfaceFormat.colorSpace = surfaceFormats[0].colorSpace;
			desiredSurfaceFormat.format = surfaceFormats[0].format;
		}
	}

	//Create the swapchain!
	VkSwapchainCreateInfoKHR swapchainCreateInfo{};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = mVkSurface;
	swapchainCreateInfo.presentMode = presentMode;
	swapchainCreateInfo.imageExtent = sizeOfImage;
	swapchainCreateInfo.imageFormat = desiredSurfaceFormat.format;
	swapchainCreateInfo.imageColorSpace = desiredSurfaceFormat.colorSpace;
	swapchainCreateInfo.imageUsage = imageUsage;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.minImageCount = numberOfImages;
	swapchainCreateInfo.preTransform = surfaceTransform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.clipped = VK_TRUE;

	if (vkCreateSwapchainKHR(mVkDevice, &swapchainCreateInfo, nullptr, &mVkSwapchain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swapchain");
	}

	//Aquire the swapchain images
	uint32_t imageCount = 0;
	vkGetSwapchainImagesKHR(mVkDevice, mVkSwapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(mVkDevice, mVkSwapchain, &imageCount, swapchainImages.data());
}
void DeviceContext::createSurface(VkInstance instance, GLFWwindow* window)
{
	glfwCreateWindowSurface(instance, window, nullptr, &mVkSurface);
}

void DeviceContext::getQueueFamilyProperties(std::vector<VkQueueFamilyProperties>& queueFamilyProperties)
{
	uint32_t queueFamilyPropertyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &queueFamilyPropertyCount, nullptr);
	queueFamilyProperties.resize(queueFamilyPropertyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());
}
void DeviceContext::findQueueFamilies()
{
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	getQueueFamilyProperties(queueFamilyProperties);


	for (int i = 0; i < queueFamilyProperties.size(); i++)
	{
		int queueIndex = i;
		VkBool32 presentationSupported = false;
		if ((queueFamilyProperties[queueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
		{
			mGraphicsQueueInfo.familyIndex = queueIndex;
			break;
		}
	}
	mGraphicsQueueInfo.priorities.push_back(1.0f);
	bool foundOnlyComputeQueue = false;
	for (int i = 0; i < queueFamilyProperties.size(); i++)
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
