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




} // end namespace graphics