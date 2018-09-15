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
	vk::ImageView imageView;
	vk::Sampler sampler;
	vk::DeviceMemory memory;
	vk::MemoryPropertyFlags memoryProperties;
	vk::DescriptorImageInfo descriptor;

	uint32_t mipLevels;
	uint32_t layerCount;



	void updateDescriptor();

	static std::shared_ptr<VulkanTexture> create(
		std::shared_ptr<VulkanDevice> device
	);



private:
	VulkanTexture() {}
	VulkanTexture(const VulkanTexture&) = delete;
	VulkanTexture(VulkanTexture&&) = delete;
	void operator=(const VulkanTexture&) = delete;
	void operator=(VulkanTexture&&) = delete;
};


} // end namespace graphics