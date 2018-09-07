#include "vulkan_context.h"
#include <string>
#include <set>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

namespace graphics
{

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
		vulkanInstanceExtensions.push_back("VK_EXT_debug_report");
		vulkanInstanceExtensions.push_back("VK_EXT_debug_utils");
	}

	return createVulkanInstance(vulkanInstanceLayers, vulkanInstanceExtensions);
}

vk::SurfaceKHR VulkanContext::createVulkanSurface(std::shared_ptr<VulkanWindow> window, const vk::Instance & instance)
{
	assert(window && instance);
	return window->createVulkanSurface(instance);
}


}// end namespace graphics