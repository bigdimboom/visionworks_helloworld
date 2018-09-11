#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

#define SURFACE_FORMAT_B8G8R8A8_UNORM "B8G8R8A8_UNORM"
#define SURFACE_FORMAT_R8G8B8A8_UNORM "R8G8B8A8_UNORM"
#define SURFACE_FORMAT_B8G8R8_UNORM "B8G8R8_UNORM"
#define SURFACE_FORMAT_R8G8B8_UNORM "R8G8B8_UNORM"

#define SURFACE_COLOR_SPACE "SRGB_NONLINEAR"

namespace graphics
{

struct SwapChainBuffers
{
	vk::Image image;
	vk::ImageView view;
	vk::Framebuffer frameBuffer;
};

struct SwapChainSupport
{
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
	vk::SurfaceCapabilitiesKHR capabilities;
};

class VulkanSwapChain
{
public:
	vk::Format colorFormat;
	vk::ColorSpaceKHR colorSpace;

	vk::Extent2D actualExtent;
	vk::PresentModeKHR presentMode;
	vk::SurfaceFormatKHR surfaceFormat;
	std::vector<SwapChainBuffers> buffers;

	uint32_t queueGraphicsIndex = 0;
	vk::SwapchainKHR swapChain = nullptr;

	static SwapChainSupport querySwapChainSupport(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);
	static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);



};

} // end namespace graphics