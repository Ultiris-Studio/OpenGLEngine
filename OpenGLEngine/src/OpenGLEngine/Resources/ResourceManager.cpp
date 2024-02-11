#include "depch.h"

#include <glad/glad.h>
#include <OpenGLEngine/Resources/ResourceManager.h>

namespace OpenGLEngine
{
	std::shared_ptr<Texture> ResourceManager::getTexture(const std::string& id, bool gamma)
	{
		if (m_Textures.find(id) != m_Textures.cend())
		{
			return m_Textures.at(id);
		}
		else
		{
			m_Textures[id] = Texture::CreateTexture(id, gamma);
			return m_Textures.at(id);
		}
	}

	std::shared_ptr<Material> ResourceManager::getMaterial(const std::string& id)
	{
		if (m_Materials.find(id) != m_Materials.cend())
		{
			return m_Materials.at(id);
		}
		else
		{
			m_Materials[id] = Material::CreateMaterial();
			return m_Materials.at(id);
		}
	}

	std::shared_ptr<Model> ResourceManager::getModel(const std::string& id)
	{
		if (m_Models.find(id) != m_Models.cend())
		{
			return m_Models.at(id);
		}
		else
		{
			m_Models[id] = Model::CreateModel(id);
			return m_Models.at(id);
		}
	}

	void ResourceManager::Reset()
	{
		for (auto& texture : m_Textures)
		{
			texture.second.reset();
		}

		for (auto& material : m_Materials)
		{
			material.second.reset();
		}

		for (auto& model : m_Models)
		{
			model.second.reset();
		}

		m_Textures.clear();
		m_Materials.clear();
		m_Models.clear();
	}
}