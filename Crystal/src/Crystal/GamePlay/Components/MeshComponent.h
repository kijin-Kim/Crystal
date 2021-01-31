#pragma once
#include "BaseComponents.h"
#include "Crystal/Renderer/Mesh.h"

namespace Crystal {
	class MeshComponent : public RenderComponent
	{
	public:
		MeshComponent(const std::string& name);
		virtual ~MeshComponent() = default;

		virtual void Update(float deltaTime) override { RenderComponent::Update(deltaTime);}
		/////// TEMPORARY //////
		void SetMesh(Renderable* modelMesh) { m_Renderables.push_back(modelMesh); }
	};
}
