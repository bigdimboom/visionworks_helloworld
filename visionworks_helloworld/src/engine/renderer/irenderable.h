#pragma once
#include <vulkan/vulkan.hpp>

namespace renderer
{

class IRenderable
{
public:
	virtual ~IRenderable() {}

	virtual void initData() = 0;
	virtual vk::CommandBuffer render(uint32_t frameID, vk::RenderPass renderPass) = 0;
	virtual void cleanup() = 0;
};

} // end namespace renderer