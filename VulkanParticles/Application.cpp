#include "Application.h"

Application::Application(std::string name, int width, int height):mName(name)
{
	mWindow = Window(width, height, mName.c_str());
	mVkInstance = nullptr;
}

int Application::initialize()
{
	mWindow.initialize();

	uint32_t extensionsCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
	mAvailableExtensions.resize(extensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, mAvailableExtensions.data());
	
	uint32_t count = 0;
	const char** extensions = glfwGetRequiredInstanceExtensions(&count);
	for (int i = 0; i < count; i++)
	{
		mDesiredExtensions.push_back(extensions[i]);
	}
	uint32_t foundExtension = 0;
	for (int i = 0; i < mDesiredExtensions.size(); i++)
	{
		const char* desiredExtension = mDesiredExtensions[i];
		for (int j = 0; j < extensionsCount; j++)
		{
			if (strcmp(desiredExtension, mAvailableExtensions[j].extensionName) == 0)
			{
				foundExtension++;
				break;
			}
		}
	}

	if (foundExtension < mDesiredExtensions.size())
	{
		std::cout << "Missing extensions\n";
		return 1;
	}

	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = nullptr;
	applicationInfo.pApplicationName = mName.c_str();
	applicationInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	applicationInfo.pEngineName = "Tea";
	applicationInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	applicationInfo.apiVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);

	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.enabledLayerCount = 0;
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(mDesiredExtensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = mDesiredExtensions.data();

	if (vkCreateInstance(&instanceCreateInfo, nullptr, &mVkInstance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance");
	}

	mDeviceContext.initialize();
	mDeviceContext.selectPhysicalDevice(mVkInstance);

	return 0;
}

int Application::run()
{
	mWindow.createWindow();
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
	return 0;
}

void Application::update()
{
}

void Application::render()
{
}
