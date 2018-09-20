#define SDL_MAIN_HANDLED
#include "test/hello_vulkan_test.h"

int main(int argc, const char** argv)
{
	return test::HelloVulkanTest(argc, argv).exec();
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
