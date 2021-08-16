#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/CollisionShapes.h"
#include "Crystal/GamePlay/Components/PostProcessComponent.h"

namespace Crystal {

	enum class EVolumeType
	{
		VT_Box,
		VT_Sphere
	};

	enum class EVolumeBehavior
	{
		VB_EnableWhenOverlap,
		VB_EnableWhenNotOverlap
	};

	class PostProcessVolumeActor : public Actor
	{
	public:
		PostProcessVolumeActor() = default;
		~PostProcessVolumeActor() override = default;

		void Initialize() override
		{
			Actor::Initialize();

			m_PostProcessComponent = CreateComponent<PostProcessComponent>("PostProcessComponent");
			m_PostProcessComponent->AddMaterial(CreateShared<Material>());

			m_MainComponent = m_PostProcessComponent;

			SetHiddenInGame(true);
		}

		void Update(float deltaTime) override
		{
			Actor::Update(deltaTime);

			switch (m_VolumeType)
			{
			case EVolumeType::VT_Box:
				{
					// Caculate Post Transform
					DirectX::XMFLOAT4X4 postTransform = Matrix4x4::Scale(Vector3::Multiply(
						m_BoundingOrientedBox.Extents, {2.0f, 2.0f, 2.0f}));
					postTransform = Matrix4x4::Multiply(postTransform,
					                                    Matrix4x4::RotationQuaternion(m_BoundingOrientedBox.Orientation));
					postTransform = Matrix4x4::Multiply(postTransform, Matrix4x4::Translation(m_BoundingOrientedBox.Center));
					m_PostScaledTransform = Matrix4x4::Multiply(postTransform, GetWorldTransform());
				}
				break;
			case EVolumeType::VT_Sphere:
				{
					// Caculate Post Transform
					DirectX::XMFLOAT4X4 postTransform = Matrix4x4::Scale(m_BoundingSphere.Radius);
					postTransform = Matrix4x4::Multiply(postTransform, Matrix4x4::Translation(m_BoundingSphere.Center));
					m_PostScaledTransform = Matrix4x4::Multiply(postTransform, GetWorldTransform());
				}
				break;
			}
		}

		const DirectX::XMFLOAT4X4& GetPostScaledTransform() const { return m_PostScaledTransform; }

		void SetSphereCenter(const DirectX::XMFLOAT3& center) { m_BoundingSphere.Center = center; }
		void SetSphereRadius(const float radius) { m_BoundingSphere.Radius = radius; }

		void SetBoxCenter(const DirectX::XMFLOAT3& center) { m_BoundingOrientedBox.Center = center; }
		void SetBoxExtents(const DirectX::XMFLOAT3& extent) { m_BoundingOrientedBox.Extents = extent; }
		void SetBoxOrientation(const DirectX::XMFLOAT4& orientation) { m_BoundingOrientedBox.Orientation = orientation; }

		Crystal::Shared<PostProcessComponent> GetPostProcessComponent() const { return m_PostProcessComponent; }


		Collision::BoundingOrientedBox GetWorldBoundingOrientedBox() const
		{
			Collision::BoundingOrientedBox worldBoundingOrientedBox = Collision::BoundingOrientedBox();
			m_BoundingOrientedBox.Transform(worldBoundingOrientedBox, DirectX::XMLoadFloat4x4(&GetWorldTransform()));
			return worldBoundingOrientedBox;
		}

		Collision::BoundingSphere GetWorldBoundingSphere() const
		{
			Collision::BoundingSphere worldBoundingSphere = Collision::BoundingSphere();
			m_BoundingSphere.Transform(worldBoundingSphere, DirectX::XMLoadFloat4x4(&GetWorldTransform()));
			return worldBoundingSphere;
		}

		void OnBeginOverlap(const OverlapResult& overlapResult);
		void OnEndOverlap(const OverlapResult& overlapResult);
		bool GetIsOverlapped() const;


		void SetIsEnabled(bool bEnabled)
		{
			m_PostProcessComponent->SetIsEnabled(bEnabled);
			if(bEnabled)
			{
				m_bIsFirstTimeCheckOverlap = true;
			}
		}
		bool GetIsEnabled() const { return m_PostProcessComponent->GetIsEnabled(); }


		void SetVolumeType(EVolumeType type) { m_VolumeType = type; }
		EVolumeType GetVolumeType() const { return m_VolumeType; }

		void SetVolumeBehavior(EVolumeBehavior behavior) { m_VolumeBehavior = behavior; }
		EVolumeBehavior GetVolumeBehavior() const { return m_VolumeBehavior; }


		void SetPostProcessOrder(uint32_t order);
		uint32_t GetPostProcessOrder() const;

		void SetHiddenInGame(bool bHiddenInGame);
		bool GetHiddenInGame() const;

		bool GetIsFirstTimeCheckOverlapped() const
		{
			return m_bIsFirstTimeCheckOverlap;
		}

		void SetUnBound(float bUnbound)
		{
			m_bUnbound = bUnbound;
			if(m_bUnbound)
			{
				SetHiddenInGame(false);
			}
			else
			{
				m_bIsFirstTimeCheckOverlap = true;
			}
		}
		bool GetUnbound() const { return m_bUnbound; }

		STATIC_TYPE_IMPLE(PostProcessVolumeActor)

	private:
		Shared<PostProcessComponent> m_PostProcessComponent = nullptr;
		Collision::BoundingOrientedBox m_BoundingOrientedBox = {};
		Collision::BoundingSphere m_BoundingSphere = {};
		EVolumeType m_VolumeType = EVolumeType::VT_Box;
		EVolumeBehavior m_VolumeBehavior = EVolumeBehavior::VB_EnableWhenOverlap;

		DirectX::XMFLOAT4X4 m_PostScaledTransform = Matrix4x4::Identity();

		bool m_bIsOverlapped = false;


		bool m_bIsEnabled = true;

		bool m_bIsFirstTimeCheckOverlap = true;

		bool m_bUnbound = false;
	};

}
