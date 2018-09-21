#include "gui_overlay.h"
#include "../graphics/vulkan_context.h"
#include "../graphics/vulkan_descriptor_set.h"

namespace renderer
{
// HELPERS
void GuiOverlay::check_vk_result(VkResult err)
{
	if (err == 0) return;
	printf("VkResult %d\n", err);
	if (err < 0)
		abort();
}

// MEMBERS
GuiOverlay::GuiOverlay(std::shared_ptr<se::dispatcher> dispatcher, std::shared_ptr<graphics::VulkanContext> context)
	: d_dispatcher(dispatcher)
	, d_context(context)
{
	assert(context && dispatcher);

	d_descriptorSet = graphics::VulkanDescriptorSet::create(vk::Device(*context->device), 100);

	d_init_info.Instance = d_context->instance;
	d_init_info.PhysicalDevice = d_context->device->physicalDevice;
	d_init_info.Device = vk::Device(*d_context->device);
	d_init_info.QueueFamily = d_context->device->queueFamilyIndices.graphics;
	d_init_info.Queue = d_context->device->queue(d_init_info.QueueFamily);
	d_init_info.PipelineCache = NULL;
	d_init_info.DescriptorPool = d_descriptorSet->descriptorPool;
	d_init_info.Allocator = NULL;
	d_init_info.CheckVkResultFn = check_vk_result;
}

GuiOverlay::~GuiOverlay()
{
	if (!d_commands.empty())
	{
		vk::Device(*d_context->device).freeCommandBuffers(d_context->device->graphicsCmdPool, d_commands);
		d_commands.clear();

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}
}

void GuiOverlay::startFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame((SDL_Window*)d_context->window->intenalPointer());
	ImGui::NewFrame();
}

void GuiOverlay::endFrame()
{
	ImGui::Render();
}

void GuiOverlay::initData()
{
	d_dispatcher->listen_any([](const se::any_event& ev) {
		if (se::is<SDL_Event>(ev))
		{
			auto real_ev = se::into<SDL_Event>(ev);
			ImGui_ImplSDL2_ProcessEvent(&real_ev);
		}
	});

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplSDL2_InitForVulkan((SDL_Window*)d_context->window->intenalPointer());
	ImGui_ImplVulkan_Init(&d_init_info, vk::RenderPass(*d_context->defaultRenderPass));
	ImGui::StyleColorsDark();

	{
		vk::CommandPool command_pool = d_context->device->graphicsCmdPool;
		auto tmpCmdBuffer = vk::Device(*d_context->device).allocateCommandBuffers(vk::CommandBufferAllocateInfo(command_pool, vk::CommandBufferLevel::ePrimary, 1))[0];

		tmpCmdBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
		ImGui_ImplVulkan_CreateFontsTexture(tmpCmdBuffer);
		tmpCmdBuffer.end();

		vk::SubmitInfo info;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &tmpCmdBuffer;
		d_context->device->queue(d_init_info.QueueFamily).submit(info, nullptr);
		vk::Device(*d_context->device).waitIdle();

		ImGui_ImplVulkan_InvalidateFontUploadObjects();

		tmpCmdBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
		vk::Device(*d_context->device).freeCommandBuffers(command_pool, 1, &tmpCmdBuffer);
	}

	d_commands = vk::Device(*d_context->device).allocateCommandBuffers(
		vk::CommandBufferAllocateInfo(d_context->device->graphicsCmdPool,
		vk::CommandBufferLevel::ePrimary,
		d_context->swapChain->frameCount));
}

vk::CommandBuffer GuiOverlay::render(uint32_t frameID)
{
	assert(frameID >= 0 && frameID < d_commands.size());

	auto& command = d_commands[frameID];

	//vk::CommandBufferInheritanceInfo info(vk::RenderPass(*d_context->defaultRenderPass));

	//static vk::RenderPassBeginInfo renderPassInfo(d_context->defaultRenderPass->renderpass,
	//											  d_context->defaultRenderPass->frameBuffers[frameID],
	//											  vk::Rect2D({}, d_context->swapChain->actualExtent),
	//											  (uint32_t)d_context->defaultRenderPass->clearValues.size(),
	//											  d_context->defaultRenderPass->clearValues.data());

	//renderPassInfo.framebuffer = d_context->defaultRenderPass->frameBuffers[frameID];

	command.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse, nullptr));
	{
		//command.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command);
		//command.endRenderPass();
	}
	command.end();

	return command;
}

void GuiOverlay::cleanup()
{
	vk::Device(*d_context->device).freeCommandBuffers(d_context->device->graphicsCmdPool, d_commands);
	d_commands.clear();

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

} // end namespace renderer