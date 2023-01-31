#pragma once

#include "Layer.h"
#include <vector>

namespace DuckEngine
{
	class LayerManager
	{
	public:
		LayerManager();
		~LayerManager();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerIndex = 0;
	};
}