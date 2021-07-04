#include "cspch.h"
#include "PhysicsSystem.h"

#include "Crystal/Types.h"

namespace Crystal {


	void PhysicsSystem::Update(const float deltaTime)
	{
		Object::Update(deltaTime);

#if 1
		// Bounding Sphere
		for (const auto& lhsWeak : m_BoundingSphereComponents)
		{
			auto sphereComplhs = Cast<BoundingSphereComponent>(lhsWeak);
			if (!sphereComplhs)
				continue;

			auto sphereLhs = sphereComplhs->GetWorldBoundingSphere();

			// Bounding Sphere
			for (const auto& rhsWeak : m_BoundingSphereComponents)
			{
				auto sphereCompRhs = Cast<BoundingSphereComponent>(rhsWeak);
				if (!sphereCompRhs)
					continue;

				if (sphereComplhs == sphereCompRhs)
					continue;


				auto sphereRhs = sphereCompRhs->GetWorldBoundingSphere();


				float totalDist = 0.0f;
				if (sphereLhs.Intersects(sphereRhs, totalDist))
				{
					ResolveVelocity(sphereComplhs, sphereCompRhs, totalDist);
					ResolvePenetration(sphereComplhs, sphereCompRhs, totalDist);

					
					HitResult hitResult = {};
					hitResult.HitActor = Cast<Actor>(sphereCompRhs->GetOuter());
					sphereComplhs->OnHit(hitResult);
				}
			}
		}


#endif
	}


	void PhysicsSystem::ResolveCollision(const std::shared_ptr<CollisionComponent>& lhsComponent,
	                                     const std::shared_ptr<CollisionComponent>& rhsComponent, float penetration)
	{
		ResolveVelocity(lhsComponent, rhsComponent, penetration);
		ResolvePenetration(lhsComponent, rhsComponent, penetration);
	}

	void PhysicsSystem::ResolveVelocity(const std::shared_ptr<CollisionComponent>& lhsComponent,
	                                    const std::shared_ptr<CollisionComponent>& rhsComponent, float penetration)
	{
		auto contactNormal = Vector3::Normalize(Vector3::Subtract(lhsComponent->GetWorldPosition(),
		                                                          rhsComponent->GetWorldPosition()));

		float totalInverseMass = lhsComponent->GetInverseMass() + rhsComponent->GetInverseMass();
		if (totalInverseMass <= 0)
			return;

		//========== Resolve velocity =================================
		auto relativeVelocity = Vector3::Subtract(lhsComponent->GetVelocity(), rhsComponent->GetVelocity());
		auto seperatingVelocity = Vector3::Dot(relativeVelocity, contactNormal);
		if (seperatingVelocity > 0)
			return;

		// �и��ӵ�, ��ݷ�
		const float restitution = 1.0f;
		float newSepVelocity = -seperatingVelocity * restitution; // �浹�� �и��ӵ� = -�浹�� �и��ӵ� * �ݹ߰��
		float deltaVelocity = newSepVelocity - seperatingVelocity; // �ӵ� ��ȭ��


		float impulse = deltaVelocity / totalInverseMass; // ��ݷ�

		auto impulsePerIMass = Vector3::Multiply(contactNormal, impulse); // InverseMass �� impulse

		lhsComponent->SetVelocity(Vector3::Add(lhsComponent->GetVelocity(), Vector3::Multiply(impulsePerIMass,
		                                                                                      lhsComponent->GetInverseMass())));

		rhsComponent->SetVelocity(Vector3::Add(rhsComponent->GetVelocity(), Vector3::Multiply(impulsePerIMass,
		                                                                                      -rhsComponent->GetInverseMass())));
	}

	void PhysicsSystem::ResolvePenetration(const std::shared_ptr<CollisionComponent>& lhsComponent,
	                                       const std::shared_ptr<CollisionComponent>& rhsComponent, float penetration)
	{
		auto contactNormal = Vector3::Normalize(Vector3::Subtract(lhsComponent->GetWorldPosition(),
		                                                          rhsComponent->GetWorldPosition()));

		float totalInverseMass = lhsComponent->GetInverseMass() + rhsComponent->GetInverseMass();
		if (totalInverseMass <= 0)
			return;


		//========== Resolve penetration ==============================
		auto distPerIMass = Vector3::Multiply(contactNormal, penetration / totalInverseMass);

		auto lhsDist = Vector3::Multiply(distPerIMass, lhsComponent->GetInverseMass());
		auto rhsDist = Vector3::Multiply(distPerIMass, -rhsComponent->GetInverseMass());

		lhsComponent->SetLocalPosition(Vector3::Add(lhsComponent->GetLocalPosition(), lhsDist));
		rhsComponent->SetLocalPosition(Vector3::Add(rhsComponent->GetLocalPosition(), rhsDist));
	}

	void PhysicsSystem::RegisterPhysicsWorldComponent(std::weak_ptr<Component> compWeak)
	{
		auto component = compWeak.lock();
		if (!component)
			return;

		auto staticType = component->StaticType();

		if (staticType == "RayComponent")
		{
			m_RayComponents.push_back(Cast<RayComponent>(component));
		}
		else if (staticType == "BoundingBoxComponent")
		{
			m_BoundingBoxComponents.push_back(Cast<BoundingBoxComponent>(component));
		}
		else if (staticType == "BoundingOrientedBoxComponent")
		{
			m_BoundingOrientedBoxComponents.push_back(Cast<BoundingOrientedBoxComponent>(component));
		}
		else if (staticType == "BoundingSphereComponent")
		{
			m_BoundingSphereComponents.push_back(Cast<BoundingSphereComponent>(component));
		}
	}

	bool PhysicsSystem::LineTraceSingle(HitResult& outHitResult, const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float dist, const CollisionParams& collisionParams)
	{
		// Bounding Sphere

		float nearest = dist;
		bool result = false;
		
		for (const auto& lhsWeak : m_BoundingSphereComponents)
		{
	
			
			auto sphereComplhs = Cast<BoundingSphereComponent>(lhsWeak);
			if (!sphereComplhs)
				continue;

			auto ownerActor = Cast<Actor>(sphereComplhs->GetOuter());
			if(collisionParams.ShouldBeIgnored(ownerActor))
			{
				continue;
			}

			auto sphereLhs = sphereComplhs->GetWorldBoundingSphere();


			
			DirectX::XMVECTOR originVector = DirectX::XMLoadFloat3(&origin);
			DirectX::XMVECTOR directionVector = DirectX::XMLoadFloat3(&direction);
			
			bool currentResult = sphereLhs.Intersects(originVector, directionVector, dist);
			if (currentResult && dist < nearest)
			{
				nearest = dist;
				outHitResult.HitActor = ownerActor;
			}


			result |= currentResult;
			
		}

		return result;
		
	}


}
