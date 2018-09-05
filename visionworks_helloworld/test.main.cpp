#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>


int main(int argc, const char** argv)
{
	glm::mat4 matrix(1.0);
	std::cout << glm::to_string(matrix) << "\n";

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "sdl init error.\n");
		return EXIT_FAILURE;
	}

	if (SDL_Vulkan_LoadLibrary(nullptr) != 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "sdl vulkan library init error.\n");
		return EXIT_FAILURE;
	}

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
	SDL_Vulkan_UnloadLibrary();
	SDL_Quit();


	return EXIT_SUCCESS;
}


