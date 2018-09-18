#include "vulkan_descriptor_set.h"

namespace graphics
{


VulkanDescriptorSet::~VulkanDescriptorSet()
{
	if (descriptorSet)
	{
		logicalDevice.freeDescriptorSets(descriptorPool, { descriptorSet });
		descriptorSet = nullptr;
	}

	if (layout)
	{
		assert(logicalDevice);
		logicalDevice.destroyDescriptorSetLayout(layout);
		layout = nullptr;
	}

	if (descriptorPool)
	{
		assert(logicalDevice);
		logicalDevice.destroyDescriptorPool(descriptorPool);
		descriptorPool = nullptr;
	}
}

std::shared_ptr<VulkanDescriptorSet> VulkanDescriptorSet::create(vk::Device logicalDevice, uint32_t size)
{
	assert(logicalDevice);

	std::vector<vk::DescriptorPoolSize> pool_size =
	{

		{ vk::DescriptorType::eSampler, size },
		{ vk::DescriptorType::eCombinedImageSampler, size },
		{ vk::DescriptorType::eSampledImage, size },
		{ vk::DescriptorType::eStorageImage, size },
		{ vk::DescriptorType::eUniformTexelBuffer, size },
		{ vk::DescriptorType::eStorageTexelBuffer, size },
		{ vk::DescriptorType::eUniformBuffer, size },
		{ vk::DescriptorType::eStorageBuffer, size },
		{ vk::DescriptorType::eUniformBufferDynamic, size },
		{ vk::DescriptorType::eStorageBufferDynamic, size },
		{ vk::DescriptorType::eInputAttachment, size }

	};

	auto ds = std::shared_ptr<VulkanDescriptorSet>(new VulkanDescriptorSet());

	ds->logicalDevice = logicalDevice;
	vk::DescriptorPoolCreateInfo create_info(
		vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		(uint32_t)pool_size.size() * size,
		(uint32_t)pool_size.size(),
		pool_size.data()
	);
	ds->descriptorPool = ds->logicalDevice.createDescriptorPool(create_info);

	// TODO:




	return ds;
}

void VulkanDescriptorSet::addLayoutBinding(uint32_t binding, vk::DescriptorType dtype, vk::ShaderStageFlags shaderStages)
{
	assert(logicalDevice && descriptorPool);
	d_layoutBindings[binding] = vk::DescriptorSetLayoutBinding(binding, dtype, 1, shaderStages);
}

void VulkanDescriptorSet::addImageBinding(uint32_t binding, const vk::DescriptorImageInfo & imageInfo, vk::DescriptorType type)
{
	assert(type == vk::DescriptorType::eCombinedImageSampler ||
		   type == vk::DescriptorType::eInputAttachment ||
		   type == vk::DescriptorType::eSampledImage ||
		   type == vk::DescriptorType::eSampler ||
		   type == vk::DescriptorType::eStorageImage);

	d_writeBindings[binding] = vk::WriteDescriptorSet(
	{},
		binding, 0, 1,
		type,
		&imageInfo,
		nullptr,
		nullptr
	);
}

void VulkanDescriptorSet::addBufferBinding(uint32_t binding, const vk::DescriptorBufferInfo & bufferInfo, vk::DescriptorType type)
{
	assert(type == vk::DescriptorType::eStorageBuffer ||
		   type == vk::DescriptorType::eStorageBufferDynamic ||
		   type == vk::DescriptorType::eUniformBuffer ||
		   type == vk::DescriptorType::eUniformBufferDynamic);

	d_writeBindings[binding] = vk::WriteDescriptorSet(
	{},
		binding, 0, 1,
		type,
		nullptr,
		&bufferInfo,
		nullptr
	);
}

void VulkanDescriptorSet::addTexelBufferBinding(uint32_t binding, const vk::BufferView & bufferView, vk::DescriptorType type)
{
	assert(type == vk::DescriptorType::eStorageTexelBuffer ||
		   type == vk::DescriptorType::eUniformTexelBuffer);

	d_writeBindings[binding] = vk::WriteDescriptorSet(
	{},
		binding, 0, 1,
		type,
		nullptr,
		nullptr,
		&bufferView
	);

}

void VulkanDescriptorSet::build(uint32_t nSets)
{
	assert(logicalDevice && descriptorPool);

	layoutBindings.clear();
	for (const auto& elem : d_layoutBindings)
	{
		layoutBindings.push_back(elem.second);
	}

	layout = logicalDevice.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo(
		vk::DescriptorSetLayoutCreateFlags(),
		(uint32_t)layoutBindings.size(), layoutBindings.data()
	));

	descriptorSet = logicalDevice.allocateDescriptorSets(vk::DescriptorSetAllocateInfo(descriptorPool, 1, &layout))[0];

	writeDescriptorSets.clear();
	for (auto& elem : d_writeBindings)
	{
		elem.second.dstSet = descriptorSet;
		writeDescriptorSets.push_back(elem.second);
	}

	logicalDevice.updateDescriptorSets(writeDescriptorSets, {});
}

}