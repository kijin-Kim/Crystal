#include "cspch.h"
#include "BaseComponents.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {

	RenderComponent::RenderComponent(const std::string& name) : TransformComponent(name)
	{
		Renderer::Instance().RegisterRenderComponent(this);	
	}

}