﻿#include "TestPawn.h"

#include "Missile.h"
#include "Crystal/Types.h"
#include "Crystal/GamePlay/World/Level.h"

BOOST_CLASS_EXPORT(TestPawn)

void TestPawn::Initialize()
{
	Pawn::Initialize();


	auto material = std::make_unique<Crystal::Material>();
	material->ShadingModel = Crystal::EShadingModel::SM_DefaultLit;


	auto sphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingOrientedBoxComponent");
	sphereComponent->SetRadius(50.0f);
	sphereComponent->SetMass(20000.0f);
	sphereComponent->BindOnHitEvent([this](const Crystal::HitResult& hitResult)
	{
			this->m_Health -= 1;
	});


	m_MainComponent = sphereComponent;

	auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("MeshComponent");
	staticMeshComponent->AddMaterial(std::move(material));
	staticMeshComponent->AttachTo(m_MainComponent);

	auto springArmComponent = CreateComponent<Crystal::SpringArmComponent>("SpringArmComponent");
	springArmComponent->SetOffsetPosition({0, 45.0f, -100.0f});
	springArmComponent->AttachTo(m_MainComponent);


	m_CameraComponent = CreateComponent<Crystal::CameraComponent>("CameraComponent");
	m_CameraComponent->SetFieldOfView(90.0f);
	m_CameraComponent->SetNearPlane(20.0f);
	m_CameraComponent->SetViewport({0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f});
	m_CameraComponent->SetFarPlane(100000.0f);
	m_CameraComponent->AttachTo(springArmComponent);

	m_MovementComponent = CreateComponent<Crystal::MovementComponent>("MovementComponent");
	m_MovementComponent->SetTargetComponent(m_MainComponent);
}

void TestPawn::Update(const float deltaTime)
{
	Pawn::Update(deltaTime);

	CS_DEBUG_INFO("%d", m_Health);


	m_FireTimer.Tick();

	if (m_bShouldFire && m_FireTimer.GetElapsedTime() >= m_FireInterval)
	{
		m_FireTimer.Reset();
		OnFire();
	}
}

void TestPawn::SetupInputComponent(Crystal::InputComponent* inputComponent)
{
	Pawn::SetupInputComponent(inputComponent);

	inputComponent->BindAxis("MoveForward", CS_AXIS_FN(TestPawn::MoveForward));
	inputComponent->BindAxis("MoveRight", CS_AXIS_FN(TestPawn::MoveRight));
	inputComponent->BindAxis("MoveUp", CS_AXIS_FN(TestPawn::MoveUp));
	inputComponent->BindAxis("RollRight", CS_AXIS_FN(TestPawn::RollRight));

	inputComponent->BindAxis("LookUp", CS_AXIS_FN(TestPawn::RotatePitch));
	inputComponent->BindAxis("Turn", CS_AXIS_FN(TestPawn::RotateYaw));

	inputComponent->BindAction("Fire", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(TestPawn::BeginFire));
	inputComponent->BindAction("Fire", Crystal::EKeyEvent::KE_Released, CS_ACTION_FN(TestPawn::EndFire));

	inputComponent->BindAction("FireMissile", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(TestPawn::FireMissile));
}

void TestPawn::RotateYaw(float value)
{
	value *= 0.05f;
	m_MainComponent->RotateYaw(value);
}

void TestPawn::RotatePitch(float value)
{
	value *= 0.05f;
	m_MainComponent->RotatePitch(value);
}

void TestPawn::MoveForward(float value)
{
	value *= 3000000;
	DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetLocalForwardVector(), value);
	Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->AddForce(force);
}

void TestPawn::MoveRight(float value)
{
	value *= 3000000;
	DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetLocalRightVector(), value);
	Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->AddForce(force);
}

void TestPawn::MoveUp(float value)
{
	value *= 3000000;
	DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetLocalUpVector(), value);
	Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->AddForce(force);
}

void TestPawn::RollRight(float value)
{
	m_MainComponent->RotateRoll(-value);
}

void TestPawn::BeginFire()
{
	CS_DEBUG_INFO("BeginFire!!");
	m_bShouldFire = true;
}

void TestPawn::EndFire()
{
	CS_DEBUG_INFO("EndFire!!");
	m_bShouldFire = false;
}

void TestPawn::FireMissile()
{
	CS_DEBUG_INFO("FireMissile !!");

	auto level = Crystal::Cast<Crystal::Level>(GetOuter());
	if(level)
	{
		Crystal::Actor::ActorSpawnParams spawnParams = {};

		auto playerPosition = GetPosition();
		spawnParams.Position = { playerPosition.x, playerPosition.y, playerPosition.z + 30.0f };
		spawnParams.Rotation = GetRotation();
		
		auto missile = level->SpawnActor<Missile>(spawnParams).lock();
	
		
		
	}
	
}

void TestPawn::OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser)
{
	auto actorCausedDamage = damageCauser.lock();
	if(!actorCausedDamage)
	{
		return;
	}

	auto staticType = actorCausedDamage->StaticType();
	if(staticType == "TestPawn")
	{
		m_Health -= damage;
	}
	
}

void TestPawn::OnFire()
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
				hitActor->OnTakeDamage(1.0f, Crystal::Cast<Actor>(weak_from_this()));
			}
		}
	}
}
