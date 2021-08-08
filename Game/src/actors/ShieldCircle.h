#pragma once
#include "Kraken.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/LightComponent.h"
#include "Crystal/GamePlay/Objects/Actors/LightActor.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

class ShieldCircle : public Crystal::Actor
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Actor>(*this);
	}

public:
	ShieldCircle() = default;
	~ShieldCircle() override = default;

	void Initialize() override
	{
		Crystal::Actor::Initialize();

		auto& resourceManager = Crystal::ResourceManager::Instance();
		
		auto material = Crystal::CreateShared<Crystal::Material>();
		material->EmissiveColor = { 80.0f / 255.0f * 3.0f, 226.0f / 255.0f * 3.0f, 0.0f };
		material->TexCoordMultiplier = { 7.0f, 7.0f };
		material->Opacity = 0.2f;
		material->OpacityTexture = resourceManager.GetTexture("assets/textures/bump.png");
		
		material->ShadingModel = Crystal::EShadingModel::SM_Unlit;
		material->BlendMode = Crystal::EBlendMode::BM_Translucent;
		material->bTwoSided = false;
		
		auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		boundingSphereComponent->SetRadius(2300.0f);
		boundingSphereComponent->IgnoreActorClassOf("Kraken");
		boundingSphereComponent->IgnoreActorClassOf("SpaceWhale");
		boundingSphereComponent->SetInverseMass(0.0f);
		m_MainComponent = boundingSphereComponent;
		

		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");

		staticMeshComponent->AddMaterial(std::move(material));
		staticMeshComponent->SetUnitScale(2300.0f);
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/1M_Sphere.fbx"));
		staticMeshComponent->AttachTo(m_MainComponent);
		
		m_MainComponent = staticMeshComponent;
	}

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override
	{
		m_Health -= damage;
		if(m_Health <= 0.0f)
		{
			Destroy();
			Crystal::Cast<Kraken>(GetInstigator())->SetPhase(2);
		}
	}

	


	STATIC_TYPE_IMPLE(ShieldCircle)

	

private:
	float m_Health = 150.0f;
};
