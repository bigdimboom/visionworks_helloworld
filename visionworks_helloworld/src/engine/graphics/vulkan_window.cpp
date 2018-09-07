#include "vulkan_window.h"
#include <assert.h>
#include <iostream>
#include <SDL2/SDL_vulkan.h>

namespace graphics
{

// HELPERS
VulkanWindow::VulkanWindow()
{
}

// MEMBER FUNCTIONS
VulkanWindow::~VulkanWindow()
{
	if (d_window)
	{
		SDL_DestroyWindow(d_window);
		d_window = nullptr;
	}
}

std::shared_ptr<VulkanWindow> VulkanWindow::create(const std::string & title, int width, int height, bool fullScreen, float gamma)
{
	std::shared_ptr<VulkanWindow> window(new VulkanWindow());
	window->d_width = width;
	window->d_height = height;
	window->d_isFullScreen = fullScreen;
	window->d_title = title;

	int creationFlags = SDL_WINDOW_VULKAN;

	if (fullScreen)
	{
		creationFlags |= SDL_WINDOW_FULLSCREEN;
		//creationFlags |= SDL_WINDOW_SHOWN;
	}

	window->d_window = SDL_CreateWindow(window->d_title.c_str(),
										SDL_WINDOWPOS_CENTERED,
										SDL_WINDOWPOS_CENTERED,
										window->d_width,
										window->d_height,
										creationFlags);

	if (!window->d_window)
	{
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "sdl window create error.\n");
		return nullptr;
	}

	unsigned count = 0;
	SDL_Vulkan_GetInstanceExtensions(window->d_window, &count, nullptr);
	window->d_requiredExtensions.resize(count);
	SDL_Vulkan_GetInstanceExtensions(window->d_window, &count, window->d_requiredExtensions.data());

	return window;
}

void VulkanWindow::setFullScreen(bool enabled)
{
	assert(d_window);
	d_isFullScreen = enabled;

	if (d_isFullScreen)
	{
		SDL_SetWindowFullscreen(d_window, SDL_WINDOW_FULLSCREEN);
	}
	else
	{
		SDL_SetWindowFullscreen(d_window, 0);
	}

	SDL_GetWindowSize(d_window, &d_width, &d_height);
}

bool VulkanWindow::isFullScreen() const
{
	assert(d_window);
	return d_isFullScreen;
}

int VulkanWindow::width() const
{
	assert(d_window);
	return d_width;
}

int VulkanWindow::height() const
{
	assert(d_window);
	return d_height;
}

const std::string & VulkanWindow::title() const
{
	assert(d_window);
	return d_title;
}

const std::vector<const char*>& VulkanWindow::requiredExtensions() const
{
	assert(d_window);
	return d_requiredExtensions;
}

float VulkanWindow::gamma() const
{
	assert(d_window);
	return SDL_GetWindowBrightness(d_window);
}

void VulkanWindow::setGamma(float gamma)
{
	assert(d_window);
	SDL_SetWindowBrightness(d_window, gamma);
}

vk::SurfaceKHR VulkanWindow::createVulkanSurface(const vk::Instance & vulkanInstance)
{
	assert(d_window && vulkanInstance);

	SDL_vulkanSurface surface = nullptr;
	if (!SDL_Vulkan_CreateSurface(d_window, vulkanInstance, &surface))
	{
		throw std::runtime_error("failed to create window surface!");
	}
	return surface;
}

}