#include "cspch.h"
#include "CollisionComponent.h"
#include "Crystal/GamePlay/World/World.h"
#include "Crystal/GamePlay/World/Level.h"

namespace Crystal {

	void CollisionComponent::RegisterComponent()
	{
		PrimitiveComponent::RegisterComponent();

		auto owner = Cast<Actor>(GetOwner());

		auto level = Cast<Level>(owner->GetOwner());

		level->RegisterPhysicsWorldComponent(Cast<CollisionComponent>(shared_from_this()));
	}

}