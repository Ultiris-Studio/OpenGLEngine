#pragma once


#include "../SceneManager.h"

class Scene;

namespace OpenGLEngine
{
	class EntityPropertiePanel
	{
	public:
		EntityPropertiePanel();

		void OnImGuiRender(SceneManager& sceneManager);
		void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	private:
		std::shared_ptr<Texture> m_ModelTexture;
	};
}