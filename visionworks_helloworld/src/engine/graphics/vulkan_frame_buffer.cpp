#include "vulkan_frame_buffer.h"
#include "vulkan_helper.h"
#include "vulkan_depth_resource.h"
#include "vulkan_render_pass.h"
#include "vulkan_swapchain.h"

namespace graphics
{

bool FramebufferAttachment::hasDepth() const
{
	return VulkanHelper::isDepthFormat(this->format);
}

bool FramebufferAttachment::hasStencil() const
{
	return VulkanHelper::isStencilFormat(this->format);
}

bool FramebufferAttachment::isDepthStencil() const
{
	return hasDepth() || hasStencil();
}

VulkanFrameBuffer::~VulkanFrameBuffer()
{
	if (frameBuffer)
	{
		assert(logicalDevice);
		logicalDevice.destroyFramebuffer(frameBuffer);
		frameBuffer = nullptr;
	}
}

std::shared_ptr<VulkanFrameBuffer> VulkanFrameBuffer::create(std::shared_ptr<VulkanSwapChain> swapChain,
															 std::shared_ptr<VulkanDepthResource> depthResource, 
															 std::shared_ptr<VulkanRenderPass> renderPass)
{
	assert(swapChain->logicalDevice == depthResource->logicalDevice);
	assert(swapChain->logicalDevice);
	assert(depthResource->physicalDevice);

	auto fb = std::shared_ptr<VulkanFrameBuffer>(new VulkanFrameBuffer());
	fb->logicalDevice = swapChain->logicalDevice;
	fb->renderPass = renderPass->renderpass;

	std::vector<vk::ImageView> attachmentViews;
	std::vector<vk::ImageSubresourceRange> imageRanges;

	for (auto& elem : swapChain->buffers)
	{
		attachmentViews.push_back(elem.imageView);
		imageRanges.push_back(elem.imageSubresourceRange);
	}
	attachmentViews.push_back(depthResource->imageView);
	imageRanges.push_back(depthResource->imageSubresourceRange);

	uint32_t maxLayers = 0;
	for (auto& elem : imageRanges)
	{
		if (elem.layerCount >= maxLayers)
		{
			maxLayers = elem.layerCount;
		}
	}

	vk::FramebufferCreateInfo fbci;
	fbci.renderPass = renderPass->renderpass;
	fbci.attachmentCount = (uint32_t)attachmentViews.size();
	fbci.pAttachments = attachmentViews.data();
	fbci.width = swapChain->actualExtent.width;
	fbci.height = swapChain->actualExtent.height;
	fbci.layers = maxLayers;

	fb->frameBuffer = fb->logicalDevice.createFramebuffer(fbci);

	return fb;
}

} // end namespace graphics