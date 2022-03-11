#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vge
{
	class VgeCamera
	{
	public:
		void SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
		void SetPerspectiveProjection(float fovy, float aspect, float near, float far);

		glm::mat4 GetProjection() const;
	private:
		glm::mat4 projectionMatrix{1.f};
	};
}
