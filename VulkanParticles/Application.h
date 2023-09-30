#pragma once
#include<string>
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include<vector>
#include<iostream>
#include"DeviceContext.h"
#include"Window.h"
class Application
{
public:
	Application(std::string name, int width, int height);
	int initialize();
	void createDebugCallbacks();
	int run();
	int clear();

#ifdef _DEBUG
	static VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback(
		VkDebugReportFlagsEXT       flags,
		VkDebugReportObjectTypeEXT  objectType,
		uint64_t                    object,
		size_t                      location,
		int32_t                     messageCode,
		const char* pLayerPrefix,
		const char* pMessage,
		void* pUserData)
	{
		std::cerr << pMessage << std::endl;
		return VK_FALSE;
	}
#endif

private:
	std::string mName;
	VkInstance mVkInstance;
	DeviceContext mDeviceContext;
	Window mWindow;
	std::vector<VkExtensionProperties> mAvailableExtensions;
	std::vector<VkLayerProperties> mAvailableLayers;
	std::vector<const char*> mDesiredInstanceExtensions;
	std::vector<const char*> mDesiredInstanceLayers;
	std::vector<const char*> mDesiredDeviceExtensions;
	VkPhysicalDeviceFeatures mDesiredDeviceFeatures;
	void update();
	void render();
	int checkInstanceExtensionSupport();
	int checkInstanceLayerSupport();

#ifdef _DEBUG
	VkDebugReportCallbackEXT mDebugReportCallback;
	PFN_vkDestroyDebugReportCallbackEXT mVkDestroyDebugReportCallbackEXT;
#endif // DEBUG
};