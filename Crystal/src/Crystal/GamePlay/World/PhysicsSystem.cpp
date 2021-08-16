#include "cspch.h"
#include "PhysicsSystem.h"

#include "Level.h"
#include "Crystal/Types.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/PostProcessComponent.h"
#include "Crystal/GamePlay/Controllers/AIController.h"
#include "Crystal/GamePlay/Objects/Actors/PostProcessVolumeActor.h"
#include "Crystal/Renderer/Scene.h"
#include "Crystal/GamePlay/Components/CameraComponent.h"


namespace Crystal {


	void PhysicsSystem::Update(const float deltaTime)
	{
		Object::Update(deltaTime);
#if 1
		auto scene = GetScene();
		if(!scene)
		{
			return;
		}

		// -------------------------------------------------------------------------
		// Sphere vs. Sphere
		// -------------------------------------------------------------------------

		if (!scene->BoundingSphereComponents.empty())
		{
			for (int i = 0; i < (int)scene->BoundingSphereComponents.size() - 1; i++)
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

					if (collisionCompLhs->HasActorClassWhitelist() || collisionCompRhs->HasActorClassWhitelist())
					{
						auto actorTypeLhs = Cast<Actor>(collisionCompLhs->GetOuter())->StaticType();
						auto actorTypeRhs = Cast<Actor>(collisionCompRhs->GetOuter())->StaticType();

						if (collisionCompLhs->IsWhitelistActorClass(actorTypeRhs) || collisionCompRhs->IsWhitelistActorClass(actorTypeLhs))
						{
							continue;
						}
					}


					auto collisionRhs = collisionCompRhs->GetWorldBoundingSphere();

					auto collisionTypeRhs = collisionCompRhs->GetCollisionType();


					float totalDist = 0.0f;
					DirectX::XMVECTOR contactNormalVector;
					if (collisionLhs.Intersects(collisionRhs, contactNormalVector, totalDist))
					{
						if (collisionTypeLhs == ECollisionType::CT_Overlap && (!collisionCompLhs->IsOverlappedWith(collisionCompRhs) || collisionCompLhs->
							GetIsFirstTimeCheckOverlapped() || collisionCompRhs->GetIsFirstTimeCheckOverlapped()))
						{
							// OnBeginOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = Cast<Actor>(collisionCompRhs->GetOuter());
							overlappedResult.OverlappedComponent = collisionCompRhs;

							collisionCompLhs->OnBeginOverlap(overlappedResult);
						}

						if (collisionTypeRhs == ECollisionType::CT_Overlap && (!collisionCompRhs->IsOverlappedWith(collisionCompLhs) || collisionCompLhs->
							GetIsFirstTimeCheckOverlapped() || collisionCompRhs->GetIsFirstTimeCheckOverlapped()))
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
							ResolvePenetration(collisionCompLhs, collisionCompRhs, contactNormal, totalDist);

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
						if (collisionTypeLhs == ECollisionType::CT_Overlap && (collisionCompLhs->IsOverlappedWith(collisionCompRhs) || collisionCompLhs->
							GetIsFirstTimeCheckOverlapped() || collisionCompRhs->GetIsFirstTimeCheckOverlapped()))
						{
							// OnEndOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = Cast<Actor>(collisionCompRhs->GetOuter());
							overlappedResult.OverlappedComponent = collisionCompRhs;

							collisionCompLhs->OnEndOverlap(overlappedResult);
						}

						if (collisionTypeRhs == ECollisionType::CT_Overlap && (collisionCompRhs->IsOverlappedWith(collisionCompLhs) || collisionCompLhs->
							GetIsFirstTimeCheckOverlapped() || collisionCompRhs->GetIsFirstTimeCheckOverlapped()))
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


				if (collisionCompLhs->HasActorClassWhitelist() || collisionCompRhs->HasActorClassWhitelist())
				{
					auto actorTypeLhs = Cast<Actor>(collisionCompLhs->GetOuter())->StaticType();
					auto actorTypeRhs = Cast<Actor>(collisionCompRhs->GetOuter())->StaticType();

					if (collisionCompLhs->IsWhitelistActorClass(actorTypeRhs) || collisionCompRhs->IsWhitelistActorClass(actorTypeLhs))
					{
						continue;
					}
				}

				auto collisionRhs = collisionCompRhs->GetWorldBoundingOrientedBox();


				auto collisionTypeRhs = collisionCompRhs->GetCollisionType();

				float totalDist = 0.0f;
				DirectX::XMVECTOR contactNormalVector;

				if (collisionLhs.Intersects(collisionRhs, contactNormalVector, totalDist))
				{
					if (collisionTypeLhs == ECollisionType::CT_Overlap && (!collisionCompLhs->IsOverlappedWith(collisionCompRhs) || collisionCompLhs->
						GetIsFirstTimeCheckOverlapped() || collisionCompRhs->GetIsFirstTimeCheckOverlapped()))
					{
						// OnBeginOverlap
						OverlapResult overlappedResult = {};
						overlappedResult.OverlappedActor = Cast<Actor>(collisionCompRhs->GetOuter());
						overlappedResult.OverlappedComponent = collisionCompRhs;

						collisionCompLhs->OnBeginOverlap(overlappedResult);
					}

					if (collisionTypeRhs == ECollisionType::CT_Overlap && (!collisionCompRhs->IsOverlappedWith(collisionCompLhs) || collisionCompLhs->
						GetIsFirstTimeCheckOverlapped() || collisionCompRhs->GetIsFirstTimeCheckOverlapped()))
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
						ResolvePenetration(collisionCompLhs, collisionCompRhs, contactNormal, totalDist);

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
					if (collisionTypeLhs == ECollisionType::CT_Overlap && (collisionCompLhs->IsOverlappedWith(collisionCompRhs) || collisionCompLhs->
						GetIsFirstTimeCheckOverlapped() || collisionCompRhs->GetIsFirstTimeCheckOverlapped()))
					{
						// OnEndOverlap
						OverlapResult overlappedResult = {};
						overlappedResult.OverlappedActor = Cast<Actor>(collisionCompRhs->GetOuter());
						overlappedResult.OverlappedComponent = collisionCompRhs;

						collisionCompLhs->OnEndOverlap(overlappedResult);
					}

					if (collisionTypeRhs == ECollisionType::CT_Overlap && (collisionCompRhs->IsOverlappedWith(collisionCompLhs) || collisionCompLhs->
						GetIsFirstTimeCheckOverlapped() || collisionCompRhs->GetIsFirstTimeCheckOverlapped()))
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
			for (int i = 0; i < (int)scene->BoundingOrientedBoxComponents.size() - 1; i++)
			{
				auto collisionCompLhs = Cast<BoundingOrientedBoxComponent>(scene->BoundingOrientedBoxComponents[i]);

				if (!collisionCompLhs)
					continue;

				auto collisionLhs = collisionCompLhs->GetWorldBoundingOrientedBox();


				auto collisionTypeLhs = collisionCompLhs->GetCollisionType();


				for (int j = i + 1; j < scene->BoundingOrientedBoxComponents.size(); j++)
				{
					auto collisionCompRhs = Cast<BoundingOrientedBoxComponent>(scene->BoundingOrientedBoxComponents[j]);
					if (!collisionCompRhs)
						continue;

					if (collisionCompLhs->HasActorClassWhitelist() || collisionCompRhs->HasActorClassWhitelist())
					{
						auto actorTypeLhs = Cast<Actor>(collisionCompLhs->GetOuter())->StaticType();
						auto actorTypeRhs = Cast<Actor>(collisionCompRhs->GetOuter())->StaticType();

						if (collisionCompLhs->IsWhitelistActorClass(actorTypeRhs) || collisionCompRhs->IsWhitelistActorClass(actorTypeLhs))
						{
							continue;
						}
					}


					auto collisionRhs = collisionCompRhs->GetWorldBoundingOrientedBox();

					auto collisionTypeRhs = collisionCompRhs->GetCollisionType();


					float totalDist = 0.0f;
					DirectX::XMVECTOR contactNormalVector;
					if (collisionLhs.Intersects(collisionRhs, contactNormalVector, totalDist))
					{

						if (collisionTypeLhs == ECollisionType::CT_Overlap && (!collisionCompLhs->IsOverlappedWith(collisionCompRhs) || collisionCompLhs->
							GetIsFirstTimeCheckOverlapped() || collisionCompRhs->GetIsFirstTimeCheckOverlapped()))
						{
							// OnBeginOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = Cast<Actor>(collisionCompRhs->GetOuter());
							overlappedResult.OverlappedComponent = collisionCompRhs;

							collisionCompLhs->OnBeginOverlap(overlappedResult);
						}

						if (collisionTypeRhs == ECollisionType::CT_Overlap && (!collisionCompRhs->IsOverlappedWith(collisionCompLhs) || collisionCompLhs->
							GetIsFirstTimeCheckOverlapped() || collisionCompRhs->GetIsFirstTimeCheckOverlapped()))
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
							ResolvePenetration(collisionCompLhs, collisionCompRhs, contactNormal, totalDist);

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
						if (collisionTypeLhs == ECollisionType::CT_Overlap && (collisionCompLhs->IsOverlappedWith(collisionCompRhs) || collisionCompLhs->
							GetIsFirstTimeCheckOverlapped() || collisionCompRhs->GetIsFirstTimeCheckOverlapped()))
						{
							// OnEndOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = Cast<Actor>(collisionCompRhs->GetOuter());
							overlappedResult.OverlappedComponent = collisionCompRhs;

							collisionCompLhs->OnEndOverlap(overlappedResult);
						}

						if (collisionTypeRhs == ECollisionType::CT_Overlap && (collisionCompRhs->IsOverlappedWith(collisionCompLhs) || collisionCompLhs->
							GetIsFirstTimeCheckOverlapped() || collisionCompRhs->GetIsFirstTimeCheckOverlapped()))
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
		// AIPerception
		// -------------------------------------------------------------------------


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
				auto sourceComp = Cast<AIPerceptionSourceComponent>(scene->AIPerceptionSources[j]);
				if (!sourceComp)
				{
					continue;
				}

				auto sourceOwner = Cast<Actor>(sourceComp->GetOuter());
				auto perceptionOwner = Cast<AIController>(perceptionComp->GetOuter())->GetPossessedPawn();

				if (perceptionComp->GetIsSightEnabled() && sourceComp->GetIsSightEnabled())
				{
					auto sightFrustum = perceptionComp->GetWorldSightFrustum();
					auto sourceSight = sourceComp->GetSightStimulus();

					if (sightFrustum.Contains(DirectX::XMLoadFloat3(&sourceSight.Position))
						&& IsFirstVisible(sightFrustum.Origin, sourceSight.Position, sourceOwner, perceptionOwner,
						                  perceptionComp->GetActorClassVisibilityWhitelist()))
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
					auto sourceNoise = sourceComp->GetNoiseStimulus();

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
			}
		}


		// -------------------------------------------------------------------------
		// PostProcess-Volume
		// -------------------------------------------------------------------------
		for (int i = 0; i < scene->PostProcesses.size(); i++)
		{
			auto postProcessComponent = Cast<PostProcessComponent>(scene->PostProcesses[i]);

			if (!postProcessComponent)
				continue;


			auto volumeActor = Cast<PostProcessVolumeActor>(postProcessComponent->GetOuter());
			if (!volumeActor->GetIsEnabled())
			{
				continue;
			}

			if (volumeActor->GetUnbound())
			{
				continue;
			}

			auto playerPawn = Cast<Pawn>(GetLevel()->GetPlayerPawn());
			if (!playerPawn)
			{
				continue;
			}
			auto camera = Cast<CameraComponent>(playerPawn->GetComponentByClass("CameraComponent"));
			if (!camera)
			{
				continue;
			}

			auto volumeType = volumeActor->GetVolumeType();
			auto cameraPosition = camera->GetWorldPosition();
			auto cameraPositionVector = DirectX::XMLoadFloat3(&cameraPosition);

			switch (volumeType)
			{
			case EVolumeType::VT_Box:
				{
					auto collisionLhs = volumeActor->GetWorldBoundingOrientedBox();

					if (collisionLhs.Contains(cameraPositionVector))
					{
						if (!volumeActor->GetIsOverlapped() || volumeActor->GetIsFirstTimeCheckOverlapped())
						{
							// OnBeginOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = playerPawn;

							volumeActor->OnBeginOverlap(overlappedResult);
						}
					}
					else
					{
						if (volumeActor->GetIsOverlapped() || volumeActor->GetIsFirstTimeCheckOverlapped())
						{
							// OnEndOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = playerPawn;

							volumeActor->OnEndOverlap(overlappedResult);
						}
					}
				}
				break;
			case EVolumeType::VT_Sphere:
				{
					auto collisionLhs = volumeActor->GetWorldBoundingSphere();

					if (collisionLhs.Contains(cameraPositionVector))
					{
						if (!volumeActor->GetIsOverlapped() || volumeActor->GetIsFirstTimeCheckOverlapped())
						{
							// OnBeginOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = playerPawn;

							volumeActor->OnBeginOverlap(overlappedResult);
						}
					}
					else
					{
						if (volumeActor->GetIsOverlapped() || volumeActor->GetIsFirstTimeCheckOverlapped())
						{
							// OnEndOverlap
							OverlapResult overlappedResult = {};
							overlappedResult.OverlappedActor = playerPawn;

							volumeActor->OnEndOverlap(overlappedResult);
						}
					}
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
		if (lhsComponent->HasFiniteMass())
		{
			lhsComponent->SetVelocity(Vector3::Add(lhsComponent->GetVelocity(), Vector3::Multiply(impulsePerIMass,
			                                                                                      lhsComponent->GetInverseMass())));
		}


		if (rhsComponent->HasFiniteMass())
		{
			rhsComponent->SetVelocity(Vector3::Add(rhsComponent->GetVelocity(), Vector3::Multiply(impulsePerIMass,
			                                                                                      -rhsComponent->GetInverseMass())));
		}
	}

	void PhysicsSystem::ResolvePenetration(const std::shared_ptr<CollisionComponent>& lhsComponent,
	                                       const std::shared_ptr<CollisionComponent>& rhsComponent, const DirectX::XMFLOAT3& contactNorm, float penetration)
	{
		auto contactNormal = Vector3::Normalize(Vector3::Subtract(lhsComponent->GetWorldPosition(), rhsComponent->GetWorldPosition()));

		if (Equal(penetration, 0.0f) || Vector3::IsZero(contactNormal))
		{
			return;
		}


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

	DirectX::XMFLOAT3 PhysicsSystem::CalculateImpulse(const std::shared_ptr<CollisionComponent>& lhsComponent,
	                                                  const std::shared_ptr<CollisionComponent>& rhsComponent, const DirectX::XMFLOAT3& contactNorm)
	{
		auto contactNormal = Vector3::Normalize(Vector3::Subtract(lhsComponent->GetWorldPosition(), rhsComponent->GetWorldPosition()));

		if (Vector3::IsNan(contactNormal))
		{
			return Vector3::Zero;
		}


		//========== Resolve velocity =================================
		auto relativeVelocity = Vector3::Subtract(lhsComponent->GetVelocity(), rhsComponent->GetVelocity());
		auto seperatingVelocity = Vector3::Dot(relativeVelocity, contactNormal);
		if (seperatingVelocity > 0)
			return Vector3::Zero;

		// 분리속도, 충격량
		const float restitution = 1.0f;
		float newSepVelocity = -seperatingVelocity * restitution; // 충돌후 분리속도 = -충돌전 분리속도 * 반발계수
		float deltaVelocity = newSepVelocity - seperatingVelocity; // 속도 변화량


		return Vector3::Multiply(contactNormal, deltaVelocity);
	}

	bool PhysicsSystem::IsFirstVisible(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, Weak<Actor> target, Weak<Actor> self,
	                                   const std::vector<std::string>& visibilityWhitelist)
	{
		auto targetActor = Cast<Actor>(target.lock());
		if (!targetActor)
		{
			return false;
		}

		Crystal::HitResult hitResult = {};
		Crystal::CollisionParams collisionParams = {};
		collisionParams.IgnoreActors.push_back(self);
		collisionParams.IgnoreClasses = visibilityWhitelist;


		auto diff = Crystal::Vector3::Subtract(end, start);
		auto length = Crystal::Vector3::Length(diff);
		auto direction = Crystal::Vector3::Divide(diff, length);

		bool result = LineTraceSingle(hitResult, start, direction, length, collisionParams);
		if (!result)
		{
			return false;
		}

		auto hitActor = Cast<Actor>(hitResult.HitActor);
		if (!hitActor)
		{
			return false;
		}

		return hitActor == targetActor;
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
			auto collisionCompLhs = Cast<BoundingSphereComponent>(lhsWeak);
			if (!collisionCompLhs)
				continue;

			if (!collisionCompLhs->GetIsLineTracable())
				continue;

			auto ownerActor = Cast<Actor>(collisionCompLhs->GetOuter());
			if (collisionParams.ShouldBeIgnored(ownerActor))
			{
				continue;
			}


			auto collisionLhs = collisionCompLhs->GetWorldBoundingSphere();

			DirectX::XMVECTOR originVector = DirectX::XMLoadFloat3(&origin);
			DirectX::XMVECTOR directionVector = DirectX::XMLoadFloat3(&direction);

			bool currentResult = collisionLhs.Intersects(originVector, directionVector, dist);
			if (currentResult && dist < nearest)
			{
				nearest = dist;
				outHitResult.HitActor = ownerActor;
				outHitResult.HitComponent = collisionCompLhs;
				outHitResult.HitPosition = Vector3::Multiply(direction, dist);
			}

			result |= currentResult;
		}


		for (const auto& lhsWeak : scene->BoundingOrientedBoxComponents)
		{
			auto collisionCompLhs = Cast<BoundingOrientedBoxComponent>(lhsWeak);
			if (!collisionCompLhs)
				continue;

			if (!collisionCompLhs->GetIsLineTracable())
				continue;

			auto ownerActor = Cast<Actor>(collisionCompLhs->GetOuter());
			if (collisionParams.ShouldBeIgnored(ownerActor))
			{
				continue;
			}

			auto collisionLhs = collisionCompLhs->GetWorldBoundingOrientedBox();

			DirectX::XMVECTOR originVector = DirectX::XMLoadFloat3(&origin);
			DirectX::XMVECTOR directionVector = DirectX::XMLoadFloat3(&direction);

			bool currentResult = collisionLhs.Intersects(originVector, directionVector, dist);
			if (currentResult && dist < nearest)
			{
				nearest = dist;
				outHitResult.HitActor = ownerActor;
				outHitResult.HitComponent = collisionCompLhs;
				outHitResult.HitPosition = Vector3::Multiply(direction, dist);
			}

			result |= currentResult;
		}


		return result;
	}

	Shared<Scene> PhysicsSystem::GetScene()
	{
		return GetLevel()->GetScene();
	}

	Shared<World> PhysicsSystem::GetWorld()
	{
		return Cast<World>(GetOuter());
	}

	Shared<Level> PhysicsSystem::GetLevel()
	{
		return GetWorld()->GetCurrentLevel();
	}


}
