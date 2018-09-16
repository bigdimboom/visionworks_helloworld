#include "vulkan_texture.h"
#include <assert.h>
#include "vulkan_device.h"
#include "vulkan_helper.h"

namespace graphics
{

VulkanTexture::~VulkanTexture()
{
	assert(device);

	for (const auto& elem : this->d_imageViewPool)
	{
		assert(elem.imageView);
		device->logicalDevice.destroyImageView(elem.imageView);
	}

	for (const auto& elem : this->d_imageSamplerPool)
	{
		assert(elem.sampler);
		device->logicalDevice.destroySampler(elem.sampler);
	}

	if (image)
	{
		device->logicalDevice.destroyImage(image);
		image = nullptr;
	}

	if (memory)
	{
		device->logicalDevice.freeMemory(memory);
		memory = nullptr;
	}
}


void VulkanTexture::bind(vk::DeviceSize offset)
{
	assert(device);
	device->logicalDevice.bindImageMemory(image, memory, offset);
}

vk::ImageView VulkanTexture::acquireImageView(vk::ImageViewType imageViewType,
											  vk::ComponentMapping compMapping,
											  vk::ImageSubresourceRange range,
											  int* indexHandle)
{
	assert(device && image);

	int count = 0;

	for (const auto& elem : d_imageViewPool)
	{
		if (elem.compMapping == compMapping &&
			elem.subresourceRange == range &&
			elem.viewtype == imageViewType)
		{
			assert(elem.imageView);

			if (indexHandle)
			{
				*indexHandle = count;
			}

			return elem.imageView;
		}

		++count;
	}

	vk::ImageViewCreateInfo imageViewCi;
	imageViewCi.setFormat(format);
	imageViewCi.setImage(image);
	imageViewCi.setComponents(compMapping);
	imageViewCi.setSubresourceRange(range);
	imageViewCi.setViewType(imageViewType);

	auto view = device->logicalDevice.createImageView(imageViewCi);

	VulkanImageView vulkan_image_view;
	vulkan_image_view.compMapping = compMapping;
	vulkan_image_view.imageView = view;
	vulkan_image_view.subresourceRange = range;
	vulkan_image_view.viewtype = imageViewType;

	this->d_imageViewPool.push_back(vulkan_image_view);

	if (indexHandle)
	{
		*indexHandle = (int)d_imageViewPool.size() - 1;
	}

	return view;
}

int VulkanTexture::findImageViewHandle(vk::ImageView view)
{
	for (int i = 0; i < (int)d_imageViewPool.size(); ++i)
	{
		if (d_imageViewPool[i].imageView == view)
		{
			return i;
		}
	}
	
	return -1;
}

vk::ImageView VulkanTexture::acquireImageView(int handle)
{
	assert(device && handle < d_imageViewPool.size());
	return d_imageViewPool[handle].imageView;
}

vk::Sampler VulkanTexture::acquireImageSampler(vk::Filter minFilter, 
											   vk::Filter magFilter, 
											   vk::SamplerMipmapMode mipmapMode, 
											   vk::SamplerAddressMode addrMode, 
											   float minLod, float maxLod, float lodBias,
											   vk::CompareOp compareOp)
{
	assert(device);

	for (const auto& elem : d_imageSamplerPool)
	{
		if (elem.samplerInfo.minFilter == minFilter &&
			elem.samplerInfo.magFilter == magFilter &&
			elem.samplerInfo.mipmapMode == mipmapMode &&
			elem.samplerInfo.addressModeU == addrMode &&
			elem.samplerInfo.compareOp == compareOp)
		{
			return elem.sampler;
		}
	}

	vk::SamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.minFilter = minFilter;
	samplerCreateInfo.magFilter = magFilter;
	samplerCreateInfo.mipmapMode = mipmapMode;
	samplerCreateInfo.addressModeU = addrMode;
	samplerCreateInfo.addressModeV = addrMode;
	samplerCreateInfo.addressModeW = addrMode;
	samplerCreateInfo.mipLodBias = lodBias;
	samplerCreateInfo.compareOp = compareOp;
	samplerCreateInfo.minLod = minLod;
	samplerCreateInfo.maxLod = maxLod;

	// Only enable anisotropic filtering if enabled on the devicec
	samplerCreateInfo.maxAnisotropy = device->enabledFeatures.samplerAnisotropy ? device->properties.limits.maxSamplerAnisotropy : 1.0f;
	samplerCreateInfo.anisotropyEnable = device->enabledFeatures.samplerAnisotropy;
	samplerCreateInfo.borderColor = vk::BorderColor::eFloatOpaqueWhite;

	auto sampler = vk::Device(*device).createSampler(samplerCreateInfo);

	VulkanImageSampler samplerData;
	samplerData.sampler = sampler;
	samplerData.samplerInfo = samplerCreateInfo;
	d_imageSamplerPool.push_back(samplerData);

	return sampler;
}

vk::Sampler VulkanTexture::acquireImageSampler(vk::SamplerCreateInfo ci)
{
	assert(device);
	for (const auto& elem : d_imageSamplerPool)
	{
		if (elem.samplerInfo == ci)
		{
			return elem.sampler;
		}
	}


	auto sampler = vk::Device(*device).createSampler(ci);

	VulkanImageSampler samplerData;
	samplerData.sampler = sampler;
	samplerData.samplerInfo = ci;
	d_imageSamplerPool.push_back(samplerData);

	return sampler;
}

vk::Sampler VulkanTexture::acquireImageSampler(int handle)
{
	assert(device && handle < d_imageSamplerPool.size());
	return d_imageSamplerPool[handle].sampler;
}

int VulkanTexture::findImageSamplerHandle(vk::Sampler sampler)
{
	for (int i = 0; i < (int)d_imageSamplerPool.size(); ++i)
	{
		if (d_imageSamplerPool[i].sampler == sampler)
		{
			return i;
		}
	}

	return -1;
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
	texture->device = device;
	texture->format = format;
	texture->imageCreateInfo = imageCreateInfo;
	texture->resolution = resolution;
	texture->usage = usage;
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

	texture->image = vk::Device(*texture->device).createImage(imageCI);
	assert(texture->image);

	auto memreq = vk::Device(*texture->device).getImageMemoryRequirements(texture->image);
	auto typeIndex = VulkanHelper::getMemoryTypeIndex(texture->device->physicalDevice, memreq.memoryTypeBits, memoryProperties);
	texture->memory = vk::Device(*texture->device).allocateMemory(vk::MemoryAllocateInfo(memreq.size, typeIndex));
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