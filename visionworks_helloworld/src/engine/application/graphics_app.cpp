#include "graphics_app.h"
#include <SDL2/SDL.h>

namespace app
{


GraphicApp::GraphicApp()
	: AppBase()
	, d_dispath(std::make_shared<se::dispatcher>())
{
}

GraphicApp::GraphicApp(int argc, const char ** argv)
	: AppBase(argc, argv)
	, d_dispath(std::make_shared<se::dispatcher>())
{
}

GraphicApp::~GraphicApp()
{
}

int GraphicApp::exec()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return EXIT_FAILURE;
	}



	SDL_Quit();

	return 0;
}

void GraphicApp::quitApp()
{
	d_isRunning = false;
}

std::shared_ptr<se::dispatcher> GraphicApp::dispatcher()
{
	return std::shared_ptr<se::dispatcher>();
}

} // end namespace app