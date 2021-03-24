#include "cspch.h"
#include "CollisionComponent.h"
#include "Crystal\GamePlay\World\World.h"

namespace Crystal {

	void CollisionComponent::OnCreate()
	{
		PrimitiveComponent::OnCreate();

		Level* level = (Level*)GetOwner()->GetParent();
		level->RegisterCollisionComponent(this);
	}

}

