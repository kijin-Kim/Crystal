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
	}


	void PrimitiveComponent::AddMaterial(std::unique_ptr<Pipeline::MaterialBase> material)
	{
		m_Materials.push_back(std::move(material));
	}


	const std::vector<std::unique_ptr<Crystal::Pipeline::MaterialBase>>& PrimitiveComponent::GetMaterials() const
	{
		return m_Materials;
	}

	const Pipeline::MaterialBase* PrimitiveComponent::GetMaterial(uint32_t index) const
	{
		return m_Materials[index].get();
	}


}
