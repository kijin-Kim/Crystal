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

	class LineComponent : public CollisionComponent
	{
	public:
		LineComponent(const std::string& name) : CollisionComponent(name)
		{
			SetPrimitiveComponentType(PrimitiveComponent::EPrimitiveComponentType::Line);
			SetRenderable(new Line());
		}

		void SetOrigin(const DirectX::XMFLOAT3& origin) { m_Origin = origin; }
		void SetDirection(const DirectX::XMFLOAT3& direcion) { m_Direction = direcion; }
		void SetMaxDistance(float maxDistance) { m_MaxDistance = maxDistance; }

		const DirectX::XMFLOAT3& GetOrigin() const { return m_Origin; }
		const DirectX::XMFLOAT3& GetDirection() const { return m_Direction; }
		float GetMaxDistance() const { return m_MaxDistance; }


	private:
		DirectX::XMFLOAT3 m_Origin = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 m_Direction = { 1.0f, 0.0f, 0.0f };
		float m_MaxDistance = 1.0f;
	};
}
