#pragma once
#include <Wuya/Core/Layer.h>

class EditorLayer : public Wuya::ILayer
{
public:
	EditorLayer();
	virtual ~EditorLayer() = default;

	virtual void OnImGuiRender() override;
};

