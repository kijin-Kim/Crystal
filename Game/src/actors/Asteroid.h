#pragma once
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"

class Asteroid : public Crystal::Actor
{
public:
	Asteroid()
	{
		auto& resourceManager = Crystal::ResourceManager::Instance();


		

		auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		boundingSphereComponent->SetRadius(400.0f);

		m_MainComponent = boundingSphereComponent;


		auto pbrMaterial = std::make_shared<Crystal::Material>();
		pbrMaterial->SetObjectOwner(resourceManager.GetShader("PBRShader_Static"),
			Crystal::Material::MaterialOwnerType::Owner_Shader);
		pbrMaterial->Set("AlbedoTexture", resourceManager.GetTexture("Asteroid_Blue_Albedo"));
		pbrMaterial->Set("NormalTexture", resourceManager.GetTexture("Asteroid_Blue_Normal"));


		auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
		meshComponent->SetRenderable(resourceManager.GetRenderable("Asteroid_Mesh_1"));
		meshComponent->AddMaterial(pbrMaterial);
		
		SetAttachment(meshComponent, m_MainComponent);


		auto randomScale = rand() % 4;

		m_MainComponent->RotateRoll(rand() % 360);
		m_MainComponent->RotatePitch(rand() % 360);
		m_MainComponent->RotateYaw(rand() % 360);
		m_MainComponent->SetScale(randomScale);

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