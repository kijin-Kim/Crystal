#include "Drone.h"

#include "DroneAIController.h"
#include "DroneLaser.h"
#include "MyPlayerPawn.h"
#include "Crystal/GamePlay/AI/Blackboard.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/GamePlay/World/Level.h"


void Drone::Initialize()
{
	Crystal::Pawn::Initialize();

	auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
	boundingOrientedBoxComponent->SetExtents({ 90.0f * 2.0f, 30.0f * 2.0f, 85.0f * 2.0f});
	boundingOrientedBoxComponent->SetMass(10000.0f);
	boundingOrientedBoxComponent->SetCollisionType(Crystal::ECollisionType::CT_Block);
	boundingOrientedBoxComponent->IgnoreActorClassOf("ShieldSphere");
	boundingOrientedBoxComponent->IgnoreActorClassOf("Kraken");

	m_MainComponent = boundingOrientedBoxComponent;

	auto& resourceManager = Crystal::ResourceManager::Instance();

	auto material = Crystal::CreateShared<Crystal::Material>();
	material->BlendMode = Crystal::EBlendMode::BM_Opaque;
	material->ShadingModel = Crystal::EShadingModel::SM_Lit;

	material->AlbedoTexture = resourceManager.GetTexture("assets/textures/T_LightFrigate_GK3_Dif.tga");
	material->MetallicTexture = resourceManager.GetTexture("assets/textures/T_LightFrigate_GK3_M.tga");
	material->RoughnessTexture = resourceManager.GetTexture("assets/textures/T_LightFrigate_GK3_R.tga");
	material->NormalTexture = resourceManager.GetTexture("assets/textures/T_LightFrigate_GK3_Norm.tga");
	

	auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_LightFrigate_GK3.fbx"));
	staticMeshComponent->AddMaterial(material);
	staticMeshComponent->AttachTo(m_MainComponent);
	staticMeshComponent->SetUnitScale(8.0f);

	m_LeftFireSocketComponent = CreateComponent<Crystal::TransformComponent>("LeftFireSocketComponent");
	m_LeftFireSocketComponent->SetLocalPosition({ -26.0f, 0.0f, 49.0f });
	m_LeftFireSocketComponent->AttachTo(staticMeshComponent);

	m_RightFireSocketComponent = CreateComponent<Crystal::TransformComponent>("RightFireSocketComponent");
	m_RightFireSocketComponent->SetLocalPosition({ 26.0f, 0.0f, 49.0f });
	m_RightFireSocketComponent->AttachTo(staticMeshComponent);


	auto barBgMat = Crystal::CreateShared<Crystal::Material>();
	barBgMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hpBarBg.png");
	barBgMat->bUseAlbedoTextureAlpha = true;

	m_HealthBarBgComponent = CreateComponent<Crystal::TextureComponent>("HpBackgroundTextureComponent");
	m_HealthBarBgComponent->AddMaterial(barBgMat);
	m_HealthBarBgComponent->SetScaleX(0.072f);
	m_HealthBarBgComponent->SetScaleY(0.048f);


	auto hpBarFillMat = Crystal::CreateShared<Crystal::Material>();
	hpBarFillMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/HpBarFill.png");
	hpBarFillMat->bUseAlbedoTextureAlpha = true;

	m_HealthBarWidth = hpBarFillMat->AlbedoTexture.lock()->GetWidth();
	m_HealthBarHeight = hpBarFillMat->AlbedoTexture.lock()->GetHeight();

	m_HealthBarFillComponent = CreateComponent<Crystal::TextureComponent>("HpFillTextureComponent");
	m_HealthBarFillComponent->AddMaterial(hpBarFillMat);
	m_HealthBarFillComponent->SetScaleX(0.072f * 0.5f);
	m_HealthBarFillComponent->SetScaleY(0.048f);

	UpdateHealth();
	SetShowHealthBar(false);
}

void Drone::Update(float deltaTime)
{
	Actor::Update(deltaTime);

	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if (level)
	{
		float healthPercent = m_CurrentHealth / m_MaxHealth;
		healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);


		auto playerController = Crystal::Cast<Crystal::PlayerController>(level->GetPlayerController(0));
		auto position2D = playerController->ProjectWorldToCameraSpace(GetPosition());
		position2D.y += 100.0f;
		m_HealthBarBgComponent->SetWorldPosition({ position2D.x, position2D.y, 2.0f });
		m_HealthBarFillComponent->SetWorldPosition({
			position2D.x - m_HealthBarWidth * m_HealthBarBgComponent->GetScale().x * (1.0f - healthPercent), position2D.y, 1.0f
			});
	}


	if (m_bShouldShowHealthBar)
	{
		m_HealthBarShowTimer.Tick();
		if (m_HealthBarShowTimer.GetElapsedTime() >= m_HealthBarShowTime)
		{
			m_HealthBarShowTimer.Reset();
			SetShowHealthBar(false);
		}
	}
		
}

void Drone::OnFire()
{

	Crystal::Actor::ActorSpawnParams spawnParams = {};
	spawnParams.Instigator = Crystal::Cast<Actor>(weak_from_this());

	static bool bUseLeftSocket = false;

	if(bUseLeftSocket)
	{
		spawnParams.Position = m_LeftFireSocketComponent->GetWorldPosition();
		bUseLeftSocket = false;
	}
	else
	{
		spawnParams.Position = m_RightFireSocketComponent->GetWorldPosition();
		bUseLeftSocket = true;
	}

	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if(level)
	{
		auto playerPawn = Crystal::Cast<MyPlayerPawn>(level->GetPlayerPawn());
		auto playerPosition = playerPawn->GetPosition();

		auto direction = Crystal::Vector3::Normalize(Crystal::Vector3::Subtract(playerPosition, spawnParams.Position));

		float angle = Crystal::Vector3::AngleBetweenNormals(Crystal::Vector3::UnitZ, direction);
		auto rotationAxis = Crystal::Vector3::Normalize(Crystal::Vector3::Cross(Crystal::Vector3::UnitZ, direction));


		DirectX::XMFLOAT4 rotation = Crystal::Vector4::Quaternion::Identity;
		if (!Crystal::Vector3::IsZero(rotationAxis) && !Crystal::Vector3::Equal(Crystal::Vector3::UnitZ, direction, 0.001f) && !Crystal::Equal(
			fabs(angle), 0.0f))
		{
			rotation = Crystal::Vector4::QuaternionRotationAxis(rotationAxis, angle);
		}

		spawnParams.Rotation = rotation;
		level->SpawnActor<DroneLaser>(spawnParams);
	}

	
}

void Drone::OnTakeDamage(float damage, Crystal::Weak<Actor> causer)
{
	auto damageCauser = causer.lock();
	if (damageCauser->StaticType() != "MyPlayerPawn")
	{
		return;
	}

	m_CurrentHealth -= damage;
	UpdateHealth();
	SetShowHealthBar(true);
	if (m_CurrentHealth <= 0.0f)
	{
		Destroy();
	}

	auto droneAIController = Crystal::Cast<DroneAIController>(GetController());
	if (droneAIController)
	{
		droneAIController->GetBlackboardComponent()->SetValueAsFloat3("LastSeenLocation", damageCauser->GetPosition());
	}
}

void Drone::SetShowHealthBar(bool bShow)
{
	m_bShouldShowHealthBar = bShow;
	if (m_bShouldShowHealthBar)
	{
		m_HealthBarShowTimer.Reset();
	}

	m_HealthBarBgComponent->SetHiddenInGame(!bShow);
	m_HealthBarFillComponent->SetHiddenInGame(!bShow);
}

void Drone::UpdateHealth() const
{
	float healthPercent = m_CurrentHealth / m_MaxHealth;
	healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);
	m_HealthBarFillComponent->SetScaleX(m_HealthBarBgComponent->GetScale().x * healthPercent);
}
