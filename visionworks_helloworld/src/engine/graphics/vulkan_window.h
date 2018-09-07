#pragma once
#include <string>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>
#include <vulkan/vulkan.hpp>

namespace graphics
{

class VulkanWindow
{
public:
	~VulkanWindow();

	// MEMBER FUNCTIONS
	static std::shared_ptr<VulkanWindow> create(const std::string& title, int width, int height, bool fullScreen = false, float gamma = 1.0f);

	void setFullScreen(bool enabled);
	bool isFullScreen() const;

	int width() const;
	int height() const;
	const std::string& title() const;
	const std::vector<const char*>& requiredExtensions() const;
	
	float gamma() const;
	void setGamma(float gamma);

	vk::SurfaceKHR createVulkanSurface(const vk::Instance& vulkanInstance);

private:
	VulkanWindow(const VulkanWindow&) = delete;
	VulkanWindow(VulkanWindow&&) = delete;
	void operator=(const VulkanWindow&) = delete;
	void operator=(VulkanWindow&&) = delete;

	VulkanWindow();

	bool d_isFullScreen = false;
	SDL_Window* d_window = nullptr;
	int d_width = 0, d_height = 0;
	std::string d_title = "Debug";
	std::vector<const char*> d_requiredExtensions;

};

}