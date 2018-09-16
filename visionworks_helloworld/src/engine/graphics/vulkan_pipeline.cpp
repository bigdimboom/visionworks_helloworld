#include "vulkan_pipeline.h"
#include "vulkan_device.h"
#include "vulkan_helper.h"
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
VulkanPipeline::~VulkanPipeline()
{
	d_shaders.clear();
}

std::shared_ptr<VulkanPipeline> VulkanPipeline::create(vk::Device logicalDevice, vk::DescriptorPool descriptorPool)
{
	auto program = std::shared_ptr<VulkanPipeline>(new VulkanPipeline());
	program->descriptorPool = descriptorPool;
	program->logicalDevice = logicalDevice;
	return program;
}

void VulkanPipeline::addShader(std::shared_ptr<VulkanShader> shader)
{
	assert(logicalDevice && descriptorPool);
	d_shaders[shader->shaderStage] = shader;
}

void VulkanPipeline::addShaderGLSL(const char * path, vk::ShaderStageFlagBits shaderStage)
{
	assert(logicalDevice && descriptorPool);
	d_shaders[shaderStage] = VulkanShader::createWithGLSL(logicalDevice, path, shaderStage);
}

void VulkanPipeline::addShaderSPIRV(const char * path, vk::ShaderStageFlagBits shaderStage)
{
	assert(logicalDevice && descriptorPool);
	d_shaders[shaderStage] = VulkanShader::createWithSPIRV(logicalDevice, path, shaderStage);
}

void VulkanPipeline::addVertexInputBinding(const vk::VertexInputBindingDescription & binding)
{
	assert(logicalDevice && descriptorPool);
	d_vertexInputStates[binding.binding].binding = binding;
}

void VulkanPipeline::setVertexInputAttrib(const vk::VertexInputAttributeDescription & inputVertexAttrib)
{
	assert(logicalDevice && descriptorPool);
	
	if (d_vertexInputStates.count(inputVertexAttrib.binding) == 0)
	{
		std::cerr << "warning: input vertex state binding is not set.\n";
	}
	d_vertexInputStates[inputVertexAttrib.binding].attributes.push_back(inputVertexAttrib);
}

void VulkanPipeline::specifyInputAssemblyState(vk::PrimitiveTopology topology, bool primitive_restart_enable)
{
	assert(logicalDevice && descriptorPool);
	inputAssemblyStateInfo.setTopology(topology);
	inputAssemblyStateInfo.setPrimitiveRestartEnable(primitive_restart_enable);
}

void VulkanPipeline::addViewport(const vk::Viewport & viewport)
{
	assert(logicalDevice && descriptorPool);
	d_viewportInfoData.viewports.push_back(viewport);
}

void VulkanPipeline::addScissor(const vk::Rect2D & scissors)
{
	assert(logicalDevice && descriptorPool);
	d_viewportInfoData.scissors.push_back(scissors);
}

void VulkanPipeline::specifyViewportAndScissorTestState(const ViewportInfo & info)
{
	assert(logicalDevice && descriptorPool);
	d_viewportInfoData = info;
}

void VulkanPipeline::build()
{
	assert(logicalDevice && descriptorPool);

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




}

} // end namespace graphics