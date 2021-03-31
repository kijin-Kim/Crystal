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

		LightComponent* GetLightComponent() const { return m_LightComponent; }


		STATIC_TYPE_IMPLE(LightActor)
	protected:
		LightComponent* m_LightComponent = nullptr;
	};

	class DirectionalLightActor : public LightActor
	{
	public:
		DirectionalLightActor()
		{
			m_LightComponent = CreateComponent<Crystal::DirectionalLightComponent>("DirectionalLightComponent");
		}
		~DirectionalLightActor() override = default;

		STATIC_TYPE_IMPLE(DirectionalLightActor)
	};
}