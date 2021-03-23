#pragma once
#include <vector>

namespace Crystal {
	class Layer
	{
	public:
		Layer() = default;
		virtual ~Layer() = default;

		virtual void Enter() {}
		virtual void Exit() {}
		virtual void Update(float deltatTime) {}
	};

	class LayerStack final
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	private:
		std::vector<Layer*> m_Layers;
	};
}
