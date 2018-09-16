#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include <SDL2/SDL.h>

namespace graphics
{

class VulkanHelper
{
public:
	/*create vulkan instance */
	static vk::Instance createVulkanInstance(const std::vector<const char*>& instanceLayers, 
											 const std::vector<const char*>& instanceExtensions, 
											 const char* enginename, const char* appname, 
											 uint32_t engineVersion, uint32_t appVersion);

	/*Create & destory Debug Callback*/
	static vk::DebugUtilsMessengerEXT createDebugCallback(const vk::Instance & instance);
	static void destoryDebugCallback(const vk::Instance & instance, vk::DebugUtilsMessengerEXT & debug);

	/*create vulkan surface for window*/
	static vk::SurfaceKHR createVulkanSurface(const vk::Instance& instance, SDL_Window* window);

	/*check if this GPU supports this type of queue family*/
	static bool isFamilyQueueSupported(const vk::PhysicalDevice& physicalDevice, vk::QueueFlagBits queueFamilyBit, std::vector<uint32_t>& queueFamilyIndices);
	/*check if this index on the GPU is dedicated queue, usually a graphics queue supports all types*/
	static bool isIndexDedicatedFamilyQueue(const vk::PhysicalDevice& physicalDevice, uint32_t index, vk::QueueFlagBits queueFamilyBit);
	/*return the best family queue index assignment on this GPU*/
	static uint32_t getOptimalFamilyQueueIndex(const vk::PhysicalDevice& phsicalDevice, vk::QueueFlagBits queueFlags);
	/*return the memory type index(location) bit on this GPU given memory type and memory properties*/
	static uint32_t getMemoryTypeIndex(const vk::PhysicalDevice& physicalDevice, uint32_t mem_type_bits, vk::MemoryPropertyFlags flags);

	/**create command pool**/
	static vk::CommandPool createCommandPool(const vk::Device& logicalDevice, uint32_t queueFamilyIndex, vk::CommandPoolCreateFlags createFlags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

	/*allocate command buffers*/
	static std::vector<vk::CommandBuffer> allocateCommandBuffers(const vk::Device& logicalDevice,
																 const vk::CommandPool& cmdPool,
																 vk::CommandBufferLevel& level,
																 uint32_t count);


	/**test if the format is a depth format**/
	static bool isDepthFormat(const vk::Format& format);

	/**test if the format is a stencil format**/
	static bool isStencilFormat(const vk::Format& format);

	/*load shader module*/
	static vk::ShaderModule loadShaderSPIRV(const char *fileName, const vk::Device& device);
	static vk::ShaderModule loadShaderGLSL(const char *fileName, const vk::Device& device, vk::ShaderStageFlagBits stage);

	/**check error**/
	static void check_vk_result(vk::Result err);

};

} // end namespace graphics