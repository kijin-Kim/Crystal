#pragma once
#include "SpaceWhaleAIController.h"
#include "Crystal/GamePlay/AI/Blackboard.h"
#include "Crystal/GamePlay/Components/AIComponent.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/Resources/ResourceManager.h"

class SpaceWhale : public Crystal::Pawn
{

public:
	void Initialize() override;


	void End() override;
	void Update(float deltaTime) override;

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;
	
	
	STATIC_TYPE_IMPLE(SpaceWhale)

private:
	Crystal::Timer m_PolluteGaugeIncrementTimer = {};
	float m_PolluteGaugeIncrementInterval = 3.0f;
};
