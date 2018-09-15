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
	descriptor.imageLayout = layout;
	descriptor.imageView = imageView;
	descriptor.sampler = sampler;
}

std::shared_ptr<VulkanTexture> VulkanTexture::create(std::shared_ptr<VulkanDevice> device,
													 vk::Format format, vk::ImageLayout layout,
													 vk::Extent3D resolution,
													 uint32_t mipLevels, uint32_t arrayLayers,
													 vk::ImageUsageFlags usage,
													 vk::MemoryPropertyFlags memoryProperties,
													 vk::SampleCountFlagBits sampleCountBit,
													 vk::ImageCreateFlags imageCreateInfo)
{
	assert(vk::Device(*device));

	auto texture = std::shared_ptr<VulkanTexture>(new VulkanTexture());
	texture->physicalDevice = device->physicalDevice;
	texture->logicalDevice = device->logicalDevice;
	texture->format = format;
	texture->layout = layout;
	texture->resolution = resolution;
	texture->layerCount = arrayLayers;
	texture->mipLevels = mipLevels;
	texture->memoryProperties = memoryProperties;

	vk::ImageCreateInfo imageCI(
		imageCreateInfo,
		getImageType(device, resolution),
		format,
		resolution,
		mipLevels,
		arrayLayers,
		sampleCountBit,
		vk::ImageTiling::eOptimal,
		usage,
		vk::SharingMode::eExclusive
	);

	texture->image = texture->logicalDevice.createImage(imageCI);







	return texture;
}

vk::ImageType VulkanTexture::getImageType(std::shared_ptr<VulkanDevice> device, const vk::Extent3D & resolution)
{
	int count = 0;
	if (resolution.width == 0)
	{
		count++;
	}
	if (resolution.height == 0)
	{
		count++;
	}
	if (resolution.depth == 0)
	{
		count++;
	}

	switch (count)
	{
	case 1:
		return vk::ImageType::e1D;
	case 2:
		return vk::ImageType::e2D;
	case 3:
		return vk::ImageType::e3D;
	default:
		assert(1 != 1 && "incorrect image type");
		break;
	}

	return vk::ImageType::e2D;
}


} // end namespace graphics