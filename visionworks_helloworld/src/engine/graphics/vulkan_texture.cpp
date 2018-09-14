#include "vulkan_texture.h"
#include <assert.h>
#include "vulkan_device.h"

namespace graphics
{

VulkanTexture::~VulkanTexture()
{
	assert(logicalDevice && physicalDevice);
	
	if (imageView)
	{
		logicalDevice.destroyImageView(imageView);
		imageView = nullptr;
	}

	if (image)
	{
		logicalDevice.destroyImage(image);
		image = nullptr;
	}

	if (sampler)
	{
		logicalDevice.destroySampler(sampler);
		sampler = nullptr;
	}

	if (memory)
	{
		logicalDevice.freeMemory(memory);
		memory = nullptr;
	}
}

void VulkanTexture::updateDescriptor()
{
	assert(logicalDevice && physicalDevice);
	assert(imageView && sampler);
	descriptor.imageLayout = imageLayout;
	descriptor.imageView = imageView;
	descriptor.sampler = sampler;
}

std::shared_ptr<VulkanTexture> VulkanTexture::create(std::shared_ptr<VulkanDevice> device, 
													 vk::Extent3D resolution, 
													 vk::Format format, 
													 vk::ImageUsageFlags usage, 
													 vk::ImageType imageType,
													 uint32_t mipLevels, 
													 uint32_t imageLayers,
													 vk::MemoryPropertyFlags memoryPropertyFlags,
													 vk::ComponentMapping componentMapping,
													 vk::ImageSubresourceRange imageSubresourceRange,
													 vk::ImageTiling tiling, 
													 vk::SampleCountFlagBits samplecount, 
													 vk::ImageCreateFlags ciflags)
{
	assert(device);
	auto texture = std::shared_ptr<VulkanTexture>(new VulkanTexture());

	texture->physicalDevice = device->physicalDevice;
	texture->logicalDevice = device->logicalDevice;

	vk::ImageCreateInfo ci(
		ciflags,
		imageType,
		format,
		resolution,
		mipLevels,
		imageLayers,
		samplecount,
		tiling,
		usage,
		vk::SharingMode::eExclusive
	);
	ci.setInitialLayout(vk::ImageLayout::eUndefined);

	texture->image = texture->logicalDevice.createImage(ci);
	texture->imageUsage = usage;
	texture->format = format;
	texture->layerCount = imageLayers;
	texture->mipLevels = mipLevels;

	// TODO:
	vk::ImageViewCreateInfo imageViewCi;
	imageViewCi.setFormat(texture->format);
	


	return texture;
}

} // end namespace graphics