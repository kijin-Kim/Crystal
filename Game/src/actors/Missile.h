#pragma once
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/ProjectileMovementComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/Resources/Material.h"

class Missile : public Crystal::Actor
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Actor>(*this);
	}

public:
	Missile() = default;
	~Missile() override = default;

	void Initialize() override
	{
		auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		boundingSphereComponent->SetRadius(1.0f);

		m_MainComponent = boundingSphereComponent;

		auto material = std::make_unique<Crystal::Material>();
		material->ShadingModel = Crystal::EShadingModel::SM_Lit;
		material->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/T_SpaceShipsVol4_Weapons_Dif.tga");
		material->MetallicTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/T_SpaceShipsVol4_Weapons_M.tga");
		material->RoughnessTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/T_SpaceShipsVol4_Weapons_R.tga");
		material->NormalTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/T_SpaceShipsVol4_Weapons_Norm.tga");
		material->EmissiveTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/T_SpaceShipsVol4_Weapons_G.tga");


		auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
		meshComponent->AddMaterial(std::move(material));
		meshComponent->AttachTo(m_MainComponent);

		meshComponent->SetRenderable(Crystal::ResourceManager::Instance().GetRenderable<Crystal::StaticMesh>("assets/models/SM_Missile_B.fbx"));

		m_MainComponent->SetMass(5000.0f);

		auto projectileMovementComponent = CreateComponent<Crystal::ProjectileMovementComponent>("ProjectileMovementComponent");
		projectileMovementComponent->SetTargetComponent(m_MainComponent);
		//projectileMovementComponent->SetTargetVelocity({0.0f, 0.0f, 3000.0f});
	}


	STATIC_TYPE_IMPLE(Missile)
};
