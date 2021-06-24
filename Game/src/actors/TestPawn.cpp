#include "TestPawn.h"

#include "Crystal/GamePlay/World/Level.h"

BOOST_CLASS_EXPORT(TestPawn)

void TestPawn::Initialize()
{
	Pawn::Initialize();
	

	auto material = std::make_unique<Crystal::Material>();
	material->ShadingModel = Crystal::EShadingModel::ShadingModel_DefaultLit;


	auto boxComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingOrientedBoxComponent");
	boxComponent->SetRadius(908.0f / 2.0f);
	boxComponent->SetMass(20000.0f);

	m_MainComponent = boxComponent;

	auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("MeshComponent");
	staticMeshComponent->AddMaterial(std::move(material));
	staticMeshComponent->AttachTo(m_MainComponent);

	auto springArmComponent = CreateComponent<Crystal::SpringArmComponent>("SpringArmComponent");
	springArmComponent->SetOffsetPosition({ 0, 450.0f, -1500.0f });
	springArmComponent->AttachTo(m_MainComponent);
	

	m_CameraComponent = CreateComponent<Crystal::CameraComponent>("CameraComponent");
	m_CameraComponent->SetFieldOfView(60.0f);
	m_CameraComponent->SetNearPlane(100.0f);
	m_CameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f });
	m_CameraComponent->SetFarPlane(1000000.0f);
	m_CameraComponent->AttachTo(springArmComponent);

	m_MovementComponent = CreateComponent<Crystal::MovementComponent>("MovementComponent");
	m_MovementComponent->SetTargetComponent(m_MainComponent);

}

void TestPawn::Update(const float deltaTime)
{
	Pawn::Update(deltaTime);


	m_FireTimer.Tick();
	
	if(m_bShouldFire && m_FireTimer.GetElapsedTime() >= m_FireInterval)
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
	value *= 30000000;
	DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetLocalForwardVector(), value);
	Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->AddForce(force);
}

void TestPawn::MoveRight(float value)
{
	value *= 30000000;
	DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetLocalRightVector(), value);
	Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->AddForce(force);
}

void TestPawn::MoveUp(float value)
{
	value *= 30000000;
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

void TestPawn::OnFire()
{
	CS_DEBUG_INFO("Fired");
	const auto start = m_CameraComponent->GetWorldPosition();
	const auto direction = m_CameraComponent->GetWorldForwardVector();
	const float maxDistance = 10000.0f;

	auto level = Crystal::Cast<Crystal::Level>(GetOuter());
	if (level)
		level->DrawDebugLine(start, direction, maxDistance, Crystal::Vector3::Green);
}
