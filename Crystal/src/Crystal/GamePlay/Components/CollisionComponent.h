#pragma once
#include "BaseComponents.h"
#include <DirectXCollision.h>
#include "Crystal\Resources\Meshes.h"
#include "PrimitiveComponent.h"

namespace Crystal {

	class CollisionComponent : public PrimitiveComponent
	{
	public:
		CollisionComponent(const std::string& name) : PrimitiveComponent(name)
		{
			SetPrimitiveComponentType(PrimitiveComponent::EPrimitiveComponentType::Collision);
		}
	};

	class BoundingSphereComponent : public CollisionComponent
	{
	public:
		BoundingSphereComponent(const std::string& name) : CollisionComponent(name)
		{
			SetPrimitiveComponentType(PrimitiveComponent::EPrimitiveComponentType::BoundingSphere);
			SetRenderable(new LineSphere());
		}

		void SetRadius(const float radius) { m_BoundingSphere.Radius = radius; }
		float GetRadius() const { return m_BoundingSphere.Radius; }

	private:
		DirectX::BoundingSphere m_BoundingSphere;
	};
}
