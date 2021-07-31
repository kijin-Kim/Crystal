#include "MyPlayerPawn.h"

#include "Missile.h"
#include "MyHUD.h"
#include "Crystal/Types.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/GamePlay/Components/MovementComponent.h"

BOOST_CLASS_EXPORT(MyPlayerPawn)

void MyPlayerPawn::Initialize()
{
	Pawn::Initialize();

	auto sphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingOrientedBoxComponent");
	sphereComponent->SetCollisionType(Crystal::ECollisionType::CT_Block);
	sphereComponent->SetRadius(50.0f);
	sphereComponent->SetMass(7000.0f);
	sphereComponent->BindOnHitEvent([this](const Crystal::HitResult& hitResult)
	{
		if (m_bIsInVunlnerable)
			return;

		m_Health -= 1;
		UpdateHealth();
	});


	//auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
	//boundingOrientedBoxComponent->SetExtents({90.0f / 2.0f, 30.0f / 2.0f, 85.0f / 2.0f});
	//boundingOrientedBoxComponent->SetMass(7000.0f);
	//boundingOrientedBoxComponent->BindOnHitEvent([this](const Crystal::HitResult& hitResult)
	//{
	//	if (m_bIsInVunlnerable)
	//		return;

	//	m_Health -= 1;
	//	UpdateHealth();
	//});

	m_MainComponent = sphereComponent;


	auto& resourceManager = Crystal::ResourceManager::Instance();

	auto material = Crystal::CreateShared<Crystal::Material>();
	material->AlbedoTexture = resourceManager.GetTexture("assets/textures/T_Frigate_BE2/T_M_SM_Frigate_BE2_MI_Frigate_BE2_White_BaseColor.tga");
	material->MetallicTexture = resourceManager.GetTexture("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Metallic.tga");
	material->RoughnessTexture = resourceManager.GetTexture("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Roughness.tga");
	material->NormalTexture = resourceManager.GetTexture("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Norm.tga");
	material->ShadingModel = Crystal::EShadingModel::SM_Lit;
	material->BlendMode = Crystal::EBlendMode::BM_Opaque;


	auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("MeshComponent");
	staticMeshComponent->AddMaterial(material);
	staticMeshComponent->AttachTo(m_MainComponent);
	staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_Frigate_BE2.fbx"));


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
}

void MyPlayerPawn::Begin()
{
	Pawn::Begin();
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
	const auto direction = m_CameraComponent->GetWorldForwardVector();
	const float maxDistance = 10000.0f;

	auto level = Crystal::Cast<Crystal::Level>(GetOuter());
	if (level)
	{
		Crystal::HitResult hitResult = {};
		Crystal::CollisionParams collisionParams = {};
		collisionParams.IgnoreActors.push_back(Crystal::Cast<Actor>(shared_from_this()));

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
	}
}

void MyPlayerPawn::UpdateHealth()
{
	m_Health = max(m_Health, 0);
	auto level = Crystal::Cast<Crystal::Level>(GetOuter());
	if (level)
	{
		auto hud = Crystal::Cast<MyHUD>(level->GetHUD());
		if (hud)
		{
			hud->OnHealthUpdated(m_Health, 100);
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
}
