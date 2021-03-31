#pragma once
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"

class Asteroid : public Crystal::Actor
{
public:
	Asteroid()
	{
		auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		boundingSphereComponent->SetRadius(1000.0f);
		m_MainComponent = boundingSphereComponent;
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
		boundingOrientedBox->SetExtents({ 1000.0f, 400.0f, 1000.0f });
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