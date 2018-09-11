#include "vulkan_device.h"
#include <assert.h>
#include "vulkan_helper.h"

namespace graphics
{

// HELPERS
void VulkanDevice::operator=(const VulkanDevice & other)
{
	this->physicalDevice = other.physicalDevice;
	this->logicalDevice = other.logicalDevice;
	this->properties = other.properties;
	this->features = other.features;
	this->enabledFeatures = other.enabledFeatures;
	this->memoryProperties = other.memoryProperties;
	this->queueFamilyProperties = other.queueFamilyProperties;
	this->supportedExtensions = other.supportedExtensions;
	this->queueFamilyIndices = other.queueFamilyIndices;

	this->graphicsCmdPool = other.graphicsCmdPool;
}

void VulkanDevice::operator=(VulkanDevice && other)
{
	this->physicalDevice = other.physicalDevice;
	this->logicalDevice = other.logicalDevice;
	this->properties = other.properties;
	this->features = other.features;
	this->enabledFeatures = other.enabledFeatures;
	this->memoryProperties = other.memoryProperties;
	this->queueFamilyProperties = other.queueFamilyProperties;
	this->supportedExtensions = other.supportedExtensions;
	this->queueFamilyIndices = other.queueFamilyIndices;

	this->graphicsCmdPool = other.graphicsCmdPool;
}

void VulkanDevice::createLogicalDevice()
{
	// init family queue index
	assert(physicalDevice);

	float queuePriority[] = { 0.0f };
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

	// 1. setup graphics queue
	if (queueFlags & vk::QueueFlagBits::eGraphics)
	{
		queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), this->queueFamilyIndices.graphics, 1, queuePriority));
	}
	else
	{
		this->queueFamilyIndices.graphics = VK_NULL_HANDLE;
	}

	// 2. setup compute queue
	if (queueFlags & vk::QueueFlagBits::eCompute)
	{
		if (this->queueFamilyIndices.compute != this->queueFamilyIndices.graphics)
		{
			queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), this->queueFamilyIndices.compute, 1, queuePriority));
		}
	}
	else
	{
		this->queueFamilyIndices.compute = this->queueFamilyIndices.graphics;
	}

	// 3. setup compute queue
	if (queueFlags & vk::QueueFlagBits::eTransfer)
	{
		// Dedicated transfer queue
		if (this->queueFamilyIndices.transfer != this->queueFamilyIndices.graphics && 
			this->queueFamilyIndices.transfer != this->queueFamilyIndices.compute)
		{
			queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), this->queueFamilyIndices.transfer, 1, queuePriority));
		}
	}
	else
	{
		this->queueFamilyIndices.transfer = this->queueFamilyIndices.graphics;
	}

	// add swapchain extension
	std::vector<const char*> deviceExtensions;
	for (const auto& elem : supportedExtensions)
	{
		if (elem == std::string(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
		{
			deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		}
	}
	if (deviceExtensions.empty())
	{
		throw std::runtime_error("no swap chain exists, this gpu does not support swap chain");
	}


	// new api uses "debug_util", deprecating VK_EXT_DEBUG_MARKER_EXTENSION_NAME
	//// Enable the debug marker extension if it is present (likely meaning a debugging tool is present)
	//if (extensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
	//{
	//	deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
	//	enableDebugMarkers = true;
	//}

	// creating a logical device
	vk::DeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.setEnabledLayerCount(0); 	// device level layers are deprecated
	deviceCreateInfo.setPpEnabledLayerNames(nullptr); // device level layers are deprecated
	deviceCreateInfo.setEnabledExtensionCount((uint32_t)deviceExtensions.size());
	deviceCreateInfo.setPpEnabledExtensionNames(deviceExtensions.data());
	deviceCreateInfo.setQueueCreateInfoCount((uint32_t)queueCreateInfos.size());
	deviceCreateInfo.setPQueueCreateInfos(queueCreateInfos.data());
	deviceCreateInfo.setPEnabledFeatures(&features);

	logicalDevice = physicalDevice.createDevice(deviceCreateInfo);

	if (logicalDevice)
	{
		graphicsCmdPool = createCmdPool(queueFamilyIndices.graphics);
		assert(graphicsCmdPool);
	}

	enabledFeatures = features;
}

// MEMBER FUNCTIONS
VulkanDevice::VulkanDevice(const vk::PhysicalDevice & device, vk::QueueFlags queueFlgas)
{
	this->physicalDevice = device;

	assert(physicalDevice);

	// Store Properties features, limits and properties of the physical device for later use
	// Device properties also contain limits and sparse properties
	properties = physicalDevice.getProperties();
	// Features should be checked by the examples before using them
	features = physicalDevice.getFeatures();
	// Memory properties are used regularly for creating all kinds of buffers
	memoryProperties = physicalDevice.getMemoryProperties();
	// Queue family properties, used for setting up requested queues upon device creation
	queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
	assert(queueFamilyProperties.size() > 0);
	// type of queue enabled
	this->queueFlags = queueFlgas;
	// Get list of supported device levels extensions
	auto deviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	for (auto ext : deviceExtensions)
	{
		supportedExtensions.push_back(ext.extensionName);
	}
	// get the best family index queue for the device 
	this->queueFamilyIndices.graphics = VulkanHelper::getOptimalFamilyQueueIndex(physicalDevice, vk::QueueFlagBits::eGraphics);
	this->queueFamilyIndices.transfer = VulkanHelper::getOptimalFamilyQueueIndex(physicalDevice, vk::QueueFlagBits::eTransfer);
	this->queueFamilyIndices.compute = VulkanHelper::getOptimalFamilyQueueIndex(physicalDevice, vk::QueueFlagBits::eCompute);

	createLogicalDevice();
}

VulkanDevice::~VulkanDevice()
{
	if (logicalDevice)
	{
		logicalDevice.destroy();
		logicalDevice = nullptr;
	}
}

uint32_t VulkanDevice::getMemoryType(uint32_t typeBits, vk::MemoryPropertyFlags properties)
{
	assert(physicalDevice);
	return VulkanHelper::getMemoryTypeIndex(physicalDevice, typeBits, properties);
}

uint32_t VulkanDevice::getQueueFamilyIndex(vk::QueueFlagBits queueFlags)
{
	assert(physicalDevice);

	switch (queueFlags)
	{
	case vk::QueueFlagBits::eGraphics:
		return queueFamilyIndices.graphics;
	case vk::QueueFlagBits::eCompute:
		return queueFamilyIndices.compute;
	case vk::QueueFlagBits::eTransfer:
		return queueFamilyIndices.transfer;
	default:
		assert(1 != 1 && "unsupported type");
	}
	return 0;
}

vk::CommandPool VulkanDevice::createCmdPool(uint32_t queueFamilyIndex, vk::CommandPoolCreateFlags createFlags)
{
	assert(logicalDevice && physicalDevice);
	return VulkanHelper::createCommandPool(logicalDevice, queueFamilyIndex, createFlags);
}

std::shared_ptr<VulkanDevice> VulkanDevice::create(const vk::PhysicalDevice & device, vk::QueueFlags queueFlags)
{
	return std::shared_ptr<VulkanDevice>(new VulkanDevice(device, queueFlags));
}

} //end namespace graphics