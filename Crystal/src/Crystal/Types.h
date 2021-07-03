#pragma once
#include "GamePlay/Objects/Actors/Actor.h"

namespace Crystal {

	struct HitResult
	{
		Weak<Actor> HitActor = {};
	};

	struct CollisionParams
	{
		std::vector<Weak<Actor>> IgnoreActors;


		bool ShouldBeIgnored(const Shared<Actor>& actor) const
		{
			auto it = std::find_if(IgnoreActors.begin(), IgnoreActors.end(), [&actor](const Weak<Actor>& other)->bool
			{
				auto otherActor = other.lock();
				if(!otherActor)
				{
					return false;
				}

				return otherActor == actor;
			});


			return it != IgnoreActors.end();
		}
	};
	
}
