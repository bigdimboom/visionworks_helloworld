#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

namespace graphics
{

class VulkanContext;
class VulkanRenderPass;

class VulkanMasterRenderer
{
public:
	~VulkanMasterRenderer();

	uint32_t acquireFrame() const;
	void addCommandMainPass(vk::CommandBuffer cmd, std::shared_ptr<VulkanRenderPass> renderPass);
	void addCommandSubpass(vk::CommandBuffer master, vk::CommandBuffer subpass);
	void presentFrame();

	static std::shared_ptr<VulkanMasterRenderer> create(std::shared_ptr<VulkanContext> context);

private:

	VulkanMasterRenderer(const VulkanMasterRenderer&) = delete;
	VulkanMasterRenderer(VulkanMasterRenderer&&) = delete;
	void operator=(const VulkanMasterRenderer&) = delete;
	void operator=(VulkanMasterRenderer&&) = delete;

	std::shared_ptr<VulkanContext> d_context;

	std::vector<vk::Fence> d_fence;
	std::vector<vk::Semaphore> d_imageAcquiringSemaphore;
	std::vector<vk::Semaphore> d_imageRenderingSemaphore;

	uint32_t d_currentIndex = 0;

	std::vector<vk::CommandBuffer> d_masters;
	std::vector<std::shared_ptr<VulkanRenderPass>> d_renderPasses;
	std::vector<std::vector<vk::CommandBuffer>> d_slaves;
};

} // end namespace graphics