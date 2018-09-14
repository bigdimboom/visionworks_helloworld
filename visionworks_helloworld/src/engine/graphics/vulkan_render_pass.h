#pragma once
#include <vulkan/vulkan.hpp>
#include "vulkan_swapchain.h"
#include "vulkan_depth_resource.h"

namespace graphics
{

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

	static std::shared_ptr<VulkanRenderPass> create(
		std::shared_ptr<VulkanSwapChain> swapChain,
		std::shared_ptr<VulkanDepthResource> depth
	);

	//static std::shared_ptr<VulkanRenderPass> create();


private:
	VulkanRenderPass() {}
	VulkanRenderPass(const VulkanRenderPass&) = delete;
	VulkanRenderPass(VulkanRenderPass&&) = delete;
	void operator=(const VulkanRenderPass&) = delete;
	void operator=(VulkanRenderPass&&) = delete;
};

} // end namespace graphics