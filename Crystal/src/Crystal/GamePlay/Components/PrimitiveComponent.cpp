#include "cspch.h"
#include "PrimitiveComponent.h"

#include "Crystal/GamePlay/World/Level.h"


namespace Crystal {


	void PrimitiveComponent::AddMaterial(Shared<Material> material)
	{
		m_Materials.push_back(material);
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


