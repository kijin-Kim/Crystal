#pragma once
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/ProjectileMovementComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Resources/ResourceManager.h"

class Missile : public Crystal::Actor
{

public:
	Missile() = default;
	~Missile() override = default;

	void Initialize() override
	{

		auto boundingOrientedBox = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingSphereComponent");
		boundingOrientedBox->SetExtents({2.0f, 2.0f, 18.0f});
		boundingOrientedBox->SetMass(5.0f);



		m_MainComponent = boundingOrientedBox;

		auto material = std::make_unique<Crystal::Material>();
		/*material->ShadingModel = Crystal::EShadingModel::SM_Lit;
		material->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/T_SpaceShipsVol4_Weapons_Dif.tga");
		material->MetallicTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/T_SpaceShipsVol4_Weapons_M.tga");
		material->RoughnessTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/T_SpaceShipsVol4_Weapons_R.tga");
		material->NormalTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/T_SpaceShipsVol4_Weapons_Norm.tga");
		material->EmissiveTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/T_SpaceShipsVol4_Weapons_G.tga");*/

		material->ShadingModel = Crystal::EShadingModel::SM_Unlit;;
		material->BlendMode = Crystal::EBlendMode::BM_Opaque;
		material->EmissiveColor = { 120 / 255.0f * 10.0f, 250 / 255.0f * 10.0f, 0.7f * 10.0f };


		auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
		meshComponent->AddMaterial(std::move(material));
		meshComponent->AttachTo(m_MainComponent);
		meshComponent->SetUnitScale(0.5f);

		meshComponent->SetRenderable(Crystal::ResourceManager::Instance().GetRenderable<Crystal::StaticMesh>("assets/models/SM_Missile_B.fbx"));


		auto projectileMovementComponent = CreateComponent<Crystal::ProjectileMovementComponent>("ProjectileMovementComponent");
		projectileMovementComponent->SetTargetComponent(m_MainComponent);
	}


	STATIC_TYPE_IMPLE(Missile)
};


