#include "cspch.h"
#include "MeshComponent.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {
	MeshComponent::MeshComponent(const std::string& name) : RenderComponent(name)
	{
		Renderer::Instance().RegisterMeshComponent(this);
	}
}