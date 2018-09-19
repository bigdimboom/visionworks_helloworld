#pragma once
#include "camera_base.h"
#include <glm/gtc/quaternion.hpp>

namespace cam
{

class FreeCamera : public CameraBase
{
public:
	FreeCamera(int width, int height, CameraType type = CameraType::Pespective);
	~FreeCamera();

	void translateForward(float deltaMM);
	void translateRight(float deltaMM);
	void translateUp(float deltaMM);

	void pitch(float deltaDegree);
	void roll(float deltaDegree);
	void yaw(float deltaDegree);

	void reset(const std::string& conf = CAM_CONF);


private:

	struct QuatData
	{
		float pitch = 0.0f;
		float yaw = 0.0f;
		float roll = 0.0f;
		glm::vec3 translation = glm::vec3(0.0);
		glm::quat camQuat;
	}d_qData;

	void updateView();
};


} // end namespae cam