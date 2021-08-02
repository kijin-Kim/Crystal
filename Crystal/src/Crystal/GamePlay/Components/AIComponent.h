#pragma once
#include "CollisionShapes.h"
#include "Component.h"
#include "Crystal/Types.h"
#include "Crystal/Core/Timer.h"


#define CS_STIMULUS_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Crystal {

	class AIPerceptionComponent : public Component
	{
	public:
		AIPerceptionComponent();
		~AIPerceptionComponent() override = default;

		void RegisterComponent() override;

		void Update(float deltaTime) override;

		void OnSightUpdated(const SightStimulus& stimulus);
		void BindOnSightUpdatedEvent(const std::function<void(const SightStimulus&)>& event);
		void ReceiveSightStimulus(const SightStimulus& stimulus);
		void ForgetSightStimulus(const SightStimulus& stimulus);
		
		void OnHearingUpdated(const NoiseStimulus& stimuluse);
		void BindOnHearingUpdatedEvent(const std::function<void(const NoiseStimulus&)>& event);
		void ReceiveNoiseStimulus(const NoiseStimulus& stimulus);
		void ForgetNoiseStimulus(const NoiseStimulus& stimulus);

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

		const Collision::BoundingSphere& GetWorldHearingSphere() const;
		const Collision::BoundingFrustum& GetWorldSightFrustum() const;

		
		
		std::vector<SightStimulus>::iterator FindSightStimulus(const SightStimulus& stimulus);
		bool GetIsAlreadyPercepted(const SightStimulus& stimulus);
		const std::vector<SightStimulus>& GetPerceptedSightStimuli() const { return m_PerceptedSightStimuli; }

		std::vector<NoiseStimulus>::iterator FindNoiseStimulus(const NoiseStimulus& stimulus);
		bool GetIsAlreadyPercepted(const NoiseStimulus& stimulus);
		const std::vector<NoiseStimulus>& GetPerceptedNoiseStimuli() const { return m_PerceptedNoiseStimuli; }
		
		
		
		STATIC_TYPE_IMPLE(AIPerceptionComponent)

	private:
		void ReCalculateBoundingFrustum();

	private:
		float m_SensingInterval = 1.0f / 60.0f;

		bool m_bIsHearingEnabled = false;
		float m_HearingRange = 0.0f;
		Collision::BoundingSphere m_HearingSphere = {};
		DirectX::XMFLOAT4X4 m_HearingSphereTransform = Matrix4x4::Identity();
		
		bool m_bIsSightEnabled = false;
		float m_SightRange = 1000.0f;
		float m_SightWidth = 100.0f;
		float m_SightHeight = 100.0f;
		Collision::BoundingFrustum m_SightFrustum = {};
		DirectX::XMFLOAT4X4 m_SightFrustumTransform = Matrix4x4::Identity();


		std::function<void(const SightStimulus&)> m_OnSightUpdatedEvent;
		std::function<void(const NoiseStimulus&)> m_OnHearingUpdatedEvent;

		std::vector<SightStimulus> m_PerceptedSightStimuli;
		std::vector<NoiseStimulus> m_PerceptedNoiseStimuli;
		
	};

	class AIPerceptionSourceComponent : public Component
	{	
	public:
		AIPerceptionSourceComponent() = default;
		~AIPerceptionSourceComponent() override = default;

		void Begin() override;

		void RegisterComponent() override;

		
		void MakeNoiseAtLocation(const DirectX::XMFLOAT3& position, float maxRange);

		void Update(float deltaTime) override;

		void SetIsHearingEnabled(bool bEnable) { m_bIsHearingEnabled = bEnable; }
		bool GetIsHearingEnabled() const { return m_bIsHearingEnabled; }

		void SetIsSightEnabled(bool bEnable) { m_bIsSightEnabled = bEnable; }
		bool GetIsSightEnabled() const { return m_bIsSightEnabled; }
		


		const SightStimulus& GetSightStimulus() const { return m_SightStimulus; }
		const std::vector<NoiseStimulus>& GetNoiseStimuli() const { return m_NoiseStimuli; }
		void ClearNoiseStimuli() { m_NoiseStimuli.clear(); }
		
		

		STATIC_TYPE_IMPLE(AIPerceptionSourceComponent)

	private:
		bool m_bIsHearingEnabled = false;
		bool m_bIsSightEnabled = false;


		SightStimulus m_SightStimulus = SightStimulus::Create();
		std::vector<NoiseStimulus> m_NoiseStimuli;
		
	};
}
