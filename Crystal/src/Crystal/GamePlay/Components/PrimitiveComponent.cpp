#include "cspch.h"
#include "PrimitiveComponent.h"

#include "Crystal/GamePlay/World/Level.h"

BOOST_CLASS_EXPORT(Crystal::PrimitiveComponent)

namespace Crystal {

	void PrimitiveComponent::RegisterComponent()
	{
#ifndef CS_NM_DEDICATED
		TransformComponent::RegisterComponent();

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

		
		level->RegisterRendererComponent(Cast<PrimitiveComponent>(shared_from_this()));
#endif
	}


	void PrimitiveComponent::AddMaterial(Shared<Material> material)
	{
		m_Materials.push_back(std::move(material));
	}


	const std::vector<Shared<Crystal::Material>>& PrimitiveComponent::GetMaterials() const
	{
		return m_Materials;
	}

	Material* PrimitiveComponent::GetMaterial(uint32_t index) const
	{
		return m_Materials[index].get();
	}


}
