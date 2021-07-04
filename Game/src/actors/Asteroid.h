#pragma once
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/ParticleComponent.h"
#include "Crystal/Renderer/Pipelines/RenderPipelines/GeometryStaticPipeline.h"

class Asteroid : public Crystal::Actor
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Actor>(*this);
	}

public:
	Asteroid() = default;
	~Asteroid() override = default;

	void Initialize() override
	{
		auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		boundingSphereComponent->SetRadius(400.0f);

		m_MainComponent = boundingSphereComponent;

		auto material = std::make_unique<Crystal::Material>();
		material->ShadingModel = Crystal::EShadingModel::ShadingModel_DefaultLit;

		auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
		meshComponent->AddMaterial(std::move(material));
		meshComponent->AttachTo(m_MainComponent);

		auto randomScale = rand() % 1 + 1;

		m_MainComponent->RotateRoll(rand() % 360);
		m_MainComponent->RotatePitch(rand() % 360);
		m_MainComponent->RotateYaw(rand() % 360);
		m_MainComponent->SetScale(randomScale);
		m_MainComponent->SetMass(5000.0f * randomScale);

		SetPosition({(float)(rand() % 10000 - 5000), (float)(rand() % 10000 - 5000), (float)(rand() % 10000 - 5000)});
	}

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override
	{
		Destroy();
	}

	
	STATIC_TYPE_IMPLE(Asteroid)
};
	