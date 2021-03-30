#pragma once
#include "Actor.h"
#include "Crystal\GamePlay\Components\LightComponent.h"

namespace Crystal {

	class LightActor : public Actor
	{
	public:
		LightActor(Object* object) : Actor(object)
		{
		}

		~LightActor() override = default;

		LightComponent* GetLightComponent() const { return m_LightComponent; }


	protected:
		LightComponent* m_LightComponent = nullptr;
	};


	class DirectionalLightActor : public LightActor
	{
	public:
		DirectionalLightActor(Object* object) : LightActor(object)
		{
			m_LightComponent = CreateComponent<Crystal::DirectionalLightComponent>("DirectionalLightComponent");
		}
		~DirectionalLightActor() override = default;
	};

}
