#include "vulkan_swapchain.h"
#include <assert.h>

namespace graphics
{

SwapChainSupport VulkanSwapChain::querySwapChainSupport(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
{
	assert(physicalDevice && surface);

	SwapChainSupport swapChainSupport;
	swapChainSupport.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	swapChainSupport.surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);
	swapChainSupport.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

	auto queueFamiliesProp = physicalDevice.getQueueFamilyProperties();
	for (int i = 0; i < (int)queueFamiliesProp.size(); ++i)
	{
		auto present = physicalDevice.getSurfaceSupportKHR(i, surface);
	}


	return swapChainSupport;
}




} // end namespace graphics