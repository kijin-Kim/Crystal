#pragma once
#include "Crystal/GamePlay/Objects/Object.h"

namespace Crystal {
	class Level;
	class World;
	class Actor;
	class CollisionComponent;
	struct Scene;

	class PhysicsSystem : public Object
	{
	public:
		PhysicsSystem() = default;
		~PhysicsSystem() override = default;

		void Update(const float deltaTime) override;


		bool LineTraceSingle(struct HitResult& outHitResult, const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float dist,
		                     const struct CollisionParams& collisionParams);


		Shared<Scene> GetScene();
		Shared<World> GetWorld();
		Shared<Level> GetLevel();
		


		STATIC_TYPE_IMPLE(PhysicsSystem)
	private:
		void ResolveVelocity(const std::shared_ptr<CollisionComponent>& lhsComponent,
		                     const std::shared_ptr<CollisionComponent>& rhsComponent, const DirectX::XMFLOAT3& impulse);

		void ResolvePenetration(const std::shared_ptr<CollisionComponent>& lhsComponent,
		                        const std::shared_ptr<CollisionComponent>& rhsComponent, const DirectX::XMFLOAT3& contactNormal,
		                        float penetration);

		DirectX::XMFLOAT3 CalculateImpulse(const std::shared_ptr<CollisionComponent>& lhsComponent,
		                                          const std::shared_ptr<CollisionComponent>& rhsComponent, const DirectX::XMFLOAT3& contactNormal);

		bool IsFirstVisible(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, Weak<Actor> actor, Weak<Actor> selfActor, const std::vector<std::string>& visibilityWhitelist);
	};
}
