#include "pch.h"
#include "LayerStack.h"

namespace Wuya
{
	LayerStack::~LayerStack()
	{
		for (auto* layer : m_Layers)
		{
			layer->OnDetached();
			delete layer;
		}
	}

	void LayerStack::PushLayer(ILayer* layer)
	{
		m_Layers.emplace(begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverlay(ILayer* layer)
	{
		m_Layers.emplace_back(layer);
	}

	void LayerStack::PopLayer(ILayer* layer)
	{
		auto it = std::find(begin(), begin() + m_LayerInsertIndex, layer);
		if (it != begin() + m_LayerInsertIndex)
		{
			layer->OnAttached();
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
		else
		{
			CORE_LOG_WARN("Can't find layer ({0}) to pop up.", layer->GetName());
		}
	}

	void LayerStack::PopOverlay(ILayer* layer)
	{
		auto it = std::find(begin() + m_LayerInsertIndex, end(), layer);
		if (it != end())
		{
			layer->OnAttached();
			m_Layers.erase(it);
		}
		else
		{
			CORE_LOG_WARN("Can't find layer ({0}) to pop up.", layer->GetName());
		}
	}
}