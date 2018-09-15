#pragma once
#include <vulkan/vulkan.hpp>
// too much variable for based class?

namespace graphics
{

class VulkanDevice;

class VulkanTexture
{
public:
	~VulkanTexture();

	vk::PhysicalDevice physicalDevice;
	vk::Device logicalDevice;
	vk::Image image;
	vk::Format format;
	vk::Extent3D resolution;
	vk::ImageLayout layout;

	vk::DeviceMemory memory;
	vk::MemoryPropertyFlags memoryProperties;
	uint32_t size = 0; uint32_t alignment = 0;

	vk::ImageView imageView;
	vk::Sampler sampler;
	
	vk::DescriptorImageInfo descriptor;

	uint32_t mipLevels = 1;
	uint32_t layerCount = 1;

	void updateDescriptor();

	static std::shared_ptr<VulkanTexture> create(
		std::shared_ptr<VulkanDevice> device,
		vk::Format format,
		vk::Extent3D resolution,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags memoryProperties,
		uint32_t mipLevels = 1, uint32_t arrayLayers = 1,
		vk::SampleCountFlagBits sampleCountBit = vk::SampleCountFlagBits::e1,
		vk::ImageCreateFlags imageCreateInfo = {}
	);

	/**static member functions**/
	static vk::ImageType getImageType(const vk::Extent3D & resolution);


private:
	VulkanTexture() {}
	VulkanTexture(const VulkanTexture&) = delete;
	VulkanTexture(VulkanTexture&&) = delete;
	void operator=(const VulkanTexture&) = delete;
	void operator=(VulkanTexture&&) = delete;
};


} // end namespace graphics