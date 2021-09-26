#include "MyPlayerPawn.h"

#include "Laser.h"
#include "Missile.h"
#include "MyHUD.h"
#include "PlayerShield.h"
#include "Crystal/Types.h"
#include "Crystal/GamePlay/Components/AIComponent.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/GamePlay/Components/MovementComponent.h"
#include "Crystal/GamePlay/Objects/Actors/PostProcessVolumeActor.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"

BOOST_CLASS_EXPORT(MyPlayerPawn)

void MyPlayerPawn::Initialize()
{
	Pawn::Initialize();

	auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
	boundingOrientedBoxComponent->SetExtents({90.0f / 2.0f, 30.0f / 2.0f, 85.0f / 2.0f});
	boundingOrientedBoxComponent->SetMass(3000.0f);

	m_MainComponent = boundingOrientedBoxComponent;


	auto& resourceManager = Crystal::ResourceManager::Instance();

	auto baseMat = Crystal::CreateShared<Crystal::Material>();
	baseMat->AlbedoTexture = resourceManager.GetTexture("assets/textures/T_InterceptorBaseV3_BaseColor.tga");
	baseMat->MetallicTexture = resourceManager.GetTexture("assets/textures/T_InterceptorBaseV3_Metalness.tga");
	baseMat->RoughnessTexture = resourceManager.GetTexture("assets/textures/T_InterceptorBaseV3_Roughness.tga");
	baseMat->NormalTexture = resourceManager.GetTexture("assets/textures/T_InterceptorBaseV3_Normal.tga");
	baseMat->ShadingModel = Crystal::EShadingModel::SM_Lit;
	baseMat->BlendMode = Crystal::EBlendMode::BM_Translucent;

	auto partsMat = Crystal::CreateShared<Crystal::Material>();
	partsMat->AlbedoTexture = resourceManager.GetTexture("assets/textures/T_InterceptorPartsV3_BaseColor.tga");
	partsMat->MetallicTexture = resourceManager.GetTexture("assets/textures/T_InterceptorPartsV3_Metalness.tga");
	partsMat->RoughnessTexture = resourceManager.GetTexture("assets/textures/T_InterceptorPartsV3_Roughness.tga");
	partsMat->NormalTexture = resourceManager.GetTexture("assets/textures/T_InterceptorPartsV3_Normal.tga");
	partsMat->ShadingModel = Crystal::EShadingModel::SM_Lit;
	partsMat->BlendMode = Crystal::EBlendMode::BM_Translucent;


	m_StaticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("MeshComponent");
	m_StaticMeshComponent->AddMaterial(baseMat);
	m_StaticMeshComponent->AddMaterial(partsMat);
	m_StaticMeshComponent->AttachTo(m_MainComponent);
	m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MK3InterceptorMainLite.fbx"));


	m_LeftFireSocketComponent = CreateComponent<Crystal::TransformComponent>("LeftFireSocketComponent");
	m_LeftFireSocketComponent->SetLocalPosition({ -42.0f, -4.1f, 0.0f });
	m_LeftFireSocketComponent->AttachTo(m_StaticMeshComponent);
	
	m_RightFireSocketComponent = CreateComponent<Crystal::TransformComponent>("RightFireSocketComponent");
	m_RightFireSocketComponent->SetLocalPosition({ 42.0f, -4.1f, 0.0f });
	m_RightFireSocketComponent->AttachTo(m_StaticMeshComponent);


	auto springArmComponent = CreateComponent<Crystal::SpringArmComponent>("SpringArmComponent");
	springArmComponent->SetOffsetPosition({0, 45.0f, -100.0f});
	springArmComponent->SetSpringConstant(100.0f);
	springArmComponent->AttachTo(m_MainComponent);
	springArmComponent->SetDamping(0.6f);
	


	m_CameraComponent = CreateComponent<Crystal::CameraComponent>("CameraComponent");
	m_CameraComponent->SetFieldOfView(85.0f);
	m_CameraComponent->SetNearPlane(20.0f);
	m_CameraComponent->SetViewport({0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f});
	m_CameraComponent->SetFarPlane(100000.0f);
	m_CameraComponent->AttachTo(springArmComponent);

	m_MovementComponent = CreateComponent<Crystal::PawnMovementComponent>("MovementComponent");
	m_MovementComponent->SetTargetComponent(m_MainComponent);
	m_MovementComponent->SetMaxAcceleration(3000000.f);


	m_AIPerceptionSourceComponent = CreateComponent<Crystal::AIPerceptionSourceComponent>("AIPerceptionSourceComponent");
	m_AIPerceptionSourceComponent->SetIsHearingEnabled(true);
	m_AIPerceptionSourceComponent->SetIsSightEnabled(true);

}

void MyPlayerPawn::Begin()
{
	Pawn::Begin();

	auto level = GetLevel().lock();
	if(level)
	{
		ActorSpawnParams spawnParams;
		spawnParams.Instigator = Crystal::Cast<Actor>(weak_from_this());

		m_Inventory = level->SpawnActor<Inventory>(spawnParams);
		m_Inventory.lock()->AddItem(EItemType::IT_HealPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_HealPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_HealPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_PowerPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_PowerPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_PowerPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_PowerPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_PowerPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_ShieldPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_ShieldPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_ShieldPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_ShieldPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_ShieldPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_ShieldPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_ShieldPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_ShieldPotion);
		m_Inventory.lock()->AddItem(EItemType::IT_Ore);
		m_Inventory.lock()->AddItem(EItemType::IT_Ore);
		m_Inventory.lock()->AddItem(EItemType::IT_Ore);
	}
	
	UpdateHealth();
}



void MyPlayerPawn::Update(const float deltaTime)
{
	Pawn::Update(deltaTime);

	m_FireTimer.Tick();

	if (m_bShouldFire && m_FireTimer.GetElapsedTime() >= m_FireInterval)
	{
		m_FireTimer.Reset();
		OnFire();
	}


	if(!m_PlayerShield.expired())
	{
		auto shield = Crystal::Cast<PlayerShield>(m_PlayerShield);

		auto direction = Crystal::Vector3::Normalize(Crystal::Vector3::Subtract(m_StaticMeshComponent->GetWorldPosition(), m_CameraComponent->GetWorldPosition()));
		shield->SetPosition(Crystal::Vector3::Add(m_StaticMeshComponent->GetWorldPosition(), Crystal::Vector3::Multiply(direction, 2.0f)));
	}
	else
	{
		auto shieldPostProcess = Crystal::Cast<Crystal::PostProcessVolumeActor>(ShieldPostProcess);
		if(shieldPostProcess)
		{
			shieldPostProcess->SetHiddenInGame(true);
		}
		
	}


	AddDamagedOpacityMultiplier(-deltaTime);
	AddHealOpacityMultiplier(-deltaTime);

	if(m_bShouldHeal)
	{
		m_HealIntervalTimer.Tick();
		if(m_HealIntervalTimer.GetElapsedTime() >= m_HealInterval)
		{
			m_HealIntervalTimer.Reset();
			AddHealOpacityMultiplier(0.7f);
			m_Health += m_HealAmount;
			m_Health = min(m_Health, m_MaxHealth);
			UpdateHealth();
		}

		m_HealTimer.Tick();
		if (m_HealTimer.GetElapsedTime() >= m_MaxHealTime)
		{
			m_HealTimer.Reset();
			m_bShouldHeal = false;
		}
	}



	const auto start = m_CameraComponent->GetWorldPosition();
	const auto direction = Crystal::Vector3::Normalize(m_CameraComponent->GetWorldForwardVector());
	float maxDistance = 50000.0f;

	auto level = Crystal::Cast<Crystal::Level>(GetOuter());
	if (level)
	{
		Crystal::HitResult hitResult = {};
		Crystal::CollisionParams collisionParams = {};
		collisionParams.IgnoreActors.push_back(Crystal::Cast<Actor>(shared_from_this()));
		collisionParams.IgnoreClasses.push_back("BoundingOrientedBoxActor");
		collisionParams.IgnoreClasses.push_back("PolluteSphere");
		collisionParams.IgnoreClasses.push_back("Laser");
		collisionParams.IgnoreClasses.push_back("DroneLaser");
		collisionParams.IgnoreClasses.push_back("AllyDroneLaser");
		collisionParams.IgnoreClasses.push_back("Asteroid");
		collisionParams.IgnoreClasses.push_back("WhiteAsteroid");
		collisionParams.IgnoreClasses.push_back("SpaceStationCoreActor");
		collisionParams.IgnoreClasses.push_back("DummySpaceStation");
		collisionParams.IgnoreClasses.push_back("DummyStargate");
		collisionParams.IgnoreClasses.push_back("Sun");


		DirectX::XMFLOAT3 endDirection = m_CameraComponent->GetWorldForwardVector();

		bool result = level->LineTraceSingle(hitResult, start, direction, maxDistance, collisionParams);
		if (result)
		{
			auto hitActor = hitResult.HitActor.lock();
			m_HoveredActor = hitActor;
			m_HoveredDistance = maxDistance;
			Crystal::Cast<MyHUD>(level->GetHUD())->OnActorHovered(hitActor, maxDistance);
		}
		else
		{
			m_HoveredActor = {};
			m_HoveredDistance = 0.0f;
			Crystal::Cast<MyHUD>(level->GetHUD())->OnActorNotHovered();
		}
	}


}

void MyPlayerPawn::SetupInputComponent(Crystal::InputComponent* inputComponent)
{
	Pawn::SetupInputComponent(inputComponent);

	inputComponent->BindAxis("MoveForward", CS_AXIS_FN(MyPlayerPawn::MoveForward));
	inputComponent->BindAxis("MoveRight", CS_AXIS_FN(MyPlayerPawn::MoveRight));
	inputComponent->BindAxis("MoveUp", CS_AXIS_FN(MyPlayerPawn::MoveUp));
	inputComponent->BindAxis("RollRight", CS_AXIS_FN(MyPlayerPawn::RollRight));

	inputComponent->BindAxis("LookUp", CS_AXIS_FN(MyPlayerPawn::RotatePitch));
	inputComponent->BindAxis("Turn", CS_AXIS_FN(MyPlayerPawn::RotateYaw));

	inputComponent->BindAction("Fire", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(MyPlayerPawn::BeginFire));
	inputComponent->BindAction("Fire", Crystal::EKeyEvent::KE_Released, CS_ACTION_FN(MyPlayerPawn::EndFire));

	inputComponent->BindAction("ToggleMenu", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(MyPlayerPawn::ToggleMenu));

	inputComponent->BindAction("UsePowerItem", Crystal::EKeyEvent::KE_Released, CS_ACTION_FN(MyPlayerPawn::UsePowerItem));
	inputComponent->BindAction("UseHealItem", Crystal::EKeyEvent::KE_Released, CS_ACTION_FN(MyPlayerPawn::UseHealItem));
	inputComponent->BindAction("UseShieldItem", Crystal::EKeyEvent::KE_Released, CS_ACTION_FN(MyPlayerPawn::UseShieldItem));

	inputComponent->BindAction("ShowDebugCollision", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(MyPlayerPawn::ShowDebugCollision));
	inputComponent->BindAction("HideDebugCollision", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(MyPlayerPawn::HideDebugCollision));
	inputComponent->BindAction("ShowDebugAI", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(MyPlayerPawn::ShowDebugAI));
	inputComponent->BindAction("HideDebugAI", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(MyPlayerPawn::HideDebugAI));

	inputComponent->BindAction("Interact", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(MyPlayerPawn::OnInteractWithHovered));
	inputComponent->BindAction("OpenNewLevel", Crystal::EKeyEvent::KE_Released, CS_ACTION_FN(MyPlayerPawn::OpenTitleLevel));
}

void MyPlayerPawn::RotateYaw(float value)
{
	value *= 0.05f;
	m_MainComponent->RotateYaw(value);
}

void MyPlayerPawn::RotatePitch(float value)
{
	value *= 0.05f;
	m_MainComponent->RotatePitch(value);
}

void MyPlayerPawn::MoveForward(float value)
{
	AddInputVector(m_MainComponent->GetLocalForwardVector(), value);
}

void MyPlayerPawn::MoveRight(float value)
{
	AddInputVector(m_MainComponent->GetLocalRightVector(), value);
}

void MyPlayerPawn::MoveUp(float value)
{
	AddInputVector(m_MainComponent->GetLocalUpVector(), value);
}

void MyPlayerPawn::RollRight(float value)
{
	m_MainComponent->RotateRoll(-value);
}

void MyPlayerPawn::BeginFire()
{
	m_bShouldFire = true;
}

void MyPlayerPawn::EndFire()
{
	CS_DEBUG_INFO("EndFire!!");
	m_bShouldFire = false;
}

void MyPlayerPawn::ToggleMenu()
{
}

void MyPlayerPawn::OpenTitleLevel()
{
	CS_DEBUG_INFO("Released");
	auto world = Crystal::Cast<Crystal::World>(GetWorld());
	world->PopLevel();
}

void MyPlayerPawn::OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser)
{
	auto actorCausedDamage = damageCauser.lock();
	if (!actorCausedDamage)
	{
		return;
	}

	auto staticType = actorCausedDamage->StaticType();

	auto shield = Crystal::Cast<PlayerShield>(m_PlayerShield);

	if (!shield)
	{
		m_Health -= damage;
		UpdateHealth();
		AddDamagedOpacityMultiplier(0.7f);
	}
	else
	{
		shield->OnTakeDamage(damage, damageCauser);
	}
}

Crystal::Weak<Inventory> MyPlayerPawn::GetInventory() const
{
	return m_Inventory;
}

void MyPlayerPawn::OnInteractWithHovered()
{
	auto hovered = m_HoveredActor.lock();
	if(hovered)
	{
		auto type = hovered->StaticType();
		if (type == "SpaceStation")
		{
			auto world = GetWorld().lock();
			if (world)
			{
				if (world->GetCurrentLevel()->GetObjectName() == "MainLevel")
				{
					world->PushLevel("ShopLevel");
				}
			}

		} else if(type == "Stargate")
		{
			auto world = GetWorld().lock();
			if(world)
			{
				if(world->GetCurrentLevel()->GetObjectName() == "MainLevel")
				{
					world->PushLevel("StargateLevel");
				}
			}
		}
	}
	
}

void MyPlayerPawn::OnFire()
{
	CS_DEBUG_INFO("Fired");
	const auto start = m_CameraComponent->GetWorldPosition();
	const auto direction = Crystal::Vector3::Normalize(m_CameraComponent->GetWorldForwardVector());
	float maxDistance = 50000.0f;

	
	
	auto level = Crystal::Cast<Crystal::Level>(GetOuter());
	if (level)
	{
		Crystal::HitResult hitResult = {};
		Crystal::CollisionParams collisionParams = {};
		collisionParams.IgnoreActors.push_back(Crystal::Cast<Actor>(shared_from_this()));
		collisionParams.IgnoreClasses.push_back("BoundingOrientedBoxActor");
		collisionParams.IgnoreClasses.push_back("PolluteSphere");
		collisionParams.IgnoreClasses.push_back("Laser");
		collisionParams.IgnoreClasses.push_back("DroneLaser");
		collisionParams.IgnoreClasses.push_back("AllyDroneLaser");

		Crystal::Actor::ActorSpawnParams spawnParams = {};

		if(m_bUseLeftSocket)
		{
			spawnParams.Position = m_LeftFireSocketComponent->GetWorldPosition();
			m_bUseLeftSocket = false;
		}
		else
		{
			spawnParams.Position = m_RightFireSocketComponent->GetWorldPosition();
			m_bUseLeftSocket = true;
		}

		DirectX::XMFLOAT3 endDirection = m_CameraComponent->GetWorldForwardVector();
		bool result = level->LineTraceSingle(hitResult, start, direction, maxDistance, collisionParams);

		if (result)
		{
			auto hitActor = hitResult.HitActor.lock();
			auto hitComponent = hitResult.HitComponent.lock();
			
			
			if (hitActor && hitComponent)
			{
				hitActor->OnTakeDamage(m_Power, Crystal::Cast<Actor>(weak_from_this()));
			}

			Crystal::Cast<MyHUD>(level->GetHUD())->OnPlayerHitSucceed();
		}


		
		
		auto angle = Crystal::Vector3::AngleBetweenNormals(Crystal::Vector3::UnitZ, endDirection);
		auto rotationAxis = Crystal::Vector3::Normalize(Crystal::Vector3::Cross(Crystal::Vector3::UnitZ, endDirection));

		DirectX::XMFLOAT4 rotation = Crystal::Vector4::Quaternion::Identity;

		if (!Crystal::Vector3::IsZero(rotationAxis) && !Crystal::Vector3::Equal(Crystal::Vector3::UnitZ, endDirection, 0.001f) && !Crystal::Equal(
			fabs(angle), 0.0f))
		{
			rotation = Crystal::Vector4::QuaternionRotationAxis(rotationAxis, angle);
		}
		
		spawnParams.Rotation = rotation;
		level->SpawnActor<Laser>(spawnParams);
	}
}

void MyPlayerPawn::AddDamagedOpacityMultiplier(float opacity)
{
	auto damagedPostProcess = DamagedPostProcessActor.lock();
	if (damagedPostProcess)
	{
		auto postProcessComponent = damagedPostProcess->GetPostProcessComponent();
		auto mat = postProcessComponent->GetMaterials()[0];
		mat->OpacityMultiplier += opacity;
		mat->OpacityMultiplier = std::clamp(mat->OpacityMultiplier, 0.0f, 1.0f);
	}
}

void MyPlayerPawn::AddHealOpacityMultiplier(float opacity)
{
	auto healPostProcess = HealPostProcessActor.lock();
	if (healPostProcess)
	{
		auto postProcessComponent = healPostProcess->GetPostProcessComponent();
		auto mat = postProcessComponent->GetMaterials()[0];
		mat->OpacityMultiplier += opacity;
		mat->OpacityMultiplier = std::clamp(mat->OpacityMultiplier, 0.0f, 1.0f);
	}
}

void MyPlayerPawn::UpdateHealth()
{
	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if (level)
	{
		auto hud = Crystal::Cast<MyHUD>(level->GetHUD());
		if (hud)
		{
			hud->OnPlayerHealthUpdated(m_Health, m_MaxHealth);
		}
	}
}


void MyPlayerPawn::UsePowerItem()
{
	auto inventory = m_Inventory.lock();
	if(!inventory)
	{
		return;
	}

	uint32_t count = inventory->GetItemCount(EItemType::IT_PowerPotion);
	if(count <= 0)
	{
		return;
	}

	inventory->RemoveItem(EItemType::IT_PowerPotion);

	m_Power *= 1.2f;
}

void MyPlayerPawn::UseHealItem()
{
	auto inventory = m_Inventory.lock();
	if (!inventory)
	{
		return;
	}

	uint32_t count = inventory->GetItemCount(EItemType::IT_HealPotion);
	if (count <= 0)
	{
		return;
	}

	inventory->RemoveItem(EItemType::IT_HealPotion);


	m_bShouldHeal = true;
	m_HealTimer.Reset();
	m_HealIntervalTimer.Reset();
}

void MyPlayerPawn::UseShieldItem()
{
	auto inventory = m_Inventory.lock();
	if (!inventory)
	{
		return;
	}

	uint32_t count = inventory->GetItemCount(EItemType::IT_ShieldPotion);
	if (count <= 0)
	{
		return;
	}

	inventory->RemoveItem(EItemType::IT_ShieldPotion);


	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if (level)
	{
		ActorSpawnParams spawnParams;
		spawnParams.Position = GetPosition();
		m_PlayerShield = level->SpawnActor<PlayerShield>(spawnParams);
	}

	auto shieldPostProcess = Crystal::Cast<Crystal::PostProcessVolumeActor>(ShieldPostProcess);
	if(shieldPostProcess)
	{
		shieldPostProcess->SetHiddenInGame(false);
	}

}

void MyPlayerPawn::ShowDebugCollision()
{
	auto world = GetWorld().lock();
	if (!world)
	{
		return;
	}

	world->SetShowDebugCollision(true);
}

void MyPlayerPawn::HideDebugCollision()
{
	auto world = GetWorld().lock();
	if (!world)
	{
		return;
	}

	world->SetShowDebugCollision(false);
}

void MyPlayerPawn::ShowDebugAI()
{
	auto world = GetWorld().lock();
	if (!world)
	{
		return;
	}

	auto& worldConfig = world->GetWorldConfig();
	world->SetShowDebugAI(true);
}

void MyPlayerPawn::HideDebugAI()
{
	auto world = GetWorld().lock();
	if (!world)
	{
		return;
	}

	auto& worldConfig = world->GetWorldConfig();
	world->SetShowDebugAI(false);
}

