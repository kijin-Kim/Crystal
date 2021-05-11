#pragma once
#include "Component.h"
#include <DirectXCollision.h>
#include "Crystal/Resources/Meshes.h"
#include "PrimitiveComponent.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/Renderer/Pipelines/RenderPipelines/LinePipeline.h"
#include "CollisionShapes.h"


namespace Crystal {
	class CollisionComponent : public PrimitiveComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<PrimitiveComponent>(*this);
			ar & m_PostScaledTransform;
			ar & m_bCollisionEnabled;
		}

	public:
		CollisionComponent()
		{
			auto material = std::make_unique<NewMaterial>();
			material->ShadingModel = EShadingModel::ShadingModel_Unlit;
			material->AlbedoColor = Vector3::Yellow;


			AddMaterial(std::move(material));
		}

		~CollisionComponent() override
		{
		}

		void SetLineColor(const DirectX::XMFLOAT3& color)
		{
			GetMaterial(0)->AlbedoColor = color;
		}

		const DirectX::XMFLOAT3& GetLineColor() const
		{
			/*auto color = m_MaterialsOld[0]->GetFloatInput("Color"); 
			return { color.x, color.y, color.z };*/
		}

		void RegisterComponent() override;

		void Update(const float deltaTime) override
		{
			auto position = GetLocalPosition();
			auto velocity = GetVelocity();

			position = Vector3::Add(position, Vector3::Multiply(velocity, deltaTime));
			SetLocalPosition(position);

			float inverseMass = GetInverseMass();
			auto accelertion = Vector3::Multiply(m_ForceAccum, inverseMass);

			velocity = Vector3::Add(velocity, Vector3::Multiply(accelertion, deltaTime));

			// Drag
			const float damping = 0.3f;
			velocity = Vector3::Multiply(velocity, pow(damping, deltaTime));
			SetVelocity(velocity);

			m_ForceAccum = Vector3::Zero;

			PrimitiveComponent::Update(deltaTime);
		}

		const DirectX::XMFLOAT4X4& GetPostScaledTransform() const { return m_PostScaledTransform; }

		void SetCollisionEnabled(bool bEnable) { m_bCollisionEnabled = bEnable; }
		bool IsCollisionEnabled() const override { return m_bCollisionEnabled; }


		STATIC_TYPE_IMPLE(CollisionComponent)
	protected:
		DirectX::XMFLOAT4X4 m_PostScaledTransform = Matrix4x4::Identity();


	private:
		bool m_bCollisionEnabled = true;
	};

	class RayComponent : public CollisionComponent
	{
	public:
		RayComponent()
		{
			auto& resourceManager = ResourceManager::Instance();
		}

		~RayComponent() override = default;


		void Update(const float deltaTime) override
		{
			CollisionComponent::Update(deltaTime);

			// Caculate Post Transform
			const DirectX::XMFLOAT3 currentDirection = {1.0f, 0.0f, 0.0f};

			const auto scaleMatrix = Matrix4x4::Scale(m_MaxDistance);
			auto rotationAxis = Vector3::Normalize(Vector3::Cross(currentDirection, m_Direction));
			if (Vector3::IsZero(rotationAxis))
			{
				rotationAxis = {0.0f, 0.0f, -1.0f};
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


		STATIC_TYPE_IMPLE(RayComponent)
	private:
		DirectX::XMFLOAT3 m_Origin = {0.0f, 0.0f, 0.0f};
		DirectX::XMFLOAT3 m_Direction = {1.0f, 0.0f, 0.0f};
		float m_MaxDistance = 1.0f;
	};

	class BoundingBoxComponent : public CollisionComponent
	{
		
	public:
		BoundingBoxComponent()
		{
			auto& resourceManager = ResourceManager::Instance();
#ifndef CS_NM_DEDICATED
			//SetRenderable(resourceManager.GetRenderable("LineBoxMesh"));
#endif
		}

		~BoundingBoxComponent() override = default;

		void Update(const float deltaTime) override
		{
			CollisionComponent::Update(deltaTime);

			// Caculate Post Transform
			DirectX::XMFLOAT4X4 postTransform = Matrix4x4::Scale(Vector3::Multiply(
				m_BoundingBox.Extents, {2.0f, 2.0f, 2.0f}));
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


		STATIC_TYPE_IMPLE(BoundingBoxComponent)

	private:
		DirectX::BoundingBox m_BoundingBox = {};
	};

	class BoundingOrientedBoxComponent : public CollisionComponent
	{
	public:
		BoundingOrientedBoxComponent()
		{
			auto& resourceManager = ResourceManager::Instance();
#ifndef CS_NM_DEDICATED
			//SetRenderable(resourceManager.GetRenderable("LineBoxMesh"));
#endif
		}

		~BoundingOrientedBoxComponent() override = default;

		void Update(const float deltaTime) override
		{
			CollisionComponent::Update(deltaTime);

			// Caculate Post Transform
			DirectX::XMFLOAT4X4 postTransform = Matrix4x4::Scale(Vector3::Multiply(
				m_BoundingOrientedBox.Extents, {2.0f, 2.0f, 2.0f}));
			postTransform = Matrix4x4::Multiply(postTransform,
			                                    Matrix4x4::RotationQuaternion(m_BoundingOrientedBox.Orientation));
			postTransform = Matrix4x4::Multiply(postTransform, Matrix4x4::Translation(m_BoundingOrientedBox.Center));
			m_PostScaledTransform = Matrix4x4::Multiply(postTransform, m_WorldTransform);
		}

		void SetCenter(const DirectX::XMFLOAT3& center) { m_BoundingOrientedBox.Center = center; }
		void SetExtents(const DirectX::XMFLOAT3& extent) { m_BoundingOrientedBox.Extents = extent; }
		void SetOrientation(const DirectX::XMFLOAT4& orientation) { m_BoundingOrientedBox.Orientation = orientation; }

		Collision::BoundingOrientedBox GetWorldBoundingOrientedBox()
		{
			Collision::BoundingOrientedBox worldBoundingOrientedBox = {};
			m_BoundingOrientedBox.Transform(worldBoundingOrientedBox, XMLoadFloat4x4(&m_WorldTransform));
			return worldBoundingOrientedBox;
		}

		const Collision::BoundingOrientedBox& GetBoundingOrientedBox() const
		{
			return m_BoundingOrientedBox;
		}

		STATIC_TYPE_IMPLE(BoundingOrientedBoxComponent)

	private:
		Collision::BoundingOrientedBox m_BoundingOrientedBox = {};
	};

	class BoundingSphereComponent : public CollisionComponent
	{
	public:
		BoundingSphereComponent()
		{
			auto& resourceManager = ResourceManager::Instance();
#ifndef CS_NM_DEDICATED
			//SetRenderable(resourceManager.GetRenderable("LineSphereMesh"));
#endif
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

		Collision::BoundingSphere GetWorldBoundingSphere()
		{
			Collision::BoundingSphere worldBoundingSphere = {};
			m_BoundingSphere.Transform(worldBoundingSphere, XMLoadFloat4x4(&m_WorldTransform));
			return worldBoundingSphere;
		}

		STATIC_TYPE_IMPLE(BoundingSphereComponent)
	private:
		Collision::BoundingSphere m_BoundingSphere = {};
	};
}
