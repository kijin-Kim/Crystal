#include "cspch.h"
#include "PrimitiveComponent.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {
	void PrimitiveComponent::OnCreate()
	{
		TransformComponent::OnCreate();
		Renderer::Instance().RegisterRenderComponent(this);
	}
}