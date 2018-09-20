#pragma once
#include <vulkan/vulkan.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace graphics
{

class VulkanDescriptorSet;
class VulkanRenderPass;

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

class VulkanGraphicsPipeline
{
public:

	~VulkanGraphicsPipeline();

	vk::Device logicalDevice;
	std::shared_ptr<VulkanRenderPass> renderPass;

	std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;

	// shader states
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

	// vertex binding states
	vk::PipelineVertexInputStateCreateInfo vertexInputStateInfo;

	// input assenmbly data, e.g. triangle or .....
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo;

	// TODO: TessellationState
	bool isTessellationStateEnabled = false;
	vk::PipelineTessellationStateCreateInfo tessellationStateInfo;

	// Depth testing
	vk::PipelineDepthStencilStateCreateInfo depthStencilStateInfo;

	// Viewport
	vk::PipelineViewportStateCreateInfo viewportInfo;

	// raster
	vk::PipelineRasterizationStateCreateInfo rasterInfo;

	// multisample
	vk::PipelineMultisampleStateCreateInfo multiSampleInfo;

	// Blend state
	vk::PipelineColorBlendStateCreateInfo colorBlendStateInfo;

	// dynamic state
	vk::PipelineDynamicStateCreateInfo dynamicStateinfo;

	// graphics pipeline layout
	vk::PipelineLayout layout;

	// graphcis pipeline
	vk::Pipeline pipeline;

	////////////////////////////////////////////////////////////////////////////

	static std::shared_ptr<VulkanGraphicsPipeline> create(std::shared_ptr<VulkanRenderPass> renderPass);

	///////////////////////////////////////////////////////////////////////////

	// shader methods
	void addShader(std::shared_ptr<VulkanShader> shader);
	void addShaderGLSL(const char* shaderFilepath, vk::ShaderStageFlagBits shaderStage);
	void addShaderSPIRV(const char* shaderFilepath, vk::ShaderStageFlagBits shaderStage);

	// vtx methods
	void addVertexInputBinding(const vk::VertexInputBindingDescription& binding);
	void setVertexInputAttrib(const vk::VertexInputAttributeDescription& inputVertexAttrib);
	void setInputAssemblyState(vk::PrimitiveTopology topology, bool primitive_restart_enable = false);

	// viewport methods
	void addViewport(const vk::Viewport& viewport);
	void addScissor(const vk::Rect2D& scissors);
	void setViewportAndScissorTestState(const ViewportInfo& info);

	// Raster states
	void setRasterizationState(bool clampDepthEnabled = false,
							   bool rasterDiscardEnabled = false,
							   vk::PolygonMode polyMode = vk::PolygonMode::eFill,
							   vk::CullModeFlags culling = vk::CullModeFlagBits::eNone,
							   vk::FrontFace frontFace = vk::FrontFace::eCounterClockwise,
							   bool depthBiasEnabled = false,
							   float depth_bias_constant_factor = 0.0f,
							   float depth_bias_clamp = 0.0f,
							   float depth_bias_slope_factor = 0.0f,
							   float line_width = 1.0f);

	// depth and stencil state
	void setDepthAndStencilState(bool depthTestEnabled = true,
								 bool depthWriteEnabled = true,
								 vk::CompareOp depthCompareOp = vk::CompareOp::eLess,
								 bool depthBoundsTestEnabled = false,
								 float minDepthBounds = 0.0f,
								 float maxDepthBounds = 1.0f,
								 bool stencilTestEnabled = false,
								 vk::StencilOpState frontStencilTestParams = {},
								 vk::StencilOpState backStencilTestParams = {});

	// multisample
	void setDefaultMultisampleState();

	// blend state
	void setColorBlendState(bool logicOpEnabled = false, vk::LogicOp logicOp = vk::LogicOp::eCopy,
							const std::vector<vk::PipelineColorBlendAttachmentState>& attachments = {},
							const std::array<float, 4>& blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f });

	// dynamic states
	void useDefaultDynamicStates();
	void setDynamicStates(const std::vector<vk::DynamicState>& dynamicStates);
	void clearAllDynamicStates();

	// descriptor sets
	void addDescriptorSet(std::shared_ptr<VulkanDescriptorSet> descriptorSet);
	void clearAllDescriptorSets();

	void build(vk::PipelineCreateFlags ciFlags = {});



private:
	VulkanGraphicsPipeline() {}
	VulkanGraphicsPipeline(const VulkanGraphicsPipeline&) = delete;
	VulkanGraphicsPipeline(VulkanGraphicsPipeline&&) = delete;
	void operator=(const VulkanGraphicsPipeline&) = delete;
	void operator=(VulkanGraphicsPipeline&&) = delete;

	// shader internal data 
	std::unordered_map<vk::ShaderStageFlagBits, std::shared_ptr<VulkanShader>> d_shaders;
	// vertex input internal data
	std::unordered_map<uint32_t, VertexInput> d_vertexInputStates;
	std::vector<vk::VertexInputBindingDescription> d_vertexBindings;
	std::vector<vk::VertexInputAttributeDescription> d_vertexAttributes;
	// viewport internal data
	ViewportInfo d_viewportInfoData;
	// color blend internal data
	std::vector<vk::PipelineColorBlendAttachmentState> d_blendAttachments;
	// enabled dynamic state
	std::vector<vk::DynamicState> d_dynamicStates;
};

} // end namespace graphics