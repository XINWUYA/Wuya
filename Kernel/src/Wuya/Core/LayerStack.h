#pragma once
#include "Layer.h"

namespace Wuya
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(const SharedPtr<ILayer>& layer);
		void PushOverlay(const SharedPtr<ILayer>& layer);
		void PopLayer(const SharedPtr<ILayer>& layer);
		void PopOverlay(const SharedPtr<ILayer>& layer);

		std::vector<SharedPtr<ILayer>>::iterator begin() { return m_Layers.begin(); }
		std::vector<SharedPtr<ILayer>>::iterator end() { return m_Layers.end(); }
		std::vector<SharedPtr<ILayer>>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		std::vector<SharedPtr<ILayer>>::reverse_iterator rend() { return m_Layers.rend(); }
		std::vector<SharedPtr<ILayer>>::const_iterator begin() const { return m_Layers.begin(); }
		std::vector<SharedPtr<ILayer>>::const_iterator end() const { return m_Layers.end(); }
		std::vector<SharedPtr<ILayer>>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
		std::vector<SharedPtr<ILayer>>::const_reverse_iterator rend() const { return m_Layers.rend(); }

	private:
		std::vector<SharedPtr<ILayer>> m_Layers{};
		uint32_t m_LayerInsertIndex{ 0 };
	};

}