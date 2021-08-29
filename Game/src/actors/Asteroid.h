#pragma once
#include "Crystal/Core/Timer.h"
#include "Crystal/GamePlay/Components/AIComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"

namespace Crystal {
	class TextureComponent;
}

class Asteroid : public Crystal::Actor
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Actor>(*this);
	}

public:
	Asteroid() = default;
	~Asteroid() override = default;

	void Initialize() override;

	STATIC_TYPE_IMPLE(Asteroid)

};

class WhiteAsteroid : public Crystal::Actor
{


public:
	WhiteAsteroid() = default;
	~WhiteAsteroid() override = default;

	void Initialize() override;

	void SetAsteroidType(int type);


	STATIC_TYPE_IMPLE(WhiteAsteroid)

private:
	Crystal::Shared<Crystal::StaticMeshComponent> m_StaticMeshComponent;
};



class FakeAsteroid : public Crystal::Actor
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Actor>(*this);
	}

public:
	FakeAsteroid() = default;
	~FakeAsteroid() override = default;

	void Initialize() override;
	STATIC_TYPE_IMPLE(FakeAsteroid)
};


class FakeWhiteAsteroid : public Crystal::Actor
{

public:
	FakeWhiteAsteroid() = default;
	~FakeWhiteAsteroid() override = default;

	void Initialize() override;
	STATIC_TYPE_IMPLE(FakeWhiteAsteroid)
};



class DestructibleAsteroid : public Crystal::Actor
{
public:
	DestructibleAsteroid() = default;
	~DestructibleAsteroid() override = default;

	void Initialize() override;

	void SetShowHealthBar(bool bShow);

	void Update(float deltaTime) override;

	void UpdateHealth();
	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;

	STATIC_TYPE_IMPLE(DestructibleAsteroid)

protected:
	const float m_MaxHealth = 60.0f;
	float m_CurrentHealth = m_MaxHealth;

private:
	Crystal::Shared<Crystal::TextureComponent> m_HealthBarFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_HealthBarBgComponent = nullptr;
	Crystal::Shared<Crystal::AIPerceptionSourceComponent> m_AIPerceptionSourceComponent = nullptr;
	int m_HealthBarWidth = 0;
	int m_HealthBarHeight = 0;


	Crystal::Timer m_HealthBarShowTimer = {};
	const float m_HealthBarShowTime = 3.0f;
	bool m_bShouldShowHealthBar = false;
};


class HealAsteroid : public DestructibleAsteroid
{
public:
	HealAsteroid() = default;
	~HealAsteroid() override = default;


	void Initialize() override;

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;

	STATIC_TYPE_IMPLE(HealAsteroid)
};

class PowerAsteroid : public DestructibleAsteroid
{
public:
	PowerAsteroid() = default;
	~PowerAsteroid() override = default;


	void Initialize() override;

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;


	STATIC_TYPE_IMPLE(PowerAsteroid)
};


class ShieldAsteroid : public DestructibleAsteroid
{
public:
	ShieldAsteroid() = default;
	~ShieldAsteroid() override = default;

	void Initialize() override;

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;

	STATIC_TYPE_IMPLE(ShieldAsteroid)

};
