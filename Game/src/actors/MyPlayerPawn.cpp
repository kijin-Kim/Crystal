#include "MyPlayerPawn.h"

#include "Laser.h"
#include "Missile.h"
#include "MyHUD.h"
#include "PlayerShield.h"
#include "Crystal/Types.h"
#include "Crystal/GamePlay/Components/AIComponent.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/GamePlay/Components/MovementComponent.h"

BOOST_CLASS_EXPORT(MyPlayerPawn)

void MyPlayerPawn::Initialize()
{
	Pawn::Initialize();


	auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
	boundingOrientedBoxComponent->SetExtents({90.0f / 2.0f, 30.0f / 2.0f, 85.0f / 2.0f});
	boundingOrientedBoxComponent->SetMass(7000.0f);
	/*boundingOrientedBoxComponent->BindOnHitEvent([this](const Crystal::HitResult& hitResult)
	{
		if (m_bIsInVunlnerable)
			return;

		m_Health -= 1;
		UpdateHealth();
	});*/

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


	m_LeftSocketComponent = CreateComponent<Crystal::TransformComponent>("LeftSocketComponent");
	m_LeftSocketComponent->SetLocalPosition({ -42.0f, -4.1f, 0.0f });
	m_LeftSocketComponent->AttachTo(m_StaticMeshComponent);
	
	m_RightSocketComponent = CreateComponent<Crystal::TransformComponent>("RightSocketComponent");
	m_RightSocketComponent->SetLocalPosition({ 42.0f, -4.1f, 0.0f });
	m_RightSocketComponent->AttachTo(m_StaticMeshComponent);


	//m_ThrustMat = Crystal::CreateShared<Crystal::Material>();
	//m_ThrustMat->EmissiveColor = { 255.0f / 255.0f * 1.0f, 127.0f / 255.0f * 1.0f, 0.0f };
	//m_ThrustMat->OpacityTexture = resourceManager.GetTexture("assets/textures/TilingNoise05.tga");
	//m_ThrustMat->ShadingModel = Crystal::EShadingModel::SM_Unlit;
	//m_ThrustMat->BlendMode = Crystal::EBlendMode::BM_Translucent;


	//m_ThrustsStaticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("ThrustStaticMeshComponent");
	//m_ThrustsStaticMeshComponent->SetLocalPosition({ 0.0f, 4.0f, -30.0f});
	//m_ThrustsStaticMeshComponent->AddMaterial(m_ThrustMat);
	//m_ThrustsStaticMeshComponent->AttachTo(m_StaticMeshComponent);
	//m_ThrustsStaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_RocketFlare.fbx"));
	//m_ThrustsStaticMeshComponent->SetUnitScale(1.0f);
	

	auto springArmComponent = CreateComponent<Crystal::SpringArmComponent>("SpringArmComponent");
	springArmComponent->SetOffsetPosition({0, 45.0f, -100.0f});
	springArmComponent->AttachTo(m_MainComponent);


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
	
	m_Health = m_MaxHealth;
	UpdateHealth();
}

void MyPlayerPawn::Update(const float deltaTime)
{
	Pawn::Update(deltaTime);

	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if(level)
	{
		auto myHud = Crystal::Cast<MyHUD>(level->GetHUD());
		if(myHud->GetPolluteGauge() >= myHud->GetMaxPolluteGauge())
		{
			m_Health -= 0.5f;
			UpdateHealth();
		}
	}
	

	
	m_FireTimer.Tick();

	if (m_bShouldFire && m_FireTimer.GetElapsedTime() >= m_FireInterval)
	{
		m_FireTimer.Reset();
		OnFire();
	}


	if(!m_PlayerShield.expired())
	{
		m_PlayerShield.lock()->SetPosition(Crystal::Vector3::Add(GetPosition(), { 0.0f, 0.0f, -0.5f}));
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

	inputComponent->BindAction("FireMissile", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(MyPlayerPawn::FireMissile));

	inputComponent->BindAction("UsePowerItem", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(MyPlayerPawn::UsePowerItem));
	inputComponent->BindAction("UseHealItem", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(MyPlayerPawn::UseHealItem));
	inputComponent->BindAction("UseShieldItem", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(MyPlayerPawn::UseShieldItem));

	inputComponent->BindAction("ShowDebugCollision", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(MyPlayerPawn::ToggleShowDebugCollision));
	inputComponent->BindAction("ShowDebugAI", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(MyPlayerPawn::ToggleShowDebugAI));
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
	CS_DEBUG_INFO("BeginFire!!");
	m_AIPerceptionSourceComponent->MakeNoiseAtLocation(m_MainComponent->GetWorldPosition(), 100.0f);
	m_bShouldFire = true;
}

void MyPlayerPawn::EndFire()
{
	CS_DEBUG_INFO("EndFire!!");
	m_bShouldFire = false;
}

void MyPlayerPawn::FireMissile()
{
	CS_DEBUG_INFO("FireMissile !!");

	auto level = Crystal::Cast<Crystal::Level>(GetOuter());
	if (level)
	{
		Crystal::Actor::ActorSpawnParams spawnParams = {};

		auto playerPosition = GetPosition();
		spawnParams.Position = {playerPosition.x, playerPosition.y, playerPosition.z + 30.0f};
		spawnParams.Rotation = Crystal::Vector4::QuaternionMultiply(GetRotationQuat(), m_CameraComponent->GetRotationQuat());

		auto missile = level->SpawnActor<Missile>(spawnParams).lock();
	}
}

void MyPlayerPawn::OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser)
{
	if (m_bIsInVunlnerable)
		return;

	auto actorCausedDamage = damageCauser.lock();
	if (!actorCausedDamage)
	{
		return;
	}

	auto staticType = actorCausedDamage->StaticType();
	if (staticType == "MyPlayerPawn")
	{
		m_Health -= damage;
	}
}

void MyPlayerPawn::OnFire()
{
	CS_DEBUG_INFO("Fired");
	const auto start = m_CameraComponent->GetWorldPosition();
	const auto direction = Crystal::Vector3::Normalize(m_CameraComponent->GetWorldForwardVector());
	const float maxDistance = 100000.0f;

	static bool bUseLeftSocket = true;
	
	auto level = Crystal::Cast<Crystal::Level>(GetOuter());
	if (level)
	{
		Crystal::HitResult hitResult = {};
		Crystal::CollisionParams collisionParams = {};
		collisionParams.IgnoreActors.push_back(Crystal::Cast<Actor>(shared_from_this()));
		collisionParams.IgnoreClasses.push_back("PolluteCircle");


		Crystal::Actor::ActorSpawnParams spawnParams = {};

		if(bUseLeftSocket)
		{
			spawnParams.Position = m_LeftSocketComponent->GetWorldPosition();
			bUseLeftSocket = false;
		}
		else
		{
			spawnParams.Position = m_RightSocketComponent->GetWorldPosition();
			bUseLeftSocket = true;
		}

		DirectX::XMFLOAT3 endDirection = m_CameraComponent->GetWorldForwardVector();
		bool result = level->LineTraceSingle(hitResult, start, direction, maxDistance, collisionParams);
		if (result)
		{
			auto hitActor = hitResult.HitActor.lock();
			if (hitActor)
			{
				if (hitActor->StaticType() == "HealAsteroid")
				{
					m_bHasItem[ItemType_Heal] = true;
					UpdateItemStatus(ItemType_Heal, true);
				}
				else if (hitActor->StaticType() == "PowerAsteroid")
				{
					m_bHasItem[ItemType_Power] = true;
					UpdateItemStatus(ItemType_Power, true);
				}
				else if (hitActor->StaticType() == "ShieldAsteroid")
				{
					m_bHasItem[ItemType_Shield] = true;
					UpdateItemStatus(ItemType_Shield, true);
				}

				hitActor->OnTakeDamage(m_Power, Crystal::Cast<Actor>(weak_from_this()));
			}
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

void MyPlayerPawn::UpdateItemStatus(ItemType itemType, bool bAcquired)
{
	auto level = Crystal::Cast<Crystal::Level>(GetOuter());
	if (!level)
	{
		return;
	}

	auto hud = Crystal::Cast<MyHUD>(level->GetHUD());
	if (!hud)
	{
		return;
	}


	if (bAcquired)
	{
		hud->OnItemAcquired(itemType);
	}
	else
	{
		hud->OnItemUsed(itemType);
	}
}

void MyPlayerPawn::UsePowerItem()
{
	if (!m_bHasItem[ItemType_Power])
	{
		return;
	}

	m_bHasItem[ItemType_Power] = false;
	UpdateItemStatus(ItemType_Power, false);
	m_Power += 2.0f;
}

void MyPlayerPawn::UseHealItem()
{
	if (!m_bHasItem[ItemType_Heal])
	{
		return;
	}

	m_bHasItem[ItemType_Heal] = false;
	UpdateItemStatus(ItemType_Heal, false);
	m_Health += 10;
	m_Health = min(m_Health, 100);
	UpdateHealth();
}

void MyPlayerPawn::UseShieldItem()
{
	if (!m_bHasItem[ItemType_Shield])
	{
		return;
	}

	m_bHasItem[ItemType_Shield] = false;
	UpdateItemStatus(ItemType_Shield, false);
	m_bIsInVunlnerable = true;


	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if (level)
	{
		ActorSpawnParams spawnParams;
		spawnParams.Position = GetPosition();
		//m_PlayerShield = level->SpawnActor<PlayerShield>(spawnParams);
	}

}

void MyPlayerPawn::ToggleShowDebugCollision()
{
	auto world = GetWorld().lock();
	if (!world)
	{
		return;
	}

	auto& worldConfig = world->GetWorldConfig();
	world->SetShowDebugCollision(!worldConfig.bShowDebugCollision);
}

void MyPlayerPawn::ToggleShowDebugAI()
{
	auto world = GetWorld().lock();
	if (!world)
	{
		return;
	}

	auto& worldConfig = world->GetWorldConfig();
	world->SetShowDebugAI(!worldConfig.bShowDebugAI);
}
