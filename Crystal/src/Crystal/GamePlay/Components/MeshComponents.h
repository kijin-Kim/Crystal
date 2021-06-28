#pragma once
#include "Component.h"
#include "PrimitiveComponent.h"

namespace Crystal {
	class MeshComponent : public PrimitiveComponent
	{
		using Super = PrimitiveComponent;

		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<PrimitiveComponent>(*this);
			ar & m_bCastShadow;
		}

	public:
		MeshComponent() = default;
		~MeshComponent() override = default;

		void SetCastShadow(bool bCastShadow) { m_bCastShadow = bCastShadow; }
		bool GetCastShadow() const { return m_bCastShadow; }

		STATIC_TYPE_IMPLE(MeshComponent)

	private:
		bool m_bCastShadow = true;
	};

	class StaticMeshComponent : public MeshComponent
	{
		using Super = MeshComponent;

		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<MeshComponent>(*this);
		}

	public:
		StaticMeshComponent() = default;
		~StaticMeshComponent() override = default;

		void RegisterComponent() override;

		STATIC_TYPE_IMPLE(StaticMeshComponent)


	private:
	};

	class SkeletalMeshComponent : public MeshComponent
	{
		using Super = MeshComponent;

		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<MeshComponent>(*this);
		}

	public:
		SkeletalMeshComponent() = default;
		~SkeletalMeshComponent() override = default;

		void RegisterComponent() override;
		STATIC_TYPE_IMPLE(SkeletalMeshComponent)
	};
}
