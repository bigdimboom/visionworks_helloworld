#include "vulkan_window.h"
#include <assert.h>

namespace graphics
{

VulkanWindow::VulkanWindow()
{
}

VulkanWindow::~VulkanWindow()
{
	destory();
}

std::shared_ptr<VulkanWindow> VulkanWindow::create(const char * title, int width, int height, bool enableFullScreen, float gamma)
{
	auto data = /*std::make_shared<Window>();*/ std::shared_ptr<VulkanWindow>(new VulkanWindow());

	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);

	int flags = SDL_WINDOW_VULKAN /*| SDL_WINDOW_RESIZABLE*/; // no resizing for now

	if (enableFullScreen)
	{
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}

	data->d_isFullScreen = enableFullScreen;
	data->d_width = width;
	data->d_height = height;
	data->d_title = title;
	data->d_windowGamma = gamma;

	data->d_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED , SDL_WINDOWPOS_CENTERED, width, height, flags);
	
	if (!data->d_window)
	{
		throw std::runtime_error("unable to create window");
		return nullptr;
	}

	data->setBrightness(gamma);

	if (enableFullScreen)
	{
		SDL_GetWindowSize(data->d_window, &width, &height);
	}

	unsigned extensions_count = 0;
	SDL_Vulkan_GetInstanceExtensions(data->d_window, &extensions_count, nullptr);
	data->d_vulkanInstanceExtensions.resize(extensions_count);
	SDL_Vulkan_GetInstanceExtensions(data->d_window, &extensions_count, data->d_vulkanInstanceExtensions.data());

	return data;
}

void VulkanWindow::setBrightness(float gamma)
{
	assert(d_window);
	if (SDL_SetWindowBrightness(d_window, gamma) != 0)
	{
		throw std::runtime_error("Unable set gamma");
	}
	d_windowGamma = gamma;
}

float VulkanWindow::brightness() const
{
	assert(d_window);
	return d_windowGamma;
}

void VulkanWindow::setFullScreen(bool isFullScreen)
{
	assert(d_window);
	d_isFullScreen = isFullScreen;
	if (SDL_SetWindowFullscreen(d_window, SDL_WINDOW_FULLSCREEN) != 0)
	{
		throw std::runtime_error("full screen error");
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

const std::string & VulkanWindow::surfaceColor() const
{
	assert(d_window);
	return d_surfaceColor;
}

const std::string & VulkanWindow::surfaceFormat() const
{
	assert(d_window);
	return d_surfaceFormat;
}

const std::vector<const char*>& VulkanWindow::requiredVkInstanceExtensions() const
{
	assert(d_window);
	return d_vulkanInstanceExtensions;
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

void VulkanWindow::destory()
{
	if (d_window)
	{
		SDL_DestroyWindow(d_window);
	}
}

} // end namespace graphics