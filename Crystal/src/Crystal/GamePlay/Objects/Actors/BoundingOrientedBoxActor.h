#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"

namespace Crystal {

	class BoundingOrientedBoxActor : public Actor
	{
	public:
		BoundingOrientedBoxActor() = default;
		~BoundingOrientedBoxActor() override = default;

		void Initialize() override
		{
			Actor::Initialize();

			m_BoundingOrientedBoxComponent = CreateComponent<BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
			m_BoundingOrientedBoxComponent->SetExtents({ 1.0f, 1.0f, 1.0f });
			m_BoundingOrientedBoxComponent->SetCollisionType(ECollisionType::CT_Block);
			m_BoundingOrientedBoxComponent->SetInverseMass(0.0f);
			m_BoundingOrientedBoxComponent->IgnoreActorClassOf("BoundingOrientedBoxActor");

			m_MainComponent = m_BoundingOrientedBoxComponent;
		}

		void SetExtents(const DirectX::XMFLOAT3& extents)
		{
			m_BoundingOrientedBoxComponent->SetExtents(extents);
		}



		STATIC_TYPE_IMPLE(BoundingOrientedBoxActor)

	private:
		Shared<BoundingOrientedBoxComponent> m_BoundingOrientedBoxComponent = nullptr;

	};

}


