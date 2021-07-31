#include "cspch.h"
#include "PhysicsSystem.h"

#include "Level.h"
#include "Crystal/Types.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/Renderer/Scene.h"


namespace Crystal {


	void PhysicsSystem::Update(const float deltaTime)
	{
		Object::Update(deltaTime);

#if 1

		auto scene = GetScene();

		for (int i = 0; i < scene->BoundingSphereComponents.size() - 1; i++)
		{
			auto sphereCompLhs = Cast<BoundingSphereComponent>(scene->BoundingSphereComponents[i]);

			if (!sphereCompLhs)
				continue;

			
			auto sphereLhs = sphereCompLhs->GetWorldBoundingSphere();
			

			auto collisionTypeLhs = sphereCompLhs->GetCollisionType();
			
			
			// Bounding Sphere
			for (int j = i + 1; j < scene->BoundingSphereComponents.size(); j++)
			{
				auto sphereCompRhs = Cast<BoundingSphereComponent>(scene->BoundingSphereComponents[j]);
				if (!sphereCompRhs)
					continue;

				auto sphereRhs = sphereCompRhs->GetWorldBoundingSphere();

				auto collisionTypeRhs = sphereCompRhs->GetCollisionType();


				float totalDist = 0.0f;
				if (sphereLhs.Intersects(sphereRhs, totalDist))
				{

					if (collisionTypeLhs == ECollisionType::CT_Block && collisionTypeRhs == ECollisionType::CT_Block)
					{
						ResolveCollision(sphereCompLhs, sphereCompRhs, totalDist);
						HitResult hitResult = {};
						hitResult.HitActor = Cast<Actor>(sphereCompRhs->GetOuter());
						sphereCompLhs->OnHit(hitResult);
						continue;
					}
					
					if (collisionTypeLhs == ECollisionType::CT_Overlap && !sphereCompLhs->IsOverlappedWith(sphereCompRhs))
					{
						// OnBeginOverlap
						sphereCompLhs->OnBeginOverlap(sphereCompRhs);	
					}

					if (collisionTypeRhs == ECollisionType::CT_Overlap && !sphereCompRhs->IsOverlappedWith(sphereCompLhs))
					{
						// OnBeginOverlap
						sphereCompRhs->OnBeginOverlap(sphereCompLhs);
					}
					
				}
				else
				{
					if (collisionTypeLhs == ECollisionType::CT_Overlap && sphereCompLhs->IsOverlappedWith(sphereCompRhs))
					{
						// OnBeginOverlap
						sphereCompLhs->OnEndOverlap(sphereCompRhs);
					}

					if (collisionTypeRhs == ECollisionType::CT_Overlap && sphereCompRhs->IsOverlappedWith(sphereCompLhs))
					{
						// OnBeginOverlap
						sphereCompRhs->OnEndOverlap(sphereCompLhs);
					}
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

	bool PhysicsSystem::LineTraceSingle(HitResult& outHitResult, const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float dist,
	                                    const CollisionParams& collisionParams)
	{
		// Bounding Sphere

		float nearest = dist;
		bool result = false;


		auto scene = GetScene();

		for (const auto& lhsWeak : scene->BoundingSphereComponents)
		{
			auto sphereComplhs = Cast<BoundingSphereComponent>(lhsWeak);
			if (!sphereComplhs)
				continue;

			auto ownerActor = Cast<Actor>(sphereComplhs->GetOuter());
			if (collisionParams.ShouldBeIgnored(ownerActor))
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

	const Shared<Scene>& PhysicsSystem::GetScene()
	{
		auto level = Cast<Level>(GetOuter());
		if (level)
		{
			return level->GetScene();
		}

		return nullptr;
	}


}
