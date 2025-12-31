#include "SpaceStationCoreActor.h"

#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

void SpaceStationCoreActor::Initialize()
{
	Actor::Initialize();


	auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
	boundingOrientedBoxComponent->SetExtents({ 600.0f / 2.0f, 580.0f / 2.0f, 2200.0f / 2.0f });
	boundingOrientedBoxComponent->SetMass(100000.0f);

	m_MainComponent = boundingOrientedBoxComponent;

	auto& resourceManager = Crystal::ResourceManager::Instance();


	auto baseMaterial1 = Crystal::CreateShared<Crystal::Material>();
	baseMaterial1->AlbedoTexture = resourceManager.GetTexture("assets/textures/T_MSS_1_Dif.tga");
	baseMaterial1->MetallicTexture = resourceManager.GetTexture("assets/textures/T_MSS_1_Metal.tga");
	baseMaterial1->RoughnessTexture = resourceManager.GetTexture("assets/textures/T_MSS_1_Rough.tga");
	baseMaterial1->EmissiveTexture = resourceManager.GetTexture("assets/textures/T_MSS_1_Glow.tga");
	baseMaterial1->NormalTexture = resourceManager.GetTexture("assets/textures/T_MSS_1_Norm.tga");

	auto baseMaterial2 = Crystal::CreateShared<Crystal::Material>();
	baseMaterial2->AlbedoTexture = resourceManager.GetTexture("assets/textures/T_MSS_2_Dif.tga");
	baseMaterial2->MetallicTexture = resourceManager.GetTexture("assets/textures/T_MSS_2_Metal.tga");
	baseMaterial2->RoughnessTexture = resourceManager.GetTexture("assets/textures/T_MSS_2_Rough.tga");
	baseMaterial2->NormalTexture = resourceManager.GetTexture("assets/textures/T_MSS_2_Norm.tga");


	auto towerMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("TowerMeshComponent");
	towerMeshComponent->AddMaterial(baseMaterial1);
	towerMeshComponent->AttachTo(m_MainComponent);
	towerMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Tower.fbx"));
	towerMeshComponent->RotateYaw(180.0f);
	towerMeshComponent->RotatePitch(90.0f);





	float angle = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		auto habitatMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("HabitatMeshComponent");
		habitatMeshComponent->SetLocalPosition({ 0.0f, 0.0f, -1620.0f });
		habitatMeshComponent->AddMaterial(baseMaterial1);
		habitatMeshComponent->AttachTo(towerMeshComponent);
		habitatMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Habitat.fbx"));
		habitatMeshComponent->RotateRoll(angle);
		angle += 90.0f;
	}


	angle = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("SubTowerMeshComponent");
		meshComponent->RotateRoll(angle);
		meshComponent->SetLocalPosition(Crystal::Vector3::RotateQuaternion({ 0.0f, -822.0f, -400.0f }, meshComponent->GetRotationQuat()));
		meshComponent->AddMaterial(baseMaterial1);
		meshComponent->AttachTo(towerMeshComponent);
		meshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_TowerB.fbx"));
		angle += 90.0f;
	}

	angle = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("ConnectMeshComponent");
		meshComponent->RotateRoll(angle);
		meshComponent->SetLocalPosition(Crystal::Vector3::RotateQuaternion({ 0.0f, -1450.0f, -1620.0f }, meshComponent->GetRotationQuat()));
		meshComponent->AddMaterial(baseMaterial1);
		meshComponent->AttachTo(towerMeshComponent);
		meshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Connect.fbx"));
		angle += 90.0f;
	}

	angle = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("ModuleBMeshComponent");
		meshComponent->RotateRoll(angle);
		meshComponent->SetLocalPosition(Crystal::Vector3::RotateQuaternion({ +1640.0f, 0.0f, -300.0f }, meshComponent->GetRotationQuat()));
		meshComponent->AddMaterial(baseMaterial1);
		meshComponent->AttachTo(towerMeshComponent);
		meshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_ModuleB.fbx"));
		angle += 90.0f;
	}

	angle = 0.0f;
	for (int i = 0; i < 2; i++)
	{
		auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("ControlMeshComponent");
		meshComponent->RotateRoll(angle);
		meshComponent->SetLocalPosition(Crystal::Vector3::RotateQuaternion({ 0.0f, -129.0f, -2406.0f }, meshComponent->GetRotationQuat()));
		meshComponent->AddMaterial(baseMaterial1);
		meshComponent->AttachTo(towerMeshComponent);
		meshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Control.fbx"));
		angle += 180.0f;
	}

}


