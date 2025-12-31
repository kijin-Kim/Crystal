#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/LightComponent.h"

namespace Crystal {


	class DirectionalLightActor : public Actor
	{


	public:
		DirectionalLightActor() = default;
		~DirectionalLightActor() override = default;

		void Initialize() override
		{
			
			m_LightComponent = CreateComponent<DirectionalLightComponent>("DirectionalLightComponent");
			m_LightComponent->SetLocalPosition(Vector3::Zero);
			m_MainComponent = m_LightComponent;
		}


		STATIC_TYPE_IMPLE(DirectionalLightActor)

	protected:
		Shared<DirectionalLightComponent> m_LightComponent = nullptr;
	};

	class PointLightActor : public Actor
	{

	public:
		PointLightActor() = default;
		~PointLightActor() override = default;

		void Initialize() override
		{
			m_LightComponent = CreateComponent<PointLightComponent>("PointLightComponent");
			m_LightComponent->SetLocalPosition(Vector3::Zero);
			m_MainComponent = m_LightComponent;
		}


		STATIC_TYPE_IMPLE(PointLightActor)

	protected:
		Shared<PointLightComponent> m_LightComponent = nullptr;
	};
}


