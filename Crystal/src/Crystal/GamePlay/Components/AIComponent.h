#pragma once
//#include "CollisionShapes.h"
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

		void SetIsHearingEnabled(bool bEnable) { m_bIsHearingEnabled = bEnable; }
		bool GetIsHearingEnabled() const { return m_bIsHearingEnabled; }

		
		const DirectX::XMFLOAT4X4& GetHearingSphereTransform() const;
		


		STATIC_TYPE_IMPLE(AIPerceptionComponent)
	private:
		float m_SensingInterval = 1.0f / 60.0f;

		bool m_bIsHearingEnabled = false;
		//Collision::BoundingSphere m_HearingSphere = {};
		DirectX::XMFLOAT4X4 m_HearingSphereTransform = Matrix4x4::Identity();
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
