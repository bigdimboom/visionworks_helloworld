#pragma once
#include "irenderable.h"

#include <memory>
#include <SDL2/SDL.h>
#include <vulkan/vulkan.hpp>

#include "../api/imgui.h"
#include "../api/imgui_impl_sdl.h"
#include "../api/imgui_impl_vulkan.h"
#include "../event/safe_event.h"

namespace graphics { class VulkanContext; class VulkanDescriptorSet; }


namespace renderer
{

class GuiOverlay : public IRenderable
{
public:
	GuiOverlay(std::shared_ptr<se::dispatcher> dispatcher, std::shared_ptr<graphics::VulkanContext> context);

	~GuiOverlay();

	void initData() override;
	void startFrame();
	void endFrame();
	vk::CommandBuffer render(uint32_t frameID, vk::RenderPass renderPass) override;
	void cleanup() override;


private:
	std::shared_ptr<se::dispatcher> d_dispatcher;
	std::shared_ptr<graphics::VulkanContext> d_context;

	ImGui_ImplVulkan_InitInfo d_init_info;
	std::vector<vk::CommandBuffer> d_commands;
	std::shared_ptr<graphics::VulkanDescriptorSet> d_descriptorSet;

	static void check_vk_result(VkResult err);

	GuiOverlay(GuiOverlay&&) = delete;
	GuiOverlay(const GuiOverlay&) = delete;
	void operator=(GuiOverlay&&) = delete;
	void operator=(const GuiOverlay &) = delete;
};

} // end namespace renderer