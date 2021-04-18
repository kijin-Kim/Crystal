#pragma once
#include "Crystal/GamePlay/Objects/Object.h"
#include "Crystal/GamePlay/Components/Component.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"

namespace Crystal {

	class PhysicsSystem : public Updatable
	{
	public:
		PhysicsSystem() = default;
		~PhysicsSystem() override = default;

		void Update(const float deltaTime) override;

		void RegisterPhysicsWorldComponent(std::weak_ptr<Component> compWeak);


		STATIC_TYPE_IMPLE(PhysicsWorld)
	private:
		void ResolveCollision(const std::shared_ptr<CollisionComponent>& lhsComponent,
			const std::shared_ptr<CollisionComponent>& rhsComponent, float penetration);

		void ResolveVelocity(const std::shared_ptr<CollisionComponent>& lhsComponent,
			const std::shared_ptr<CollisionComponent>& rhsComponent, float penetration);

		void ResolvePenetration(const std::shared_ptr<CollisionComponent>& lhsComponent,
			const std::shared_ptr<CollisionComponent>& rhsComponent, float penetration);

	private:
		// Actor Has OwnerShip
		std::vector<std::weak_ptr<RayComponent>> m_RayComponents;
		std::vector<std::weak_ptr<BoundingBoxComponent>> m_BoundingBoxComponents;
		std::vector<std::weak_ptr<BoundingOrientedBoxComponent>> m_BoundingOrientedBoxComponents;
		std::vector<std::weak_ptr<BoundingSphereComponent>> m_BoundingSphereComponents;
	};
}