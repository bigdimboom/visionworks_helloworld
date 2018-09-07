#pragma once

#include <vector>
#include <memory>
#include <vulkan/vulkan.hpp>
#include "vulkan_window.h"

namespace graphics
{

class VulkanContext
{
public:
	std::shared_ptr<VulkanWindow> window = nullptr;
	vk::Instance instance = nullptr;
	vk::DebugReportCallbackEXT debugReport = nullptr;
	vk::SurfaceKHR surface = nullptr;

	static bool initialize();
	static void uninitialize();

	static const std::string& AppName();
	static const std::string& EngineName();
	static const uint32_t& AppVersion();
	static const uint32_t& EngineVersion();

	static void checkVulkanResult(vk::Result result);

	static std::shared_ptr<VulkanWindow> createWindow(const std::string& title, int width, int height, bool fullScreen = false, float gamma = 1.0f);
	static vk::Instance createVulkanInstance(const std::vector<const char*>& instanceLayers, const std::vector<const char*>& instanceExtensions);
	static vk::Instance createVulkanInstance(std::shared_ptr<VulkanWindow> window, bool enableDebug = true);
	static vk::SurfaceKHR createVulkanSurface(std::shared_ptr<VulkanWindow> window, const vk::Instance& instance);
	static vk::DebugReportCallbackEXT createDebugCallback(const vk::Instance& instance);
	static void destoryDebugCallback(const vk::Instance& instance, vk::DebugReportCallbackEXT& debug);
	static const std::vector<vk::PhysicalDevice> getPhysicalDeviceList(const vk::Instance& instance);


};

}// end namespace graphics