#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

namespace graphics
{

class VulkanDevice;

class VulkanBuffer
{
public:
	~VulkanBuffer();

	vk::PhysicalDevice physicalDevice;
	vk::Device logicalDevice;
	vk::Buffer buffer;
	vk::DeviceMemory memory;
	vk::DescriptorBufferInfo descriptor;
	vk::DeviceSize size;
	vk::DeviceSize alignment;

	/** @brief Usage flags to be filled by external source at buffer creation (to query at some later point) */
	vk::BufferUsageFlags usageFlags;
	/** @brief Memory propertys flags to be filled by external source at buffer creation (to query at some later point) */
	vk::MemoryPropertyFlags memoryProperties;

	static std::shared_ptr<VulkanBuffer> create(
		std::shared_ptr<VulkanDevice> device,
		size_t size, vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags memtype);

	/**
	* Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
	*
	* @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete buffer range.
	* @param offset (Optional) Byte offset from beginning
	*/
	void* map(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);

	/**
	* Unmap a mapped memory range
	*
	* @note Does not return a result as vkUnmapMemory can't fail
	*/
	void  unmap();

	/**
	* Attach the allocated memory block to the buffer
	*
	* @param offset (Optional) Byte offset (from the beginning) for the memory region to bind
	*/
	void bind(vk::DeviceSize offset = 0);

	/**
	* Setup the default descriptor for this buffer
	*
	* @param size (Optional) Size of the memory range of the descriptor
	* @param offset (Optional) Byte offset from beginning
	*
	*/
	void setupDescriptor(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);

	/**
	* Copies the specified data to the mapped buffer
	*
	* @param data Pointer to the data to copy
	* @param size Size of the data to copy in machine units
	*
	*/
	void upload(void* data, vk::DeviceSize size);

	/**
	* Flush a memory range of the buffer to make it visible to the device
	*
	* @note Only required for non-coherent memory
	*
	* @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the complete buffer range.
	* @param offset (Optional) Byte offset from beginning
	*/
	void flush(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);

	/**
	* Invalidate a memory range of the buffer to make it visible to the host
	*
	* @note Only required for non-coherent memory
	*
	* @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate the complete buffer range.
	* @param offset (Optional) Byte offset from beginning
	*
	*/
	void invalidate(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);

	/**
	* queryif buffer memory is host visible
	*/
	bool isHostVisible() const;




private:
	VulkanBuffer() {}
	VulkanBuffer(const VulkanBuffer&) = delete;
	VulkanBuffer(VulkanBuffer&&) = delete;
	void operator=(const VulkanBuffer&) = delete;
	void operator=(VulkanBuffer&&) = delete;

	std::shared_ptr<VulkanBuffer> d_pool = nullptr;
	std::vector<std::shared_ptr<VulkanBuffer>> d_subResources;
};

} // end namespace graphics