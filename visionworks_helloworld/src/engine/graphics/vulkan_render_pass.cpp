#include "vulkan_render_pass.h"
#include "vulkan_swapchain.h"
#include "vulkan_depth_resource.h"
#include "vulkan_helper.h"

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

VulkanRenderPass::~VulkanRenderPass()
{
	if (renderpass)
	{
		assert(logicalDevice);
		logicalDevice.destroyRenderPass(renderpass);
		renderpass = nullptr;
	}

	if (!frameBuffers.empty())
	{
		assert(logicalDevice);

		for (const auto& fb : frameBuffers)
		{
			logicalDevice.destroyFramebuffer(fb);
		}

		frameBuffers.clear();
	}

	if (d_samplerData.sampler)
	{
		assert(logicalDevice);
		logicalDevice.destroySampler(d_samplerData.sampler);
		d_samplerData.sampler = nullptr;
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


	vk::SubpassDescription subpass;
	vk::AttachmentReference colorRef(0, vk::ImageLayout::eColorAttachmentOptimal);
	vk::AttachmentReference depthRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	rpData->attachments.push_back(attachments[AttachmentKey::COLOR]);
	rpData->attachments.push_back(attachments[AttachmentKey::DEPTH]);

	subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
	subpass.setColorAttachmentCount(1u);
	subpass.setPColorAttachments(&colorRef);
	subpass.setPDepthStencilAttachment(&depthRef);
	subpass.setInputAttachmentCount(0);
	subpass.setPInputAttachments(nullptr);
	subpass.setPreserveAttachmentCount(0);
	subpass.setPPreserveAttachments(nullptr);
	subpass.setPResolveAttachments(nullptr);

	rpData->subpasses.push_back(subpass);

	// TODO: it might be bug here.
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

	// render opass
	rpData->renderpass = rpData->logicalDevice.createRenderPass(renderPassInfo);
	assert(rpData->renderpass);


	// frame buffer
	std::vector<vk::ImageSubresourceRange> imageRanges;

	for (auto& elem : swapChain->buffers)
	{
		imageRanges.push_back(elem.imageSubresourceRange);
		imageRanges.push_back(depth->imageSubresourceRange);
	}

	uint32_t maxLayers = 0;
	for (auto& elem : imageRanges)
	{
		if (elem.layerCount >= maxLayers)
		{
			maxLayers = elem.layerCount;
		}
	}

	for (auto& elem : swapChain->buffers)
	{
		std::vector<vk::ImageView> attachmentViews;
		attachmentViews.push_back(elem.imageView);
		attachmentViews.push_back(depth->imageView);

		vk::FramebufferCreateInfo fbci;
		fbci.renderPass = rpData->renderpass;
		fbci.attachmentCount = (uint32_t)attachmentViews.size();
		fbci.pAttachments = attachmentViews.data();
		fbci.width = swapChain->actualExtent.width;
		fbci.height = swapChain->actualExtent.height;
		fbci.layers = maxLayers;
		rpData->frameBuffers.push_back(rpData->logicalDevice.createFramebuffer(fbci));
	}

	// TODO: clearValues

	return rpData;
}

vk::Sampler VulkanRenderPass::acquireSampler(vk::Filter magFilter, 
											 vk::Filter minFilter, 
											 vk::SamplerAddressMode adressMode)
{
	assert(physicalDevice && logicalDevice);

	if (d_samplerData.info.minFilter == magFilter &&
		d_samplerData.info.minFilter == minFilter &&
		d_samplerData.info.addressModeU == adressMode &&
		d_samplerData.info.addressModeU == adressMode &&
		d_samplerData.info.addressModeU == adressMode)
	{
		return d_samplerData.sampler;
	}

	if (d_samplerData.sampler)
	{
		logicalDevice.destroySampler(d_samplerData.sampler);
		d_samplerData.sampler = nullptr;
	}

	d_samplerData.info.magFilter = magFilter;
	d_samplerData.info.minFilter = minFilter;
	d_samplerData.info.mipmapMode = vk::SamplerMipmapMode::eLinear;
	d_samplerData.info.addressModeU = adressMode;
	d_samplerData.info.addressModeV = adressMode;
	d_samplerData.info.addressModeW = adressMode;
	d_samplerData.info.mipLodBias = 0.0f;
	d_samplerData.info.maxAnisotropy = 1.0f;
	d_samplerData.info.minLod = 0.0f;
	d_samplerData.info.maxLod = 1.0f;
	d_samplerData.info.borderColor = vk::BorderColor::eFloatOpaqueWhite;;

	d_samplerData.sampler = logicalDevice.createSampler(d_samplerData.info);

	return d_samplerData.sampler;
}

void VulkanRenderPass::clearAll(float r, float g, float b, float a)
{
	assert(physicalDevice && logicalDevice);
	clearValues[0].color.setFloat32({ r, g, b, a });
	clearValues[1].setDepthStencil(vk::ClearDepthStencilValue(1.0f, 0));
}

} // end namespace graphics