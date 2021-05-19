#include "cspch.h"
#include "LightComponent.h"

#include "Crystal/GamePlay/World/Level.h"

BOOST_CLASS_EXPORT(Crystal::LightComponent)
BOOST_CLASS_EXPORT(Crystal::LocalLightComponent)
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

}
