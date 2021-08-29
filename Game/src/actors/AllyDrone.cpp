#include "AllyDrone.h"

#include "AllyDroneLaser.h"
#include "Laser.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

void AllyDrone::Initialize()
{
	Crystal::Pawn::Initialize();

	auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
	boundingOrientedBoxComponent->SetExtents({ 90.0f / 2.0f, 30.0f / 2.0f, 85.0f / 2.0f});
	boundingOrientedBoxComponent->SetMass(10000.0f);
	boundingOrientedBoxComponent->SetCollisionType(Crystal::ECollisionType::CT_Block);
	boundingOrientedBoxComponent->BindOnHitEvent([this](const Crystal::HitResult& hitResult)
		{
			/*auto staticType = hitResult.HitActor.lock()->StaticType();
			if (staticType == "BoundingOrientedBoxActor")
			{
				auto droneAIController = Crystal::Cast<DroneAIController>(GetController());
				auto blackboardComponent = droneAIController->GetBlackboardComponent();
				blackboardComponent->SetValueAsFloat3("RandomPositionFromCenter", Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 500.0f));
			}*/
		});


	m_MainComponent = boundingOrientedBoxComponent;

	auto& resourceManager = Crystal::ResourceManager::Instance();

	auto material = Crystal::CreateShared<Crystal::Material>();
	material->BlendMode = Crystal::EBlendMode::BM_Opaque;
	material->ShadingModel = Crystal::EShadingModel::SM_Lit;

	material->AlbedoTexture = resourceManager.GetTexture("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Dif.tga");
	material->MetallicTexture = resourceManager.GetTexture("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Metallic.tga");
	material->RoughnessTexture = resourceManager.GetTexture("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Roughness.tga");
	material->NormalTexture = resourceManager.GetTexture("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Norm.tga");


	auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_Frigate_BE2.fbx"));
	staticMeshComponent->AddMaterial(material);
	staticMeshComponent->AttachTo(m_MainComponent);
	staticMeshComponent->SetUnitScale(1.0f);

	m_FireSocketComponent = CreateComponent<Crystal::TransformComponent>("FireSocketComponent");
	m_FireSocketComponent->SetLocalPosition({ 0.0f, 0.0f, 85.0f });
	m_FireSocketComponent->AttachTo(m_MainComponent);



	auto indicatorMat = Crystal::CreateShared<Crystal::Material>();
	indicatorMat->AlbedoTexture= Crystal::ResourceManager::Instance().GetTexture("assets/textures/AllyDroneIndicator.png");
	indicatorMat->bUseAlbedoTextureAlpha = true;

	m_IndicatorTextueComponent = CreateComponent<Crystal::TextureComponent>("HpBackgroundTextureComponent");
	m_IndicatorTextueComponent->AddMaterial(indicatorMat);
	m_IndicatorTextueComponent->SetScaleX(0.01f);
	m_IndicatorTextueComponent->SetScaleY(0.01f);

}

void AllyDrone::Update(float deltaTime)
{
	Pawn::Update(deltaTime);

	auto level = GetLevel().lock();
	if(level)
	{
		auto playerController = Crystal::Cast<Crystal::PlayerController>(level->GetPlayerController(0));
		if (playerController)
		{
			auto position2D = playerController->ProjectWorldToCameraSpace(GetPosition());
			m_IndicatorTextueComponent->SetWorldPosition({ position2D.x, position2D.y, 2.0f });
		}
	}

	
}

void AllyDrone::OnFire()
{
	Crystal::Actor::ActorSpawnParams spawnParams = {};
	spawnParams.Instigator = Crystal::Cast<Actor>(weak_from_this());

	spawnParams.Position = m_FireSocketComponent->GetWorldPosition();

	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if (level)
	{
		spawnParams.Rotation = GetRotationQuat();
		level->SpawnActor<AllyDroneLaser>(spawnParams);
	}
}
