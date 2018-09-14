#pragma once
#include <vulkan/vulkan.hpp>

namespace graphics
{

class VulkanContext;

class VulkanDevice
{
	friend class VulkanContext;
public:
	~VulkanDevice();

	/** @brief physical device representation (hardware view of device) */
	vk::PhysicalDevice physicalDevice;
	/** @brief Logical device representation (application's view of the device) */
	vk::Device logicalDevice;
	/** @brief Properties of the physical device including limits that the application can check against */
	vk::PhysicalDeviceProperties properties;
	/** @brief Features of the physical device that an application can use to check if a feature is supported */
	vk::PhysicalDeviceFeatures features;
	/** @brief Features that have been enabled for use on the physical device */
	vk::PhysicalDeviceFeatures enabledFeatures;
	/** @brief Memory types and heaps of the physical device */
	vk::PhysicalDeviceMemoryProperties memoryProperties;
	/** @brief Queue family properties of the physical device */
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
	/** @brief List of extensions supported by the device */
	std::vector<std::string> supportedExtensions;

	/** @brief Default command pool for the graphics queue family index */
	vk::CommandPool graphicsCmdPool;

	///** @brief Set to true when the debug marker extension is detected */
	//bool enableDebugMarkers = false;

	/** @brief Contains queue family indices */
	struct QueueFamilyIndicies
	{
		uint32_t graphics;
		uint32_t compute;
		uint32_t transfer;
	} queueFamilyIndices;

	vk::QueueFlags queueFlags = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eTransfer;

	/**  @brief Typecast to VkDevice */
	operator vk::Device() { return logicalDevice; };

	/*get memory type index*/
	uint32_t getMemoryType(uint32_t typeBits, vk::MemoryPropertyFlags properties);

	/*get family queue index*/
	uint32_t getQueueFamilyIndex(vk::QueueFlagBits queueFlags);

	/*get a vulkan queue*/
	vk::Queue queue(uint32_t familyQueueIndex);

	/*commadn pool creation*/
	vk::CommandPool createCmdPool(uint32_t queueFamilyIndex, vk::CommandPoolCreateFlags createFlags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

	/**static create function**/
	static std::shared_ptr<VulkanDevice> create(const vk::PhysicalDevice& device, vk::QueueFlags queueFlags);

private:

	VulkanDevice(const VulkanDevice&) =delete;
	VulkanDevice(VulkanDevice&&) = delete;

	VulkanDevice() {};
	VulkanDevice(const vk::PhysicalDevice& device, vk::QueueFlags queueFlags);
	void operator=(const VulkanDevice& other);
	void operator=(VulkanDevice&& other);

	// HELPERS
	void createLogicalDevice();
};

} //end namespace graphics