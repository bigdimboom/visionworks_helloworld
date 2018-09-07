#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vulkan/vulkan.hpp>

#include "src/engine/graphics/vulkan_context.h"
#include "src/engine/graphics/vulkan_window.h"


int main(int argc, const char** argv)
{
	glm::mat4 matrix(1.0);
	std::cout << glm::to_string(matrix) << "\n";

	graphics::VulkanContext::initialize();

	auto window = graphics::VulkanContext::createWindow("test", 800, 600);
	auto instance = graphics::VulkanContext::createVulkanInstance(window, true);
	auto debug = graphics::VulkanContext::createDebugCallback(instance);
	auto surface = graphics::VulkanContext::createVulkanSurface(window, instance);
	auto gpulist = graphics::VulkanContext::getPhysicalDeviceList(instance);


	SDL_Event ev;
	bool isExit = false;

	while (!isExit)
	{
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
			{
				isExit = true;
			}
		}
	}

	instance.destroySurfaceKHR(surface);
	graphics::VulkanContext::destoryDebugCallback(instance, debug);
	instance.destroy();
	window = nullptr;

	graphics::VulkanContext::uninitialize();

	return EXIT_SUCCESS;
}


