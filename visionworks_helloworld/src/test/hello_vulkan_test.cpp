#include "hello_vulkan_test.h"

namespace test
{

HelloVulkanTest::HelloVulkanTest(int argc, const char ** argv)
	: app::VulkanGraphicsAppBase(argc, argv)
{

}

HelloVulkanTest::~HelloVulkanTest()
{
}

bool HelloVulkanTest::init()
{
	dispatcher()->listen_any([this](const se::any_event& ev)
	{
		if (se::is<SDL_Event>(ev))
		{
			auto real_ev = se::into<SDL_Event>(ev);

			if (real_ev.type == SDL_KEYDOWN)
			{
				switch (real_ev.key.keysym.sym)
				{
				case SDLK_ESCAPE:
				case SDLK_q:
					quitApp();
					break;
				}
			}
		}
	});

	return true;
}

void HelloVulkanTest::update()
{
}

void HelloVulkanTest::render()
{

}

void HelloVulkanTest::cleanup()
{

}


} //end namespace test