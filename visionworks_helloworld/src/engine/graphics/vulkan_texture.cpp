#include "vulkan_texture.h"
#include <assert.h>
#include "vulkan_device.h"
#include "vulkan_helper.h"

namespace graphics
{

VulkanTexture::~VulkanTexture()
{
	assert(logicalDevice && physicalDevice);

	for (const auto& elem : this->d_imageViewPool)
	{
		assert(elem.imageView);
		logicalDevice.destroyImageView(elem.imageView);
	}

	if (image)
	{
		logicalDevice.destroyImage(image);
		image = nullptr;
	}

	if (memory)
	{
		logicalDevice.freeMemory(memory);
		memory = nullptr;
	}
}


void VulkanTexture::bind(vk::DeviceSize offset)
{
	assert(physicalDevice && logicalDevice);
	logicalDevice.bindImageMemory(image, memory, offset);
}

vk::ImageView VulkanTexture::acquireImageView(vk::ImageViewType imageViewType,
											  vk::ComponentMapping compMapping,
											  vk::ImageSubresourceRange range)
{
	assert(physicalDevice && logicalDevice && image);

	for (const auto& elem : d_imageViewPool)
	{
		if (elem.compMapping == compMapping &&
			elem.subresourceRange == range &&
			elem.viewtype == imageViewType)
		{
			assert(elem.imageView);
			return elem.imageView;
		}
	}

	vk::ImageViewCreateInfo imageViewCi;
	imageViewCi.setFormat(format);
	imageViewCi.setImage(image);
	imageViewCi.setComponents(compMapping);
	imageViewCi.setSubresourceRange(range);
	imageViewCi.setViewType(imageViewType);

	auto view = logicalDevice.createImageView(imageViewCi);

	VulkanImageView vulkan_image_view;
	vulkan_image_view.compMapping = compMapping;
	vulkan_image_view.imageView = view;
	vulkan_image_view.subresourceRange = range;
	vulkan_image_view.viewtype = imageViewType;

	this->d_imageViewPool.push_back(vulkan_image_view);

	return view;
}

std::shared_ptr<VulkanTexture> VulkanTexture::create(std::shared_ptr<VulkanDevice> device,
													 vk::Format format,
													 vk::Extent3D resolution,
													 vk::ImageUsageFlags usage,
													 vk::MemoryPropertyFlags memoryProperties,
													 uint32_t mipLevels, uint32_t arrayLayers,
													 vk::SampleCountFlagBits sampleCountBit,
													 vk::ImageCreateFlags imageCreateInfo)
{
	assert(vk::Device(*device));

	auto texture = std::shared_ptr<VulkanTexture>(new VulkanTexture());
	texture->physicalDevice = device->physicalDevice;
	texture->logicalDevice = device->logicalDevice;
	texture->format = format;
	texture->imageCreateInfo = imageCreateInfo;
	texture->resolution = resolution;
	texture->layerCount = arrayLayers;
	texture->mipLevels = mipLevels;
	texture->memoryProperties = memoryProperties;

	vk::ImageCreateInfo imageCI(
		imageCreateInfo,
		getImageType(resolution),
		format,
		resolution,
		mipLevels,
		arrayLayers,
		sampleCountBit,
		vk::ImageTiling::eOptimal,
		usage,
		vk::SharingMode::eExclusive
	);
	imageCI.setInitialLayout(vk::ImageLayout::eUndefined);

	texture->image = texture->logicalDevice.createImage(imageCI);
	assert(texture->image);

	auto memreq = texture->logicalDevice.getImageMemoryRequirements(texture->image);
	auto typeIndex = VulkanHelper::getMemoryTypeIndex(texture->physicalDevice, memreq.memoryTypeBits, memoryProperties);
	texture->memory = texture->logicalDevice.allocateMemory(vk::MemoryAllocateInfo(memreq.size, typeIndex));
	texture->alignment = memreq.alignment;
	texture->size = memreq.size;

	texture->bind(0);

	return texture;
}

vk::ImageType VulkanTexture::getImageType(const vk::Extent3D & resolution)
{
	int count = 0;
	if (resolution.width > 1)
	{
		count++;
	}
	if (resolution.height > 1)
	{
		count++;
	}
	if (resolution.depth > 1)
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