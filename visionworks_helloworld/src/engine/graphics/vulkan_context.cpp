#include "vulkan_context.h"
#include "vulkan_helper.h"
#include <iostream>

#define ENGINE_NAME "Hobo" 
#define APP_NAME "stereo_disp"
#define ENGINE_VERSION 1u
#define APP_VERSION 1u

namespace graphics
{

// MEMBER FUNCTIONS 
VulkanContext::~VulkanContext()
{
	if (instance)
	{
		// TODO:
		device = nullptr;
		instance.destroySurfaceKHR(surface);
		destoryDebugCallback(instance, debugMsgCallback);
		instance.destroy();


		// TODO:
		surface = nullptr;
		instance = nullptr;
	}
}

std::shared_ptr<VulkanContext> VulkanContext::create(std::shared_ptr<VulkanWindow> window, 
													 vk::QueueFlags queueTypes,
													 int physicalDeviceID, 
													 bool isDebug)
{
	assert(window);

	std::shared_ptr<VulkanContext> context( new VulkanContext());
	context->window = window;
	context->isDebugMode = isDebug;
	context->instance = createVulkanInstance(window, isDebug);

	if (!context->instance)
	{
		std::cerr << "unable to create vulkan instance.\n";
		return nullptr;
	}

	if (isDebug)
	{
		context->debugMsgCallback = createDebugCallback(context->instance);
	}

	context->surface = createVulkanSurface(context->instance, window); assert(context->surface);
	auto GPUList = context->instance.enumeratePhysicalDevices();

	assert(physicalDeviceID >=0 && physicalDeviceID < GPUList.size());

	if (GPUList.empty())
	{
		std::cerr << "unable to find vulkan physical device (GPUs).\n";
		return nullptr;
	}

	context->device = VulkanDevice::create(GPUList[physicalDeviceID], queueTypes);

	auto sss = VulkanSwapChain::querySwapChainSupport(GPUList[physicalDeviceID], context->surface);
	//context->swapChain = std::shared_ptr<VulkanSwapChain>(new VulkanSwapChain());




	return context;
}

bool VulkanContext::initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	if (SDL_Vulkan_LoadLibrary(nullptr) != 0)
	{
		SDL_Log("Unable to load vulkan: %s", SDL_GetError());
		return false;
	}

	return true;
}

void VulkanContext::shutdown()
{
	SDL_Vulkan_UnloadLibrary();
	SDL_Quit();
}

// STATIC FUNCTIONS
const std::string & VulkanContext::EngineName()
{
	static std::string engine = ENGINE_NAME;
	return engine;
}

const std::string & VulkanContext::AppName()
{
	static std::string app = APP_NAME;
	return app;
}

const uint32_t & VulkanContext::EngineVersion()
{
	static uint32_t engineVersion = ENGINE_VERSION;
	return engineVersion;
}

const uint32_t & VulkanContext::AppVersion()
{
	static uint32_t appVersion = APP_VERSION;
	return appVersion;
}

std::shared_ptr<VulkanWindow> VulkanContext::createWindow(const std::string & title, int width, int height, bool fullScreen, float gamma)
{
	return VulkanWindow::create(title.c_str(), width, height, fullScreen, gamma);
}

vk::Instance VulkanContext::createVulkanInstance(const std::vector<const char*>& instanceLayers, const std::vector<const char*>& instanceExtensions)
{
	return VulkanHelper::createVulkanInstance(instanceLayers, instanceExtensions, EngineName().c_str(), AppName().c_str(), EngineVersion(), AppVersion());
}

vk::Instance VulkanContext::createVulkanInstance(std::shared_ptr<VulkanWindow> window, bool enableDebug)
{
	assert(window);

	auto vulkanInstanceLayers = std::vector<const char*>();
	auto vulkanInstanceExtensions = window->requiredVkInstanceExtensions();

	if (enableDebug)
	{
		vulkanInstanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
		vulkanInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return createVulkanInstance(vulkanInstanceLayers, vulkanInstanceExtensions);
}

vk::DebugUtilsMessengerEXT VulkanContext::createDebugCallback(const vk::Instance & instance)
{
	return VulkanHelper::createDebugCallback(instance);
}

void VulkanContext::destoryDebugCallback(const vk::Instance & instance, vk::DebugUtilsMessengerEXT & debug)
{
	VulkanHelper::destoryDebugCallback(instance, debug);
}

vk::SurfaceKHR VulkanContext::createVulkanSurface(const vk::Instance & instance, std::shared_ptr<VulkanWindow> window)
{
	assert(instance && window);
	return window->createVulkanSurface(instance);
}

std::vector<vk::PhysicalDevice> VulkanContext::physicalDeviceList(const vk::Instance & vulkanInst)
{
	assert(vulkanInst);
	return vulkanInst.enumeratePhysicalDevices();
}

uint32_t VulkanContext::getOptimalFamilyQueueIndex(const vk::PhysicalDevice & phsicalDevice, vk::QueueFlagBits queueFlags)
{
	assert(phsicalDevice);
	return VulkanHelper::getOptimalFamilyQueueIndex(phsicalDevice, queueFlags);
}


} // end namespace graphics