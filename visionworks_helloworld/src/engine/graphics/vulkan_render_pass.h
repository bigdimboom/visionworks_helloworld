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

class VulkanRenderPass
{
public:
	~VulkanRenderPass();

	vk::PhysicalDevice physicalDevice;
	vk::Device logicalDevice;
	vk::RenderPass renderpass;
	std::vector<vk::AttachmentDescription> attachments;
	std::vector<vk::SubpassDescription> subpasses;
	std::vector<vk::SubpassDependency> denpendencies;
	std::vector<vk::Framebuffer> frameBuffers;
	std::array<vk::ClearValue, 2> clearValues;

	/**  @brief Typecast to VkDevice */
	operator vk::RenderPass() { return renderpass; };

	static std::shared_ptr<VulkanRenderPass> createDefault(
		std::shared_ptr<VulkanSwapChain> swapChain,
		std::shared_ptr<VulkanDepthResource> depth
	);

	/**
	* Creates a default sampler for sampling from any of the framebuffer attachments
	* Applications are free to create their own samplers for different use cases
	*
	* @param magFilter Magnification filter for lookups
	* @param minFilter Minification filter for lookups
	* @param adressMode Adressing mode for the U,V and W coordinates
	* user should not delete vk::imageView, RAII is applied in this object
	*
	*/
	vk::Sampler acquireSampler(vk::Filter magFilter, vk::Filter minFilter, vk::SamplerAddressMode adressMode);


	/*
	* set screen color
	*/
	void clearAll(float r, float g, float b, float a);

private:
	VulkanRenderPass() {}
	VulkanRenderPass(const VulkanRenderPass&) = delete;
	VulkanRenderPass(VulkanRenderPass&&) = delete;
	void operator=(const VulkanRenderPass&) = delete;
	void operator=(VulkanRenderPass&&) = delete;

	struct
	{
		vk::SamplerCreateInfo info;
		vk::Sampler sampler;
	}d_samplerData;

};

} // end namespace graphics