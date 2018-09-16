#pragma once
#include "../engine/application/graphics_app.h"
#include <memory>


namespace test
{


class HelloVulkanTest : public app::VulkanGraphicsAppBase
{
public:
	HelloVulkanTest(int argc, const char** argv);
	~HelloVulkanTest();

	bool init() override;

	void update() override;

	void render() override;

	void cleanup() override;

private:



};




} // end namespace test