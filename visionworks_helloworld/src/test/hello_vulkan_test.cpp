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
	dispatcher()->listen_any([this](const se::any_event& ev) {
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		static bool mouse_on = false;
		static bool firstMouse = true;

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
				case SDLK_w:
					d_freeCam->translateForward(-0.05f * time);
					break;
				case SDLK_s:
					d_freeCam->translateForward(0.05f * time);
					break;
				case SDLK_a:
					d_freeCam->translateRight(-0.05f * time);
					break;
				case SDLK_d:
					d_freeCam->translateRight(0.05f * time);
					break;
				}
			}
			else
			{
				switch (real_ev.type) {

				case SDL_MOUSEMOTION:
					if (mouse_on) cameraMotion((float)real_ev.motion.x, (float)real_ev.motion.y, firstMouse);
					break;
				case SDL_MOUSEBUTTONDOWN:
					switch (real_ev.button.button)
					{
					case SDL_BUTTON_RIGHT:
						mouse_on = true;
						SDL_SetRelativeMouseMode(SDL_TRUE);
						break;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					switch (real_ev.button.button)
					{
					case SDL_BUTTON_RIGHT:
						SDL_SetRelativeMouseMode(SDL_FALSE);
						firstMouse = true;
						mouse_on = false;
						break;
					}
					break;
				}
			}
		}
	});


	d_mesh.vertices =
	{
		// front
	{ { -1.0, -1.0,  1.0 },{ 1.0, 0.0, 0.0, 1.0 } },
	{ { 1.0, -1.0,  1.0 },{ 0.0, 1.0, 0.0, 1.0 } },
	{ { 1.0,  1.0,  1.0 },{ 0.0, 0.0, 1.0, 1.0 } },
	{ { -1.0,  1.0,  1.0 },{ 1.0, 1.0, 1.0, 0.0 } },
	// back		  		
	{ { -1.0, -1.0, -1.0 },{ 1.0, 0.0, 0.0, 1.0 } },
	{ { 1.0, -1.0, -1.0 },{ 0.0, 1.0, 0.0, 1.0 } },
	{ { 1.0,  1.0, -1.0 },{ 0.0, 0.0, 1.0, 1.0 } },
	{ { -1.0,  1.0, -1.0 },{ 1.0, 1.0, 1.0, 1.0 } }
	};

	std::for_each(d_mesh.vertices.begin(), d_mesh.vertices.end(), [](Vertex& v) {   v.position /= 2.0; });

	d_mesh.indices =
	{
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3,
	};

	// VBO
	uint32_t vertex_size = sizeof(Vertex) * (uint32_t)d_mesh.vertices.size();
	uint32_t index_size = sizeof(uint32_t) * (uint32_t)d_mesh.indices.size();
	uint32_t stage_size = vertex_size >= index_size ? vertex_size : index_size;
	auto transferCmdPool = vulkanContext()->device->createCmdPool(vulkanContext()->device->queueFamilyIndices.transfer);
	auto transferCmd = vk::Device(*vulkanContext()->device).allocateCommandBuffers(
		vk::CommandBufferAllocateInfo(transferCmdPool, vk::CommandBufferLevel::ePrimary, 1)
	)[0];

	vk::SubmitInfo submitInfo;
	submitInfo.setCommandBufferCount(1);
	submitInfo.setPCommandBuffers(&transferCmd);

	auto stageBuff = graphics::VulkanBuffer::create(vulkanContext()->device, stage_size,
													vk::BufferUsageFlagBits::eTransferSrc,
													vk::MemoryPropertyFlagBits::eHostCoherent |
													vk::MemoryPropertyFlagBits::eHostVisible);


	stageBuff->upload(d_mesh.vertices.data(), vertex_size);
	d_mesh.vbo = graphics::VulkanBuffer::create(vulkanContext()->device, vertex_size,
												vk::BufferUsageFlagBits::eVertexBuffer |
												vk::BufferUsageFlagBits::eTransferDst,
												vk::MemoryPropertyFlagBits::eDeviceLocal);

	transferCmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
	stageBuff->copyTo(transferCmd, d_mesh.vbo, vk::BufferCopy(0, 0, vertex_size));
	transferCmd.end();
	vulkanContext()->device->queue(vulkanContext()->device->queueFamilyIndices.transfer).submit(submitInfo, nullptr);
	vk::Device(*vulkanContext()->device).waitIdle();


	stageBuff->upload(d_mesh.indices.data(), index_size);
	d_mesh.ebo = graphics::VulkanBuffer::create(vulkanContext()->device, index_size,
												vk::BufferUsageFlagBits::eIndexBuffer |
												vk::BufferUsageFlagBits::eTransferDst,
												vk::MemoryPropertyFlagBits::eDeviceLocal);


	transferCmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
	stageBuff->copyTo(transferCmd, d_mesh.ebo, vk::BufferCopy(0, 0, index_size));
	transferCmd.end();
	vulkanContext()->device->queue(vulkanContext()->device->queueFamilyIndices.transfer).submit(submitInfo, nullptr);
	vk::Device(*vulkanContext()->device).waitIdle();

	stageBuff = nullptr;
	vk::Device(*vulkanContext()->device).freeCommandBuffers(transferCmdPool, transferCmd);
	vk::Device(*vulkanContext()->device).destroyCommandPool(transferCmdPool);

	d_pipeline = graphics::VulkanGraphicsPipeline::create(vulkanContext()->defaultRenderPass);

	auto res = vulkanContext()->swapChain->actualExtent;
	d_pipeline->addViewport(vk::Viewport(0.0f, 0.0f, (float)res.width, (float)res.height));
	d_pipeline->addScissor(vk::Rect2D({}, res));

	d_pipeline->addVertexInputBinding(vk::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex));
	d_pipeline->setVertexInputAttrib(vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, 0));
	d_pipeline->setVertexInputAttrib(vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32A32Sfloat, sizeof(glm::vec3)));
	d_pipeline->setInputAssemblyState(vk::PrimitiveTopology::eTriangleList);

	// TODO: testing shader
	d_pipeline->addShaderSPIRV("assets/shaders/cube.vert", vk::ShaderStageFlagBits::eVertex);
	d_pipeline->addShaderSPIRV("assets/shaders/cube.frag", vk::ShaderStageFlagBits::eFragment);

	// TODO:
	d_pipeline->build({});

	d_freeCam = std::make_shared<cam::FreeCamera>(vulkanContext()->swapChain->actualExtent.width,
												  vulkanContext()->swapChain->actualExtent.height,
												  cam::CameraType::Orthogonal);

	d_commands = vk::Device(*vulkanContext()->device).allocateCommandBuffers(vk::CommandBufferAllocateInfo(
		vulkanContext()->device->graphicsCmdPool,
		vk::CommandBufferLevel::ePrimary,
		vulkanContext()->swapChain->frameCount
	));

	d_dawQueue = vulkanContext()->device->queue(vulkanContext()->device->queueFamilyIndices.graphics);

	vulkanContext()->defaultRenderPass->clearAll(1.0, 0.0, 0.0, 0.0);

	for (int i = 0; i < d_commands.size(); ++i)
	{
		auto& command = d_commands[i];

		command.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
		{
			command.beginRenderPass(vk::RenderPassBeginInfo(vulkanContext()->defaultRenderPass->renderpass, vulkanContext()->defaultRenderPass->frameBuffers[i], vk::Rect2D({}, res),
				(uint32_t)vulkanContext()->defaultRenderPass->clearValues.size(), vulkanContext()->defaultRenderPass->clearValues.data()),
									vk::SubpassContents::eInline);
			{
				command.setViewport(0, vk::Viewport(0.0f, 0.0f, (float)res.width, (float)res.height));
				command.setScissor(0, vk::Rect2D({}, res));

				command.bindPipeline(vk::PipelineBindPoint::eGraphics, d_pipeline->pipeline);
				vk::DeviceSize offsets[] = { 0 };
				command.bindVertexBuffers(0, 1, &d_mesh.vbo->buffer, offsets);
				command.bindIndexBuffer(d_mesh.ebo->buffer, 0, vk::IndexType::eUint32);
				command.drawIndexed((uint32_t)d_mesh.indices.size(), 1, 0, 0, 0);
			}
			command.endRenderPass();
		}
		command.end();
	}

	d_fence.resize(d_commands.size());
	d_imageAcquiringSemaphore.resize(d_commands.size());
	d_imageRenderingSemaphore.resize(d_commands.size());
	for (int i = 0; i < d_commands.size(); ++i)
	{
		d_imageAcquiringSemaphore[i] = vk::Device(*vulkanContext()->device).createSemaphore(vk::SemaphoreCreateInfo());
		d_imageRenderingSemaphore[i] = vk::Device(*vulkanContext()->device).createSemaphore(vk::SemaphoreCreateInfo());
		d_fence[i] = vk::Device(*vulkanContext()->device).createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
	}

	d_ui = std::shared_ptr<renderer::GuiOverlay>(new renderer::GuiOverlay(dispatcher(), vulkanContext()));
	d_ui->initData();

	return true;
}

void HelloVulkanTest::update()
{
}

void HelloVulkanTest::render()
{
	// TODO: adding present
	static uint32_t index = 0;

	d_ui->startFrame();
	{
		ImGui::Begin("transform data.");
		ImGui::Text("fps average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Separator();
		ImGui::End();
	}
	d_ui->endFrame();

	index = vulkanContext()->swapChain->acquireNewFrame(d_imageAcquiringSemaphore[index]);

	vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eAllGraphics;

	vk::Device(*vulkanContext()->device).waitForFences(1, &d_fence[index], VK_TRUE, UINT64_MAX);
	vk::Device(*vulkanContext()->device).resetFences(1, &d_fence[index]);

	static std::vector<vk::CommandBuffer> commands(10);
	commands.clear();
	commands.push_back(d_commands[index]);
	commands.push_back(d_ui->render(index, nullptr));

	vk::SubmitInfo submitinfo;
	submitinfo.setPWaitDstStageMask(&flags);
	submitinfo.setCommandBufferCount((uint32_t)commands.size());
	submitinfo.setPCommandBuffers(commands.data());

	submitinfo.setWaitSemaphoreCount(1);
	submitinfo.setPWaitSemaphores(&d_imageAcquiringSemaphore[index]);
	submitinfo.setSignalSemaphoreCount(1);
	submitinfo.setPSignalSemaphores(&d_imageRenderingSemaphore[index]);

	d_dawQueue.submit(submitinfo, d_fence[index]);

	vulkanContext()->swapChain->queuePresent(d_dawQueue, index, d_imageRenderingSemaphore[index]);

	index = ((index + 1) % d_commands.size());
}

void HelloVulkanTest::cleanup()
{
	//if (!d_commands.empty())
	//{
	//	for(const auto& cmd : d_commands)
	//	{
	//		vk::Device(*vulkanContext()->device).freeCommandBuffers(vulkanContext()->device->graphicsCmdPool, d_commands);
	//	}
	//}

	for (int i = 0; i < d_commands.size(); ++i)
	{
		vk::Device(*vulkanContext()->device).destroySemaphore(d_imageAcquiringSemaphore[i]);
		vk::Device(*vulkanContext()->device).destroySemaphore(d_imageRenderingSemaphore[i]);
		vk::Device(*vulkanContext()->device).destroyFence(d_fence[i]);
	}
}

void HelloVulkanTest::cameraMotion(float xpos, float ypos, bool & firstMouse)
{
	float WINDOW_WIDTH = (float)vulkanContext()->swapChain->actualExtent.width;
	float WINDOW_HEIGHT = (float)vulkanContext()->swapChain->actualExtent.height;

	static float lastX = WINDOW_WIDTH / 2;
	static float lastY = WINDOW_HEIGHT / 2;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	d_freeCam->pitch(-yoffset * 0.008f);
	d_freeCam->yaw(xoffset * 0.008f);
}


} //end namespace test