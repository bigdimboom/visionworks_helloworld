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
}

void GuiOverlay::prepareUI()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame((SDL_Window*)d_context->window->intenalPointer());
	ImGui::NewFrame();
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

	// TODO:
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
		vk::CommandBufferAllocateInfo(d_context->device->graphicsCmdPool, vk::CommandBufferLevel::ePrimary, d_context->swapChain->frameCount)
	);
}

void GuiOverlay::render(cam::CameraBase* cam)
{
	ImGui::Render();

	// TODO:
	//uint32_t& frameIndex = d_vulkan.getFrameData().frameIndex;
	auto& cmd = d_commands[0];

	vk::CommandBufferInheritanceInfo info(vk::RenderPass(*d_context->defaultRenderPass));
	cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue, &info));
	{
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
	}
	cmd.end();

	//d_vulkan.addRenderPassCommands(cmd);
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