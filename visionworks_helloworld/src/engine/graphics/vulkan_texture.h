#pragma once
#include <vulkan/vulkan.hpp>
// too much variable for based class?

namespace graphics
{

class VulkanDevice;

struct VulkanImageView
{
	vk::ImageView imageView;
	vk::ImageViewType viewtype;
	vk::ComponentMapping compMapping;
	vk::ImageSubresourceRange subresourceRange;
};

class VulkanTexture
{
public:
	~VulkanTexture();

	vk::PhysicalDevice physicalDevice;
	vk::Device logicalDevice;
	vk::Image image;
	vk::Format format;
	vk::Extent3D resolution;
	vk::ImageUsageFlags usage;
	vk::ImageCreateFlags imageCreateInfo;

	vk::DeviceMemory memory;
	vk::MemoryPropertyFlags memoryProperties;
	vk::DeviceSize size = 0; vk::DeviceSize alignment = 0;

	uint32_t mipLevels = 1;
	uint32_t layerCount = 1;

	void bind(vk::DeviceSize offset);

	vk::ImageView acquireImageView(vk::ImageViewType imageViewType,
								   vk::ComponentMapping compMapping,
								   vk::ImageSubresourceRange range);


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

	std::vector<VulkanImageView> d_imageViewPool;
};


} // end namespace graphics