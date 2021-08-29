#include "Cruiser.h"

#include "Crystal/GamePlay/Components/CollisionComponent.h"

void Cruiser::Initialize()
{
	Pawn::Initialize();

	auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
	boundingOrientedBoxComponent->SetExtents({ 600.0f / 2.0f, 580.0f / 2.0f, 2200.0f / 2.0f});
	boundingOrientedBoxComponent->SetMass(100000.0f);

	m_MainComponent = boundingOrientedBoxComponent;

	auto& resourceManager = Crystal::ResourceManager::Instance();


	auto cruiserMaterial = Crystal::CreateShared<Crystal::Material>();
	cruiserMaterial->AlbedoTexture = resourceManager.GetTexture("assets/textures/T_Cruiser_GX5_Dif.tga");
	cruiserMaterial->MetallicTexture = resourceManager.GetTexture("assets/textures/T_Cruiser_GX5_Metallic.tga");
	cruiserMaterial->RoughnessTexture = resourceManager.GetTexture("assets/textures/T_Cruiser_GX5_Roughness.tga");
	cruiserMaterial->NormalTexture = resourceManager.GetTexture("assets/textures/T_Cruiser_GX5_Norm.tga");
	cruiserMaterial->EmissiveTexture = resourceManager.GetTexture("assets/textures/T_Cruiser_GX5_Emissive.tga");

	auto cruiserMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	cruiserMeshComponent->SetLocalPosition({ 0.0f, -50.0f, 0.0f });
	cruiserMeshComponent->AddMaterial(cruiserMaterial);
	cruiserMeshComponent->AttachTo(m_MainComponent);
	cruiserMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_Cruiser_GX5.fbx"));
	cruiserMeshComponent->SetUnitScale(0.1f);
	cruiserMeshComponent->RotateYaw(180.0f);
	cruiserMeshComponent->RotatePitch(90.0f);


	auto bridgeMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	bridgeMeshComponent->SetLocalPosition({ 0.0f, -50.0f, 0.0f });
	bridgeMeshComponent->AddMaterial(cruiserMaterial);
	bridgeMeshComponent->AttachTo(m_MainComponent);
	bridgeMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_Cruiser_GX5_Bridge.fbx"));
	bridgeMeshComponent->SetUnitScale(0.1f);
	bridgeMeshComponent->RotateYaw(180.0f);
	bridgeMeshComponent->RotatePitch(90.0f);

}

void Cruiser::Update(float deltaTime)
{
	Pawn::Update(deltaTime);


	float acceleration = 50000000.0f;
	m_MainComponent->AddForce(Crystal::Vector3::Multiply(Crystal::Vector3::Normalize(GetForwardVector()), acceleration));
}
