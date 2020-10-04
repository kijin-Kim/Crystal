#include "cspch.h"
#include "MeshComponent.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {


	MeshComponent::MeshComponent()
	{
		Renderer::Instance().RegisterMeshComponent(this);
	}

}