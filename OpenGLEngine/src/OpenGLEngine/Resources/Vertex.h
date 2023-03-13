#pragma once

#include <glm/glm.hpp>

namespace OpenGLEngine
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
	};
}