#include "depch.h"
#include "EditorAssetManager.h"

namespace OpenGLEngine
{
	EditorAssetManager::EditorAssetManager()
	{
	}

	EditorAssetManager::~EditorAssetManager()
	{
	}

	void EditorAssetManager::LoadAsset(std::string name)
	{
		if (!IsAssetLoaded(name))
		{
			m_LoadedAssets[name] = std::make_shared<Asset>();
		}
	}

	void EditorAssetManager::UnloadAsset(std::string name)
	{
	}

	std::shared_ptr<Asset> EditorAssetManager::GetAsset(std::string name) const
	{
		if (m_LoadedAssets.find(name) == m_LoadedAssets.end())
			return nullptr;

		return m_LoadedAssets.at(name);
	}

	bool EditorAssetManager::IsAssetLoaded(std::string name) const
	{
		return (m_LoadedAssets.find(name) == m_LoadedAssets.end());
	}
}