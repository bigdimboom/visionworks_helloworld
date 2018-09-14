#include "vulkan_swapchain.h"
#include <glm/glm.hpp>
#include <iostream>
#include "vulkan_helper.h"

namespace graphics
{

// MEMBER FUNCTIONS
VulkanSwapChain::~VulkanSwapChain()
{
	if (swapChain)
	{
		assert(logicalDevice);
		logicalDevice.destroySwapchainKHR(swapChain);
		std::for_each(buffers.begin(), buffers.end(), [this](SwapChainBuffers& buffer) {
			logicalDevice.destroyImageView(buffer.imageView);
			//logicalDevice.destroyImage(buffer.image);
		});
		swapChain = nullptr;
	}
}

std::shared_ptr<VulkanSwapChain> VulkanSwapChain::create(const vk::PhysicalDevice & physicalDevice,
														 const vk::Device logicalDevice,
														 const vk::SurfaceKHR & surface,
														 const vk::Extent2D & resolution,
														 uint32_t graphicsQueueIndex,
														 vk::Format format,
														 vk::ColorSpaceKHR color,
														 vk::SwapchainKHR oldSwapChain,
														 int frameCount, int imageArrayLayers)
{
	assert(physicalDevice && surface && logicalDevice);

	std::shared_ptr<VulkanSwapChain> data(new VulkanSwapChain());
	data->logicalDevice = logicalDevice;
	data->colorSpace = color;
	data->colorFormat = format;
	data->actualExtent = resolution;
	data->frameCount = frameCount;
	data->swapChain = oldSwapChain;
	data->frameLayers = imageArrayLayers;

	struct SwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::PresentModeKHR> presentModes;
		std::vector<vk::SurfaceFormatKHR> surfaceFormats;
	};

	auto querySwapChainSupport = [](vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) -> SwapChainSupportDetails {
		SwapChainSupportDetails swapChainSupport;

		swapChainSupport.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
		swapChainSupport.surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);
		swapChainSupport.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

		return swapChainSupport;
	};

	auto chooseSwapSurfaceFormat = [data](const std::vector<vk::SurfaceFormatKHR>& availableFormats) -> vk::SurfaceFormatKHR {

		// First check if only one format, VK_FORMAT_UNDEFINED, is available, which would imply that any format is available
		if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
		{
			vk::SurfaceFormatKHR surfaceformat;
			surfaceformat.format = data->colorFormat;
			surfaceformat.colorSpace = data->colorSpace;
			return surfaceformat;
		}

		for (const auto& sFormat : availableFormats)
		{
			if (sFormat.format == data->colorFormat &&
				sFormat.colorSpace == data->colorSpace)
			{
				return sFormat;
			}
		}

		assert(availableFormats.size() > 0 && "no supported format");
		return availableFormats[0];
	};

	auto chooseSwapPresentMode = [frameCount](const std::vector<vk::PresentModeKHR>& availablePresentModes) ->vk::PresentModeKHR {
		vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

		for (const auto& mode : availablePresentModes)
		{
			switch (mode)
			{
			case vk::PresentModeKHR::eMailbox:
				if (frameCount > 2)
				{
					return mode;
				}
				break;
			case vk::PresentModeKHR::eImmediate:
				if (frameCount == 1)
				{
					bestMode = mode;
				}
				break;
			default:
				break;
			}
		}
		return bestMode;
	};

	auto chooseSwapExtent = [&resolution](const vk::SurfaceCapabilitiesKHR capabilities) ->vk::Extent2D {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}

		vk::Extent2D actualExtent;
		actualExtent.setWidth(resolution.width);
		actualExtent.setHeight(resolution.height);
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
		return actualExtent;
	};

	auto selectImageCount = [data](vk::PresentModeKHR present_mode, const vk::SurfaceCapabilitiesKHR& capabilities) -> int {
		if (data->frameCount <= 0)
		{
			if (present_mode == vk::PresentModeKHR::eMailbox)
				return 3;
			if (present_mode == vk::PresentModeKHR::eFifo || present_mode == vk::PresentModeKHR::eFifoRelaxed)
				return 2;
			if (present_mode == vk::PresentModeKHR::eImmediate)
				return 1;
			return 1;
		}

		uint32_t ccc = data->frameCount;
		return glm::clamp(ccc, capabilities.minImageCount, capabilities.maxImageCount);
	};

	auto swapChainSupport = querySwapChainSupport(physicalDevice, surface);
	data->surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.surfaceFormats);
	data->presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	data->frameCount = selectImageCount(data->presentMode, swapChainSupport.capabilities);
	data->actualExtent = chooseSwapExtent(swapChainSupport.capabilities);


	auto old_swapChain = data->swapChain;
	auto limits = physicalDevice.getProperties().limits;

	vk::CompositeAlphaFlagBitsKHR compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	// Simply select the first composite alpha format available
	std::vector<vk::CompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
		vk::CompositeAlphaFlagBitsKHR::ePostMultiplied,
		vk::CompositeAlphaFlagBitsKHR::eInherit
	};
	for (auto& compositeAlphaFlag : compositeAlphaFlags) {
		if (swapChainSupport.capabilities.supportedCompositeAlpha & compositeAlphaFlag) {
			compositeAlpha = compositeAlphaFlag;
			break;
		};
	}

	vk::SwapchainCreateInfoKHR swapChainCreateInfo(
		vk::SwapchainCreateFlagsKHR(),
		surface,
		data->frameCount,
		data->surfaceFormat.format,
		data->surfaceFormat.colorSpace,
		data->actualExtent,
		glm::clamp((uint32_t)data->frameLayers, 1u, limits.maxImageArrayLayers),
		vk::ImageUsageFlagBits::eColorAttachment,
		vk::SharingMode::eExclusive,
		0, nullptr,
		swapChainSupport.capabilities.currentTransform,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		data->presentMode,
		VK_TRUE,
		old_swapChain
	);

	if (!physicalDevice.getSurfaceSupportKHR(graphicsQueueIndex, surface))
	{
		std::cout << "Separate graphics and presenting queues are not supported yet!";
		return nullptr;
	}

	data->presentQueueIndex = graphicsQueueIndex; assert(data->presentQueueIndex != UINT32_MAX);
	swapChainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
	swapChainCreateInfo.setQueueFamilyIndexCount(1);
	swapChainCreateInfo.setPQueueFamilyIndices(&data->presentQueueIndex);
	swapChainCreateInfo.compositeAlpha = compositeAlpha;

	// create swapchain
	data->swapChain = data->logicalDevice.createSwapchainKHR(swapChainCreateInfo);
	data->buffers.resize(data->frameCount);

	auto swapChainImages = data->logicalDevice.getSwapchainImagesKHR(data->swapChain);

	for (int i = 0; i < swapChainImages.size(); ++i)
	{
		data->buffers[i].imageSubresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
		data->buffers[i].image = swapChainImages[i];
		data->buffers[i].imageView =
			data->logicalDevice.createImageView(vk::ImageViewCreateInfo(
			vk::ImageViewCreateFlags(),
			swapChainImages[i],
			vk::ImageViewType::e2D,
			data->surfaceFormat.format,
			vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA),
			data->buffers[i].imageSubresourceRange
			));
	}

	return data;
}

uint32_t VulkanSwapChain::acquireNewFrame(vk::Semaphore sema)
{
	assert(logicalDevice && swapChain);
	auto val = logicalDevice.acquireNextImageKHR(swapChain, UINT64_MAX, sema, nullptr);
#ifdef _DEBUG
	VulkanHelper::check_vk_result(val.result);
#endif // _DEBUG
	return val.value;
}

void VulkanSwapChain::queuePresent(vk::Queue queue, uint32_t imageIndex, vk::Semaphore waitSemaphore)
{
	vk::PresentInfoKHR presentInfo;
	presentInfo.setSwapchainCount(1);
	presentInfo.setPSwapchains(&swapChain);
	presentInfo.setPImageIndices(&imageIndex);

	if (waitSemaphore)
	{
		presentInfo.setWaitSemaphoreCount(1);
		presentInfo.setPWaitSemaphores(&waitSemaphore);
	}

	queue.presentKHR(presentInfo);
}

} // end namespace graphics