#pragma once
#include "../engine/application/graphics_app.h"
#include <memory>
#include <vector>

#include "../engine/graphics/vulkan_buffer.h"
#include "../engine/graphics/vulkan_pipeline.h"

#include "../engine/camera/free_camera.h"

#include <glm/glm.hpp>

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

	struct Vertex
	{
		glm::vec3 position;
		glm::vec4 color;
	};

	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::shared_ptr<graphics::VulkanBuffer> vbo;
		std::shared_ptr<graphics::VulkanBuffer> ebo;
	}d_mesh;

	struct UniformBufferObject
	{
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);
	};

	std::shared_ptr<cam::FreeCamera> d_freeCam;

	std::shared_ptr<graphics::VulkanGraphicsPipeline> d_pipeline;

	std::vector<vk::CommandBuffer> d_commands;

	vk::Queue d_dawQueue;

	std::vector<vk::Semaphore> d_imageAcquiringSemaphore;
	std::vector<vk::Semaphore> d_imageRenderingSemaphore;
	std::vector<vk::Fence> d_fence;


	void cameraMotion(float xpos, float ypos, bool& firstMouse);
};




} // end namespace test