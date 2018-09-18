#pragma once
#include <vulkan/vulkan.hpp>

namespace graphics
{

class VulkanSwapChain
{
public:
	~VulkanSwapChain();

	struct SwapChainBuffers
	{
		vk::Image image;
		vk::ImageView imageView;
		vk::ImageSubresourceRange imageSubresourceRange;
	};

	vk::Format colorFormat = vk::Format::eB8G8R8A8Unorm;
	vk::ColorSpaceKHR colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;

	int frameCount = 0;
	int frameLayers = 1;

	uint32_t presentQueueIndex = UINT32_MAX;

	vk::Extent2D actualExtent;
	vk::PresentModeKHR presentMode;
	vk::SurfaceFormatKHR surfaceFormat;

	vk::Device logicalDevice = nullptr;
	std::vector<SwapChainBuffers> buffers;
	vk::SwapchainKHR swapChain = nullptr;

	// CREATE FUNCTIONS
	static std::shared_ptr<VulkanSwapChain> create(const vk::PhysicalDevice & physicalDevice,
												   const vk::Device logicalDevice,
												   const vk::SurfaceKHR & surface,
												   const vk::Extent2D& resolution,
												   uint32_t graphicsQueueIndex,
												   vk::Format format = vk::Format::eB8G8R8A8Unorm,
												   vk::ColorSpaceKHR color = vk::ColorSpaceKHR::eSrgbNonlinear,
												   vk::SwapchainKHR oldSwapChain = nullptr,
												   int frameCount = 3, int imageArrayLayers = 1);

	// MEMBER FUNCTIONS
	uint32_t acquireNewFrame(vk::Semaphore sema, vk::Fence fence = nullptr);
	void queuePresent(vk::Queue queue, uint32_t imageIndex, vk::Semaphore waitSemaphore = nullptr);

private:
	VulkanSwapChain(const VulkanSwapChain&) = delete;
	VulkanSwapChain(VulkanSwapChain&&) = delete;
	void operator=(const VulkanSwapChain&) = delete;
	void operator=(VulkanSwapChain&&) = delete;

	VulkanSwapChain() {};


};

} // end namespace graphics