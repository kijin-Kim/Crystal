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


	class LocalLightActor : public LightActor
	{
	public:
		LocalLightActor()
		{
			m_LightComponent = CreateComponent<LocalLightComponent>("LocalLightComponent");
			m_LightComponent->SetLocalPosition(Vector3::Zero);
			m_LightComponent->SetLocalPosition(Vector3::White);

			m_MainComponent = m_LightComponent;

		}
		~LocalLightActor() override = default;


		STATIC_TYPE_IMPLE(LocalLightActor)
	};

	

	class DirectionalLightActor : public LightActor
	{
	public:
		DirectionalLightActor()
		{
			m_LightComponent = CreateComponent<DirectionalLightComponent>("DirectionalLightComponent");
			m_LightComponent->SetLocalPosition(Vector3::Zero);
			m_LightComponent->SetLocalPosition(Vector3::White);

			m_MainComponent = m_LightComponent;
			
		}
		~DirectionalLightActor() override = default;


		STATIC_TYPE_IMPLE(DirectionalLightActor)
	};
}
