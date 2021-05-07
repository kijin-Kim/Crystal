#pragma once
#include "Component.h"
#include "PrimitiveComponent.h"

namespace Crystal {
	class MeshComponent : public PrimitiveComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<PrimitiveComponent>(*this);
		}
		
	public:
		MeshComponent()
		{
		}
		virtual ~MeshComponent() = default;


		STATIC_TYPE_IMPLE(MeshComponent)
	};

	class StaticMeshComponent : public MeshComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<MeshComponent>(*this);
		}
	public:
		StaticMeshComponent()
		{
		};
		virtual ~StaticMeshComponent() = default;

		STATIC_TYPE_IMPLE(StaticMeshComponent)
	};

	class SkeletalMeshComponent : public MeshComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<MeshComponent>(*this);
		}
	public:
		SkeletalMeshComponent()
		{
		};
		virtual ~SkeletalMeshComponent() = default;

		STATIC_TYPE_IMPLE(SkeletalMeshComponent)
	};
}
