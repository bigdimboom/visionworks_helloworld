#include "camera_base.h"
#include <glm/ext.hpp>
#include "../graphics/vulkan_window.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>

namespace cam
{

CameraBase::CameraBase(CameraType type)
{
	CameraBase(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, type);
}

CameraBase::CameraBase(int width, int height, CameraType type)
{
	d_data.width = width;
	d_data.height = height;

	switch (type)
	{
	case CameraType::Pespective:
		setPerspective();
		break;
	case CameraType::Orthogonal:
		setOrthogonal();
		break;
	default:
		throw std::runtime_error("unsupported");
	}
	setView();

#ifdef LOAD_CONFIG
	{
		if (!std::experimental::filesystem::exists(CAM_CONF))
		{
			if (!save(CAM_CONF))
			{
				throw std::runtime_error("unale to save data");
			}
		}

		if (!load(CAM_CONF))
		{
			throw std::runtime_error("unale to load data");
		}
	}
#endif // LOAD_CONFIG
}

CameraBase::~CameraBase()
{
}

CameraBase::CameraBase(const CameraBase & other)
	: d_data(other.d_data)
{
}

CameraBase::CameraBase(CameraBase && other)
	: d_data(std::move(other.d_data))
{
}

void CameraBase::operator=(const CameraBase & other)
{
	d_data = other.d_data;
}

void CameraBase::operator=(CameraBase && other)
{
	d_data = std::move(other.d_data);
}

void CameraBase::setPosition(const glm::vec3 & eye)
{
	d_data.position = glm::vec4(eye, 1.0);
	setView();
}

void CameraBase::setPosition(const glm::vec4 & eye)
{
	d_data.position = eye;
	setView();
}

void CameraBase::setCameraPose(const glm::mat4 & view)
{
	d_data.view = view;
	d_data.viewInv = glm::inverse(view);
}

void CameraBase::setProjection(const glm::mat4 & proj)
{
	d_data.proj = proj;
	d_data.projInv = glm::inverse(proj);
}

void CameraBase::setProjectionMode(CameraType type, float fovY)
{
	switch (type)
	{
	case CameraType::Pespective:
		setPerspective();
		break;
	case CameraType::Orthogonal:
		setOrthogonal();
		break;
	default:
		throw std::runtime_error("unsupported");
	}

	d_data.fovY = fovY;
	d_data.camType = type;
}

const glm::mat4 & CameraBase::projMatrix() const
{
	return d_data.proj;
}

const glm::mat4 & CameraBase::viewMatrix() const
{
	return d_data.view;
}

const glm::mat4 & CameraBase::projInvMatrix() const
{
	return d_data.projInv;
}

const glm::mat4 & CameraBase::viewInvMatrix() const
{
	return d_data.viewInv;
}

const glm::vec4 & CameraBase::up() const
{
	return d_data.up;
}

const glm::vec4 & CameraBase::forward() const
{
	return d_data.forward;
}

const glm::vec4 & CameraBase::postion() const
{
	return d_data.position;
}

bool CameraBase::save(const std::string & config)
{
	nlohmann::json j;

	j["width"] = d_data.width;
	j["height"] = d_data.height;
	j["near"] = d_data.rangeMin;
	j["far"] = d_data.rangeMax;
	j["fovY"] = d_data.fovY;

	j["position"] = { d_data.position.x, d_data.position.y, d_data.position.z };
	j["up"] = { d_data.up.x, d_data.up.y, d_data.up.z };
	j["forward"] = { d_data.forward.x, d_data.forward.y, d_data.forward.z };

	j["cam_type"] = (int)d_data.camType;


	std::ofstream out(config);

	if (!out.is_open())
	{
		return false;
	}

	out << std::setw(4) << j << std::endl;

	return true;
}

bool CameraBase::load(const std::string & config)
{
	if (!std::experimental::filesystem::exists(CAM_CONF))
	{
		return false;
	}

	std::ifstream i(config);
	nlohmann::json j;
	i >> j;

	std::vector<std::string> keys = {
		"width",
		"height",
		"near",
		"far",
		"fovY",
		"position",
		"up",
		"forward",
		"cam_type"
	};

	for (auto& elem : keys)
	{
		if (!j.count(elem))
		{
			throw std::runtime_error("bad camera config data");
		}
		else
		{
			if ((elem == "up" || elem == "position" || elem == "forward") && !j[elem].is_array())
			{
				throw std::runtime_error("bad camera config data");
			}
		}
	}

	d_data.width = j["width"].get<int>();
	d_data.height = j["height"].get<int>();
	d_data.rangeMin = j["near"].get<float>();
	d_data.rangeMax = j["far"].get<float>();
	d_data.fovY = j["fovY"].get<float>();

	d_data.position.x = j["position"][0];
	d_data.position.y = j["position"][1];
	d_data.position.z = j["position"][2];
	d_data.position.w = 1.0;

	d_data.up.x = j["up"][0];
	d_data.up.y = j["up"][1];
	d_data.up.z = j["up"][2];
	d_data.up.w = 0.0;

	d_data.forward.x = j["forward"][0];
	d_data.forward.y = j["forward"][1];
	d_data.forward.z = j["forward"][2];
	d_data.forward.w = 0.0;

	d_data.camType = (CameraType)(j["cam_type"].get<int>());

	setView();

	switch (d_data.camType)
	{
	case CameraType::Pespective:
		setPerspective();
		break;
	case CameraType::Orthogonal:
		setOrthogonal();
		break;
	default:
		throw std::runtime_error("error type");
		break;
	}

	return true;
}

// HELPERS
void CameraBase::setPerspective()
{
	d_data.proj = glm::perspective(glm::radians(d_data.fovY), (float)d_data.width / (float)d_data.height, d_data.rangeMin, d_data.rangeMax);
	d_data.proj[1][1] *= -1;
	d_data.projInv = glm::inverse(d_data.proj);
}

void CameraBase::setOrthogonal()
{
	auto range = float(d_data.rangeMax - d_data.rangeMin) / 2.0f;
	d_data.proj = glm::ortho(0.0f, (float)d_data.width, (float)d_data.height, 0.0f, -range, range);
	d_data.projInv = glm::inverse(d_data.proj);
	//std::cout << "wrong set up?.\n";
}

void CameraBase::setView()
{
	auto center = glm::normalize(d_data.position + d_data.forward);
	d_data.view = glm::lookAt(glm::vec3(d_data.position), glm::vec3(center), glm::vec3(d_data.up));
	d_data.viewInv = glm::inverse(d_data.view);
}


} // end namespace cam