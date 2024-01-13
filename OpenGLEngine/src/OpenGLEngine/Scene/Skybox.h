#pragma once

#include <OpenGLEngine/Resources/Model.h>
#include <OpenGLEngine/Resources/Materials/CubeMap.h>
#include <OpenGLEngine/Shader/Shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace OpenGLEngine
{
	class Skybox
	{
	private:
		std::shared_ptr<Model> m_Model;

		Shader m_EquirectangularToCubemapShader;
		Shader m_IrradianceShader;
		Shader m_BackgroundShader;

		unsigned int hdrTexture;
		unsigned int envCubemap;
		unsigned int irradianceMap;

		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[6] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};
	public:
		Skybox();

		void BindCubeMap();

		void BindIrradianceMap();

		Shader* GetShader()
		{
			return &m_BackgroundShader;
		}

		Model* GetModel()
		{
			return m_Model.get();
		}
	};
}

