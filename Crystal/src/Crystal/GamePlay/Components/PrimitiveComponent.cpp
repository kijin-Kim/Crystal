#include "cspch.h"
#include "PrimitiveComponent.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"

namespace Crystal {
	void PrimitiveComponent::OnCreate()
	{
		TransformComponent::OnCreate();
	}

	void PrimitiveComponent::RegisterComponent()
	{
		TransformComponent::RegisterComponent();

		Renderer::Instance().RegisterRendererComponent(Cast<PrimitiveComponent>(shared_from_this()));

	}


	void PrimitiveComponent::AddMaterial(std::unique_ptr<Pipeline::MaterialBase> material)
	{
		m_Materials.push_back(std::move(material));
	}


	const std::vector<std::unique_ptr<Crystal::Pipeline::MaterialBase>>& PrimitiveComponent::GetMaterials() const
	{
		return m_Materials;
	}

}