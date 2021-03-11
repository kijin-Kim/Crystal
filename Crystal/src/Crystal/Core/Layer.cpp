#include "cspch.h"
#include "Layer.h"

namespace Crystal {
	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
			delete layer;
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		if (!m_Layers.empty())
			m_Layers.back()->Exit();
		layer->Enter();
		m_Layers.emplace_back(layer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			(*it)->Exit();
			m_Layers.erase(it);
			if (!m_Layers.empty())
				m_Layers.back()->Enter();
		}
		else
			CS_WARN("레이어가 존재하지 않습니다");
	}
}