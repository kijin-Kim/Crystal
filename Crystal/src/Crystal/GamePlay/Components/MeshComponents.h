#pragma once
#include "BaseComponents.h"
#include "PrimitiveComponent.h"

namespace Crystal {

	class MeshComponent : public PrimitiveComponent
	{
	public:
		MeshComponent(const std::string& name) : PrimitiveComponent(name) 
		{ 
			SetPrimitiveComponentType(EPrimitiveComponentType::Mesh);
		}
		virtual ~MeshComponent() = default;
	};

	class StaticMeshComponent : public MeshComponent
	{
	public:
		StaticMeshComponent(const std::string& name) : MeshComponent(name) 
		{
			SetPrimitiveComponentType(EPrimitiveComponentType::StaticMesh);
		};
		virtual ~StaticMeshComponent() = default;
	};

	class SkeletalMeshComponent : public MeshComponent
	{
	public:
		SkeletalMeshComponent(const std::string& name) :MeshComponent(name) 
		{ 
			SetPrimitiveComponentType(EPrimitiveComponentType::SkeletalMesh);
		};
		virtual ~SkeletalMeshComponent() = default;
	};
}
