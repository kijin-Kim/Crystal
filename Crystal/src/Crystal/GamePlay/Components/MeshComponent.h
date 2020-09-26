#pragma once
#include "RenderComponent.h"
#include "Crystal/Renderer/Model.h"

namespace Crystal {

	class MeshComponent : public RenderComponent
	{
	public:
		MeshComponent() = default;
		virtual ~MeshComponent() = default;
		
		/////// TEMPORARY //////
		void SetMesh(const std::shared_ptr<Model>& modelMesh) { m_ModelMesh = modelMesh; }

		const std::shared_ptr<Model>& GetDrawable() const { return m_ModelMesh; }

	private:
		std::shared_ptr<Model> m_ModelMesh = nullptr;
		//Materials
	};
}
