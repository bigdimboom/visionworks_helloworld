#pragma once
#include <vulkan/vulkan.hpp>
// too much variable for based class?

namespace graphics
{

class VulkanDevice;

struct VulkanImageView
{
	vk::ImageView imageView;
	vk::ImageViewType viewtype;
	vk::ComponentMapping compMapping;
	vk::ImageSubresourceRange subresourceRange;
};

struct VulkanImageSampler
{
	vk::Sampler sampler;
	vk::SamplerCreateInfo samplerInfo;
};

class VulkanTexture
{
public:
	~VulkanTexture();

	std::shared_ptr<VulkanDevice> device;

	vk::Image image;
	vk::Format format;
	vk::Extent3D resolution;
	vk::ImageUsageFlags usage;
	vk::ImageCreateFlags imageCreateInfo;

	vk::DeviceMemory memory;
	vk::MemoryPropertyFlags memoryProperties;
	vk::DeviceSize size = 0; vk::DeviceSize alignment = 0;

	uint32_t mipLevels = 1;
	uint32_t layerCount = 1;

	void bind(vk::DeviceSize offset);

	/**image view can be acquired by this functions, user should not delete vk::imageView, RAII is applied in this object**/
	vk::ImageView acquireImageView(vk::ImageViewType imageViewType,
								   vk::ComponentMapping compMapping,
								   vk::ImageSubresourceRange range,
								   int* indexHandle = nullptr);

	/**@ return -1 means "can't find"**/
	int findImageViewHandle(vk::ImageView view);

	/**@ return nullptr means "can't find"**/
	vk::ImageView acquireImageView(int handle);


	/**image sampler can be acquired by these functions, user should not delete vk::imageView, RAII is applied in this object**/
	vk::Sampler acquireImageSampler(vk::Filter minFilter = vk::Filter::eLinear,
									vk::Filter magFilter = vk::Filter::eLinear,
									vk::SamplerMipmapMode mipmapMode = vk::SamplerMipmapMode::eLinear,
									vk::SamplerAddressMode addrMode = vk::SamplerAddressMode::eClampToEdge,
									float minLod = 0.0f, float maxLod = 1.0, float lodBias = 0.0f,
									vk::CompareOp compareOp = vk::CompareOp::eNever);

	vk::Sampler acquireImageSampler(vk::SamplerCreateInfo ci);
	

	/**@ return nullptr means "can't find"**/
	vk::Sampler acquireImageSampler(int handle);

	/**@ return -1 means "can't find"**/
	int findImageSamplerHandle(vk::Sampler sampler);


	static std::shared_ptr<VulkanTexture> create(
		std::shared_ptr<VulkanDevice> device,
		vk::Format format,
		vk::Extent3D resolution,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags memoryProperties,
		uint32_t mipLevels = 1, uint32_t arrayLayers = 1,
		vk::SampleCountFlagBits sampleCountBit = vk::SampleCountFlagBits::e1,
		vk::ImageCreateFlags imageCreateInfo = {}
	);

	/**static member functions**/
	static vk::ImageType getImageType(const vk::Extent3D & resolution);


private:
	VulkanTexture() {}
	VulkanTexture(const VulkanTexture&) = delete;
	VulkanTexture(VulkanTexture&&) = delete;
	void operator=(const VulkanTexture&) = delete;
	void operator=(VulkanTexture&&) = delete;

	std::vector<VulkanImageView> d_imageViewPool;
	std::vector<VulkanImageSampler> d_imageSamplerPool;
};


} // end namespace graphics