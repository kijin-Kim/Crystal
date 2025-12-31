#include "cspch.h"
#include "PostProcessComponent.h"

#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"

namespace Crystal {

	void PostProcessComponent::RegisterComponent()
	{
		PrimitiveComponent::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto& scene = level->GetScene();

		scene->PostProcesses.push_back(Cast<PostProcessComponent>(shared_from_this()));

	}
}


