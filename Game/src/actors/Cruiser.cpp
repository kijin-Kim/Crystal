#include "Cruiser.h"

#include "MyHUD.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/World/Level.h"

void Cruiser::Begin()
{
	Pawn::Begin();
	UpdateHealth();
}

void Cruiser::Initialize()
{
	Pawn::Initialize();

	auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
	boundingOrientedBoxComponent->SetExtents({600.0f / 2.0f, 580.0f / 2.0f, 2200.0f / 2.0f});
	boundingOrientedBoxComponent->SetMass(100000.0f);

	m_MainComponent = boundingOrientedBoxComponent;

	m_OuterSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
	m_OuterSphereComponent->SetRadius(5000.0f);
	m_OuterSphereComponent->SetCollisionType(Crystal::ECollisionType::CT_Overlap);
	m_OuterSphereComponent->AttachTo(m_MainComponent);
	m_OuterSphereComponent->IgnoreActorClassOf("Laser");
	m_OuterSphereComponent->IgnoreActorClassOf("DroneLaser");

	auto& resourceManager = Crystal::ResourceManager::Instance();


	auto cruiserMaterial = Crystal::CreateShared<Crystal::Material>();
	cruiserMaterial->AlbedoTexture = resourceManager.GetTexture("assets/textures/T_Cruiser_GX5_Dif.tga");
	cruiserMaterial->MetallicTexture = resourceManager.GetTexture("assets/textures/T_Cruiser_GX5_Metallic.tga");
	cruiserMaterial->RoughnessTexture = resourceManager.GetTexture("assets/textures/T_Cruiser_GX5_Roughness.tga");
	cruiserMaterial->NormalTexture = resourceManager.GetTexture("assets/textures/T_Cruiser_GX5_Norm.tga");
	cruiserMaterial->EmissiveTexture = resourceManager.GetTexture("assets/textures/T_Cruiser_GX5_Emissive.tga");

	auto cruiserMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	cruiserMeshComponent->SetLocalPosition({0.0f, -50.0f, 0.0f});
	cruiserMeshComponent->AddMaterial(cruiserMaterial);
	cruiserMeshComponent->AttachTo(m_MainComponent);
	cruiserMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_Cruiser_GX5.fbx"));
	cruiserMeshComponent->SetUnitScale(0.1f);
	cruiserMeshComponent->RotateYaw(180.0f);
	cruiserMeshComponent->RotatePitch(90.0f);


	auto bridgeMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	bridgeMeshComponent->SetLocalPosition({0.0f, -50.0f, 0.0f});
	bridgeMeshComponent->AddMaterial(cruiserMaterial);
	bridgeMeshComponent->AttachTo(m_MainComponent);
	bridgeMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_Cruiser_GX5_Bridge.fbx"));
	bridgeMeshComponent->SetUnitScale(0.1f);
	bridgeMeshComponent->RotateYaw(180.0f);
	bridgeMeshComponent->RotatePitch(90.0f);


	m_AIPerceptionSourceComponent = CreateComponent<Crystal::AIPerceptionSourceComponent>("AIPerceptionSourceComponent");
	m_AIPerceptionSourceComponent->SetIsHearingEnabled(true);
	m_AIPerceptionSourceComponent->SetIsSightEnabled(true);
}

void Cruiser::Update(float deltaTime)
{
	Pawn::Update(deltaTime);

	int overlappedCount = 0;

	auto& overlappeds = m_OuterSphereComponent->GetOverlappedComponents();
	for (auto& weak : overlappeds)
	{
		auto overlapped = weak.lock();
		if (overlapped)
		{
			auto outer = overlapped->GetOuter().lock();
			if (outer)
			{
				if (outer->StaticType() == "Drone")
				{
					overlappedCount++;
				}
			}
		}
	}

	if (overlappedCount <= 5)
	{
		DirectX::XMFLOAT3 targetLocation = {0.0f, 0.0f, 40000.0f};
		auto newFacing = Crystal::Vector3::Normalize(Crystal::Vector3::Subtract(targetLocation, GetPosition()));

		auto normalizedForward = Crystal::Vector3::Normalize(GetForwardVector());
		auto rotationAxis = Crystal::Vector3::Normalize(Crystal::Vector3::Cross(normalizedForward, newFacing));

		if (!Crystal::Vector3::IsZero(rotationAxis) && !Crystal::Vector3::Equal(normalizedForward, newFacing, 0.001f))
		{
			auto angle = Crystal::Vector3::AngleBetweenNormals(normalizedForward, newFacing);
			if (fabs(angle) >= DirectX::XMConvertToRadians(10.0f))
			{
				auto quat = Crystal::Vector4::QuaternionRotationAxis(rotationAxis, angle);

				auto rotation = GetRotationQuat();
				auto newQuat = Crystal::Vector4::QuaternionMultiply(rotation, quat);
				SetRotationQuat(newQuat);
			}
		}

		float acceleration = 50000000.0f;
		m_MainComponent->AddForce(Crystal::Vector3::Multiply(Crystal::Vector3::Normalize(GetForwardVector()), acceleration));
		OnCruiserMoving();
	}
	else
	{
		OnCruiserStopped();
	}
}

void Cruiser::OnCruiserMoving()
{
	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if (level)
	{
		auto hud = Crystal::Cast<MyHUD>(level->GetHUD());
		if (hud)
		{
			hud->OnCruiserMoving();
		}
	}
}

void Cruiser::OnCruiserStopped()
{
	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if (level)
	{
		auto hud = Crystal::Cast<MyHUD>(level->GetHUD());
		if (hud)
		{
			hud->OnCruiserStopped();
		}
	}
}


void Cruiser::UpdateHealth()
{
	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if (level)
	{
		auto hud = Crystal::Cast<MyHUD>(level->GetHUD());
		if (hud)
		{
			hud->OnCruiserHealthUpdated(m_CurrentHealth, m_MaxHealth);
		}
	}
}

void Cruiser::OnTakeDamage(float damage, Crystal::Weak<Actor> causer)
{
	auto damageCauser = causer.lock();
	if (!damageCauser)
	{
		return;
	}


	if (damageCauser->StaticType() != "Drone")
	{
		return;
	}

	m_CurrentHealth -= damage;

	UpdateHealth();
}
