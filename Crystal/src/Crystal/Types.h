#pragma once
#include <boost/container_hash/hash.hpp>
#include "Core/Core.h"


namespace Crystal {

	class Actor;
	class CollisionComponent;
	

	enum class EShadingModel
	{
		SM_Unlit,
		SM_Lit,
		ShadingModelCount
	};

	enum class EBlendMode
	{
		BM_Opaque,
		BM_Translucent,
		BM_Additive,
		BlendModeCount
	};

	enum class ECollisionType
	{
		CT_Block,
		CT_Overlap
	};


	struct PipelineStateKey
	{
		EBlendMode BlendMode;
		bool bTwoSided = false;

		bool operator==(const PipelineStateKey& other) const
		{
			return BlendMode == other.BlendMode
			&& bTwoSided == other.bTwoSided;
		}
	};
	
	
	struct HitResult
	{
		Weak<Actor> HitActor = {};
		Weak<CollisionComponent> HitComponent = {};
		DirectX::XMFLOAT3 Impulse = { 0.0f, 0.0f, 0.0f };
	};

	struct OverlapResult
	{
		Weak<Actor> OverlappedActor = {};
		Weak<CollisionComponent> OverlappedComponent = {};
	};

	struct CollisionParams
	{
		std::vector<Weak<Actor>> IgnoreActors;


		bool ShouldBeIgnored(const Shared<Actor>& actor) const
		{
			auto it = std::find_if(IgnoreActors.begin(), IgnoreActors.end(), [&actor](const Weak<Actor>& other)-> bool
			{
				auto otherActor = other.lock();
				if (!otherActor)
				{
					return false;
				}

				return otherActor == actor;
			});


			return it != IgnoreActors.end();
		}
	};

}


namespace std {
	template<>
	struct hash<Crystal::PipelineStateKey>
	{
		size_t operator()(const Crystal::PipelineStateKey& k) const
		{
			using boost::hash_value;
			using boost::hash_combine;

			size_t seed = 0;
			hash_combine(seed, hash_value(k.BlendMode));
			hash_combine(seed, hash_value(k.bTwoSided));
			return seed;
		}
	};
}