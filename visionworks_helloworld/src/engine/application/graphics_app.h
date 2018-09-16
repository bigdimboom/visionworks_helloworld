#pragma once
#include <memory>
#include "app_base.h"
#include "../event/safe_event.h"

#include "../graphics/vulkan_context.h"

namespace app
{

class VulkanGraphicsAppBase : public app::AppBase
{
public:
	VulkanGraphicsAppBase();
	VulkanGraphicsAppBase(int argc, const char** argv);
	virtual ~VulkanGraphicsAppBase();

	// Inherited via AppBase
	int exec() override;

protected:

	virtual bool init() = 0;
	virtual void update() = 0;
	virtual void render() = 0;
	virtual void cleanup() = 0;

	void quitApp();
	std::shared_ptr<graphics::VulkanContext> vulkanContext();
	std::shared_ptr<se::dispatcher> dispatcher();

private:
	bool d_isRunning = false;
	std::shared_ptr<graphics::VulkanContext> d_context;
	std::shared_ptr<se::dispatcher> d_dispath;
};


} // end namespace app