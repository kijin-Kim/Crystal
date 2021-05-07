#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/LightComponent.h"

namespace Crystal {
	class LightActor : public Actor
	{
		SERIALIZE_PROPERTIES
		{
			ar & *m_LightComponent;
		}

	public:
		LightActor() = default;
		~LightActor() override = default;


		STATIC_TYPE_IMPLE(LightActor)
	protected:
		std::shared_ptr<LightComponent> m_LightComponent = nullptr;
	};


	class LocalLightActor : public LightActor
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<LightActor>(*this);
		}

	public:
		LocalLightActor() = default;
		~LocalLightActor() override = default;

		void Initialize() override
		{
			m_LightComponent = CreateComponent<LocalLightComponent>("LocalLightComponent");
			m_LightComponent->SetLocalPosition(Vector3::Zero);
			m_LightComponent->SetLocalPosition(Vector3::White);

			m_MainComponent = m_LightComponent;
		}


		STATIC_TYPE_IMPLE(LocalLightActor)
	};


	class DirectionalLightActor : public LightActor
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<LightActor>(*this);
		}

	public:
		DirectionalLightActor() = default;
		~DirectionalLightActor() override = default;

		void Initialize() override
		{
			m_LightComponent = CreateComponent<DirectionalLightComponent>("DirectionalLightComponent");
			m_LightComponent->SetLocalPosition(Vector3::Zero);
			m_LightComponent->SetLocalPosition(Vector3::White);

			m_MainComponent = m_LightComponent;
		}


		STATIC_TYPE_IMPLE(DirectionalLightActor)
	};
}
