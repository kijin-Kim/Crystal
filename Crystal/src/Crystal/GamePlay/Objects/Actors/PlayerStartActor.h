#pragma once
#include "Actor.h"


namespace Crystal {
	/*
	 * Placeholder of player actor
	 */
	
	class PlayerStartActor : public Actor
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<Actor>(*this);
		}
		
	public:
		PlayerStartActor() = default;
		~PlayerStartActor() override = default;

		STATIC_TYPE_IMPLE(PlayerStartActor)
	
	};
}
