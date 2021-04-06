#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/GamePlay/Components/LightComponent.h"

namespace Crystal {
	class LightPipeline : public RenderPipeline
	{
	public:
		

		LightPipeline() = default;
		~LightPipeline() override = default;


		void RegisterLightComponents(std::weak_ptr<LightComponent> componentWeak) 
		{ 
			auto component = componentWeak.lock();
			if (!component)
				return;

			if (component->StaticType() == "LocalLightComponent")
				m_LocalLightComponents.push_back(Cast<LocalLightComponent>(component));

			if(component->StaticType() == "DirectionalLightComponent")
				m_DirectionalComponents.push_back(Cast<DirectionalLightComponent>(component));
		}

	protected:
		std::vector<std::weak_ptr<LocalLightComponent>> m_LocalLightComponents;
		std::vector<std::weak_ptr<DirectionalLightComponent>> m_DirectionalComponents;

	};



}
