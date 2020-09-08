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
		void SetMesh(Model* model) { m_ModelMesh = model; }

	private:
		Model* m_ModelMesh;
		//Materials
	};
}
