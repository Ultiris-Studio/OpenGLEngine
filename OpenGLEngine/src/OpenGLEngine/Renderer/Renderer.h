#pragma once

#include <OpenGLEngine/Scene/EditorCamera.h>
#include <OpenGLEngine/Resources/Texture.h>
#include <OpenGLEngine/Resources/Model.h>
#include <OpenGLEngine/Resources/Materials/Material.h>
#include <OpenGLEngine/Shader/Shader.h>
#include <OpenGLEngine/Resources/ResourceManager.h>

namespace OpenGLEngine
{
	class Renderer
	{
	public:
		struct SceneData
		{
			EditorCamera* m_ActiveCamera;
			ResourceManager m_ResourceManager;
		};
		static SceneData m_SceneData;

		static void BeginScene(EditorCamera* camera);
		static void EndScene();

		static void Clear();
		static void ClearColor(const glm::vec4& color);
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	};
}