#include "cspch.h"
#include "CollisionComponent.h"
#include "Crystal/GamePlay/World/World.h"

namespace Crystal {
	void CollisionComponent::OnCreate()
	{
		PrimitiveComponent::OnCreate();

		if (auto owner = GetOwner())
		{
			auto levelWeak = owner->GetObjectOwner(Object::ObjectOwnerType::OOT_Level);
			auto level = std::static_pointer_cast<Level>(levelWeak.lock());
			if (level)
			{
				level->RegisterCollisionComponent(this);
			}

		}
	}
}