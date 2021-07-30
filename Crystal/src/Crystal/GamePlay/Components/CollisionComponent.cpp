#include "cspch.h"
#include "CollisionComponent.h"

#include "Crystal/Types.h"
#include "Crystal/GamePlay/World/World.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"

BOOST_CLASS_EXPORT(Crystal::CollisionComponent)

namespace Crystal {

	
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

	void RayComponent::RegisterComponent()
	{
		CollisionComponent::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto& scene = level->GetScene();

#ifndef CS_NM_DEDICATED
		scene->RayComponents.push_back(Cast<RayComponent>(shared_from_this()));
#endif
	}

	void BoundingBoxComponent::RegisterComponent()
	{
		CollisionComponent::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto& scene = level->GetScene();

#ifndef CS_NM_DEDICATED
		scene->BoundingBoxComponents.push_back(Cast<BoundingBoxComponent>(shared_from_this()));
#endif
	}

	void BoundingOrientedBoxComponent::RegisterComponent()
	{
		CollisionComponent::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto& scene = level->GetScene();

#ifndef CS_NM_DEDICATED
		scene->BoundingOrientedBoxComponents.push_back(Cast<BoundingOrientedBoxComponent>(shared_from_this()));
#endif
	}

	void BoundingFrustumComponent::RegisterComponent()
	{
		CollisionComponent::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto& scene = level->GetScene();

#ifndef CS_NM_DEDICATED
		scene->BoundingFrustumComponents.push_back(Cast<BoundingFrustumComponent>(shared_from_this()));
#endif
	}

	void BoundingSphereComponent::RegisterComponent()
	{
		CollisionComponent::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto& scene = level->GetScene();

#ifndef CS_NM_DEDICATED
		scene->BoundingSphereComponents.push_back(Cast<BoundingSphereComponent>(shared_from_this()));
#endif
	}


}
