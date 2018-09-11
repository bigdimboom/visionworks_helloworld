#pragma once
#include <memory>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

#define WINDOW_DEFAULT_WIDTH 800
#define WINDOW_DEFAULT_HEIGHT 600
#define WINDOW_DEFAULT_TITLE "debug"

namespace graphics
{

class VulkanWindow
{
public:

	~VulkanWindow();

	static std::shared_ptr<VulkanWindow> create(const char* title, int width, int height, bool enableFullScreen = false, float gamma = 1.0);
	
	void setBrightness(float gamma);
	float brightness() const;

	void setFullScreen(bool isFullScreen);
	bool isFullScreen() const;

	int width() const;
	int height() const;

	const std::string& title() const;
	const std::vector<const char*>& requiredVkInstanceExtensions() const;

	vk::SurfaceKHR createVulkanSurface(const vk::Instance& vulkanInstance);

	void destory();

private:
	VulkanWindow();

	VulkanWindow(const VulkanWindow& other) = delete;
	VulkanWindow(VulkanWindow&& other) = delete;
	void operator=(const VulkanWindow& other) = delete;
	void operator=(VulkanWindow&& other) =delete;

	SDL_Window* d_window = nullptr;
	int d_width = 0, d_height = 0;
	bool d_isFullScreen = false;

	float d_windowGamma = 1.0f;
	std::string d_title = "Debug";
	std::vector<const char*> d_vulkanInstanceExtensions;
};


} // end namespace graphics