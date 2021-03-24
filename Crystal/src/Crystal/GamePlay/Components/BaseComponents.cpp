#include "cspch.h"
#include "BaseComponents.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/GamePlay/World/World.h"

namespace Crystal {

	PrimitiveComponent::PrimitiveComponent(const std::string& name) : TransformComponent(name)
	{
		m_PrimitiveComponentType = EPrimitiveComponentType::None;
	}

	void PrimitiveComponent::OnCreate()
	{
		Renderer::Instance().RegisterRenderComponent(this);
	}

}