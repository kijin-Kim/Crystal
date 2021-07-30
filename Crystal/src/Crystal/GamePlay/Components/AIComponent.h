#pragma once
#include "CollisionShapes.h"
#include "Component.h"

namespace Crystal {

	class AIPerceptionComponent : public Component
	{
	public:
		AIPerceptionComponent();
		~AIPerceptionComponent() override = default;

		void RegisterComponent() override;

		void Update(float deltaTime) override;

		void SetHearingRange(float range);
		float GetHearingRange() const;

		void SetSightRange(float range);
		float GetSightRange() const;

		void SetSightWidth(float width);
		float GetSightWidth() const;

		void SetSightHeight(float height);
		float GetSightHeight() const;


		void SetIsHearingEnabled(bool bEnable) { m_bIsHearingEnabled = bEnable; }
		bool GetIsHearingEnabled() const { return m_bIsHearingEnabled; }

		void SetIsSightEnabled(bool bEnable) { m_bIsSightEnabled = bEnable; }
		bool GetIsSightEnabled() const { return m_bIsSightEnabled; }


		
		const DirectX::XMFLOAT4X4& GetHearingSphereTransform() const;
		const DirectX::XMFLOAT4X4& GetSightFrustumTransform() const;

		const Collision::BoundingSphere& GetHearingSphere() const;
		const Collision::BoundingFrustum& GetSightFrustum() const;
		


		STATIC_TYPE_IMPLE(AIPerceptionComponent)

	private:
		void ReCalculateBoundingFrustum();

	private:
		float m_SensingInterval = 1.0f / 60.0f;

		bool m_bIsHearingEnabled = false;
		Collision::BoundingSphere m_HearingSphere = {};
		DirectX::XMFLOAT4X4 m_HearingSphereTransform = Matrix4x4::Identity();
		
		bool m_bIsSightEnabled = false;
		float m_SightRange = 1000.0f;
		float m_SightWidth = 100.0f;
		float m_SightHeight = 100.0f;
		Collision::BoundingFrustum m_SightFrustum = {};
		DirectX::XMFLOAT4X4 m_SightFrustumTransform = Matrix4x4::Identity();
		
	};

	class AIPerceptionSourceComponent : public Component
	{
	public:
		AIPerceptionSourceComponent() = default;
		~AIPerceptionSourceComponent() override = default;

		void RegisterComponent() override;


		STATIC_TYPE_IMPLE(AIPerceptionSourceComponent)
	};
}
