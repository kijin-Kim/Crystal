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


		bool LineTraceSingle(struct HitResult& outHitResult, const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float dist,
		                     const struct CollisionParams& collisionParams);


		const Shared<Scene>& GetScene();


		STATIC_TYPE_IMPLE(PhysicsSystem)
	private:

		void ResolveVelocity(const std::shared_ptr<CollisionComponent>& lhsComponent,
		                     const std::shared_ptr<CollisionComponent>& rhsComponent, const DirectX::XMFLOAT3& impulse);

		void ResolvePenetration(const std::shared_ptr<CollisionComponent>& lhsComponent,
		                        const std::shared_ptr<CollisionComponent>& rhsComponent, 
			float penetration);

		const DirectX::XMFLOAT3& CalculateImpulse(const std::shared_ptr<CollisionComponent>& lhsComponent,
			const std::shared_ptr<CollisionComponent>& rhsComponent, const DirectX::XMFLOAT3& contactNormal);
	};
}
