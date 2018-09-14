#include "vulkan_depth_resource.h"
#include <assert.h>
#include "vulkan_helper.h"

namespace graphics
{

VulkanDepthResource::~VulkanDepthResource()
{
	assert(logicalDevice);
	if (imageView && deviceMemory && image)
	{
		logicalDevice.destroyImageView(imageView);
		logicalDevice.freeMemory(deviceMemory);
		logicalDevice.destroyImage(image);

		deviceMemory = nullptr;
		imageView = nullptr;
		image = nullptr;
	}
}

bool VulkanDepthResource::hasDepth() const
{
	assert(physicalDevice && logicalDevice);
	return VulkanHelper::isDepthFormat(format);
}

bool VulkanDepthResource::hasStencil() const
{
	assert(physicalDevice && logicalDevice);
	return VulkanHelper::isStencilFormat(format);
}

std::shared_ptr<VulkanDepthResource> VulkanDepthResource::create(const vk::Extent2D& resolution,
																 const vk::PhysicalDevice& physicalDevice,
																 const vk::Device& logicalDevice,
																 uint32_t depthRange)
{
	assert(physicalDevice && logicalDevice);

	auto depthData = std::shared_ptr<VulkanDepthResource>(new VulkanDepthResource());
	depthData->physicalDevice = physicalDevice;
	depthData->logicalDevice = logicalDevice;

	auto getDepthFormat = [](vk::PhysicalDevice gpu) -> vk::Format {
		std::vector<vk::Format> depthFormats =
		{
			vk::Format::eD32SfloatS8Uint,
			vk::Format::eD32Sfloat,
			vk::Format::eD24UnormS8Uint,
			vk::Format::eD16UnormS8Uint,
			vk::Format::eD16Unorm
		};

		for (auto format : depthFormats)
		{
			auto props = gpu.getFormatProperties(format);
			if (props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
			{
				return format;
			}
			throw std::runtime_error("unable to get a useful depth format");
		}

		return vk::Format::eUndefined;
	};

	depthData->format = getDepthFormat(physicalDevice);
	assert(depthData->format != vk::Format::eUndefined);

	// create an image object for depth
	vk::ImageCreateInfo image_create_info(vk::ImageCreateFlags(),
										  vk::ImageType::e2D,
										  depthData->format,
										  vk::Extent3D(resolution.width, resolution.height, depthRange),
										  1, 1,
										  vk::SampleCountFlagBits::e1,
										  vk::ImageTiling::eOptimal,
										  vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc,
										  vk::SharingMode::eExclusive, 0, nullptr,
										  vk::ImageLayout::eUndefined);

	// create image object
	depthData->image = logicalDevice.createImage(image_create_info);
	assert(depthData->image);

	// allocate and bind memory
	vk::MemoryRequirements memReqs = logicalDevice.getImageMemoryRequirements(depthData->image);
	depthData->memoryTypeIndex = VulkanHelper::getMemoryTypeIndex(physicalDevice, memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
	vk::MemoryAllocateInfo depth_mem_alloc_info(memReqs.size, depthData->memoryTypeIndex);
	depthData->deviceMemory = logicalDevice.allocateMemory(depth_mem_alloc_info);
	logicalDevice.bindImageMemory(depthData->image, depthData->deviceMemory, 0);

	// create image view
	vk::ImageAspectFlags flags;
	if (VulkanHelper::isDepthFormat(depthData->format))
	{
		flags |= vk::ImageAspectFlagBits::eDepth;
	}
	if (VulkanHelper::isStencilFormat(depthData->format))
	{
		flags |= vk::ImageAspectFlagBits::eStencil;
	}

	depthData->imageSubresourceRange = vk::ImageSubresourceRange( flags, 0, 1, 0, 1);
	vk::ImageViewCreateInfo image_view_info(vk::ImageViewCreateFlags(),
											depthData->image,
											vk::ImageViewType::e2D,
											depthData->format,
											vk::ComponentMapping(),
											depthData->imageSubresourceRange);

	depthData->imageView = logicalDevice.createImageView(image_view_info);
	return depthData;
}

} // end namespace graphics