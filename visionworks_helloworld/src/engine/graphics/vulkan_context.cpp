#include "vulkan_context.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

namespace graphics
{

bool VulkanContext::initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "sdl init error.\n");
		return false;
	}

	if (SDL_Vulkan_LoadLibrary(nullptr) != 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "sdl vulkan library init error.\n");
		return false;
	}

	return true;
}

void VulkanContext::uninitialize()
{
	SDL_Vulkan_UnloadLibrary();
	SDL_Quit();
}

}// end namespace graphics