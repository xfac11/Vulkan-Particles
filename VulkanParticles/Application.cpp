#include "Application.h"

Application::Application(std::string name, int width, int height):mName(name)
{
	mWindow = Window(width, height, mName.c_str());
	mVkInstance = nullptr;
	mDesiredDeviceExtensions = { "VK_KHR_swapchain" };

#ifdef _DEBUG
	mDesiredInstanceLayers = { "VK_LAYER_KHRONOS_validation" };
	mDesiredInstanceExtensions = { "VK_EXT_debug_report" };
#endif
	mDesiredDeviceFeatures = VkPhysicalDeviceFeatures{};
	mDesiredDeviceFeatures.geometryShader = 1;


}

int Application::initialize()
{
	mWindow.initialize();
	uint32_t count = 0;
	const char** extensions = glfwGetRequiredInstanceExtensions(&count);

	for (int i = 0; i < count; i++)
	{
		mDesiredInstanceExtensions.push_back(extensions[i]);
	}

	if (checkInstanceExtensionSupport() != 0)
	{
		return 1;
	}

	if (checkInstanceLayerSupport() != 0)
	{
		return 1;
	}
	

	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = nullptr;
	applicationInfo.pApplicationName = mName.c_str();
	applicationInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	applicationInfo.pEngineName = "Tea";
	applicationInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(mDesiredInstanceLayers.size());
	instanceCreateInfo.ppEnabledLayerNames = mDesiredInstanceLayers.data();
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(mDesiredInstanceExtensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = mDesiredInstanceExtensions.data();

	VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &mVkInstance);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance");
	}

#ifdef _DEBUG
	createDebugCallbacks();
#endif // _DEBUG



	mDeviceContext.initialize();
	mDeviceContext.selectPhysicalDevice(mVkInstance);
	mDeviceContext.createLogicalDevice(mDesiredDeviceExtensions, mDesiredDeviceFeatures);

	mWindow.createWindow();
	mDeviceContext.createSurface(mVkInstance, mWindow.getGLFWWindow());
	VkExtent2D size{ mWindow.getWidth(), mWindow.getHeight() };
	mDeviceContext.createSwapchain(size);
	return 0;
}

void Application::createDebugCallbacks()
{
	PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(mVkInstance, "vkCreateDebugReportCallbackEXT"));
	PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(mVkInstance, "vkDebugReportMessageEXT"));
	mVkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(mVkInstance, "vkDestroyDebugReportCallbackEXT"));
	/* Setup callback creation information */
	VkDebugReportCallbackCreateInfoEXT callbackCreateInfo{};
	callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	callbackCreateInfo.pNext = nullptr;
	callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
		VK_DEBUG_REPORT_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
	callbackCreateInfo.pfnCallback = &MyDebugReportCallback;
	callbackCreateInfo.pUserData = nullptr;

	/* Register the callback */
	VkResult result = vkCreateDebugReportCallbackEXT(mVkInstance, &callbackCreateInfo, nullptr, &mDebugReportCallback);
}

int Application::checkInstanceExtensionSupport()
{
	uint32_t extensionsCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
	mAvailableExtensions.resize(extensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, mAvailableExtensions.data());

	uint32_t foundExtension = 0;
	for (int i = 0; i < mDesiredInstanceExtensions.size(); i++)
	{
		const char* desiredExtension = mDesiredInstanceExtensions[i];
		for (int j = 0; j < extensionsCount; j++)
		{
			if (strcmp(desiredExtension, mAvailableExtensions[j].extensionName) == 0)
			{
				foundExtension++;
				break;
			}
		}
	}

	if (foundExtension < mDesiredInstanceExtensions.size())
	{
		std::cout << "Missing extensions\n";
		return 1;
	}
	return 0;
}

int Application::checkInstanceLayerSupport()
{
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	mAvailableLayers.resize(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, mAvailableLayers.data());

	uint32_t foundLayer = 0;
	for (int i = 0; i < mDesiredInstanceLayers.size(); i++)
	{
		const char* desiredLayer = mDesiredInstanceLayers[i];
		for (int j = 0; j < layerCount; j++)
		{
			if (strcmp(desiredLayer, mAvailableLayers[j].layerName) == 0)
			{
				foundLayer++;
				break;
			}
		}
	}

	if (foundLayer < mDesiredInstanceLayers.size())
	{
		std::cout << "Missing layers\n";
		return 1;
	}
	return 0;
}

int Application::run()
{
	
	while (!mWindow.shouldClose())
	{
		update();
		render();

		glfwPollEvents();
	}

	return 0;
}

int Application::clear()
{
	mDeviceContext.clear(mVkInstance);
	mWindow.clear(mVkInstance);


#ifdef _DEBUG
	mVkDestroyDebugReportCallbackEXT(mVkInstance, mDebugReportCallback, nullptr);
#endif // _DEBUG

	vkDestroyInstance(mVkInstance, nullptr);
	return 0;
}

void Application::update()
{
}

void Application::render()
{
}
