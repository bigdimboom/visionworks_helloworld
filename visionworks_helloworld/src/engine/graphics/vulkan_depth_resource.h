#pragma once
#include <vulkan/vulkan.hpp>

namespace graphics
{

/**depth and stencil data**/
class VulkanDepthResource
{
public:
	~VulkanDepthResource();

	vk::PhysicalDevice physicalDevice = nullptr;
	vk::Device logicalDevice = nullptr;
	vk::Format format = vk::Format::eUndefined;
	vk::Image image = nullptr;
	vk::ImageView imageView = nullptr;
	vk::ImageSubresourceRange imageSubresourceRange;

	vk::DeviceMemory deviceMemory = nullptr;
	uint32_t memoryTypeIndex;



	bool hasDepth() const;
	bool hasStencil() const;

	static std::shared_ptr<VulkanDepthResource> create(const vk::Extent2D& resolution,
													   const vk::PhysicalDevice& physicalDevice,
													   const vk::Device& device, uint32_t depthRange = 1);

private:
	VulkanDepthResource() {}
	VulkanDepthResource(const VulkanDepthResource&) = delete;
	VulkanDepthResource(VulkanDepthResource&&) = delete;
	void operator=(const VulkanDepthResource&) = delete;
	void operator=(VulkanDepthResource&&) = delete;

};

} // end namespace graphics