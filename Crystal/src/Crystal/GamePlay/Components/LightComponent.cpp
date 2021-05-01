#include "cspch.h"
#include "LightComponent.h"

#include "Crystal/GamePlay/World/Level.h"

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
		level->RegisterLightComponent(Cast<LightComponent>(shared_from_this()));
#endif
	
	}

}
