#pragma once
#include "TransformComponent.h"
#include "Crystal/Renderer/Mesh.h"

namespace Crystal {

	class MeshComponent : public TransformComponent
	{
	public:
		MeshComponent();
		virtual ~MeshComponent() = default;
		
		/////// TEMPORARY //////
		void SetMesh(const std::shared_ptr<Mesh>& modelMesh) { m_Mesh = modelMesh; }
		const std::shared_ptr<Mesh>& GetMesh() const { return m_Mesh; }

	private:
		std::shared_ptr<Mesh> m_Mesh = nullptr;
	};
}
