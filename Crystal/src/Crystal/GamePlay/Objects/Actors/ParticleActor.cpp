﻿#include "cspch.h"
#include "ParticleActor.h"

#include "Crystal/Resources/ResourceManager.h"

BOOST_CLASS_EXPORT(Crystal::ParticleActor)

namespace Crystal {

	void ParticleActor::Initialize()
	{
		Actor::Initialize();

		auto particleComponent = CreateComponent<ParticleComponent>("ParticleComponent");

		auto particleMaterial = CreateShared<Crystal::Material>();
		particleMaterial->ShadingModel = Crystal::EShadingModel::SM_Undefined;
		particleMaterial->AlbedoTexture = ResourceManager::Instance().GetTexture("assets/textures/T_Explosion_SubUV.tga");

		particleComponent->AddMaterial(std::move(particleMaterial));
		particleComponent->SetLocalPosition({0.0f, 300.0f, 0.0f});
		particleComponent->SetScale(300.0f);

		m_MainComponent = particleComponent;
	}
}
