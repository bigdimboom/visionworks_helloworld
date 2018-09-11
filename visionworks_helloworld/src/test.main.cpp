#define SDL_MAIN_HANDLED

#include <iostream>
#include "engine/graphics/vulkan_context.h"


int main(int argc, const char** argv)
{
	graphics::VulkanContext::initialize();
	auto context = graphics::VulkanContext::create(graphics::VulkanContext::createWindow("test", 800, 600));
	auto gpulist = context->physicalDeviceList(context->instance);



	context = nullptr;
	graphics::VulkanContext::shutdown();

	return EXIT_SUCCESS;
}


