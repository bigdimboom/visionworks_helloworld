#include "vulkan_swapchain.h"
#include <assert.h>

namespace graphics
{

SwapChainSupport VulkanSwapChain::querySwapChainSupport(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
{
	assert(physicalDevice && surface);

	SwapChainSupport swapChainSupport;
	swapChainSupport.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	swapChainSupport.formats = physicalDevice.getSurfaceFormatsKHR(surface);
	swapChainSupport.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

	return swapChainSupport;
}




} // end namespace graphics