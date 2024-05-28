#pragma once

#include <glm/glm.hpp>

namespace OpenGLEngine
{
	class BaseCamera
	{
	public:
		virtual const glm::mat4 getViewMatrix() const = 0;
		virtual const glm::mat4& getProjectionMatrix() const = 0;

		virtual glm::mat4 GetTransform() = 0;
	};
}