#include "cspch.h"
#include "ParticleActor.h"

BOOST_CLASS_EXPORT(Crystal::ParticleActor)

namespace Crystal {

	void ParticleActor::Initialize()
	{
		Actor::Initialize();

		auto particleComponent = CreateComponent<ParticleComponent>("DebugLineComponent");

		auto particleMaterial = std::make_unique<Crystal::Material>();
		particleMaterial->ShadingModel = Crystal::EShadingModel::ShadingModel_Undefined;

		particleComponent->AddMaterial(std::move(particleMaterial));
		particleComponent->SetLocalPosition({0.0f, 300.0f, 0.0f});
		particleComponent->SetScale(100.0f);

		m_MainComponent = particleComponent;
	}
}
