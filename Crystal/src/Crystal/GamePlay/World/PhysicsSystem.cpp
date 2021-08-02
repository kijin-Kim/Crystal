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

		// -------------------------------------------------------------------------
		// Sphere vs. Sphere
		// -------------------------------------------------------------------------

		if (!scene->BoundingSphereComponents.empty())
		{
			for (int i = 0; i < scene->BoundingSphereComponents.size() - 1; i++)
			{
				auto collisionCompLhs = Cast<BoundingSphereComponent>(scene->BoundingSphereComponents[i]);

				if (!collisionCompLhs)
					continue;

				auto collisionLhs = collisionCompLhs->GetWorldBoundingSphere();


				auto collisionTypeLhs = collisionCompLhs->GetCollisionType();


				// Bounding Sphere
				for (int j = i + 1; j < scene->BoundingSphereComponents.size(); j++)
				{
					auto collisionCompRhs = Cast<BoundingSphereComponent>(scene->BoundingSphereComponents[j]);
					if (!collisionCompRhs)
						continue;

					auto collisionRhs = collisionCompRhs->GetWorldBoundingSphere();

					auto collisionTypeRhs = collisionCompRhs->GetCollisionType();


					float totalDist = 0.0f;
					DirectX::XMVECTOR contactNormalVector;
					if (collisionLhs.Intersects(collisionRhs, contactNormalVector, totalDist))
					{
						if (collisionTypeLhs == ECollisionType::CT_Overlap && !collisionCompLhs->IsOverlappedWith(collisionCompRhs))
						{
							// OnBeginOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = Cast<Actor>(collisionCompRhs->GetOuter());
							overlappedResult.OverlappedComponent = collisionCompRhs;

							collisionCompLhs->OnBeginOverlap(overlappedResult);
						}

						if (collisionTypeRhs == ECollisionType::CT_Overlap && !collisionCompRhs->IsOverlappedWith(collisionCompLhs))
						{
							// OnBeginOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = Cast<Actor>(collisionCompLhs->GetOuter());
							overlappedResult.OverlappedComponent = collisionCompLhs;

							collisionCompRhs->OnBeginOverlap(overlappedResult);
						}

						if (collisionTypeLhs == ECollisionType::CT_Block && collisionTypeRhs == ECollisionType::CT_Block)
						{
							DirectX::XMFLOAT3 contactNormal = Vector3::Zero;
							DirectX::XMStoreFloat3(&contactNormal, contactNormalVector);

							auto impulse = CalculateImpulse(collisionCompLhs, collisionCompRhs, contactNormal);
							ResolveVelocity(collisionCompLhs, collisionCompRhs, impulse);
							ResolvePenetration(collisionCompLhs, collisionCompRhs, totalDist);

							HitResult hitResultLhs = {};
							hitResultLhs.HitActor = Cast<Actor>(collisionCompRhs->GetOuter());
							hitResultLhs.HitComponent = collisionCompRhs;
							hitResultLhs.Impulse = impulse;
							collisionCompLhs->OnHit(hitResultLhs);

							HitResult hitResultRhs = {};
							hitResultRhs.HitActor = Cast<Actor>(collisionCompLhs->GetOuter());
							hitResultRhs.HitComponent = collisionCompLhs;
							hitResultRhs.Impulse = impulse;
							collisionCompRhs->OnHit(hitResultRhs);
						}
					}
					else
					{
						if (collisionTypeLhs == ECollisionType::CT_Overlap && collisionCompLhs->IsOverlappedWith(collisionCompRhs))
						{
							// OnEndOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = Cast<Actor>(collisionCompRhs->GetOuter());
							overlappedResult.OverlappedComponent = collisionCompRhs;

							collisionCompLhs->OnEndOverlap(overlappedResult);
						}

						if (collisionTypeRhs == ECollisionType::CT_Overlap && collisionCompRhs->IsOverlappedWith(collisionCompLhs))
						{
							// OnEndOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = Cast<Actor>(collisionCompLhs->GetOuter());
							overlappedResult.OverlappedComponent = collisionCompLhs;

							collisionCompRhs->OnEndOverlap(overlappedResult);
						}
					}
				}
			}
		}


		// -------------------------------------------------------------------------
		// Sphere vs. Oriented Box
		// -------------------------------------------------------------------------

		for (int i = 0; i < scene->BoundingSphereComponents.size(); i++)
		{
			auto collisionCompLhs = Cast<BoundingSphereComponent>(scene->BoundingSphereComponents[i]);

			if (!collisionCompLhs)
				continue;

			auto collisionLhs = collisionCompLhs->GetWorldBoundingSphere();


			auto collisionTypeLhs = collisionCompLhs->GetCollisionType();


			// Bounding Sphere
			for (int j = 0; j < scene->BoundingOrientedBoxComponents.size(); j++)
			{
				auto collisionCompRhs = Cast<BoundingOrientedBoxComponent>(scene->BoundingOrientedBoxComponents[j]);
				if (!collisionCompRhs)
					continue;

				auto collisionRhs = collisionCompRhs->GetWorldBoundingOrientedBox();

				auto collisionTypeRhs = collisionCompRhs->GetCollisionType();

				float totalDist = 0.0f;
				DirectX::XMVECTOR contactNormalVector;

				if (collisionLhs.Intersects(collisionRhs, contactNormalVector, totalDist))
				{
					if (collisionTypeLhs == ECollisionType::CT_Overlap && !collisionCompLhs->IsOverlappedWith(collisionCompRhs))
					{
						// OnBeginOverlap
						OverlapResult overlappedResult = {};
						overlappedResult.OverlappedActor = Cast<Actor>(collisionCompRhs->GetOuter());
						overlappedResult.OverlappedComponent = collisionCompRhs;

						collisionCompLhs->OnBeginOverlap(overlappedResult);
					}

					if (collisionTypeRhs == ECollisionType::CT_Overlap && !collisionCompRhs->IsOverlappedWith(collisionCompLhs))
					{
						// OnBeginOverlap
						OverlapResult overlappedResult = {};
						overlappedResult.OverlappedActor = Cast<Actor>(collisionCompLhs->GetOuter());
						overlappedResult.OverlappedComponent = collisionCompLhs;

						collisionCompRhs->OnBeginOverlap(overlappedResult);
					}

					if (collisionTypeLhs == ECollisionType::CT_Block && collisionTypeRhs == ECollisionType::CT_Block)
					{
						DirectX::XMFLOAT3 contactNormal = Vector3::Zero;
						DirectX::XMStoreFloat3(&contactNormal, DirectX::XMVectorNegate(contactNormalVector));

						auto impulse = CalculateImpulse(collisionCompLhs, collisionCompRhs, contactNormal);
						ResolveVelocity(collisionCompLhs, collisionCompRhs, impulse);
						ResolvePenetration(collisionCompLhs, collisionCompRhs, totalDist);

						HitResult hitResultLhs = {};
						hitResultLhs.HitActor = Cast<Actor>(collisionCompRhs->GetOuter());
						hitResultLhs.HitComponent = collisionCompRhs;
						hitResultLhs.Impulse = impulse;
						collisionCompLhs->OnHit(hitResultLhs);

						HitResult hitResultRhs = {};
						hitResultRhs.HitActor = Cast<Actor>(collisionCompLhs->GetOuter());
						hitResultRhs.HitComponent = collisionCompLhs;
						hitResultRhs.Impulse = impulse;
						collisionCompRhs->OnHit(hitResultRhs);
					}
				}
				else
				{
					if (collisionTypeLhs == ECollisionType::CT_Overlap && collisionCompLhs->IsOverlappedWith(collisionCompRhs))
					{
						// OnEndOverlap
						OverlapResult overlappedResult = {};
						overlappedResult.OverlappedActor = Cast<Actor>(collisionCompRhs->GetOuter());
						overlappedResult.OverlappedComponent = collisionCompRhs;

						collisionCompLhs->OnEndOverlap(overlappedResult);
					}

					if (collisionTypeRhs == ECollisionType::CT_Overlap && collisionCompRhs->IsOverlappedWith(collisionCompLhs))
					{
						// OnEndOverlap
						OverlapResult overlappedResult = {};
						overlappedResult.OverlappedActor = Cast<Actor>(collisionCompLhs->GetOuter());
						overlappedResult.OverlappedComponent = collisionCompLhs;

						collisionCompRhs->OnEndOverlap(overlappedResult);
					}
				}
			}
		}


		// -------------------------------------------------------------------------
		// Oriented Box vs. Oriented Box
		// -------------------------------------------------------------------------

		if (!scene->BoundingOrientedBoxComponents.empty())
		{
			for (int i = 0; i < scene->BoundingOrientedBoxComponents.size() - 1; i++)
			{
				auto collisionCompLhs = Cast<BoundingOrientedBoxComponent>(scene->BoundingOrientedBoxComponents[i]);

				if (!collisionCompLhs)
					continue;

				auto collisionLhs = collisionCompLhs->GetWorldBoundingOrientedBox();


				auto collisionTypeLhs = collisionCompLhs->GetCollisionType();


				// Bounding Sphere
				for (int j = i + 1; j < scene->BoundingOrientedBoxComponents.size(); j++)
				{
					auto collisionCompRhs = Cast<BoundingOrientedBoxComponent>(scene->BoundingOrientedBoxComponents[j]);
					if (!collisionCompRhs)
						continue;

					auto collisionRhs = collisionCompRhs->GetWorldBoundingOrientedBox();

					auto collisionTypeRhs = collisionCompRhs->GetCollisionType();


					float totalDist = 0.0f;
					DirectX::XMVECTOR contactNormalVector;
					if (collisionLhs.Intersects(collisionRhs, contactNormalVector, totalDist))
					{
						if (collisionTypeLhs == ECollisionType::CT_Overlap && !collisionCompLhs->IsOverlappedWith(collisionCompRhs))
						{
							// OnBeginOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = Cast<Actor>(collisionCompRhs->GetOuter());
							overlappedResult.OverlappedComponent = collisionCompRhs;

							collisionCompLhs->OnBeginOverlap(overlappedResult);
						}

						if (collisionTypeRhs == ECollisionType::CT_Overlap && !collisionCompRhs->IsOverlappedWith(collisionCompLhs))
						{
							// OnBeginOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = Cast<Actor>(collisionCompLhs->GetOuter());
							overlappedResult.OverlappedComponent = collisionCompLhs;

							collisionCompRhs->OnBeginOverlap(overlappedResult);
						}

						if (collisionTypeLhs == ECollisionType::CT_Block && collisionTypeRhs == ECollisionType::CT_Block)
						{
							DirectX::XMFLOAT3 contactNormal = Vector3::Zero;

							DirectX::XMStoreFloat3(&contactNormal, contactNormalVector);
							auto impulse = CalculateImpulse(collisionCompLhs, collisionCompRhs, contactNormal);
							ResolveVelocity(collisionCompLhs, collisionCompRhs, impulse);
							ResolvePenetration(collisionCompLhs, collisionCompRhs, totalDist);

							HitResult hitResultLhs = {};
							hitResultLhs.HitActor = Cast<Actor>(collisionCompRhs->GetOuter());
							hitResultLhs.HitComponent = collisionCompRhs;
							hitResultLhs.Impulse = impulse;
							collisionCompLhs->OnHit(hitResultLhs);

							HitResult hitResultRhs = {};
							hitResultRhs.HitActor = Cast<Actor>(collisionCompLhs->GetOuter());
							hitResultRhs.HitComponent = collisionCompLhs;
							hitResultRhs.Impulse = impulse;
							collisionCompRhs->OnHit(hitResultRhs);
						}
					}
					else
					{
						if (collisionTypeLhs == ECollisionType::CT_Overlap && collisionCompLhs->IsOverlappedWith(collisionCompRhs))
						{
							// OnEndOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = Cast<Actor>(collisionCompRhs->GetOuter());
							overlappedResult.OverlappedComponent = collisionCompRhs;

							collisionCompLhs->OnEndOverlap(overlappedResult);
						}

						if (collisionTypeRhs == ECollisionType::CT_Overlap && collisionCompRhs->IsOverlappedWith(collisionCompLhs))
						{
							// OnEndOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = Cast<Actor>(collisionCompLhs->GetOuter());
							overlappedResult.OverlappedComponent = collisionCompLhs;

							collisionCompRhs->OnEndOverlap(overlappedResult);
						}
					}
				}
			}
		}


		for (int i = 0; i < scene->AIPerceptions.size(); i++)
		{
			auto perceptionComp = Cast<AIPerceptionComponent>(scene->AIPerceptions[i]);
			if (!perceptionComp)
			{
				continue;
			}

			if (!perceptionComp->GetIsSightEnabled() && !perceptionComp->GetIsHearingEnabled())
			{
				continue;
			}

	
			for (int j = 0; j < scene->AIPerceptionSources.size(); j++)
			{
				auto sourceComp = Cast<AIPerceptionSourceComponent>(scene->AIPerceptionSources[i]);
				if (!sourceComp)
				{
					continue;
				}

				auto sourceOwner = Cast<Actor>(sourceComp->GetOuter());

				if (perceptionComp->GetIsSightEnabled() && sourceComp->GetIsSightEnabled())
				{
					auto sightFrustum = perceptionComp->GetWorldSightFrustum();


					auto sourceSight = sourceComp->GetSightStimulus();

					if (sightFrustum.Contains(DirectX::XMLoadFloat3(&sourceSight.Position)))
					{
						if (!perceptionComp->GetIsAlreadyPercepted(sourceSight))
						{
							sourceSight.bIsSensed = true;
							perceptionComp->ReceiveSightStimulus(sourceSight);
							perceptionComp->OnSightUpdated(sourceSight);
						}
					}
					else
					{
						if (perceptionComp->GetIsAlreadyPercepted(sourceSight))
						{
							sourceSight.bIsSensed = false;
							perceptionComp->ForgetSightStimulus(sourceSight);
							perceptionComp->OnSightUpdated(sourceSight);
						}
					}
				}

				if (perceptionComp->GetIsHearingEnabled() && sourceComp->GetIsHearingEnabled())
				{
					auto& noises = sourceComp->GetNoiseStimuli();

					for (auto sourceNoise : noises)
					{
						auto hearingSphere = perceptionComp->GetWorldHearingSphere();
						Collision::BoundingSphere noiseSphere;
						noiseSphere.Center = sourceNoise.Position;
						noiseSphere.Radius = sourceNoise.MaxRange;

						if (hearingSphere.Intersects(noiseSphere))
						{
							if (!perceptionComp->GetIsAlreadyPercepted(sourceNoise))
							{
								sourceNoise.bIsSensed = true;
								perceptionComp->ReceiveNoiseStimulus(sourceNoise);
								perceptionComp->OnHearingUpdated(sourceNoise);
							}
						}
						else
						{
							if (perceptionComp->GetIsAlreadyPercepted(sourceNoise))
							{
								sourceNoise.bIsSensed = false;
								perceptionComp->ForgetNoiseStimulus(sourceNoise);
								perceptionComp->OnHearingUpdated(sourceNoise);
							}
						}
					}

					sourceComp->ClearNoiseStimuli();
				}
			}
		}

#endif
	}


	void PhysicsSystem::ResolveVelocity(const std::shared_ptr<CollisionComponent>& lhsComponent,
	                                    const std::shared_ptr<CollisionComponent>& rhsComponent, const DirectX::XMFLOAT3& impulse)
	{
		if (Vector3::Equal(impulse, Vector3::Zero))
		{
			return;
		}


		auto impulsePerIMass = Vector3::Divide(impulse, lhsComponent->GetInverseMass() + rhsComponent->GetInverseMass());


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

	const DirectX::XMFLOAT3& PhysicsSystem::CalculateImpulse(const std::shared_ptr<CollisionComponent>& lhsComponent,
	                                                         const std::shared_ptr<CollisionComponent>& rhsComponent, const DirectX::XMFLOAT3& contactNormal)
	{
		//========== Resolve velocity =================================
		auto relativeVelocity = Vector3::Subtract(lhsComponent->GetVelocity(), rhsComponent->GetVelocity());
		auto seperatingVelocity = Vector3::Dot(relativeVelocity, contactNormal);
		if (seperatingVelocity > 0)
			return Vector3::Zero;

		// 분리속도, 충격량
		const float restitution = 2.0f;
		float newSepVelocity = -seperatingVelocity * restitution; // 충돌후 분리속도 = -충돌전 분리속도 * 반발계수
		float deltaVelocity = newSepVelocity - seperatingVelocity; // 속도 변화량


		return Vector3::Multiply(contactNormal, deltaVelocity);
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
