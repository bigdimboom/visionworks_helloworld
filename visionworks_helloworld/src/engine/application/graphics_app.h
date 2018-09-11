#pragma once
#include <memory>
#include "app_base.h"
#include "../event/safe_event.h"

namespace app
{

class GraphicApp : public app::AppBase
{
public:
	GraphicApp();
	GraphicApp(int argc, const char** argv);
	virtual ~GraphicApp();

	// Inherited via AppBase
	int exec() override;

protected:

	virtual bool init() = 0;
	virtual void update() = 0;
	virtual void render() = 0;
	virtual void cleanup() = 0;

	void quitApp();

	std::shared_ptr<se::dispatcher> dispatcher();

private:
	bool d_isRunning = false;
	std::shared_ptr<se::dispatcher> d_dispath;
};


} // end namespace app