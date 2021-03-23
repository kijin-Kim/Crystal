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

		void SetStartPoint(const DirectX::XMFLOAT3& point) { m_StartPoint = point; }
		void SetEndPoint(const DirectX::XMFLOAT3& point) { m_EndPoint = point; }

		const DirectX::XMFLOAT3& GetStartPoint() const { return m_StartPoint; }
		const DirectX::XMFLOAT3& GetEndPoint() const { return m_EndPoint; }

	private:
		DirectX::XMFLOAT3 m_StartPoint = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 m_EndPoint = { 1.0f, 0.0f, 0.0f };
	};
}
