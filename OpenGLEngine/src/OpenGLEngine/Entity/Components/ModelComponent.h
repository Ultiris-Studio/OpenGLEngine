#pragma once

#include <OpenGLEngine/Entity/Component.h>

namespace OpenGLEngine
{
	class ModelComponent : public Component
	{
	private:
		std::shared_ptr<Model> m_Model;
	public:
		std::string m_ModelPath;

		ModelComponent() = default;
		ModelComponent(const std::string& path);

		void SetModel(const std::string& path);
		void SetModel(std::shared_ptr<Model> model) { m_Model = model; }

		Model& GetModel() { return *m_Model; }
		Model* GetPtr() { return m_Model.get(); }
	};
}