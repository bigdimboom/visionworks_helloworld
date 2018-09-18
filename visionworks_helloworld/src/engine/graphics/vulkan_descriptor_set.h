#pragma once
#include <vulkan/vulkan.hpp>
#include <memory>
#include <unordered_map>

namespace graphics
{

class VulkanDescriptorSet
{
public:
	~VulkanDescriptorSet();

	vk::Device logicalDevice;
	vk::DescriptorPool descriptorPool;
	std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
	vk::DescriptorSetLayout layout;
	vk::DescriptorSet descriptorSet;
	std::vector<vk::WriteDescriptorSet> writeDescriptorSets;

	static std::shared_ptr<VulkanDescriptorSet> create(vk::Device logicalDevice, uint32_t descriptorPoolSize = 1000);

	void addLayoutBinding(uint32_t binding, vk::DescriptorType dtype, vk::ShaderStageFlags shaderStages);
	void addImageBinding(uint32_t binding, const vk::DescriptorImageInfo& imageInfo, vk::DescriptorType type);
	void addBufferBinding(uint32_t binding, const vk::DescriptorBufferInfo& bufferInfo, vk::DescriptorType type);
	void addTexelBufferBinding(uint32_t binding, const vk::BufferView& bufferView , vk::DescriptorType type);

	void build(uint32_t nSets = 1);
	//void copyTo(std::shared_ptr<VulkanDescriptorSet> data)

private:
	VulkanDescriptorSet() {}
	VulkanDescriptorSet(const VulkanDescriptorSet&) = delete;
	VulkanDescriptorSet(VulkanDescriptorSet&&) = delete;
	void operator=(const VulkanDescriptorSet&) = delete;
	void operator=(VulkanDescriptorSet&&) = delete;

	std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> d_layoutBindings;
	std::unordered_map<uint32_t, vk::WriteDescriptorSet> d_writeBindings;
};

} // end namespace graphics