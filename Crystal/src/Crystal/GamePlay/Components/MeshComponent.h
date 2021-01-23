#pragma once
#include "BaseComponents.h"
#include "Crystal/Renderer/Mesh.h"

namespace Crystal {
	class MeshComponent : public RenderComponent
	{
	public:
		MeshComponent();
		virtual ~MeshComponent() = default;

		virtual void Update(float deltaTime) override { RenderComponent::Update(deltaTime); m_Mesh->Update(deltaTime); }
		/////// TEMPORARY //////
		void SetMesh(std::shared_ptr<Mesh> modelMesh) { m_Mesh = std::move(modelMesh); }
		Mesh* GetMesh() const { return m_Mesh.get(); }

	private:
		std::shared_ptr<Mesh> m_Mesh = nullptr;
	};
}
