#pragma once
#include "Component.h"
#include "PrimitiveComponent.h"

namespace Crystal {
	class MeshComponent : public PrimitiveComponent
	{
	public:
		MeshComponent()
		{
		}
		virtual ~MeshComponent() = default;


		STATIC_TYPE_IMPLE(MeshComponent)
	};

	class StaticMeshComponent : public MeshComponent
	{
	public:
		StaticMeshComponent()
		{
		};
		virtual ~StaticMeshComponent() = default;

		STATIC_TYPE_IMPLE(StaticMeshComponent)

	};

	class SkeletalMeshComponent : public MeshComponent
	{
	public:
		SkeletalMeshComponent()
		{
		};
		virtual ~SkeletalMeshComponent() = default;

		STATIC_TYPE_IMPLE(SkeletalMeshComponent)
	};
}
