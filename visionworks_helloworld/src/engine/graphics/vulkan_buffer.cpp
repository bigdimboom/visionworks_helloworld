#include "vulkan_buffer.h"
#include "vulkan_helper.h"
#include <iostream>

namespace graphics
{

VulkanBuffer::~VulkanBuffer()
{
	if (memory)
	{
		logicalDevice.freeMemory(memory);
		logicalDevice.destroyBuffer(buffer);
	}
}

std::shared_ptr<VulkanBuffer> VulkanBuffer::create(const vk::PhysicalDevice & physicaldevice,
												   const vk::Device & logicalDevice,
												   size_t size, vk::BufferUsageFlags usage,
												   vk::MemoryPropertyFlags memtype)
{
	assert(physicaldevice && logicalDevice);
	std::shared_ptr<VulkanBuffer> bufferData(new VulkanBuffer());
	bufferData->physicalDevice = physicaldevice;
	bufferData->logicalDevice = logicalDevice;
	bufferData->usageFlags = usage;
	bufferData->size = size;
	bufferData->memoryPropertyFlags = memtype;

	bufferData->buffer = logicalDevice.createBuffer(vk::BufferCreateInfo(
		vk::BufferCreateFlags(), size, usage, vk::SharingMode::eExclusive
	));

	auto memreq = logicalDevice.getBufferMemoryRequirements(bufferData->buffer);
	auto typeIndex = VulkanHelper::getMemoryTypeIndex(physicaldevice, memreq.memoryTypeBits, memtype);
	bufferData->memory = logicalDevice.allocateMemory(vk::MemoryAllocateInfo(memreq.size, typeIndex));
	bufferData->d_pool = nullptr;

	if (!bufferData->memory)
	{
		std::cerr << "unable to allocate memory.\n";
		return nullptr;
	}

	bufferData->bind();
	bufferData->setupDescriptor();

	// TODO:

	return bufferData;
}

void * VulkanBuffer::map(vk::DeviceSize size, vk::DeviceSize offset)
{
	assert(logicalDevice && physicalDevice && memory);
	assert(isHostVisible());
	return logicalDevice.mapMemory(memory, offset, size);
}

void VulkanBuffer::unmap()
{
	assert(logicalDevice && physicalDevice && memory);
	logicalDevice.unmapMemory(memory);
}

void VulkanBuffer::bind(vk::DeviceSize offset)
{
	assert(logicalDevice && buffer && memory);
	logicalDevice.bindBufferMemory(buffer, memory, offset);
}

void VulkanBuffer::setupDescriptor(vk::DeviceSize size, vk::DeviceSize offset)
{
	assert(physicalDevice && logicalDevice);
	descriptor.buffer = buffer;
	descriptor.offset = offset;
	descriptor.range = size;
}

void VulkanBuffer::upload(void * data, vk::DeviceSize size)
{
	void* mapped = map();
	assert(mapped);
	memcpy(mapped, data, size);
	unmap();
}

void VulkanBuffer::flush(vk::DeviceSize size, vk::DeviceSize offset)
{
	assert(physicalDevice && logicalDevice);
	vk::MappedMemoryRange mappedRange = {};
	mappedRange.memory = memory;
	mappedRange.offset = offset;
	mappedRange.size = size;
	logicalDevice.flushMappedMemoryRanges(mappedRange);
}

void VulkanBuffer::invalidate(vk::DeviceSize size, vk::DeviceSize offset)
{
	assert(physicalDevice && logicalDevice);
	vk::MappedMemoryRange mappedRange = {};
	mappedRange.memory = memory;
	mappedRange.offset = offset;
	mappedRange.size = size;
	logicalDevice.invalidateMappedMemoryRanges(mappedRange);
}

bool VulkanBuffer::isHostVisible() const
{
	assert(logicalDevice && physicalDevice);

	if ((memoryPropertyFlags & vk::MemoryPropertyFlagBits::eHostVisible))
	{
		return true;
	}
	return false;
}

} // end namespace graphics