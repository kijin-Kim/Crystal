#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/ParticleComponent.h"


namespace Crystal {
	class ParticleActor : public Actor
	{
	public:
		ParticleActor()
		{
			auto particleComponent = CreateComponent<ParticleComponent>("DebugLineComponent");

			auto particleMaterial = std::make_unique<Crystal::NewMaterial>();
			particleMaterial->ShadingModel = Crystal::EShadingModel::ShadingModel_Undefined;

			particleComponent->AddMaterial(std::move(particleMaterial));
			particleComponent->SetLocalPosition({ 0.0f, 300.0f, 0.0f });
			particleComponent->SetScale(100.0f);

			m_MainComponent = particleComponent;
		}

		~ParticleActor() override = default;

		void Begin() override
		{
		}
		void End() override
		{
		}
		void Update(const float deltaTime) override {}



		STATIC_TYPE_IMPLE(ParticleActor)
	};
}

