#include "cspch.h"
#include "MeshComponents.h"

#include "Crystal/GamePlay/World/Level.h"

BOOST_CLASS_EXPORT(Crystal::MeshComponent)

BOOST_CLASS_EXPORT(Crystal::StaticMeshComponent)

BOOST_CLASS_EXPORT(Crystal::SkeletalMeshComponent)

namespace Crystal {
	

	void StaticMeshComponent::RegisterComponent()
	{
		Super::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto& scene = level->GetScene();

		//scene->StaticMeshes.push_back(Cast<StaticMeshComponent>(shared_from_this()));
		scene->AddStaticMesh(Cast<StaticMeshComponent>(shared_from_this()));
	}

	void SkeletalMeshComponent::RegisterComponent()
	{
		Super::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto& scene = level->GetScene();

		scene->SkeletalMeshes.push_back(Cast<SkeletalMeshComponent>(shared_from_this()));
	}
}
