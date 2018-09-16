#pragma once
#include <vulkan/vulkan.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace graphics
{

class VulkanShader
{
public:
	~VulkanShader();
	
	vk::Device logicalDevice;
	vk::ShaderStageFlagBits shaderStage;
	vk::PipelineShaderStageCreateInfo pipelineShaderInfo;
	
	static std::shared_ptr<VulkanShader> createWithSPIRV(
		vk::Device logicalDevice,
		const char* SPIRVFilePath,
		vk::ShaderStageFlagBits stage
	);

	static std::shared_ptr<VulkanShader> createWithGLSL(
		vk::Device logicalDevice,
		const char* GLSLFilePath,
		vk::ShaderStageFlagBits stage
	);

private:
	VulkanShader() {}
	VulkanShader(const VulkanShader&) = delete;
	VulkanShader(VulkanShader&&) = delete;
	void operator=(const VulkanShader&) = delete;
	void operator=(VulkanShader&&) = delete;
};

class VulkanPipeline
{
public:
	~VulkanPipeline();

	vk::Device logicalDevice;
	vk::DescriptorPool descriptorPool;

	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
	vk::PipelineVertexInputStateCreateInfo vertexInputeInfo;

	static std::shared_ptr<VulkanPipeline> create(vk::Device logicalDevice, vk::DescriptorPool descriptorPool);

	void addShader(std::shared_ptr<VulkanShader> shader);
	void addShaderGLSL(const char* shaderFilepath, vk::ShaderStageFlagBits shaderStage);
	void addShaderSPIRV(const char* shaderFilepath, vk::ShaderStageFlagBits shaderStage);
	
	// TODO:
	
	
	
	
	void build();


private:
	VulkanPipeline() {}
	VulkanPipeline(const VulkanPipeline&) = delete;
	VulkanPipeline(VulkanPipeline&&) = delete;
	void operator=(const VulkanPipeline&) = delete;
	void operator=(VulkanPipeline&&) = delete;

	std::unordered_map<vk::ShaderStageFlagBits, std::shared_ptr<VulkanShader>> d_shaders;


};

} // end namespace graphics