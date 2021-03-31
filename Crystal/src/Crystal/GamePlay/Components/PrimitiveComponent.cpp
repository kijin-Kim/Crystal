#include "cspch.h"
#include "PrimitiveComponent.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {
	void PrimitiveComponent::OnCreate()
	{
		Renderer::Instance().RegisterRenderComponent(this);
	}
}