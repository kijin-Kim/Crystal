#pragma once
#include "BaseComponents.h"
#include "Crystal/Resources/Meshes.h"

namespace Crystal {

	class MeshComponent : public RenderComponent
	{
	public:
		MeshComponent(const std::string& name) : RenderComponent(name) { m_RenderComponentType = ERenderComponentType::Mesh; }
		virtual ~MeshComponent() = default;
	};

	class StaticMeshComponent : public MeshComponent
	{
	public:
		StaticMeshComponent(const std::string& name) : MeshComponent(name) { m_RenderComponentType = ERenderComponentType::StaticMesh; };
		virtual ~StaticMeshComponent() = default;
	};

	class SkeletalMeshComponent : public MeshComponent
	{
	public:
		SkeletalMeshComponent(const std::string& name) :MeshComponent(name) { m_RenderComponentType = ERenderComponentType::SkeletalMesh; };
		virtual ~SkeletalMeshComponent() = default;
	};
}
