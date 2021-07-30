#include "cspch.h"
#include "AIComponent.h"

#include "Crystal/GamePlay/Controllers/AIController.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"

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
		if(!pawn)
		{
			return;
		}

		if(m_bIsHearingEnabled)
		{
			auto translation = Matrix4x4::Translation(pawn->GetPosition());
			auto rotation = Matrix4x4::RotationQuaternion(pawn->GetRotationQuat());
			auto worldTransform = Matrix4x4::Multiply(rotation, translation);

			m_HearingSphereTransform = Matrix4x4::Multiply(Matrix4x4::Scale(m_HearingSphere.Radius), worldTransform);
		}

		if(m_bIsSightEnabled)
		{
			auto translation = Matrix4x4::Translation(pawn->GetPosition());
			auto rotation = Matrix4x4::RotationQuaternion(pawn->GetRotationQuat());
			auto worldTransform = Matrix4x4::Multiply(rotation, translation);

			m_SightFrustumTransform = Matrix4x4::Multiply(Matrix4x4::Scale(pawn->GetScale()), worldTransform);
		}
	}

	void AIPerceptionComponent::SetHearingRange(float range)
	{
		m_HearingSphere.Radius = range;
	}

	float AIPerceptionComponent::GetHearingRange() const
	{
		return m_HearingSphere.Radius;
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

	void AIPerceptionComponent::ReCalculateBoundingFrustum()
	{
		m_SightFrustum.LeftSlope = -m_SightWidth * 0.5f / m_SightRange;
		m_SightFrustum.RightSlope = m_SightWidth * 0.5f / m_SightRange;

		m_SightFrustum.BottomSlope = -m_SightHeight * 0.5f / m_SightRange;
		m_SightFrustum.TopSlope = +m_SightHeight * 0.5f / m_SightRange;

		m_SightFrustum.Near = 0.0f;
		m_SightFrustum.Far = m_SightRange;
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

	
}
