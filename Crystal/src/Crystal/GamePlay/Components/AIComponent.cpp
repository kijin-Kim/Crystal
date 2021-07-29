#include "cspch.h"
#include "AIComponent.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"

namespace Crystal {


	AIPerceptionComponent::AIPerceptionComponent()
	{
		SetHearingRange(1000.0f);
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

		auto ownerActor = Cast<Actor>(GetOuter());
		if (!ownerActor)
		{
			return;
		}

		if(m_bIsHearingEnabled)
		{
		//	m_HearingSphereTransform = Matrix4x4::Multiply(Matrix4x4::Scale(m_HearingSphere.Radius), ownerActor->GetWorldTransform());
		}

	
	}

	void AIPerceptionComponent::SetHearingRange(float range)
	{
		//m_HearingSphere.Radius = range;
	}

	float AIPerceptionComponent::GetHearingRange() const
	{
		//return m_HearingSphere.Radius;
		return 0.0f;
	}

	const DirectX::XMFLOAT4X4& AIPerceptionComponent::GetHearingSphereTransform() const
	{
		return m_HearingSphereTransform;
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
