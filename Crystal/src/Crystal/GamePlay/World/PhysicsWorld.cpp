#include "cspch.h"
#include "PhysicsWorld.h"

namespace Crystal {



	void PhysicsWorld::Update(const float deltaTime)
	{
		Updatable::Update(deltaTime);

		 
		//// Bounding Sphere
		//for (const auto & lhsWeak : m_BoundingSphereComponents)
		//{
		//	auto sphereComplhs = Cast<BoundingSphereComponent>(lhsWeak);
		//	if (!sphereComplhs)
		//		continue;

		//	auto sphereLhs = sphereComplhs->GetWorldBoundingSphere();

		//	// Bounding Sphere
		//	for (const auto & rhsWeak : m_BoundingSphereComponents)
		//	{
		//		
		//		auto sphereCompRhs = Cast<BoundingSphereComponent>(rhsWeak);
		//		if (!sphereCompRhs)
		//			continue;

		//		if (sphereComplhs == sphereCompRhs)
		//			continue;


		//		auto sphereRhs = sphereCompRhs->GetWorldBoundingSphere();

		//		
		//		float totalDist = 0.0f;
		//		if (sphereLhs.Intersects(sphereRhs, totalDist))
		//		{
		//			ResolveVelocity(sphereComplhs, sphereCompRhs, totalDist);
		//		}
		//	}
		//}

		//// Bounding Sphere
		//for (const auto & lhsWeak : m_BoundingSphereComponents)
		//{
		//	auto sphereComplhs = Cast<BoundingSphereComponent>(lhsWeak);
		//	if (!sphereComplhs)
		//		continue;

		//	auto sphereLhs = sphereComplhs->GetWorldBoundingSphere();

		//	// Bounding Sphere
		//	for (const auto & rhsWeak : m_BoundingSphereComponents)
		//	{
		//		auto sphereCompRhs = Cast<BoundingSphereComponent>(rhsWeak);
		//		if (!sphereCompRhs)
		//			continue;

		//		if (sphereComplhs == sphereCompRhs)
		//			continue;


		//		auto sphereRhs = sphereCompRhs->GetWorldBoundingSphere();

		//		float totalDist = 0.0f;
		//		if (sphereLhs.Intersects(sphereRhs, totalDist))
		//		{
		//			ResolvePenetration(sphereComplhs, sphereCompRhs, totalDist);
		//		}
		//	}
		//}
		//
		//
		//
		//

		//// Bounding Sphere
		//for (const auto& sphereCompWeak : m_BoundingSphereComponents)
		//{
		//	auto sphereCompLhs = Cast<BoundingSphereComponent>(sphereCompWeak);
		//	if (!sphereCompLhs)
		//		continue;

		//	auto sphereLhs = sphereCompLhs->GetWorldBoundingSphere();

		//	// Bounding Oriented Box
		//	for(const auto& obbCompWeak : m_BoundingOrientedBoxComponents)
		//	{
		//		auto obbCompRhs = Cast<BoundingOrientedBoxComponent>(obbCompWeak);
		//		if (!obbCompRhs)
		//			continue;

		//		auto obbRhs = obbCompRhs->GetWorldBoundingOrientedBox();

		//		if (sphereLhs.Intersects(obbRhs))
		//		{

		//		}

		//	}
		//}

		
	}


	void PhysicsWorld::ResolveCollision(const std::shared_ptr<CollisionComponent>& lhsComponent, 
		const std::shared_ptr<CollisionComponent>& rhsComponent, float penetration)
	{

		ResolveVelocity(lhsComponent, rhsComponent, penetration);
		ResolvePenetration(lhsComponent, rhsComponent, penetration);
	}

	void PhysicsWorld::ResolveVelocity(const std::shared_ptr<CollisionComponent>& lhsComponent, const std::shared_ptr<CollisionComponent>& rhsComponent, float penetration)
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

		// 분리속도, 충격량
		const float restitution = 1.0f;
		float newSepVelocity = -seperatingVelocity * restitution; // 충돌후 분리속도 = -충돌전 분리속도 * 반발계수
		float deltaVelocity = newSepVelocity - seperatingVelocity; // 속도 변화량


		float impulse = deltaVelocity / totalInverseMass; // 충격량

		auto impulsePerIMass = Vector3::Multiply(contactNormal, impulse); // InverseMass 당 impulse

		lhsComponent->SetVelocity(Vector3::Add(lhsComponent->GetVelocity(), Vector3::Multiply(impulsePerIMass,
			lhsComponent->GetInverseMass())));

		rhsComponent->SetVelocity(Vector3::Add(rhsComponent->GetVelocity(), Vector3::Multiply(impulsePerIMass,
			-rhsComponent->GetInverseMass())));

	}

	void PhysicsWorld::ResolvePenetration(const std::shared_ptr<CollisionComponent>& lhsComponent, const std::shared_ptr<CollisionComponent>& rhsComponent, float penetration)
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

	void PhysicsWorld::RegisterPhysicsWorldComponent(std::weak_ptr<Component> compWeak)
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



}