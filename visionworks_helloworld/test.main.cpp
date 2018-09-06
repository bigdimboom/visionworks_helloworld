#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vulkan/vulkan.hpp>

#include "src/engine/graphics/vulkan_context.h"


int main(int argc, const char** argv)
{
	glm::mat4 matrix(1.0);
	std::cout << glm::to_string(matrix) << "\n";

	graphics::VulkanContext::initialize();

	auto window = SDL_CreateWindow(
		"test",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800, 600,
		SDL_WINDOW_VULKAN
	);

	if (!window)
	{
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "sdl window create error.\n");
		return EXIT_FAILURE;
	}

	auto instanceExtensionProp = vk::enumerateInstanceExtensionProperties();
	auto instanceLayerProp = vk::enumerateInstanceLayerProperties();
	auto instanceVersion = vk::enumerateInstanceVersion();

	unsigned extensionCount = 0;
	std::vector<const char*> vulkanInstanceExtensions;
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
	vulkanInstanceExtensions.resize(extensionCount);
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, vulkanInstanceExtensions.data());

	std::vector<const char*> vulkanInstanceLayers;

	vulkanInstanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	vulkanInstanceExtensions.push_back("VK_EXT_debug_report");
	vulkanInstanceExtensions.push_back("VK_EXT_debug_utils");


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

	SDL_DestroyWindow(window);


	graphics::VulkanContext::uninitialize();

	return EXIT_SUCCESS;
}


