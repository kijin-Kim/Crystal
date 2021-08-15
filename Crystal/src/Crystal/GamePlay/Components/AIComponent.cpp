#include "cspch.h"
#include "AIComponent.h"

#include "Crystal/GamePlay/Controllers/AIController.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"
#include "Crystal/Types.h"

namespace Crystal {


	AIPerceptionComponent::AIPerceptionComponent()
	{
		SetHearingRange(100.0f);
	}

	void AIPerceptionComponent::RegisterComponent()
	{
		Component::RegisterComponent();

		auto ownerActor = GetOuter().lock();
		if (!ownerActor)
		{
			return;
		}

		auto level = Cast<Level>(ownerActor->GetOuter());
		if (!level)
		{
			return;
		}

		level->GetScene()->AIPerceptions.push_back(Cast<AIPerceptionComponent>(shared_from_this()));
	}

	void AIPerceptionComponent::Update(float deltaTime)
	{
		Component::Update(deltaTime);

		auto controller = Cast<AIController>(GetOuter());
		if (!controller)
		{
			return;
		}

		auto pawn = Cast<Pawn>(controller->GetPossessedPawn());
		if (!pawn)
		{
			return;
		}

		if (m_bIsHearingEnabled)
		{
			auto translation = Matrix4x4::Translation(pawn->GetPosition());
			auto rotation = Matrix4x4::RotationQuaternion(pawn->GetRotationQuat());
			auto worldTransform = Matrix4x4::Multiply(rotation, translation);

			m_HearingSphereTransform = Matrix4x4::Multiply(Matrix4x4::Scale(m_HearingRange), worldTransform);
		}

		if (m_bIsSightEnabled)
		{
			auto translation = Matrix4x4::Translation(pawn->GetPosition());
			auto rotation = Matrix4x4::RotationQuaternion(pawn->GetRotationQuat());
			auto worldTransform = Matrix4x4::Multiply(rotation, translation);

			m_SightFrustumTransform = worldTransform;
		}
	}

	void AIPerceptionComponent::OnSightUpdated(const SightStimulus& stimuli)
	{
		if (m_OnSightUpdatedEvent)
		{
			m_OnSightUpdatedEvent(stimuli);
		}
	}

	void AIPerceptionComponent::BindOnSightUpdatedEvent(const std::function<void(const SightStimulus&)>& event)
	{
		m_OnSightUpdatedEvent = event;
	}

	void AIPerceptionComponent::ReceiveSightStimulus(const SightStimulus& stimulus)
	{
		m_PerceptedSightStimuli.push_back(stimulus);
	}

	void AIPerceptionComponent::ForgetSightStimulus(const SightStimulus& stimulus)
	{
		auto it = FindSightStimulus(stimulus);
		if (it != m_PerceptedSightStimuli.end())
		{
			m_PerceptedSightStimuli.erase(it);
		}
	}

	void AIPerceptionComponent::OnHearingUpdated(const NoiseStimulus& stimuli)
	{
		if (m_OnHearingUpdatedEvent)
		{
			m_OnHearingUpdatedEvent(stimuli);
		}
	}

	void AIPerceptionComponent::BindOnHearingUpdatedEvent(const std::function<void(const NoiseStimulus&)>& event)
	{
		m_OnHearingUpdatedEvent = event;
	}

	void AIPerceptionComponent::ReceiveNoiseStimulus(const NoiseStimulus& stimulus)
	{
		auto it = std::find_if(m_PerceptedNoiseStimuli.begin(), m_PerceptedNoiseStimuli.end(), [&stimulus](const NoiseStimulus& other)-> bool
			{
				if(stimulus.Instigator.expired() || other.Instigator.expired())
				{
					return false;
				}

				return stimulus.Instigator.lock() == other.Instigator.lock();
			});

		if(it == m_PerceptedNoiseStimuli.end())
		{
			m_PerceptedNoiseStimuli.push_back(stimulus);
		}
	}

	void AIPerceptionComponent::ForgetNoiseStimulus(const NoiseStimulus& stimulus)
	{
		auto it = FindNoiseStimulus(stimulus);
		if (it != m_PerceptedNoiseStimuli.end())
		{
			m_PerceptedNoiseStimuli.erase(it);
		}
	}

	void AIPerceptionComponent::SetHearingRange(float range)
	{
		m_HearingRange = range;
	}

	float AIPerceptionComponent::GetHearingRange() const
	{
		return m_HearingRange;
	}

	void AIPerceptionComponent::SetSightRange(float range)
	{
		m_SightRange = range;
		ReCalculateBoundingFrustum();
	}

	float AIPerceptionComponent::GetSightRange() const
	{
		return m_SightRange;
	}

	void AIPerceptionComponent::SetSightWidth(float width)
	{
		m_SightWidth = width;
		ReCalculateBoundingFrustum();
	}

	float AIPerceptionComponent::GetSightWidth() const
	{
		return m_SightWidth;
	}

	void AIPerceptionComponent::SetSightHeight(float height)
	{
		m_SightHeight = height;
		ReCalculateBoundingFrustum();
	}

	float AIPerceptionComponent::GetSightHeight() const
	{
		return m_SightHeight;
	}

	const DirectX::XMFLOAT4X4& AIPerceptionComponent::GetHearingSphereTransform() const
	{
		return m_HearingSphereTransform;
	}

	const DirectX::XMFLOAT4X4& AIPerceptionComponent::GetSightFrustumTransform() const
	{
		return m_SightFrustumTransform;
	}

	const Collision::BoundingSphere& AIPerceptionComponent::GetHearingSphere() const
	{
		return m_HearingSphere;
	}

	const Collision::BoundingFrustum& AIPerceptionComponent::GetSightFrustum() const
	{
		return m_SightFrustum;
	}

	const Collision::BoundingSphere& AIPerceptionComponent::GetWorldHearingSphere() const
	{
		Collision::BoundingSphere worldBoundingSphere = {};
		m_HearingSphere.Transform(worldBoundingSphere, XMLoadFloat4x4(&m_HearingSphereTransform));
		return worldBoundingSphere;
	}

	const Collision::BoundingFrustum& AIPerceptionComponent::GetWorldSightFrustum() const
	{
		Collision::BoundingFrustum worldBoundingFrustum = {};
		m_SightFrustum.Transform(worldBoundingFrustum, XMLoadFloat4x4(&m_SightFrustumTransform));
		return worldBoundingFrustum;
	}

	std::vector<SightStimulus>::iterator AIPerceptionComponent::FindSightStimulus(const SightStimulus& stimulus)
	{
		return std::find_if(m_PerceptedSightStimuli.begin(), m_PerceptedSightStimuli.end(), [&stimulus](const SightStimulus& other)-> bool
		{
			return stimulus.ID == other.ID;
		});
	}

	bool AIPerceptionComponent::GetIsAlreadyPercepted(const SightStimulus& stimulus)
	{
		return FindSightStimulus(stimulus) != m_PerceptedSightStimuli.end();
	}

	std::vector<NoiseStimulus>::iterator AIPerceptionComponent::FindNoiseStimulus(const NoiseStimulus& stimulus)
	{
		return std::find_if(m_PerceptedNoiseStimuli.begin(), m_PerceptedNoiseStimuli.end(), [&stimulus](const NoiseStimulus& other)-> bool
		{
			return stimulus.ID == other.ID;
		});
	}

	bool AIPerceptionComponent::GetIsAlreadyPercepted(const NoiseStimulus& stimulus)
	{
		return FindNoiseStimulus(stimulus) != m_PerceptedNoiseStimuli.end();
	}



	void AIPerceptionComponent::ReCalculateBoundingFrustum()
	{
		m_SightFrustum.LeftSlope = -m_SightWidth * 0.5f / m_SightRange;
		m_SightFrustum.RightSlope = m_SightWidth * 0.5f / m_SightRange;

		m_SightFrustum.BottomSlope = -m_SightHeight * 0.5f / m_SightRange;
		m_SightFrustum.TopSlope = +m_SightHeight * 0.5f / m_SightRange;

		m_SightFrustum.Near = 0.0f;
		m_SightFrustum.Far = m_SightRange;
	}

	void AIPerceptionSourceComponent::Begin()
	{
		m_SightStimulus.Instigator = Cast<Actor>(GetOuter());
	}

	void AIPerceptionSourceComponent::RegisterComponent()
	{
		Component::RegisterComponent();

		auto ownerActor = GetOuter().lock();
		if (!ownerActor)
		{
			return;
		}

		auto level = Cast<Level>(ownerActor->GetOuter());
		if (!level)
		{
			return;
		}

		level->GetScene()->AIPerceptionSources.push_back(Cast<AIPerceptionSourceComponent>(shared_from_this()));
	}


	void AIPerceptionSourceComponent::MakeNoiseAtLocation(const DirectX::XMFLOAT3& position, float maxRange)
	{
		m_NoiseStimuli = NoiseStimulus::Create();
		m_NoiseStimuli.Instigator = Cast<Actor>(GetOuter());
		m_NoiseStimuli.Position = position;
		m_NoiseStimuli.MaxRange = maxRange;
	}

	void AIPerceptionSourceComponent::Update(float deltaTime)
	{
		Component::Update(deltaTime);

		m_SightStimulus.Position = Cast<Actor>(m_SightStimulus.Instigator)->GetPosition();
	}


}
