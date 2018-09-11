#pragma once
#include <vulkan/vulkan.hpp>
#include "vulkan_window.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"

namespace graphics
{

class VulkanContext
{
public:

	~VulkanContext();

	/**debug**/
	bool isDebugMode = true;
	vk::DebugUtilsMessengerEXT debugMsgCallback = nullptr;

	/**member variables**/
	std::shared_ptr<VulkanWindow> window;
	/** @brief vulkan abstract running environment */
	vk::Instance instance;
	/** @brief vulkan abstract drawing canvas for a window */
	vk::SurfaceKHR surface;
	/** @brief vulkan deivce from software view**/
	std::shared_ptr<VulkanDevice> device;
	/* @brief vulkan swapchain*/
	std::shared_ptr<VulkanSwapChain> swapChain;

	/****************************************************************************************************************************************************/
	// Member Functions
	static std::shared_ptr<VulkanContext> create(std::shared_ptr<VulkanWindow> window,
												 vk::QueueFlags queueTypes = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute,
												 int gpuID = 0,
												 bool debug = true);


	/****************************************************************************************************************************************************/
	// STATIC FUNCTIONS
	static bool initialize();
	static void shutdown();

	static const std::string& EngineName();
	static const std::string& AppName();
	static const uint32_t& EngineVersion();
	static const uint32_t& AppVersion();

	/**create window**/
	static std::shared_ptr<VulkanWindow> createWindow(const std::string& title, int width, int height, bool fullScreen = false, float gamma = 1.0f);

	/*create vulkan instance */
	static vk::Instance createVulkanInstance(const std::vector<const char*>& instanceLayers, const std::vector<const char*>& instanceExtensions);
	static vk::Instance createVulkanInstance(std::shared_ptr<VulkanWindow> window, bool enableDebug = true);

	/*Create & destory Debug Callback*/
	static vk::DebugUtilsMessengerEXT createDebugCallback(const vk::Instance & instance);
	static void destoryDebugCallback(const vk::Instance & instance, vk::DebugUtilsMessengerEXT & debug);

	/*create vulkan surface for window*/
	static vk::SurfaceKHR createVulkanSurface(const vk::Instance& instance, std::shared_ptr<VulkanWindow> window);

	/*get a list of physical gpu devie */
	static std::vector<vk::PhysicalDevice> physicalDeviceList(const vk::Instance& instnace);

	/*return the best family queue index assignment on this GPU*/
	static uint32_t getOptimalFamilyQueueIndex(const vk::PhysicalDevice& phsicalDevice, vk::QueueFlagBits queueFlags);


private:
	VulkanContext() {}
	VulkanContext(const VulkanContext&) = delete;
	VulkanContext(VulkanContext&&) = delete;
	void operator=(const VulkanContext&) = delete;
	void operator=(VulkanContext&&) = delete;
};

}