#pragma once
#include <glm/glm.hpp>
#include <string>

#define PROJ_MIN_RANGE_MM 0.1f
#define PROJ_MAX_RANGE_MM 5000.0f
#define FOV_Y 60.0f
#define CAM_CONF "camera_config.json"
#define LOAD_CONFIG

namespace cam
{

enum CameraType
{
	Pespective,
	Orthogonal
};

class CameraBase
{
public:
	CameraBase(CameraType type = CameraType::Pespective);
	CameraBase(int width, int height, CameraType type = CameraType::Pespective);
	~CameraBase();

	CameraBase(const CameraBase& other);
	CameraBase(CameraBase&& other);

	void operator=(const CameraBase& other);
	void operator=(CameraBase&& other);

	void setPosition(const glm::vec3& eye);
	void setPosition(const glm::vec4& eye);
	void setCameraPose(const glm::mat4& view);
	void setProjection(const glm::mat4& proj);

	// fovY is only valid when type is perspective
	void setProjectionMode(CameraType type, float fovY = FOV_Y);

	const glm::mat4& projMatrix() const;
	const glm::mat4& viewMatrix() const;
	const glm::mat4& projInvMatrix() const;
	const glm::mat4& viewInvMatrix() const;

	const glm::vec4& up() const;
	const glm::vec4& forward() const;
	const glm::vec4& postion() const;

	bool save(const std::string& config = CAM_CONF);
	bool load(const std::string& config = CAM_CONF);

protected:

	struct Internal
	{
		int width;
		int height;
		float rangeMin = PROJ_MIN_RANGE_MM;
		float rangeMax = PROJ_MAX_RANGE_MM;
		float fovY = FOV_Y;

		glm::vec4 position = glm::vec4(2.0, 2.0, 2.0, 3.5);
		glm::vec4 up = glm::vec4(0.0, 1.0, 0.0, 0.0);
		glm::vec4 forward = glm::vec4(0.0, 0.0, -1.0, 0.0);

		CameraType camType = CameraType::Pespective;

		glm::mat4 proj = glm::mat4(1.0);
		glm::mat4 view = glm::mat4(1.0);
		glm::mat4 projInv = glm::mat4(1.0);
		glm::mat4 viewInv = glm::mat4(1.0);
	}d_data;

	void setPerspective();
	void setOrthogonal();
	void setView();
};


} // end namespace cam