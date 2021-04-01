#include "cspch.h"
#include "CollisionComponent.h"
#include "Crystal/GamePlay/World/World.h"

namespace Crystal {
	void CollisionComponent::OnCreate()
	{
		PrimitiveComponent::OnCreate();

		if (auto owner = GetObjectOwner(Component::ComponentOwnerType::Owner_Actor).lock())
		{
			auto level = Cast<Level>(owner->GetObjectOwner(Actor::ActorOwnerType::Owner_Level));
			if (level)
			{
				level->RegisterCollisionComponent(this);
			}

		}
	}
}