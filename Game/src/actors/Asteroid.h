#pragma once
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/ParticleComponent.h"
#include "Crystal/Renderer/Pipelines/RenderPipelines/LightingStaticPipeline.h"

class Asteroid : public Crystal::Actor
{
public:
	Asteroid()
	{

		auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		boundingSphereComponent->SetRadius(400.0f);

		m_MainComponent = boundingSphereComponent;



		
		auto material = std::make_unique<Crystal::NewMaterial>();
		material->ShadingModel = Crystal::EShadingModel::ShadingModel_DefaultLit;

		
	
		auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
		meshComponent->AddMaterial(std::move(material));

	
		SetAttachment(meshComponent, m_MainComponent);


		


		auto randomScale = rand() % 1 + 1;

		m_MainComponent->RotateRoll(rand() % 360);
		m_MainComponent->RotatePitch(rand() % 360);
		m_MainComponent->RotateYaw(rand() % 360);
		m_MainComponent->SetScale(randomScale);
		m_MainComponent->SetMass(5000.0f * randomScale);

		SetPosition({ (float)(rand() % 10000 - 5000), (float)(rand() % 10000 - 5000), (float)(rand() % 10000 - 5000) });
		
		
	}
	~Asteroid() override = default;

	STATIC_TYPE_IMPLE(Asteroid)
};

class RandomBox : public Crystal::Actor
{
public:
	RandomBox()
	{
		auto boundingOrientedBox = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBox");
		boundingOrientedBox->SetExtents({ 100.0f, 40.0f, 100.0f });
		constexpr float angle = DirectX::XMConvertToRadians(60.0f);
		boundingOrientedBox->SetOrientation(Crystal::Vector4::QuaternionRollPitchYaw({ angle, angle, angle }));

		m_MainComponent = boundingOrientedBox;
	}
	~RandomBox() override = default;

	void Update(const float deltaTime) override
	{
		Crystal::Actor::Update(deltaTime);

		float xAngle = DirectX::XMConvertToRadians(rand() % 360 + 1);
		float yAngle = DirectX::XMConvertToRadians(rand() % 360 + 1);
		float zAngle = DirectX::XMConvertToRadians(rand() % 360 + 1);

		m_MainComponent->RotateRoll(xAngle);
		m_MainComponent->RotatePitch(yAngle);
		m_MainComponent->RotateYaw(zAngle);
	}

	STATIC_TYPE_IMPLE(RandomBox)
};