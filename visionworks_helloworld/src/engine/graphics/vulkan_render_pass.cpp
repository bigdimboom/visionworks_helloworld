#include "vulkan_render_pass.h"

namespace graphics
{

VulkanRenderPass::~VulkanRenderPass()
{
	if (renderpass)
	{
		assert(logicalDevice);
		logicalDevice.destroyRenderPass(renderpass);
		renderpass = nullptr;
	}
}

std::shared_ptr<VulkanRenderPass> VulkanRenderPass::create(std::shared_ptr<VulkanSwapChain> swapChain,
														   std::shared_ptr<VulkanDepthResource> depth)
{
	assert(swapChain && depth);
	assert(swapChain->logicalDevice == depth->logicalDevice);
	assert(swapChain->logicalDevice);
	assert(depth->physicalDevice);
	assert(depth->format != vk::Format::eUndefined);

	auto rpData = std::shared_ptr<VulkanRenderPass>(new VulkanRenderPass());
	rpData->physicalDevice = depth->physicalDevice;
	rpData->logicalDevice = depth->logicalDevice;

	enum AttachmentKey
	{
		COLOR = 0,
		DEPTH = 1,
		AttachmentSize
	};

	std::array<vk::AttachmentDescription, AttachmentKey::AttachmentSize> attachments;

	attachments[AttachmentKey::COLOR].setFormat(swapChain->surfaceFormat.format);
	attachments[AttachmentKey::COLOR].setSamples(vk::SampleCountFlagBits::e1);
	attachments[AttachmentKey::COLOR].setLoadOp(vk::AttachmentLoadOp::eClear);
	attachments[AttachmentKey::COLOR].setStoreOp(vk::AttachmentStoreOp::eStore);
	attachments[AttachmentKey::COLOR].setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
	attachments[AttachmentKey::COLOR].setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
	attachments[AttachmentKey::COLOR].setInitialLayout(vk::ImageLayout::eUndefined);
	attachments[AttachmentKey::COLOR].setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	attachments[AttachmentKey::DEPTH].setFormat(depth->format);
	attachments[AttachmentKey::DEPTH].setSamples(vk::SampleCountFlagBits::e1);
	attachments[AttachmentKey::DEPTH].setLoadOp(vk::AttachmentLoadOp::eClear);
	attachments[AttachmentKey::DEPTH].setStoreOp(vk::AttachmentStoreOp::eStore);
	attachments[AttachmentKey::DEPTH].setStencilLoadOp(vk::AttachmentLoadOp::eClear);
	attachments[AttachmentKey::DEPTH].setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
	attachments[AttachmentKey::DEPTH].setInitialLayout(vk::ImageLayout::eUndefined);
	attachments[AttachmentKey::DEPTH].setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	rpData->attachments.push_back(attachments[AttachmentKey::COLOR]);
	rpData->attachments.push_back(attachments[AttachmentKey::DEPTH]);

	vk::SubpassDescription subpass;
	vk::AttachmentReference colorRef(0, vk::ImageLayout::eColorAttachmentOptimal);
	vk::AttachmentReference depthRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);
	subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
	subpass.setColorAttachmentCount(1);
	subpass.setPColorAttachments(&colorRef);
	subpass.setPDepthStencilAttachment(&depthRef);
	subpass.setInputAttachmentCount(0);
	subpass.setPInputAttachments(nullptr);
	subpass.setPreserveAttachmentCount(0);
	subpass.setPPreserveAttachments(nullptr);
	subpass.setPResolveAttachments(nullptr);

	rpData->subpasses.push_back(subpass);

	// Subpass dependencies for layout transitions
	std::array<vk::SubpassDependency, 2> dependencies;
	dependencies[0].setSrcSubpass(VK_SUBPASS_EXTERNAL);
	dependencies[0].setDstSubpass(0);
	dependencies[0].setSrcStageMask(vk::PipelineStageFlagBits::eBottomOfPipe);
	dependencies[0].setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	dependencies[0].setSrcAccessMask(vk::AccessFlagBits::eMemoryRead);
	dependencies[0].setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);
	dependencies[0].setDependencyFlags(vk::DependencyFlagBits::eByRegion);

	dependencies[1].setSrcSubpass(0);
	dependencies[1].setDstSubpass(VK_SUBPASS_EXTERNAL);
	dependencies[1].setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	dependencies[1].setDstStageMask(vk::PipelineStageFlagBits::eBottomOfPipe);
	dependencies[1].setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);
	dependencies[1].setDstAccessMask(vk::AccessFlagBits::eMemoryRead);
	dependencies[1].setDependencyFlags(vk::DependencyFlagBits::eByRegion);

	rpData->denpendencies.push_back(dependencies[0]);
	rpData->denpendencies.push_back(dependencies[1]);

	vk::RenderPassCreateInfo renderPassInfo(vk::RenderPassCreateFlags(),
											(uint32_t)rpData->attachments.size(),
											rpData->attachments.data(),
											(uint32_t)rpData->subpasses.size(),
											rpData->subpasses.data(),
											(uint32_t)rpData->denpendencies.size(),
											rpData->denpendencies.data());

	rpData->renderpass = rpData->logicalDevice.createRenderPass(renderPassInfo);

	// TODO: clearValues

	return rpData;
}



} // end namespace graphics