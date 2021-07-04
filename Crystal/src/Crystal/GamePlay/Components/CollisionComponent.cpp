#include "cspch.h"
#include "CollisionComponent.h"

#include "Crystal/Types.h"
#include "Crystal/GamePlay/World/World.h"
#include "Crystal/GamePlay/World/Level.h"

BOOST_CLASS_EXPORT(Crystal::CollisionComponent)

namespace Crystal {

	void CollisionComponent::RegisterComponent()
	{
		PrimitiveComponent::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());

		auto level = Cast<Level>(owner->GetOuter());
		auto& scene = level->GetScene();

		level->RegisterPhysicsWorldComponent(Cast<CollisionComponent>(shared_from_this()));


#ifndef CS_NM_DEDICATED
		level->GetScene()->CollisionComponents.push_back(Cast<CollisionComponent>(shared_from_this()));
#endif		
	}

	void CollisionComponent::OnHit(const HitResult& hitResult)
	{
		if(m_OnHitEvent)
		{
			m_OnHitEvent(hitResult);
		}
	}

	void CollisionComponent::BindOnHitEvent(const std::function<void(const HitResult&)>& event)
	{
		m_OnHitEvent = event;
	}


}
