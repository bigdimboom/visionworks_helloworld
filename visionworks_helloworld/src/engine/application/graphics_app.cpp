#include "graphics_app.h"
#include <SDL2/SDL.h>

#include "../graphics/vulkan_buffer.h"
#include "../graphics/vulkan_texture.h"


namespace app
{


VulkanGraphicsAppBase::VulkanGraphicsAppBase()
{
}

VulkanGraphicsAppBase::VulkanGraphicsAppBase(int argc, const char ** argv)
	: AppBase(argc, argv)
	, d_dispath(std::make_shared<se::dispatcher>())
{
}

VulkanGraphicsAppBase::~VulkanGraphicsAppBase()
{
}

int VulkanGraphicsAppBase::exec()
{
	if (!graphics::VulkanContext::initialize())
	{
		return EXIT_FAILURE;
	}

	SDL_Event ev;
	d_dispath->add_event<SDL_Event>();

	auto window = graphics::VulkanContext::createWindow(WINDOW_DEFAULT_TITLE, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT);

	d_context = graphics::VulkanContext::create(window);

	d_isRunning = init();

	while (d_isRunning)
	{
		while (SDL_PollEvent(&ev))
		{
			d_dispath->trigger(ev);

			if (ev.type == SDL_QUIT)
			{
				d_isRunning = false;
				break;
			}
		}

		update();
		render();
	}

	vk::Device(*d_context->device).waitIdle();

	cleanup();
	d_dispath->cleanup();

	graphics::VulkanContext::shutdown();

	return EXIT_SUCCESS;
}

void VulkanGraphicsAppBase::quitApp()
{
	d_isRunning = false;
}

std::shared_ptr<graphics::VulkanContext> VulkanGraphicsAppBase::vulkanContext()
{
	return d_context;
}

std::shared_ptr<se::dispatcher> VulkanGraphicsAppBase::dispatcher()
{
	return d_dispath;
}


} // end namespace app