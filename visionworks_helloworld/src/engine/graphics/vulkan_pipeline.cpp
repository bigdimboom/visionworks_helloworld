#include "vulkan_pipeline.h"
#include "vulkan_device.h"
#include "vulkan_helper.h"

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

void VulkanPipeline::build()
{
	assert(logicalDevice && descriptorPool);

	shaderStages.clear();
	for (const auto& elem : d_shaders)
	{
		shaderStages.push_back(elem.second->pipelineShaderInfo);
	}

	// TODO:


}

} // end namespace graphics