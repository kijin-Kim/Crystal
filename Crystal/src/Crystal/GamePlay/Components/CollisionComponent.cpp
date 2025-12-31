#include "cspch.h"
#include "CollisionComponent.h"

#include "Crystal/Types.h"
#include "Crystal/GamePlay/World/World.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"


namespace Crystal {


	void CollisionComponent::Update(const float deltaTime)
	{
		PrimitiveComponent::Update(deltaTime);

		std::lock_guard<std::recursive_mutex> locker(m_OverlappedComponentsMutex);
		for (auto it = m_OverlappedComponents.begin(); it != m_OverlappedComponents.end();)
		{
			if (it->expired())
			{
				it = m_OverlappedComponents.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void CollisionComponent::AddImpulse(const DirectX::XMFLOAT3& impulse)
	{
		SetVelocity(Vector3::Add(GetVelocity(), impulse));
	}

	void CollisionComponent::OnHit(const HitResult& hitResult)
	{
		if (m_OnHitEvent)
		{
			m_OnHitEvent(hitResult);
		}
	}

	void CollisionComponent::BindOnHitEvent(const std::function<void(const HitResult&)>& event)
	{
		m_OnHitEvent = event;
	}

	void CollisionComponent::OnBeginOverlap(const OverlapResult& overlapResult)
	{
		std::lock_guard<std::recursive_mutex> locker(m_OverlappedComponentsMutex);
		m_bIsFirstTimeCheckOverlap = false;
		overlapResult.OverlappedComponent.lock()->SetIsFirstTimeCheckOverlapped(false);

		m_OverlappedComponents.push_back(overlapResult.OverlappedComponent);

		if(m_OnBeginOverlapEvent)
		{
			m_OnBeginOverlapEvent(overlapResult);
		}
	}

	void CollisionComponent::BindOnBeginOverlapEvent(const std::function<void(const OverlapResult&)>& event)
	{
		m_OnBeginOverlapEvent = event;
	}

	void CollisionComponent::OnEndOverlap(const OverlapResult& overlapResult)
	{
		std::lock_guard<std::recursive_mutex> locker(m_OverlappedComponentsMutex);
		m_bIsFirstTimeCheckOverlap = false;
		auto it = std::find_if(m_OverlappedComponents.begin(), m_OverlappedComponents.end(), [&overlapResult](Weak<CollisionComponent> other)->bool
		{
			auto overlapped = overlapResult.OverlappedComponent.lock();
			if (!overlapped)
			{
				return false;
			}

			auto otherOverlapped = other.lock();
			if (!otherOverlapped)
			{
				return false;
			}

			return overlapped == otherOverlapped;
		});

		if (it != m_OverlappedComponents.end())
		{
			std::swap(*it, m_OverlappedComponents.back());
			m_OverlappedComponents.erase(m_OverlappedComponents.end() - 1);
		}


		if (m_OnEndOverlapEvent)
		{
			m_OnEndOverlapEvent(overlapResult);
		}
	}

	void CollisionComponent::BindOnEndOverlapEvent(const std::function<void(const OverlapResult&)>& event)
	{
		m_OnEndOverlapEvent = event;
	}

	bool CollisionComponent::IsOverlappedWith(Weak<CollisionComponent> overlappedComponent)
	{
		std::lock_guard<std::recursive_mutex> locker(m_OverlappedComponentsMutex);
		auto it = std::find_if(m_OverlappedComponents.begin(), m_OverlappedComponents.end(), [&overlappedComponent](const Weak<CollisionComponent>& other)
		{
			auto overlapped = overlappedComponent.lock();
			if (!overlapped)
			{
				return false;
			}

			auto otherOverlapped = other.lock();
			if (!otherOverlapped)
			{
				return false;
			}

			return overlapped == otherOverlapped;
		});

		return it != m_OverlappedComponents.end();
	}

	void CollisionComponent::RemoveIgnoreActorClassOf(const std::string& type)
	{
		auto it = FindActorClassFromWhitelist(type);
		if(it != m_ActorClassCollisionWhitelist.end())
		{
			m_ActorClassCollisionWhitelist.erase(it);
		}
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
