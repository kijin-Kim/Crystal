#include "LightComponent.h"
#include "cspch.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {


	void LightComponent::RegisterComponent()
	{
		TransformComponent::RegisterComponent();

		Renderer::Instance().RegisterLightComponent(Cast<LightComponent>(shared_from_this()));
	}

}

