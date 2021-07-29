#pragma once
#include "Crystal/GamePlay/Objects/Object.h"

namespace Crystal {
	class CollisionComponent;
	struct Scene;

	class PhysicsSystem : public Object
	{
	public:
	
	public:
		PhysicsSystem() = default;
		~PhysicsSystem() override = default;

		void Update(const float deltaTime) override;


		bool LineTraceSingle(struct HitResult& outHitResult, const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float dist, const struct CollisionParams& collisionParams);


		const Shared<Scene>& GetScene();


		STATIC_TYPE_IMPLE(PhysicsSystem)
	private:
		void ResolveCollision(const std::shared_ptr<CollisionComponent>& lhsComponent,
			const std::shared_ptr<CollisionComponent>& rhsComponent, float penetration);

		void ResolveVelocity(const std::shared_ptr<CollisionComponent>& lhsComponent,
			const std::shared_ptr<CollisionComponent>& rhsComponent, float penetration);

		void ResolvePenetration(const std::shared_ptr<CollisionComponent>& lhsComponent,
			const std::shared_ptr<CollisionComponent>& rhsComponent, float penetration);
	};
}