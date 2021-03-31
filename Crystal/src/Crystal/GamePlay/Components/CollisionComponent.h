#pragma once
#include "Component.h"
#include <DirectXCollision.h>
#include "Crystal/Resources/Meshes.h"
#include "PrimitiveComponent.h"
#include "Crystal/Resources/ResourceManager.h"

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
		}

		void SetLineColor(const DirectX::XMFLOAT3& color) { m_Color = color; }
		const DirectX::XMFLOAT3& GetLineColor() const { return m_Color; }

		void OnCreate() override;

		const DirectX::XMFLOAT4X4& GetPostScaledTransform() const { return m_PostScaledTransform; }

	private:
		DirectX::XMFLOAT3 m_Color = { 1.0f, 1.0f, 0.0f };
	protected:
		DirectX::XMFLOAT4X4 m_PostScaledTransform = Matrix4x4::Identity(); // This is for rendering
	};

	class RayComponent : public CollisionComponent
	{
	public:
		RayComponent(const std::string& name) : CollisionComponent(name)
		{
			SetPrimitiveComponentType(PrimitiveComponent::EPrimitiveComponentType::Ray);

			auto& resourceManager = ResourceManager::Instance();
			SetRenderable(resourceManager.GetRenderable("LineMesh"));
		}

		~RayComponent() override = default;

		void Update(const float deltaTime) override
		{
			CollisionComponent::Update(deltaTime);

			// Caculate Post Transform
			const DirectX::XMFLOAT3 currentDirection = { 1.0f, 0.0f, 0.0f };

			const auto scaleMatrix = Matrix4x4::Scale(m_MaxDistance);
			auto rotationAxis = Vector3::Normalize(Vector3::Cross(currentDirection, m_Direction));
			if (Vector3::IsZero(rotationAxis))
			{
				rotationAxis = { 0.0f, 0.0f, -1.0f };
			}
			const auto rotationAngle = acosf(Vector3::Dot(currentDirection, m_Direction));
			const auto quternion =
				Vector4::QuaternionRotationAxis(rotationAxis, rotationAngle);
			const auto rotationMatrix = Matrix4x4::RotationQuaternion(quternion);
			const auto translationMatrix = Matrix4x4::Translation(m_Origin);

			DirectX::XMFLOAT4X4 postTransform = Matrix4x4::Multiply(scaleMatrix, rotationMatrix);
			postTransform = Matrix4x4::Multiply(postTransform, translationMatrix);
			m_PostScaledTransform = Matrix4x4::Multiply(postTransform, m_WorldTransform);
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

	class BoundingBoxComponent : public CollisionComponent
	{
	public:
		BoundingBoxComponent(const std::string& name) : CollisionComponent(name)
		{
			SetPrimitiveComponentType(PrimitiveComponent::EPrimitiveComponentType::BoundingBox);

			auto& resourceManager = ResourceManager::Instance();
			SetRenderable(resourceManager.GetRenderable("LineBoxMesh"));
		}

		~BoundingBoxComponent() override = default;

		void Update(const float deltaTime) override
		{
			CollisionComponent::Update(deltaTime);

			// Caculate Post Transform
			DirectX::XMFLOAT4X4 postTransform = Matrix4x4::Scale(Vector3::Multiply(
				m_BoundingBox.Extents, { 2.0f, 2.0f, 2.0f }));
			postTransform = Matrix4x4::Multiply(postTransform, Matrix4x4::Translation(m_BoundingBox.Center));
			m_PostScaledTransform = Matrix4x4::Multiply(postTransform, m_WorldTransform);
		}

		void SetCenter(const DirectX::XMFLOAT3& center) { m_BoundingBox.Center = center; }
		void SetExtents(const DirectX::XMFLOAT3& extent) { m_BoundingBox.Extents = extent; }

		DirectX::BoundingBox GetWorldBoundingBox()
		{
			DirectX::BoundingBox worldBoundingBox = {};
			m_BoundingBox.Transform(worldBoundingBox, XMLoadFloat4x4(&m_WorldTransform));
			return worldBoundingBox;
		}

	private:
		DirectX::BoundingBox m_BoundingBox = {};
	};

	class BoundingOrientedBoxComponent : public CollisionComponent
	{
	public:
		BoundingOrientedBoxComponent(const std::string& name) : CollisionComponent(name)
		{
			SetPrimitiveComponentType(PrimitiveComponent::EPrimitiveComponentType::BoundingOrientedBox);
			auto& resourceManager = ResourceManager::Instance();
			SetRenderable(resourceManager.GetRenderable("LineBoxMesh"));
		}

		~BoundingOrientedBoxComponent() override = default;

		void Update(const float deltaTime) override
		{
			CollisionComponent::Update(deltaTime);

			// Caculate Post Transform
			DirectX::XMFLOAT4X4 postTransform = Matrix4x4::Scale(Vector3::Multiply(
				m_BoundingOrientedBox.Extents, { 2.0f, 2.0f, 2.0f }));
			postTransform = Matrix4x4::Multiply(postTransform,
				Matrix4x4::RotationQuaternion(m_BoundingOrientedBox.Orientation));
			postTransform = Matrix4x4::Multiply(postTransform, Matrix4x4::Translation(m_BoundingOrientedBox.Center));
			m_PostScaledTransform = Matrix4x4::Multiply(postTransform, m_WorldTransform);
		}

		void SetCenter(const DirectX::XMFLOAT3& center) { m_BoundingOrientedBox.Center = center; }
		void SetExtents(const DirectX::XMFLOAT3& extent) { m_BoundingOrientedBox.Extents = extent; }
		void SetOrientation(const DirectX::XMFLOAT4& orientation) { m_BoundingOrientedBox.Orientation = orientation; }

		DirectX::BoundingOrientedBox GetWorldBoundingOrientedBox()
		{
			DirectX::BoundingOrientedBox worldBoundingOrientedBox = {};
			m_BoundingOrientedBox.Transform(worldBoundingOrientedBox, XMLoadFloat4x4(&m_WorldTransform));
			return worldBoundingOrientedBox;
		}

		const DirectX::BoundingOrientedBox& GetBoundingOrientedBox() const
		{
			return m_BoundingOrientedBox;
		}

	private:
		DirectX::BoundingOrientedBox m_BoundingOrientedBox = {};
	};

	class BoundingSphereComponent : public CollisionComponent
	{
	public:
		BoundingSphereComponent(const std::string& name) : CollisionComponent(name)
		{
			SetPrimitiveComponentType(PrimitiveComponent::EPrimitiveComponentType::BoundingSphere);
			auto& resourceManager = ResourceManager::Instance();
			SetRenderable(resourceManager.GetRenderable("LineSphereMesh"));
		}

		void Update(const float deltaTime) override
		{
			CollisionComponent::Update(deltaTime);

			// Caculate Post Transform
			DirectX::XMFLOAT4X4 postTransform = Matrix4x4::Scale(m_BoundingSphere.Radius);
			postTransform = Matrix4x4::Multiply(postTransform, Matrix4x4::Translation(m_BoundingSphere.Center));
			m_PostScaledTransform = Matrix4x4::Multiply(postTransform, m_WorldTransform);
		}

		void SetCenter(const DirectX::XMFLOAT3& center) { m_BoundingSphere.Center = center; }
		void SetRadius(const float radius) { m_BoundingSphere.Radius = radius; }

		DirectX::BoundingSphere GetWorldBoundingSphere()
		{
			DirectX::BoundingSphere worldBoundingSphere = {};
			m_BoundingSphere.Transform(worldBoundingSphere, XMLoadFloat4x4(&m_WorldTransform));
			return worldBoundingSphere;
		}

	private:
		DirectX::BoundingSphere m_BoundingSphere = {};
	};
}
