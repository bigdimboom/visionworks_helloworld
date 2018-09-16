#include "vulkan_buffer.h"
#include "vulkan_helper.h"
#include "vulkan_device.h"
#include <iostream>

namespace graphics
{

VulkanBuffer::~VulkanBuffer()
{
	assert(vk::Device(*device));
	if (memory)
	{
		vk::Device(*device).freeMemory(memory);
		vk::Device(*device).destroyBuffer(buffer);
	}
}

std::shared_ptr<VulkanBuffer> VulkanBuffer::create(std::shared_ptr<VulkanDevice> device,
												   size_t size, vk::BufferUsageFlags usage,
												   vk::MemoryPropertyFlags memtype)
{
	assert(device);
	assert(size > 0);

	std::shared_ptr<VulkanBuffer> bufferData(new VulkanBuffer());
	bufferData->device = device;
	bufferData->usageFlags = usage;
	bufferData->size = size;
	bufferData->memoryProperties = memtype;

	bufferData->buffer = device->logicalDevice.createBuffer(vk::BufferCreateInfo(
		vk::BufferCreateFlags(), size, usage, vk::SharingMode::eExclusive
	));

	auto memreq = device->logicalDevice.getBufferMemoryRequirements(bufferData->buffer);
	auto typeIndex = VulkanHelper::getMemoryTypeIndex(device->physicalDevice, memreq.memoryTypeBits, memtype);
	bufferData->memory = device->logicalDevice.allocateMemory(vk::MemoryAllocateInfo(memreq.size, typeIndex));
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
	assert(device && memory);
	assert(isHostVisible());
	return vk::Device(*device).mapMemory(memory, offset, size);
}

void VulkanBuffer::unmap()
{
	assert(device && memory);
	vk::Device(*device).unmapMemory(memory);
}

void VulkanBuffer::bind(vk::DeviceSize offset)
{
	assert(device && buffer && memory);
	vk::Device(*device).bindBufferMemory(buffer, memory, offset);
}

void VulkanBuffer::setupDescriptor(vk::DeviceSize size, vk::DeviceSize offset)
{
	assert(device);
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
	assert(device);
	vk::MappedMemoryRange mappedRange = {};
	mappedRange.memory = memory;
	mappedRange.offset = offset;
	mappedRange.size = size;
	vk::Device(*device).flushMappedMemoryRanges(mappedRange);
}

void VulkanBuffer::invalidate(vk::DeviceSize size, vk::DeviceSize offset)
{
	assert(device);
	vk::MappedMemoryRange mappedRange = {};
	mappedRange.memory = memory;
	mappedRange.offset = offset;
	mappedRange.size = size;
	vk::Device(*device).invalidateMappedMemoryRanges(mappedRange);
}

bool VulkanBuffer::isHostVisible() const
{
	assert(device);

	if ((memoryProperties & vk::MemoryPropertyFlagBits::eHostVisible))
	{
		return true;
	}
	return false;
}

/*STATIC MEMBER FUNCTIONS*/
void VulkanBuffer::setBufferMemoryBarrier(vk::CommandBuffer commandBuffer, 
										  vk::PipelineStageFlags srcMask, 
										  vk::PipelineStageFlags tgtMask, 
										  std::vector<BufferTransition> bufferTransitions)
{
	assert(commandBuffer);

	std::vector<vk::BufferMemoryBarrier> buffer_memory_barriers;

	for (auto & elem : bufferTransitions)
	{
		vk::BufferMemoryBarrier tmp;
		tmp.buffer = elem.buffer;
		tmp.dstAccessMask = elem.newAccess;
		tmp.srcAccessMask = elem.currentAccess;
		tmp.srcQueueFamilyIndex = elem.currentQueueFamily;
		tmp.dstQueueFamilyIndex = elem.newQueueFamily;
		tmp.offset = 0;
		tmp.size = VK_WHOLE_SIZE;
		tmp.pNext = nullptr;

		buffer_memory_barriers.push_back(tmp);
	}

	if (buffer_memory_barriers.size() > 0)
	{
		commandBuffer.pipelineBarrier(srcMask, tgtMask, {}, {}, buffer_memory_barriers, {});
	}
}

} // end namespace graphics