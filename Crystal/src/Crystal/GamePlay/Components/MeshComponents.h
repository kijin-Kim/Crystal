#pragma once
#include "Component.h"
#include "PrimitiveComponent.h"

namespace Crystal {
	class MeshComponent : public PrimitiveComponent
	{
	public:
		MeshComponent()
		{
			SetPrimitiveComponentType(EPrimitiveComponentType::Mesh);
		}
		virtual ~MeshComponent() = default;


		STATIC_TYPE_IMPLE(MeshComponent)
	};

	class StaticMeshComponent : public MeshComponent
	{
	public:
		StaticMeshComponent()
		{
			SetPrimitiveComponentType(EPrimitiveComponentType::StaticMesh);
		};
		virtual ~StaticMeshComponent() = default;

		STATIC_TYPE_IMPLE(StaticMeshComponent)

	};

	class SkeletalMeshComponent : public MeshComponent
	{
	public:
		SkeletalMeshComponent()
		{
			SetPrimitiveComponentType(EPrimitiveComponentType::SkeletalMesh);
		};
		virtual ~SkeletalMeshComponent() = default;

		STATIC_TYPE_IMPLE(SkeletalMeshComponent)
	};
}
