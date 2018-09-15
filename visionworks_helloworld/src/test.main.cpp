#define SDL_MAIN_HANDLED
//#include "engine/application/app_base.h"
//#include <opencv2/core.hpp>
//#include <opencv2/highgui.hpp>
//#include "engine/event/safe_event.h"
//
//struct hello_event {
//	void say_hello()
//	{
//		std::cout << "hello world !" << std::endl;
//	}
//};
//
//
//class AppTest : public app::AppBase
//{
//public:
//
//	AppTest(int argc, const char** argv)
//		: app::AppBase(argc, argv) {}
//
//	// Inherited via AppBase
//	virtual int exec() override
//	{
//		se::dispatcher ev_disp;
//
//		// add the hello_event struct to the available events
//		ev_disp.add_event<hello_event>();
//
//		// listen hello_event
//		ev_disp.listen(std::function<void(const hello_event& ev)>([](const hello_event& ev) {
//			std::cout << "This is a real hello_event !" << std::endl;;
//		}));
//
//		// listen any event
//		ev_disp.listen_any([](const se::any_event& ev) {
//			// make dynamic test to check the event type
//			if (se::is<hello_event>(ev)) {
//				std::cout << "any_event is an hello_event" << std::endl;
//				auto real_ev = se::into<hello_event>(ev);
//				real_ev.say_hello();
//			}
//		});
//
//		// dispatch an event
//		ev_disp.trigger(hello_event());
//
//
//		cv::Mat data(600, 800, CV_8UC3);
//		data.setTo(cv::Scalar(0,0,255));
//		cv::imshow("test", data);
//		cv::waitKey(0);
//		return EXIT_SUCCESS;
//	}
//};

//#include <ghost_sdi/Sender/DeckSender.hpp>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vector>

#include "engine/graphics/vulkan_context.h"
#include "engine/graphics/vulkan_buffer.h"
#include "engine/graphics/vulkan_texture.h"


int main(int argc, const char** argv)
{
	//return AppTest(argc, argv).exec();
	//using namespace ghost::sdi;
	//ghost::sdi::DeckSender obj(0, DisplayMode::FHD2997P, ghost::media::ImageFormat::CBYCRY_10_BT709);

	//glm::mat4 data(1.0);
	//std::cout << glm::to_string(data) << "\n";

	if (!graphics::VulkanContext::initialize())
	{
		return EXIT_FAILURE;
	}

	auto context = graphics::VulkanContext::create(graphics::VulkanContext::createWindow(WINDOW_DEFAULT_TITLE, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT));
	auto phsycalDeviceList = context->physicalDeviceList(context->instance);

	auto graphicsIndex = graphics::VulkanContext::getOptimalFamilyQueueIndex(phsycalDeviceList[0], vk::QueueFlagBits::eGraphics);
	auto transferIndex = graphics::VulkanContext::getOptimalFamilyQueueIndex(phsycalDeviceList[0], vk::QueueFlagBits::eTransfer);
	auto computeIndex = graphics::VulkanContext::getOptimalFamilyQueueIndex(phsycalDeviceList[0], vk::QueueFlagBits::eCompute);

	auto buffer = graphics::VulkanBuffer::create(context->device,
												 1024, vk::BufferUsageFlagBits::eVertexBuffer,
												 vk::MemoryPropertyFlagBits::eHostVisible);

	auto texture = graphics::VulkanTexture::create(context->device,
												   vk::Format::eB8G8R8A8Unorm,
												   vk::Extent3D(1024u, 1024u, 1u),
												   vk::ImageUsageFlagBits::eColorAttachment,
												   vk::MemoryPropertyFlagBits::eDeviceLocal);

	vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
	auto imageview = texture->acquireImageView(vk::ImageViewType::e2D, {}, range);
	auto imagesampler = texture->acquireImageSampler();


	texture = nullptr;
	buffer = nullptr;
	context = nullptr;

	graphics::VulkanContext::shutdown();

	return 0;
}


//#include <iostream>
//#include <VX/vx.h>
//#include <VX/vxu.h>
//#include <NVX/nvx.h>
//#include <NVXIO/Utility.hpp>
//#include <NVXIO/Render3D.hpp>
//#include <string>
//
//void logCallback(vx_context c,
//				 vx_reference r,
//				 vx_status s,
//				 const vx_char message[])
//{ /* Do something */
//	std::cout << "message: " << message << "\n";
//}
//
//int main(int argc, const char** argv)
//{
//	nvxio::ContextGuard openvxContext;
//	
//	if (vxGetStatus(openvxContext) != VX_SUCCESS)
//	{
//		std::cout << "unable to create openvx context.\n";
//		return EXIT_FAILURE;
//	}
//	vx_border_mode_t bordermode = { VX_BORDER_MODE_CONSTANT, 0 };
//	// example of setting context attribute
//	if (vxSetContextAttribute(openvxContext, VX_CONTEXT_ATTRIBUTE_IMMEDIATE_BORDER_MODE, &bordermode, sizeof(bordermode)) != VX_SUCCESS)
//	{
//		std::cout << "openvx context settings error.\n";
//		return EXIT_FAILURE;
//	}
//	// TODO: setting it GPU as mode
//	auto render = nvxio::createDefaultRender3D(openvxContext, 0, 0, "ha", 800, 600);
//	
//	vxRegisterLogCallback(openvxContext, (vx_log_callback_f)logCallback, vx_false_e);
//	vxDirective(openvxContext, VX_DIRECTIVE_ENABLE_PERFORMANCE);
//	
//	auto image = vxCreateImage(openvxContext, 1920, 1080, VX_DF_IMAGE_RGB);
//
//	void * ptr = NULL;
//	vx_imagepatch_addressing_t imageAccessAddr;
//	vx_rectangle_t rect = { 0, 0, 800, 800 };
//	vxAccessImagePatch(image, &rect, 0, &imageAccessAddr, &ptr, VX_READ_AND_WRITE);
//	/**Do something***/
//	vxCommitImagePatch(image, &rect, 0, &imageAccessAddr, ptr);
//	vxReleaseImage(&image);
//
//
//	return EXIT_SUCCESS;
//}
