#include "vulkan_pipeline.h"
#include "vulkan_device.h"
#include "vulkan_helper.h"
#include "vulkan_descriptor_set.h"
#include "vulkan_render_pass.h"

#include <iostream>

namespace graphics
{

VulkanShader::~VulkanShader()
{
	assert(logicalDevice);
	if (pipelineShaderInfo.module)
	{
		logicalDevice.destroyShaderModule(pipelineShaderInfo.module);
	}
}

std::shared_ptr<VulkanShader> VulkanShader::createWithSPIRV(vk::Device logicalDevice,
															const char * SPIRVFilePath,
															vk::ShaderStageFlagBits stage)
{
	auto shader = std::shared_ptr<VulkanShader>(new VulkanShader());

	shader->logicalDevice = logicalDevice;
	shader->pipelineShaderInfo.module = VulkanHelper::loadShaderSPIRV(SPIRVFilePath, logicalDevice);
	shader->pipelineShaderInfo.stage = stage;
	shader->pipelineShaderInfo.pName = "main";
	shader->shaderStage = stage;

	return shader;
}

std::shared_ptr<VulkanShader> VulkanShader::createWithGLSL(vk::Device logicalDevice,
														   const char * GLSLFilePath,
														   vk::ShaderStageFlagBits stage)
{
	auto shader = std::shared_ptr<VulkanShader>(new VulkanShader());

	shader->logicalDevice = logicalDevice;
	shader->pipelineShaderInfo.module = VulkanHelper::loadShaderGLSL(GLSLFilePath, logicalDevice, stage);
	shader->pipelineShaderInfo.stage = stage;
	shader->pipelineShaderInfo.pName = "main";
	shader->shaderStage = stage;

	return shader;
}


// pipeline impl.
VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
	d_shaders.clear();

	if (layout)
	{
		assert(logicalDevice);
		logicalDevice.destroyPipelineLayout(layout);
	}

	if (pipeline)
	{
		assert(logicalDevice);
		logicalDevice.destroyPipeline(pipeline);
		pipeline = nullptr;
	}
}

std::shared_ptr<VulkanGraphicsPipeline> VulkanGraphicsPipeline::create(std::shared_ptr<VulkanRenderPass> renderPass)
{
	assert(renderPass->logicalDevice && renderPass);

	auto program = std::shared_ptr<VulkanGraphicsPipeline>(new VulkanGraphicsPipeline());
	program->logicalDevice = renderPass->logicalDevice;
	program->renderPass = renderPass;

	// TODO: set default viewport
	// TODO: tessellationState
	program->setRasterizationState();
	program->setDefaultMultisampleState();
	program->setColorBlendState();
	program->setDepthAndStencilState(true, true, vk::CompareOp::eLess);
	program->useDefaultDynamicStates();

	return program;
}

void VulkanGraphicsPipeline::addShader(std::shared_ptr<VulkanShader> shader)
{
	assert(logicalDevice);
	d_shaders[shader->shaderStage] = shader;
}

void VulkanGraphicsPipeline::addShaderGLSL(const char * path, vk::ShaderStageFlagBits shaderStage)
{
	assert(logicalDevice);
	d_shaders[shaderStage] = VulkanShader::createWithGLSL(logicalDevice, path, shaderStage);
}

void VulkanGraphicsPipeline::addShaderSPIRV(const char * path, vk::ShaderStageFlagBits shaderStage)
{
	assert(logicalDevice);
	d_shaders[shaderStage] = VulkanShader::createWithSPIRV(logicalDevice, path, shaderStage);
}

void VulkanGraphicsPipeline::addVertexInputBinding(const vk::VertexInputBindingDescription & binding)
{
	assert(logicalDevice);
	d_vertexInputStates[binding.binding].binding = binding;
}

void VulkanGraphicsPipeline::setVertexInputAttrib(const vk::VertexInputAttributeDescription & inputVertexAttrib)
{
	assert(logicalDevice);

	if (d_vertexInputStates.count(inputVertexAttrib.binding) == 0)
	{
		std::cerr << "warning: input vertex state binding is not set.\n";
	}
	d_vertexInputStates[inputVertexAttrib.binding].attributes.push_back(inputVertexAttrib);
}

void VulkanGraphicsPipeline::setInputAssemblyState(vk::PrimitiveTopology topology, bool primitive_restart_enable)
{
	assert(logicalDevice);
	inputAssemblyStateInfo.setTopology(topology);
	inputAssemblyStateInfo.setPrimitiveRestartEnable(primitive_restart_enable);
}

void VulkanGraphicsPipeline::addViewport(const vk::Viewport & viewport)
{
	assert(logicalDevice);
	d_viewportInfoData.viewports.push_back(viewport);
}

void VulkanGraphicsPipeline::addScissor(const vk::Rect2D & scissors)
{
	assert(logicalDevice);
	d_viewportInfoData.scissors.push_back(scissors);
}

void VulkanGraphicsPipeline::setViewportAndScissorTestState(const ViewportInfo & info)
{
	assert(logicalDevice);
	d_viewportInfoData = info;
}

void VulkanGraphicsPipeline::setRasterizationState(bool clampDepthEnabled,
												   bool rasterDiscardEnabled,
												   vk::PolygonMode polyMode,
												   vk::CullModeFlags culling,
												   vk::FrontFace frontFace,
												   bool depthBiasEnabled,
												   float depth_bias_constant_factor,
												   float depth_bias_clamp,
												   float depth_bias_slope_factor,
												   float line_width)
{
	assert(logicalDevice);
	rasterInfo = vk::PipelineRasterizationStateCreateInfo(
		vk::PipelineRasterizationStateCreateFlags(),
		clampDepthEnabled, rasterDiscardEnabled,
		polyMode, culling, frontFace,
		depthBiasEnabled, depth_bias_constant_factor,
		depth_bias_clamp, depth_bias_slope_factor,
		line_width
	);
}

void VulkanGraphicsPipeline::setDefaultMultisampleState()
{
	assert(logicalDevice);
	multiSampleInfo = vk::PipelineMultisampleStateCreateInfo(
		vk::PipelineMultisampleStateCreateFlags(),
		vk::SampleCountFlagBits::e1,
		VK_FALSE,
		1.0f,
		nullptr,
		VK_FALSE,
		VK_FALSE);
}

void VulkanGraphicsPipeline::setColorBlendState(bool logicOpEnabled, vk::LogicOp logicOp,
												const std::vector<vk::PipelineColorBlendAttachmentState>& attachments,
												const std::array<float, 4>& blendConstants)
{
	assert(logicalDevice);

	d_blendAttachments = attachments;

	if (d_blendAttachments.empty())
	{
		std::cout << "no attachments specified...use default settings.\n";

		// color blend should not include depth 
		for (int i = 0; i < (int)renderPass->attachments.size(); ++i)
		{
			if (renderPass->attachments[i].finalLayout == vk::ImageLayout::ePresentSrcKHR)
			{
				vk::PipelineColorBlendAttachmentState pipeColorBlendAttachment;

				pipeColorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR |
														   vk::ColorComponentFlagBits::eG |
														   vk::ColorComponentFlagBits::eB |
														   vk::ColorComponentFlagBits::eA);

				pipeColorBlendAttachment.setSrcColorBlendFactor(vk::BlendFactor::eOne);
				pipeColorBlendAttachment.setDstColorBlendFactor(vk::BlendFactor::eZero);
				pipeColorBlendAttachment.setColorBlendOp(vk::BlendOp::eAdd);
				pipeColorBlendAttachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
				pipeColorBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
				pipeColorBlendAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);

				d_blendAttachments.push_back(pipeColorBlendAttachment);
			}
		}
	}

	colorBlendStateInfo = vk::PipelineColorBlendStateCreateInfo(
		vk::PipelineColorBlendStateCreateFlags(), logicOpEnabled, logicOp,
		(uint32_t)d_blendAttachments.size(), d_blendAttachments.data(),
		blendConstants
	);
}

void VulkanGraphicsPipeline::useDefaultDynamicStates()
{
	assert(logicalDevice);
	d_dynamicStates = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor,
		vk::DynamicState::eLineWidth
	};

	dynamicStateinfo.setDynamicStateCount((uint32_t)d_dynamicStates.size());
	dynamicStateinfo.setPDynamicStates(d_dynamicStates.data());
}

void VulkanGraphicsPipeline::setDynamicStates(const std::vector<vk::DynamicState>& dynamicStates)
{
	assert(logicalDevice);
	d_dynamicStates = dynamicStates;
}

void VulkanGraphicsPipeline::clearAllDynamicStates()
{
	assert(logicalDevice);
	d_dynamicStates.clear();
	dynamicStateinfo.setDynamicStateCount(0);
	dynamicStateinfo.setPDynamicStates(nullptr);
}

void VulkanGraphicsPipeline::addDescriptorSet(std::shared_ptr<VulkanDescriptorSet> descriptorSet)
{
	assert(logicalDevice && descriptorSet && logicalDevice == descriptorSet->logicalDevice);
	descriptorSetLayouts.push_back(descriptorSet->layout);
}

void VulkanGraphicsPipeline::clearAllDescriptorSets()
{
	assert(logicalDevice);
	descriptorSetLayouts.clear();
}

void VulkanGraphicsPipeline::setDepthAndStencilState(bool depthTestEnabled,
													 bool depthWriteEnabled,
													 vk::CompareOp depthCompareOp,
													 bool depthBoundsTestEnabled,
													 float minDepthBounds,
													 float maxDepthBounds,
													 bool stencilTestEnabled,
													 vk::StencilOpState frontStencilTestParams,
													 vk::StencilOpState backStencilTestParams)
{
	assert(logicalDevice);
	depthStencilStateInfo = vk::PipelineDepthStencilStateCreateInfo(
		vk::PipelineDepthStencilStateCreateFlags(),
		depthTestEnabled, depthWriteEnabled, depthCompareOp,
		depthBoundsTestEnabled,
		stencilTestEnabled, frontStencilTestParams, backStencilTestParams,
		minDepthBounds, maxDepthBounds
	);
}

void VulkanGraphicsPipeline::build(vk::PipelineCreateFlags ciFlags)
{
	assert(logicalDevice);

	// shader stages
	shaderStages.clear();
	for (const auto& elem : d_shaders)
	{
		shaderStages.push_back(elem.second->pipelineShaderInfo);
	}

	// input binding
	d_vertexBindings.clear();
	d_vertexAttributes.clear();
	for (const auto& elem : d_vertexInputStates)
	{
		d_vertexBindings.push_back(elem.second.binding);
		std::copy(elem.second.attributes.begin(), elem.second.attributes.end(), std::back_inserter(d_vertexAttributes));
	}
	vertexInputStateInfo.setVertexBindingDescriptionCount((uint32_t)d_vertexBindings.size());
	vertexInputStateInfo.setPVertexBindingDescriptions(d_vertexBindings.data());
	vertexInputStateInfo.setVertexAttributeDescriptionCount((uint32_t)d_vertexAttributes.size());
	vertexInputStateInfo.setPVertexAttributeDescriptions(d_vertexAttributes.data());

	// specify view port
	viewportInfo.setViewportCount((uint32_t)d_viewportInfoData.viewports.size());
	viewportInfo.setScissorCount((uint32_t)d_viewportInfoData.scissors.size());
	viewportInfo.setPScissors(d_viewportInfoData.scissors.data());
	viewportInfo.setPViewports(d_viewportInfoData.viewports.data());

#ifdef _DEBUG

	// TODO: check dups in DescriptorSetLayouts

#endif // _DEBUG

	layout = logicalDevice.createPipelineLayout(vk::PipelineLayoutCreateInfo(
		vk::PipelineLayoutCreateFlags(),
		(uint32_t)descriptorSetLayouts.size(), descriptorSetLayouts.empty() ? nullptr : descriptorSetLayouts.data()
	));

	auto ci = vk::GraphicsPipelineCreateInfo(
		ciFlags, (uint32_t)shaderStages.size(), shaderStages.data(),
		&vertexInputStateInfo, &inputAssemblyStateInfo,
		isTessellationStateEnabled ? &tessellationStateInfo : nullptr,
		&viewportInfo,
		&rasterInfo,
		&multiSampleInfo,
		&depthStencilStateInfo,
		&colorBlendStateInfo,
		d_dynamicStates.empty() ? nullptr : &dynamicStateinfo,
		layout, vk::RenderPass(*renderPass)
	);

	pipeline = logicalDevice.createGraphicsPipeline({}, ci);
}

} // end namespace graphics