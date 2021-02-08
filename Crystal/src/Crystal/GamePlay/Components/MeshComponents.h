#pragma once
#include "BaseComponents.h"
#include "Crystal/Resources/Meshes.h"

namespace Crystal {

	class MeshComponent : public RenderComponent
	{
	public:
		MeshComponent(const std::string& name);
		virtual ~MeshComponent() = default;

		void SetMesh(Mesh* mesh) { m_Renderables.push_back(mesh); }
	};

	class StaticMeshComponent : public MeshComponent
	{
	public:
		StaticMeshComponent(const std::string& name) : MeshComponent(name) {};
		virtual ~StaticMeshComponent() = default;
	};

	class SkeletalMeshComponent : public MeshComponent
	{
	public:
		SkeletalMeshComponent(const std::string& name) :MeshComponent(name) {};
		virtual ~SkeletalMeshComponent() = default;
	};
}
