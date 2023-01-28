#include "depch.h"
#include "LayerManager.h"

namespace DuckEngine
{
	LayerManager::LayerManager()
	{

	}

	LayerManager::~LayerManager()
	{
		for (Layer* layer : m_Layers)
			delete layer;
	}

	void LayerManager::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerIndex, layer);
	}

	void LayerManager::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}
}