#include "cspch.h"
#include "PrimitiveComponent.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"

namespace Crystal {
	void PrimitiveComponent::OnCreate()
	{
		TransformComponent::OnCreate();
	}

	void PrimitiveComponent::RegisterComponent()
	{
		TransformComponent::RegisterComponent();

		Renderer::Instance().RegisterRendererComponent(Cast<PrimitiveComponent>(shared_from_this()));

	}

}