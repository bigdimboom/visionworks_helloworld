#pragma once
#include <vulkan/vulkan.hpp>

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
	vk::ImageLayout imageLayout;
	vk::DeviceMemory memory;
	vk::ImageView imageView;
	vk::Extent3D resolution;
	uint32_t mipLevels;
	uint32_t layerCount;
	vk::DescriptorImageInfo descriptor;
	vk::MemoryPropertyFlags memoryPropertyFlags;
	vk::ImageUsageFlags imageUsage;
	vk::Sampler sampler;

	vk::ComponentMapping componentMapping;
	vk::ImageSubresourceRange imageSubresourceRange;

	vk::DeviceSize size;
	vk::DeviceSize alignment;

	void updateDescriptor();

	static std::shared_ptr<VulkanTexture> create(
		std::shared_ptr<VulkanDevice> device,
		vk::Extent3D resolution,
		vk::Format format,
		vk::ImageUsageFlags usage,
		vk::ImageType imageType,
		uint32_t mipLevels,
		uint32_t imageLayers,
		vk::MemoryPropertyFlags memoryPropertyFlags,
		vk::ComponentMapping componentMapping,
		vk::ImageSubresourceRange imageSubresourceRange,
		vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
		vk::SampleCountFlagBits samplecount = vk::SampleCountFlagBits::e1,
		vk::ImageCreateFlags ciflags = {}
	);



private:
	VulkanTexture() {}
	VulkanTexture(const VulkanTexture&) = delete;
	VulkanTexture(VulkanTexture&&) = delete;
	void operator=(const VulkanTexture&) = delete;
	void operator=(VulkanTexture&&) = delete;
};


} // end namespace graphics