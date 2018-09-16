#include "vulkan_helper.h"
#include <fstream>
#include <filesystem>
#include <assert.h>
#include <SDL2/SDL_vulkan.h>
#include <string>
#include <set>
#include <iostream>

namespace graphics
{

// HELPERS
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			std::cout << "[VULKAN WARNING]: " << pCallbackData->pMessage << "\n";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			std::cout << "[VULKAN ERROR]: " << pCallbackData->pMessage << "]\n";
			break;
		default:
			throw std::runtime_error("unsupported debug msg type");
		}
	}

	return VK_FALSE;
}

vk::Instance VulkanHelper::createVulkanInstance(const std::vector<const char*>& instanceLayers, 
												const std::vector<const char*>& instanceExtensions,
												const char* enginename, const char* appname,
												uint32_t engineVersion, uint32_t appVersion)
{
	auto avaiableExtensionss = vk::enumerateInstanceExtensionProperties();
	auto avaiableLayers = vk::enumerateInstanceLayerProperties();
	auto avaiableExtNames = std::set<std::string>();
	auto avaiableLayerNames = std::set<std::string>();

	for (const auto& elem : avaiableExtensionss)
	{
		avaiableExtNames.insert(elem.extensionName);
	}

	for (const auto& elem : avaiableLayers)
	{
		avaiableLayerNames.insert(elem.layerName);
	}

	for (const auto& input : instanceLayers)
	{
		std::string data(input);
		if (!avaiableLayerNames.count(data))
		{
			std::cout << "unable to find [" << data << "] layer in vulkan loader.\n";
			return nullptr;
		}
	}

	for (const auto& input : instanceExtensions)
	{
		std::string data(input);
		if (!avaiableExtNames.count(data))
		{
			std::cout << "unable to find [" << data << "] extension in vulkan loader.\n";
			return nullptr;
		}
	}

	vk::ApplicationInfo appInfo;
	vk::InstanceCreateInfo instance_create_info;

	appInfo.setApiVersion(VK_API_VERSION_1_1);
	appInfo.setEngineVersion(engineVersion);
	appInfo.setPEngineName(enginename);
	appInfo.setApplicationVersion(appVersion);
	appInfo.setPApplicationName(appname);

	instance_create_info.setPApplicationInfo(&appInfo);
	instance_create_info.setEnabledLayerCount((uint32_t)instanceLayers.size());
	instance_create_info.setPpEnabledLayerNames(instanceLayers.data());
	instance_create_info.setEnabledExtensionCount((uint32_t)instanceExtensions.size());
	instance_create_info.setPpEnabledExtensionNames(instanceExtensions.data());

	return vk::createInstance(instance_create_info);
}

vk::DebugUtilsMessengerEXT VulkanHelper::createDebugCallback(const vk::Instance & instance)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	VkDebugUtilsMessengerEXT callback;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = /*VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | */VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = NULL;

	if (func != nullptr)
	{
		check_vk_result((vk::Result)func(instance, &createInfo, nullptr, &callback));
	}

	return callback;
}

void VulkanHelper::destoryDebugCallback(const vk::Instance & instance, vk::DebugUtilsMessengerEXT & debug)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, debug, nullptr);
		debug = nullptr;
	}
}

vk::SurfaceKHR VulkanHelper::createVulkanSurface(const vk::Instance & instance, SDL_Window * window)
{
	assert(window && instance);

	SDL_vulkanSurface surface = nullptr;
	if (!SDL_Vulkan_CreateSurface(window, instance, &surface))
	{
		throw std::runtime_error("failed to create window surface!");
	}
	return surface;
}

bool VulkanHelper::isFamilyQueueSupported(const vk::PhysicalDevice & physicalDevice, vk::QueueFlagBits queueFamilyBit, std::vector<uint32_t>& queueFamilyIndices)
{
	assert(physicalDevice);

	queueFamilyIndices.clear();

	auto prop = physicalDevice.getQueueFamilyProperties();
	bool supported = false;

	for (uint32_t i = 0; i < (uint32_t)prop.size(); ++i)
	{
		if ((prop[i].queueFlags & queueFamilyBit))
		{
			queueFamilyIndices.push_back(i);
			supported = true;
		}
	}

	return supported;
}

bool VulkanHelper::isIndexDedicatedFamilyQueue(const vk::PhysicalDevice & physicalDevice, uint32_t index, vk::QueueFlagBits queueFamilyBit)
{
	assert(physicalDevice);

	auto prop = physicalDevice.getQueueFamilyProperties();
	bool isDedicated = false;

	assert(index >= 0 && index < prop.size());

	if ((prop[index].queueFlags & queueFamilyBit) && !(prop[index].queueFlags & vk::QueueFlagBits::eGraphics))
	{
		return true;
	}

	return false;
}

uint32_t VulkanHelper::getOptimalFamilyQueueIndex(const vk::PhysicalDevice & physicalDevice, vk::QueueFlagBits queueFlags)
{
	assert(physicalDevice);

	std::vector<uint32_t> indices;

	if (!isFamilyQueueSupported(physicalDevice, queueFlags, indices))
	{
		throw std::runtime_error("no such family queue supported.\n");
	}

	for (auto& index : indices)
	{
		if (isIndexDedicatedFamilyQueue(physicalDevice, index, queueFlags))
		{
			return index;
		}
	}

	return indices[0];
}

uint32_t VulkanHelper::getMemoryTypeIndex(const vk::PhysicalDevice & physicalDevice, uint32_t mem_type_bits, vk::MemoryPropertyFlags flags)
{
	assert(physicalDevice);

	auto memoryProperties = physicalDevice.getMemoryProperties();

	for (int i = 0; i < (int)memoryProperties.memoryTypeCount; ++i)
	{
		if ((mem_type_bits & 1))
		{
			if ((memoryProperties.memoryTypes[i].propertyFlags & flags))
			{
				return i;
			}
		}
		mem_type_bits >>= 1;
	}
	throw std::runtime_error("Could not find a matching memory type");
	return 0;
}

vk::CommandPool VulkanHelper::createCommandPool(const vk::Device & logicalDevice, uint32_t queueFamilyIndex, vk::CommandPoolCreateFlags createFlags)
{
	assert(logicalDevice);

	vk::CommandPoolCreateInfo cmdpoolInfo = {};
	cmdpoolInfo.setQueueFamilyIndex(queueFamilyIndex);
	cmdpoolInfo.setFlags(createFlags);

	return logicalDevice.createCommandPool(cmdpoolInfo);
}

std::vector<vk::CommandBuffer> VulkanHelper::allocateCommandBuffers(const vk::Device & logicalDevice, 
																	const vk::CommandPool & cmdPool, 
																	vk::CommandBufferLevel & level, 
																	uint32_t count)
{
	assert(logicalDevice && cmdPool);
	vk::CommandBufferAllocateInfo allocInfo(cmdPool, level, count);
	return logicalDevice.allocateCommandBuffers(allocInfo);
}

bool VulkanHelper::isDepthFormat(const vk::Format & format)
{
	static std::vector<vk::Format> formats =
	{
		vk::Format::eD16Unorm,
		vk::Format::eX8D24UnormPack32,
		vk::Format::eD32Sfloat,
		vk::Format::eD16UnormS8Uint,
		vk::Format::eD24UnormS8Uint,
		vk::Format::eD32SfloatS8Uint
	};

	return std::find(formats.begin(), formats.end(), format) != std::end(formats);
}

bool VulkanHelper::isStencilFormat(const vk::Format & format)
{
	static std::vector<vk::Format> formats =
	{
		vk::Format::eS8Uint,
		vk::Format::eD16UnormS8Uint,
		vk::Format::eD24UnormS8Uint,
		vk::Format::eD32SfloatS8Uint
	};

	return std::find(formats.begin(), formats.end(), format) != std::end(formats);
}

vk::ShaderModule VulkanHelper::loadShaderSPIRV(const char * fileName, const vk::Device & device)
{
	if (!std::experimental::filesystem::exists(fileName))
	{
		throw std::runtime_error("file does not exist");
	}

	auto readFile = [](const char* filename) -> std::vector<char> {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	};

	auto code = readFile(fileName);
	vk::ShaderModuleCreateInfo info(vk::ShaderModuleCreateFlags(), code.size(), reinterpret_cast<const uint32_t*>(code.data()));
	return device.createShaderModule(info);
}

vk::ShaderModule VulkanHelper::loadShaderGLSL(const char * fileName, const vk::Device & device, vk::ShaderStageFlagBits stage)
{
	if (!std::experimental::filesystem::exists(fileName))
	{
		throw std::runtime_error("file does not exist");
	}

	auto readTextFile = [](const char *fileName)->std::string {
		std::string fileContent;
		std::ifstream fileStream(fileName, std::ios::in);
		if (!fileStream.is_open()) {
			printf("File %s not found\n", fileName);
			return "";
		}
		std::string line = "";
		while (!fileStream.eof()) {
			getline(fileStream, line);
			fileContent.append(line + "\n");
		}
		fileStream.close();
		return fileContent;
	};

	std::string shaderSrc = readTextFile(fileName);
	const char *shaderCode = shaderSrc.c_str();
	size_t size = strlen(shaderCode);
	assert(size > 0);

	vk::ShaderModuleCreateInfo moduleCreateInfo;
	moduleCreateInfo.codeSize = 3 * sizeof(uint32_t) + size + 1;
	std::vector<uint32_t> codeData(moduleCreateInfo.codeSize);
	moduleCreateInfo.pCode = codeData.data();

	// Magic SPV number
	((uint32_t *)moduleCreateInfo.pCode)[0] = 0x07230203;
	((uint32_t *)moduleCreateInfo.pCode)[1] = 0;
	((uint32_t *)moduleCreateInfo.pCode)[2] = (VkShaderStageFlagBits)stage;
	memcpy(((uint32_t *)moduleCreateInfo.pCode + 3), shaderCode, size + 1);

	return device.createShaderModule(moduleCreateInfo);
}

void VulkanHelper::check_vk_result(vk::Result err)
{
	if (err == vk::Result::eSuccess) return;
	printf("VkResult %d\n", err);
	if ((VkResult)err < 0)
		abort();
}

} // end namespace graphics