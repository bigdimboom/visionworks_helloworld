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



struct VertexInput
{
	vk::VertexInputBindingDescription binding;
	std::vector<vk::VertexInputAttributeDescription> attributes;
};

struct ViewportInfo
{
	std::vector<vk::Viewport> viewports;
	std::vector<vk::Rect2D> scissors;
};

class VulkanPipeline
{
public:

	~VulkanPipeline();

	vk::Device logicalDevice;
	vk::DescriptorPool descriptorPool;

	// shader states
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

	// vtx states
	vk::PipelineVertexInputStateCreateInfo vertexInputStateInfo;
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo;

	// TODO: TessellationState

	// Viewport
	vk::PipelineViewportStateCreateInfo viewportInfo;

	// multisample
	vk::PipelineMultisampleStateCreateInfo multiSampleInfo;


	static std::shared_ptr<VulkanPipeline> create(vk::Device logicalDevice, vk::DescriptorPool descriptorPool);

	// shader methods
	void addShader(std::shared_ptr<VulkanShader> shader);
	void addShaderGLSL(const char* shaderFilepath, vk::ShaderStageFlagBits shaderStage);
	void addShaderSPIRV(const char* shaderFilepath, vk::ShaderStageFlagBits shaderStage);

	// vtx methods
	void addVertexInputBinding(const vk::VertexInputBindingDescription& binding);
	void setVertexInputAttrib(const vk::VertexInputAttributeDescription& inputVertexAttrib);
	void specifyInputAssemblyState(vk::PrimitiveTopology topology, bool primitive_restart_enable = false);

	// viewport methods
	void addViewport(const vk::Viewport& viewport);
	void addScissor(const vk::Rect2D& scissors);
	void specifyViewportAndScissorTestState(const ViewportInfo& info);

	// multisample
	void specifyMultisampleState(vk::SampleCountFlagBits sample_count,
								 bool per_sample_shading_enable,
								 float min_sample_shading,
								 const vk::SampleMask * sample_masks,
								 bool alpha_to_coverage_enable,
								 bool alpha_to_one_enable);




	void build();


private:
	VulkanPipeline() {}
	VulkanPipeline(const VulkanPipeline&) = delete;
	VulkanPipeline(VulkanPipeline&&) = delete;
	void operator=(const VulkanPipeline&) = delete;
	void operator=(VulkanPipeline&&) = delete;

	std::unordered_map<vk::ShaderStageFlagBits, std::shared_ptr<VulkanShader>> d_shaders;
	std::unordered_map<uint32_t, VertexInput> d_vertexInputStates;

	std::vector<vk::VertexInputBindingDescription> d_vertexBindings;
	std::vector<vk::VertexInputAttributeDescription> d_vertexAttributes;

	ViewportInfo d_viewportInfoData;
};

} // end namespace graphics