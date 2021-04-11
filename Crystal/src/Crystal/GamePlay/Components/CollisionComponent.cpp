#include "cspch.h"
#include "CollisionComponent.h"
#include "Crystal/GamePlay/World/World.h"
#include "Crystal/GamePlay/World/Level.h"

namespace Crystal {

	void CollisionComponent::RegisterComponent()
	{
		PrimitiveComponent::RegisterComponent();

		auto owner = Cast<Actor>(GetObjectOwner(Component::ComponentOwnerType::Owner_Actor));

		auto level = Cast<Level>(owner->GetObjectOwner(Actor::ActorOwnerType::Owner_Level));

		level->RegisterPhysicsWorldComponent(Cast<CollisionComponent>(shared_from_this()));
	}

}