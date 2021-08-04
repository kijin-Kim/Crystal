#include "SpaceWhale.h"

#include "MyHUD.h"
#include "Crystal/GamePlay/World/Level.h"

void SpaceWhale::Initialize()
{
	Crystal::Pawn::Initialize();

	auto& resourceManager = Crystal::ResourceManager::Instance();

	auto material = Crystal::CreateShared<Crystal::Material>();
	material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Whale Diffuse.jpg");
	material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Whale Roughness.jpg");
	material->NormalTexture = resourceManager.GetTexture("assets/textures/Whale Normals.jpg");
	material->EmissiveTexture = resourceManager.GetTexture("assets/textures/Whale Emissive.jpg");
	material->ShadingModel = Crystal::EShadingModel::SM_Lit;
	material->BlendMode = Crystal::EBlendMode::BM_Opaque;

	auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
	boundingOrientedBoxComponent->SetExtents({4.9f / 2.0f * 100.0f, 2.8f / 2.0f * 100.0f, 11.0f / 2.0f * 100.0f});
	boundingOrientedBoxComponent->SetMass(80000.0f);
	boundingOrientedBoxComponent->IgnoreActorClassOf("SpaceWhale");
	boundingOrientedBoxComponent->IgnoreActorClassOf("Kraken");


	auto skeletalMeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
	skeletalMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::SkeletalMesh>("assets/models/Biomechanical Whale Animated.fbx"));
	skeletalMeshComponent->PlayAnimation(resourceManager.GetAnimation("assets/models/Biomechanical Whale Animated.fbx"), true);
	skeletalMeshComponent->AddMaterial(material);
	skeletalMeshComponent->SetUnitScale(100.0f);

	skeletalMeshComponent->RotateYaw(180.0f);

	m_MainComponent = boundingOrientedBoxComponent;
	skeletalMeshComponent->AttachTo(m_MainComponent);
}

void SpaceWhale::End()
{
	Crystal::Pawn::End();

	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if (level)
	{
		auto hud = Crystal::Cast<MyHUD>(level->GetHUD());
		if (hud)
		{
			float currGauge = hud->GetPolluteGauge();
			hud->SetPolluteGague(currGauge - 10.0f);
		}
	}
	
}


void SpaceWhale::Update(float deltaTime)
{
	Crystal::Pawn::Update(deltaTime);

	m_PolluteGaugeIncrementTimer.Tick();
	if(m_PolluteGaugeIncrementTimer.GetElapsedTime() >= m_PolluteGaugeIncrementInterval)
	{
		m_PolluteGaugeIncrementTimer.Reset();

		auto level = Crystal::Cast<Crystal::Level>(GetLevel());
		if (level)
		{
			auto hud = Crystal::Cast<MyHUD>(level->GetHUD());
			if (hud)
			{
				float currGauge = hud->GetPolluteGauge();
				hud->SetPolluteGague(currGauge + 1.5f);
			}
		}	
	}
	
}

void SpaceWhale::OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser)
{
	Destroy();
}
