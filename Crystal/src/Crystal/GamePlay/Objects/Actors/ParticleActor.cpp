#include "cspch.h"
#include "ParticleActor.h"

#include "Crystal/Resources/ResourceManager.h"


namespace Crystal {

	void ParticleActor::Initialize()
	{
		Actor::Initialize();

		auto particleComponent = CreateComponent<ParticleComponent>("ParticleComponent");

		auto fireBallMaterial = CreateShared<Crystal::Material>();
		fireBallMaterial->AlbedoTexture = ResourceManager::Instance().GetTexture("assets/textures/T_Explosion_SubUV.tga");
		fireBallMaterial->bUseAlbedoTextureAlpha = true;
		fireBallMaterial->BlendMode = EBlendMode::BM_Additive;

		auto particleSystem = CreateShared<ParticleSystem>();

		auto fireBallEmitter = CreateShared<ParticleEmitter>();

		fireBallEmitter->MinRadius = 20.0f;
		fireBallEmitter->MaxRadius = 100.0f;

		fireBallEmitter->MinStartScale = 300.0f;
		fireBallEmitter->MaxStartScale = 400.0f;

		fireBallEmitter->MinEndScale = 800.0f;
		fireBallEmitter->MaxEndScale = 1000.0f;


		fireBallEmitter->MinVelocity = { 0.0f, 700.0f, 0.0f };
		fireBallEmitter->MaxVelocity = { 0.0f, 1000.0f, 0.0f };

		fireBallEmitter->MinRotationAngle = 0.0f;
		fireBallEmitter->MaxRotationAngle = 52.0f;

		fireBallEmitter->MinLifeTime = 1.0f;
		fireBallEmitter->MaxLifeTime = 2.0f;

		fireBallEmitter->MinSpawnCount = 3;
		fireBallEmitter->MaxSpawnCount = 3;


		fireBallEmitter->Material = fireBallMaterial;
		fireBallEmitter->HorizontalSubImageCount = 6;
		fireBallEmitter->VerticalSubImageCount = 6;


		particleSystem->AddParticleEmitter(fireBallEmitter);


		/*auto smokeMaterial = CreateShared<Crystal::Material>();
		smokeMaterial->AlbedoTexture = ResourceManager::Instance().GetTexture("assets/textures/T_Smoke_SubUV.tga");

		smokeMaterial->bUseAlbedoTextureAlpha = true;
		smokeMaterial->BlendMode = EBlendMode::BM_Translucent;

		auto smokeEmitter = CreateShared<ParticleEmitter>();

		smokeEmitter->MinRadius = 20.0f;
		smokeEmitter->MaxRadius = 100.0f;

		smokeEmitter->MinStartScale = 400.0f;
		smokeEmitter->MaxStartScale = 600.0f;

		smokeEmitter->MinEndScale = 800.0f;
		smokeEmitter->MaxEndScale = 1000.0f;


		smokeEmitter->MinVelocity = { 0.0f, 700.0f, 0.0f };
		smokeEmitter->MaxVelocity = { 0.0f, 1000.0f, 0.0f };

		smokeEmitter->MinRotationAngle = 0.0f;
		smokeEmitter->MaxRotationAngle = 52.0f;

		smokeEmitter->MinLifeTime = 1.0f;
		smokeEmitter->MaxLifeTime = 2.0f;

		smokeEmitter->MinSpawnCount = 3;
		smokeEmitter->MaxSpawnCount = 3;


		smokeEmitter->Material = smokeMaterial;
		smokeEmitter->HorizontalSubImageCount = 8;
		smokeEmitter->VerticalSubImageCount = 8;

		particleSystem->AddParticleEmitter(smokeEmitter);*/

		particleComponent->SetParticleSystem(particleSystem);

		m_MainComponent = particleComponent;
	}

	void ParticleActor::Update(const float deltaTime)
	{
		Actor::Update(deltaTime);


		m_Timer.Tick();

		if (m_Timer.GetElapsedTime() >= 0.7f)
		{
			Destroy();
		}
	}

}
