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

	auto stageBuff = graphics::VulkanBuffer::create(vulkanContext()->device, stage_size,
													vk::BufferUsageFlagBits::eTransferSrc,
													vk::MemoryPropertyFlagBits::eHostCoherent |
													vk::MemoryPropertyFlagBits::eHostVisible);


	stageBuff->upload(d_mesh.vertices.data(), vertex_size);
	d_mesh.vbo = graphics::VulkanBuffer::create(vulkanContext()->device, vertex_size,
												vk::BufferUsageFlagBits::eVertexBuffer |
												vk::BufferUsageFlagBits::eTransferDst,
												vk::MemoryPropertyFlagBits::eDeviceLocal);

	stageBuff->upload(d_mesh.indices.data(), index_size);
	d_mesh.ebo = graphics::VulkanBuffer::create(vulkanContext()->device, index_size,
												vk::BufferUsageFlagBits::eIndexBuffer |
												vk::BufferUsageFlagBits::eTransferDst,
												vk::MemoryPropertyFlagBits::eDeviceLocal);

	stageBuff = nullptr;







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

void HelloVulkanTest::cameraMotion(float xpos, float ypos, bool & firstMouse)
{
}


} //end namespace test