#include "cspch.h"
#include "MeshComponents.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {

	MeshComponent::MeshComponent(const std::string& name) : RenderComponent(name)
	{
		Renderer::Instance().RegisterRenderComponent(this);
	}
}