#include "cspch.h"
#include "PrimitiveComponent.h"

#include "Crystal/GamePlay/World/Level.h"

namespace Crystal {
	void PrimitiveComponent::OnCreate()
	{
		TransformComponent::OnCreate();
	}

	void PrimitiveComponent::RegisterComponent()
	{
#ifndef CS_NM_DEDICATED
		TransformComponent::RegisterComponent();

		auto ownerActor = GetObjectOwner(Owner_Actor).lock();
		if (!ownerActor)
		{
			return;
		}

		auto level = Cast<Level>(ownerActor->GetObjectOwner(Actor::ActorOwnerType::Owner_Level));
		if (!level)
		{
			return;
		}

		level->RegisterRendererComponent(Cast<PrimitiveComponent>(shared_from_this()));
#endif
	}


	void PrimitiveComponent::AddMaterial(std::unique_ptr<NewMaterial> material)
	{
		m_Materials.push_back(std::move(material));
	}


	const std::vector<std::unique_ptr<Crystal::NewMaterial>>& PrimitiveComponent::GetMaterials() const
	{
		return m_Materials;
	}

	NewMaterial* PrimitiveComponent::GetMaterial(uint32_t index) const
	{
		return m_Materials[index].get();
	}


}
