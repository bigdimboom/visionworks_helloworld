#include "vulkan_context.h"
#include <string>
#include <set>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <iostream>

namespace graphics
{

// HELPERS
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) 
{

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

// STATIC FUNCTIONS
bool VulkanContext::initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "sdl init error.\n");
		return false;
	}

	if (SDL_Vulkan_LoadLibrary(nullptr) != 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "sdl vulkan library init error.\n");
		return false;
	}

	return true;
}

void VulkanContext::uninitialize()
{
	SDL_Vulkan_UnloadLibrary();
	SDL_Quit();
}


const std::string & VulkanContext::AppName()
{
	static std::string appName = "openvx_app";
	return appName;
}

const std::string & VulkanContext::EngineName()
{
	static std::string engineName = "vulkan_openvx";
	return engineName;
}

const uint32_t & VulkanContext::AppVersion()
{
	static auto version = 1u;
	return version;
}

const uint32_t & VulkanContext::EngineVersion()
{
	static auto version = 1u;
	return version;
}

void VulkanContext::checkVulkanResult(vk::Result result)
{
	if (result == vk::Result::eSuccess)
	{
		return;
	}
	printf("VkResult %d\n", result);

	throw std::runtime_error("vulkan error");
}

std::shared_ptr<VulkanWindow> VulkanContext::createWindow(const std::string & title, int width, int height, bool fullScreen, float gamma)
{
	return VulkanWindow::create(title, width, height, fullScreen, gamma);
}

vk::Instance VulkanContext::createVulkanInstance(const std::vector<const char*>& layers,
												 const std::vector<const char*>& extensions)
{
	// query supported layers and extension
	auto vk_inst_layers = vk::enumerateInstanceLayerProperties();
	auto vk_inst_extensions = vk::enumerateInstanceExtensionProperties();

	std::set<std::string> supportedlayers;
	std::set<std::string> supportedextensions;

	for (auto& elem : vk_inst_layers)
	{
		supportedlayers.insert(elem.layerName);
	}

	for (auto& elem : vk_inst_extensions)
	{
		supportedextensions.insert(elem.extensionName);
	}

	for (const auto& elem : layers)
	{
		std::string tmp(elem);
		if (!supportedlayers.count(tmp))
		{
			throw std::runtime_error("deos not support layer: " + tmp);
		}
	}

	for (const auto& elem : extensions)
	{
		std::string tmp(elem);
		if (!supportedextensions.count(tmp))
		{
			throw std::runtime_error("deos not support extension: " + tmp);
		}
	}

	vk::ApplicationInfo appInfo;
	vk::InstanceCreateInfo instance_create_info;

	appInfo.setApiVersion(VK_API_VERSION_1_1);
	appInfo.setEngineVersion(EngineVersion());
	appInfo.setPEngineName(EngineName().c_str());
	appInfo.setApplicationVersion(AppVersion());
	appInfo.setPApplicationName(AppName().c_str());

	instance_create_info.setPApplicationInfo(&appInfo);
	instance_create_info.setEnabledLayerCount((uint32_t)layers.size());
	instance_create_info.setPpEnabledLayerNames(layers.data());
	instance_create_info.setEnabledExtensionCount((uint32_t)extensions.size());
	instance_create_info.setPpEnabledExtensionNames(extensions.data());

	return vk::createInstance(instance_create_info);
}

vk::Instance VulkanContext::createVulkanInstance(std::shared_ptr<VulkanWindow> window, bool enableDebug)
{
	assert(window);

	auto vulkanInstanceLayers = std::vector<const char*>();
	auto vulkanInstanceExtensions = window->requiredExtensions();

	if (enableDebug)
	{
		vulkanInstanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
		vulkanInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return createVulkanInstance(vulkanInstanceLayers, vulkanInstanceExtensions);
}

vk::DebugUtilsMessengerEXT VulkanContext::createDebugCallback(const vk::Instance & instance)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	VkDebugUtilsMessengerEXT callback;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

	if (func != nullptr) 
	{
		checkVulkanResult((vk::Result)func(instance, &createInfo, nullptr, &callback));
	}

	return callback;
}

void VulkanContext::destoryDebugCallback(const vk::Instance & instance, vk::DebugUtilsMessengerEXT & debug)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr) 
	{
		func(instance, debug, nullptr);
		debug = nullptr;
	}
}

vk::SurfaceKHR VulkanContext::createVulkanSurface(std::shared_ptr<VulkanWindow> window, const vk::Instance & instance)
{
	assert(window && instance);
	return window->createVulkanSurface(instance);
}

const std::vector<vk::PhysicalDevice> VulkanContext::getPhysicalDeviceList(const vk::Instance & instance)
{
	assert(instance);
	return instance.enumeratePhysicalDevices();
}

}// end namespace graphics