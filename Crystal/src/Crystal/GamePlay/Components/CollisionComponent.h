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
		~CollisionComponent() override
		{
			// Collision Component Owns Renderable
			delete m_Renderable;
		}

		void OnCreate() override;
	};

	class RayComponent : public CollisionComponent
	{
	public:
		RayComponent(const std::string& name) : CollisionComponent(name)
		{
			SetPrimitiveComponentType(PrimitiveComponent::EPrimitiveComponentType::Ray);
			SetRenderable(new Line());
		}

		~RayComponent() override = default;

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

	class BoundingBoxComponent : public CollisionComponent
	{
	public:
		BoundingBoxComponent(const std::string& name) : CollisionComponent(name)
		{
			SetPrimitiveComponentType(PrimitiveComponent::EPrimitiveComponentType::BoundingBox);
			SetRenderable(new LineBox);
		}

		~BoundingBoxComponent() override = default;

		void SetCenter(const DirectX::XMFLOAT3& center) { m_Center = center; }
		void SetExtents(const DirectX::XMFLOAT3& extent) { m_Extents = extent; }

		const DirectX::XMFLOAT3& GetCenter() const { return m_Center; }
		const DirectX::XMFLOAT3& GetExtents() const { return m_Extents; }

	private:
		DirectX::XMFLOAT3 m_Center = { 0.0f, 0.0f, 0.0f }; // Center of the box.
		DirectX::XMFLOAT3 m_Extents = { 1.0f, 1.0f, 1.0f }; // Distance from the center to each side.
	};

	class BoundingOrientedBoxComponent : public CollisionComponent
	{
	public:
		BoundingOrientedBoxComponent(const std::string& name) : CollisionComponent(name)
		{
			SetPrimitiveComponentType(PrimitiveComponent::EPrimitiveComponentType::BoundingOrientedBox);
			SetRenderable(new LineBox);
		}

		~BoundingOrientedBoxComponent() override = default;

		void SetCenter(const DirectX::XMFLOAT3& center) { m_Center = center; }
		void SetExtents(const DirectX::XMFLOAT3& extent) { m_Extents = extent; }
		void SetOrientation(const DirectX::XMFLOAT4& orientation) { m_Orientation = orientation; }

		const DirectX::XMFLOAT3& GetCenter() const { return m_Center; }
		const DirectX::XMFLOAT3& GetExtents() const { return m_Extents; }
		const DirectX::XMFLOAT4& GetOrientation() const { return m_Orientation; }

	private:
		DirectX::XMFLOAT3 m_Center = { 0.0f, 0.0f, 0.0f }; // Center of the box.
		DirectX::XMFLOAT3 m_Extents = { 0.0f, 0.0f, 0.0f }; // Distance from the center to each side.
		DirectX::XMFLOAT4 m_Orientation = { 0.0f, 0.0f, 0.0f, 1.0f }; // Unit quaternion representing rotation (box -> world).
	};




	class BoundingSphereComponent : public CollisionComponent
	{
	public:
		BoundingSphereComponent(const std::string& name) : CollisionComponent(name)
		{
			SetPrimitiveComponentType(PrimitiveComponent::EPrimitiveComponentType::BoundingSphere);
			SetRenderable(new LineSphere());
		}

		void SetCenter(const DirectX::XMFLOAT3& center) { m_Center = center; }
		void SetRadius(const float radius) { m_Radius = radius; }

		const DirectX::XMFLOAT3& GetCenter() const { return m_Center; }
		float GetRadius() const { return m_Radius; }

	private:
		DirectX::XMFLOAT3 m_Center = { 0.0f, 0.0f, 0.0f }; // Center of the sphere.
		float m_Radius = 0.0f; // Radius of the sphere.
	};


}
