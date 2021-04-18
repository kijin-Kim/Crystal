#include "cspch.h"
#include "LightComponent.h"

#include "Crystal/GamePlay/World/Level.h"

namespace Crystal {


	void LightComponent::RegisterComponent()
	{
		TransformComponent::RegisterComponent();

		auto ownerActor = GetObjectOwner(Owner_Actor).lock();
		if(!ownerActor)
		{
			return;
		}

		auto level = Cast<Level>(ownerActor->GetObjectOwner(Actor::ActorOwnerType::Owner_Level));
		if (!level)
		{
			return;
		}

		level->RegisterLightComponent(Cast<LightComponent>(shared_from_this()));
	
	}

}
