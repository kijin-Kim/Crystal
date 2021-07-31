#pragma once
#include "Component.h"
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
			auto material = std::make_unique<Material>();
			material->ShadingModel = EShadingModel::SM_Unlit;
			material->AlbedoColor = Vector3::Yellow;

			AddMaterial(std::move(material));
		}

		~CollisionComponent() override = default;

		void Update(const float deltaTime) override;

		void SetLineColor(const DirectX::XMFLOAT3& color)
		{
			GetMaterial(0)->AlbedoColor = color;
		}

		const DirectX::XMFLOAT3& GetLineColor() const
		{
			/*auto color = m_MaterialsOld[0]->GetFloatInput("Color");
			return { color.x, color.y, color.z };*/
		}


		const DirectX::XMFLOAT4X4& GetPostScaledTransform() const { return m_PostScaledTransform; }

		void SetCollisionEnabled(bool bEnable) { m_bCollisionEnabled = bEnable; }
		bool IsCollisionEnabled() const override { return m_bCollisionEnabled; }


		void SetCollisionType(ECollisionType type) { m_CollisionType = type; }
		ECollisionType GetCollisionType() const { return m_CollisionType; }

		void OnHit(const struct HitResult& hitResult);
		void BindOnHitEvent(const std::function<void(const struct HitResult&)>& event);

		void OnBeginOverlap(const Weak<CollisionComponent>& overlappedComponent);
		void OnEndOverlap(const Weak<CollisionComponent>& overlappedComponent);
		bool IsOverlappedWith(const Weak<CollisionComponent>& overlappedComponent);

		const std::vector <Weak<CollisionComponent>>& GetOverlappedComponents() const { return m_OverlappedComponents; }


		STATIC_TYPE_IMPLE(CollisionComponent)
	protected:
		DirectX::XMFLOAT4X4 m_PostScaledTransform = Matrix4x4::Identity();

	private:
		bool m_bCollisionEnabled = true;

		std::vector <Weak<CollisionComponent>> m_OverlappedComponents;
		
		std::function<void(const struct HitResult&)> m_OnHitEvent;
		ECollisionType m_CollisionType = ECollisionType::CT_Block;
	};

	class RayComponent : public CollisionComponent
	{
	public:
		RayComponent() = default;

		~RayComponent() override = default;

		void RegisterComponent() override;


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
		BoundingBoxComponent() = default;
		~BoundingBoxComponent() override = default;

		void RegisterComponent() override;


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

		Collision::BoundingBox GetWorldBoundingBox()
		{
			Collision::BoundingBox worldBoundingBox = {};
			m_BoundingBox.Transform(worldBoundingBox, XMLoadFloat4x4(&m_WorldTransform));
			return worldBoundingBox;
		}


		STATIC_TYPE_IMPLE(BoundingBoxComponent)

	private:
		Collision::BoundingBox m_BoundingBox = {};
	};

	class BoundingOrientedBoxComponent : public CollisionComponent
	{
	public:
		BoundingOrientedBoxComponent() = default;
		~BoundingOrientedBoxComponent() override = default;

		void RegisterComponent() override;

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


	class BoundingFrustumComponent : public CollisionComponent
	{
	public:
		BoundingFrustumComponent() = default;
		~BoundingFrustumComponent() override = default;

		void RegisterComponent() override;

		void Update(const float deltaTime) override
		{
			CollisionComponent::Update(deltaTime);
		}


		void SetFarWidth(float width)
		{
			m_FarWidth = width;
			ReCalculateBoundingFrustum();
		}

		void SetFarHeight(float height)
		{
			m_FarHeight = height;
			ReCalculateBoundingFrustum();
		}

		void SetNearZ(float nearZ)
		{
			m_NearZ = nearZ;
			ReCalculateBoundingFrustum();
		}

		void SetFarZ(float farZ)
		{
			m_FarZ = farZ;
			ReCalculateBoundingFrustum();
		}

		void ReCalculateBoundingFrustum()
		{
			m_BoundingFrustum.LeftSlope = -m_FarWidth * 0.5f / m_FarZ;
			m_BoundingFrustum.RightSlope = m_FarWidth * 0.5f / m_FarZ;

			m_BoundingFrustum.BottomSlope = -m_FarHeight * 0.5f / m_FarZ;
			m_BoundingFrustum.TopSlope = +m_FarHeight * 0.5f / m_FarZ;

			m_BoundingFrustum.Near = m_NearZ;
			m_BoundingFrustum.Far = m_FarZ;
		}


		const DirectX::XMFLOAT3& GetOrigin() const { return m_BoundingFrustum.Origin; }
		const DirectX::XMFLOAT4& GetOrientation() const { return m_BoundingFrustum.Orientation; }
		float GetRightSlope() const { return m_BoundingFrustum.RightSlope; }
		float GetLeftSlope() const { return m_BoundingFrustum.LeftSlope; }
		float GetTopSlope() const { return m_BoundingFrustum.TopSlope; }
		float GetBottomSlope() const { return m_BoundingFrustum.BottomSlope; }
		float GetNear() const { return m_BoundingFrustum.Near; }
		float GetFar() const { return m_BoundingFrustum.Far; }


		Collision::BoundingFrustum GetWorldBoundingFrustum()
		{
			Collision::BoundingFrustum worldBoundingFrustum = {};
			m_BoundingFrustum.Transform(worldBoundingFrustum, XMLoadFloat4x4(&m_WorldTransform));
			return worldBoundingFrustum;
		}

		const Collision::BoundingFrustum& GetBoundingFrustum() const
		{
			return m_BoundingFrustum;
		}


		STATIC_TYPE_IMPLE(BoundingFrustumComponent)

	private:
		Collision::BoundingFrustum m_BoundingFrustum = {};

		float m_FarWidth = 100.0f;
		float m_FarHeight = 100.0f;

		float m_FarZ = 10.0f;
		float m_NearZ = 10.0f;


		//XMFLOAT4 Orientation; // Quaternion representing rotation.

		//float RightSlope; // Positive X (X/Z)
		//float LeftSlope; // Negative X
		//float TopSlope; // Positive Y (Y/Z)
		//float BottomSlope; // Negative Y
		//float Near, Far; // Z of the near plane and far plane.
	};


	class BoundingSphereComponent : public CollisionComponent
	{
	public:
		BoundingSphereComponent() = default;

		void RegisterComponent() override;

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
