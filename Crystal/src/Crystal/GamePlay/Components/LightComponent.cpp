#include "cspch.h"
#include "LightComponent.h"

#include "Crystal/Gameplay/Controllers/PlayerController.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"

BOOST_CLASS_EXPORT(Crystal::LightComponent)
BOOST_CLASS_EXPORT(Crystal::DirectionalLightComponent)


namespace Crystal {


	void LightComponent::RegisterComponent()
	{
		TransformComponent::RegisterComponent();

		auto ownerActor = GetOuter().lock();
		if(!ownerActor)
		{
			return;
		}

		auto level = Cast<Level>(ownerActor->GetOuter());
		if (!level)
		{
			return;
		}

#ifndef CS_NM_DEDICATED
		level->GetScene()->Lights.push_back(Cast<LightComponent>(shared_from_this()));
#endif
	
	}

	void LightComponent::Update(float deltaTime)
	{
		TransformComponent::Update(deltaTime);

		if(m_bCastShadow)
		{
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
			
			auto controller = level->GetPlayerController(0).lock();
			if(!controller)
			{
				return;
			}
			auto player = controller->GetPossessedPawn().lock();
			if(!player)
			{
				return;
			}
			
			
			auto shadowCameraPosition = Vector3::Add(player->GetPosition(), Vector3::Multiply(GetLocalForwardVector(), -10000.0f));
			
			
			auto view = Matrix4x4::LookTo(shadowCameraPosition, GetLocalForwardVector(), GetLocalUpVector());
			float nearPlane = 1000.0f;
			float farPlane = 100000.0f;

			auto proj = Matrix4x4::OrthoGraphic(2048.0f * 1.0f, 2048.0f * 1.0f, nearPlane, farPlane);

			m_LightViewProjection = Matrix4x4::Multiply(view, proj);
		}
	}

}
