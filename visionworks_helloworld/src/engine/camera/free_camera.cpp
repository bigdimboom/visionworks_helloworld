#include "free_camera.h"
#include <glm/ext.hpp>

namespace cam
{

FreeCamera::FreeCamera(int width, int height, CameraType type)
	: cam::CameraBase(width, height, type)
{
	d_qData.camQuat = glm::quat_cast(d_data.view);
}

FreeCamera::~FreeCamera()
{
}

void FreeCamera::translateForward(float deltaMM)
{
	d_qData.translation.z += deltaMM;
	updateView();
}

void FreeCamera::translateRight(float deltaMM)
{
	d_qData.translation.x += deltaMM;
	updateView();
}

void FreeCamera::translateUp(float deltaMM)
{
	d_qData.translation.y += deltaMM;
	updateView();
}

void FreeCamera::pitch(float deltaDegree)
{
	d_qData.pitch += deltaDegree;
	updateView();
}

void FreeCamera::roll(float deltaDegree)
{
	d_qData.roll += deltaDegree;
	updateView();
}

void FreeCamera::yaw(float deltaDegree)
{
	d_qData.yaw += deltaDegree;
	updateView();
}

void FreeCamera::reset(const std::string & conf)
{
	load(conf);
}

// helpers
void FreeCamera::updateView()
{
	// see http://in2gpu.com/2016/03/14/opengl-fps-camera-quaternion/ for fps
	//glm::quat key_quat = glm::quat(glm::vec3(d_qData.pitch, d_qData.yaw, d_qData.roll));

	auto pitch = glm::quat(glm::vec3(d_qData.pitch, 0.0, d_qData.roll));
	auto yaw = glm::quat(glm::vec3(0.0, d_qData.yaw, d_qData.roll));
	//auto key_quat =  yaw * pitch;
	d_qData.pitch = d_qData.yaw = d_qData.roll = 0;

	//d_qData.camQuat = key_quat * d_qData.camQuat;
	d_qData.camQuat = pitch * d_qData.camQuat * yaw; // for preventing rolling under first person camera

	d_qData.camQuat = glm::normalize(d_qData.camQuat);
	glm::mat4 rotate = glm::mat4_cast(d_qData.camQuat);

	d_data.position += glm::inverse(rotate) * glm::vec4(d_qData.translation, 1.0);

	glm::mat4 translate = glm::mat4(1.0f);
	translate = glm::translate(translate, glm::vec3(-d_data.position));
	d_data.view = rotate * translate;
	d_data.viewInv = glm::inverse(d_data.view);

	d_data.up = d_data.view[1];
	d_data.forward = d_data.view[2];

	d_qData.translation = glm::vec3(0.0);
}

} // end namespae cam