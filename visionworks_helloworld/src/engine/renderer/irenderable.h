#pragma once
#include "../camera/camera_base.h"

namespace renderer
{

class IRenderable
{
public:
	virtual ~IRenderable() {}

	virtual void initData() = 0;
	virtual void render(cam::CameraBase* cam) = 0;
	virtual void cleanup() = 0;
};

} // end namespace renderer