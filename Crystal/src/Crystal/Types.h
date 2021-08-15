#pragma once
#include <boost/container_hash/hash.hpp>
#include "Core/Core.h"
#include "GamePlay/Objects/Actors/Actor.h"


namespace Crystal {

	class Actor;
	class CollisionComponent;

	template <class T>
	class IDCounter
	{
	public:
		IDCounter(const IDCounter&) = delete;
		IDCounter& operator=(const IDCounter&) = delete;

		static IDCounter<T>& Instance()
		{
			static IDCounter<T> instance;
			return instance;
		}

		uint64_t GetNewID()
		{
			static uint64_t Count = 0;
			return Count++;
		}

	private:
		IDCounter() = default;
		~IDCounter() = default;
	};

	enum ELightType
	{
		LT_Directional = 0,
		LT_Point
	};


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
		DirectX::XMFLOAT3 Impulse = {0.0f, 0.0f, 0.0f};
		DirectX::XMFLOAT3 HitPosition = {0.0f, 0.0f, 0.0f};
	};

	struct OverlapResult
	{
		Weak<Actor> OverlappedActor = {};
		Weak<CollisionComponent> OverlappedComponent = {};
	};

	struct CollisionParams
	{
		std::vector<Weak<Actor>> IgnoreActors;
		std::vector<std::string> IgnoreClasses;

		bool ShouldBeIgnored(const Shared<Actor>& actor) const
		{
			bool result = false;

			auto staticType = actor->StaticType();

			auto it = std::find_if(IgnoreClasses.begin(), IgnoreClasses.end(), [&staticType](const std::string& otherType)-> bool
			{
				return staticType == otherType;
			});


			result |= (it != IgnoreClasses.end());

			auto it2 = std::find_if(IgnoreActors.begin(), IgnoreActors.end(), [&actor](const Weak<Actor>& other)-> bool
			{
				auto otherActor = other.lock();
				if (!otherActor)
				{
					return false;
				}

				return otherActor == actor;
			});


			result |= it2 != IgnoreActors.end();

			return result;
		}
	};

	struct AIStimulus
	{
		uint64_t ID;
		std::string Tag;
		bool bIsSensed = true;
	};

	struct NoiseStimulus : public AIStimulus
	{
		Weak<Actor> Instigator = {};
		DirectX::XMFLOAT3 Position = {0.0f, 0.0f, 0.0f};
		float MaxRange = 0.0f;

		~NoiseStimulus() = default;

		NoiseStimulus(const NoiseStimulus& other)
		{
			ID = other.ID;
			Tag = other.Tag;
			bIsSensed = other.bIsSensed;
			Instigator = other.Instigator;
			Position = other.Position;
			MaxRange = other.MaxRange;
		}

		NoiseStimulus& operator=(const NoiseStimulus& other)
		{
			NoiseStimulus stimulus = {};
			stimulus.ID = other.ID;
			stimulus.Tag = other.Tag;
			stimulus.bIsSensed = other.bIsSensed;
			stimulus.Instigator = other.Instigator;
			stimulus.Position = other.Position;
			stimulus.MaxRange = other.MaxRange;

			return stimulus;
		}

		NoiseStimulus(NoiseStimulus&& other)
		{
			*this = std::move(other);
		}

		NoiseStimulus& operator=(NoiseStimulus&& other)
		{
			if (this != &other)
			{
				ID = other.ID;
				Tag = other.Tag;
				bIsSensed = other.bIsSensed;
				Instigator = other.Instigator;
				Position = other.Position;
				MaxRange = other.MaxRange;
			}
			return *this;
		}

		static NoiseStimulus Create()
		{
			NoiseStimulus stimulus = {};
			stimulus.ID = IDCounter<AIStimulus>::Instance().GetNewID();
			return stimulus;
		}

	private:
		NoiseStimulus() = default;
	};

	struct SightStimulus : public AIStimulus
	{
		Weak<Actor> Instigator = {};
		DirectX::XMFLOAT3 Position = {0.0f, 0.0f, 0.0f};

		~SightStimulus() = default;

		SightStimulus(const SightStimulus& other)
		{
			ID = other.ID;
			Tag = other.Tag;
			bIsSensed = other.bIsSensed;
			Instigator = other.Instigator;
			Position = other.Position;
		}

		SightStimulus& operator=(const SightStimulus& other)
		{
			SightStimulus stimulus = {};
			stimulus.ID = other.ID;
			stimulus.Tag = other.Tag;
			stimulus.bIsSensed = other.bIsSensed;
			stimulus.Instigator = other.Instigator;
			stimulus.Position = other.Position;
			return stimulus;
		}

		SightStimulus(SightStimulus&& other)
		{
			*this = std::move(other);
		}

		SightStimulus& operator=(SightStimulus&& other)
		{
			if (this != &other)
			{
				ID = other.ID;
				Tag = other.Tag;
				bIsSensed = other.bIsSensed;
				Instigator = other.Instigator;
				Position = other.Position;
			}

			return *this;
		}

		static SightStimulus Create()
		{
			SightStimulus stimulus = {};
			stimulus.ID = IDCounter<AIStimulus>::Instance().GetNewID();
			return stimulus;
		}


	private:
		SightStimulus() = default;
	};

}


namespace std {
	template <>
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
