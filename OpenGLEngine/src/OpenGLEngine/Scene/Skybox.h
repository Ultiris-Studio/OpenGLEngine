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
		Shader m_PrefilterShader;
		Shader m_BrdfShader;

		unsigned int hdrTexture;
		unsigned int envCubemap;
		unsigned int irradianceMap;
		unsigned int brdfLUTTexture;
		unsigned int prefilterMap;

		unsigned int quadVAO = 0;
		unsigned int quadVBO;

		void RenderQuad();
	public:
		Skybox();

		void BindCubeMap();

		void BindIrradianceMap();

		void BindPrefilterMap();

		void BindBrdfLUT();

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

