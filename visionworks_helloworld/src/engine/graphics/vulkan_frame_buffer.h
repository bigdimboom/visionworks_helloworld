#pragma once
#include <vulkan/vulkan.hpp>

namespace graphics
{

struct FramebufferAttachment
{
	vk::Image image;
	vk::DeviceMemory memory;
	vk::ImageView view;
	vk::Format format;
	vk::ImageSubresourceRange subresourceRange;
	vk::AttachmentDescription description;

	/**
	* @brief Returns true if the attachment has a depth component
	*/
	bool hasDepth() const;
	/**
	* @brief Returns true if the attachment has a stencil component
	*/
	bool hasStencil() const;
	/**
	* @brief Returns true if the attachment is a depth and/or stencil attachment
	*/
	bool isDepthStencil() const;
};


class VulkanSwapChain;
class VulkanDepthResource;
class VulkanRenderPass;

class VulkanFrameBuffer
{
public:
	~VulkanFrameBuffer();

	vk::Device logicalDevice;
	vk::RenderPass renderPass;
	vk::Framebuffer frameBuffer;

	static std::shared_ptr<VulkanFrameBuffer> create(
		std::shared_ptr<VulkanSwapChain> swapChain,
		std::shared_ptr<VulkanDepthResource> depthResource,
		std::shared_ptr<VulkanRenderPass> renderPass
	);

private:
	VulkanFrameBuffer() {};
	VulkanFrameBuffer(const VulkanFrameBuffer&) = delete;
	VulkanFrameBuffer(VulkanFrameBuffer&&) = delete;
	void operator=(const VulkanFrameBuffer &) = delete;
	void operator=(VulkanFrameBuffer &&) = delete;

};

} // end namespace graphics