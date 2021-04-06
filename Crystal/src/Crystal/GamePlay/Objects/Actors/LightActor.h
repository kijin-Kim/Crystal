#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/LightComponent.h"

namespace Crystal {
	class LightActor : public Actor
	{
	public:
		LightActor()
		{
		}

		~LightActor() override = default;


		STATIC_TYPE_IMPLE(LightActor)
	protected:
		std::shared_ptr<LightComponent> m_LightComponent = nullptr;
	};

	class DirectionalLightActor : public LightActor
	{
	public:
		DirectionalLightActor()
		{
			m_LightComponent = CreateComponent<Crystal::DirectionalLightComponent>("DirectionalLightComponent");
			m_LightComponent->SetLocalPosition({ 0.0f, 0.0f, 0.0f });
			
		}
		~DirectionalLightActor() override = default;


		STATIC_TYPE_IMPLE(DirectionalLightActor)
	};
}
